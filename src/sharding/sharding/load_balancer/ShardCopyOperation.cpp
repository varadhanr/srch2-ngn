#include "ShardCopyOperation.h"

#include "core/util/SerializationHelper.h"
#include "core/util/Assert.h"
#include "../../configuration/ShardingConstants.h"
#include "../metadata_manager/ResourceLocks.h"
#include "../metadata_manager/Cluster_Writeview.h"
#include "../ShardManager.h"
#include "LoadBalancingStartOperation.h"
#include "server/Srch2Server.h"
#include "migration/MigrationManager.h"
namespace srch2is = srch2::instantsearch;
using namespace srch2is;
using namespace std;
namespace srch2 {
namespace httpwrapper {

ShardCopyOperation::ShardCopyOperation(const unsigned operationId,
		const ClusterShardId & unassignedShard,
		NodeId srcNodeId, const ClusterShardId & shardToReplicate):
		OperationState(operationId),unassignedShardId(unassignedShard),replicaShardId(shardToReplicate),srcNodeId(srcNodeId){
	this->lockOperation = NULL;
	this->lockOperationResult = new AtomicLockOperationResult();
	this->abortedFlag = false;
	this->commitOperation = NULL;
	this->releaseOperation = NULL;
}

OperationState * ShardCopyOperation::entry(){
	/*
	 * 1. lock
	 * 2. transfer the data
	 * 3. commit
	 * 4. release
	 */
	return acquireLocks();
}


OperationState * ShardCopyOperation::acquireLocks(){
	if(lockOperation != NULL){
		return this;
	}
	vector<SingleResourceLockRequest *> lockBatch;
	SingleResourceLockRequest * xRequest = new SingleResourceLockRequest(unassignedShardId,
			NodeOperationId(ShardManager::getCurrentNodeId(),this->getOperationId()),
			ResourceLockType_X);
	SingleResourceLockRequest * sRequest = new SingleResourceLockRequest(replicaShardId,
			NodeOperationId(ShardManager::getCurrentNodeId(),this->getOperationId()),
			ResourceLockType_S);
	lockBatch.push_back(xRequest);
	lockBatch.push_back(sRequest);
	ResourceLockRequest * resourceLockRequest = new ResourceLockRequest();
	resourceLockRequest->requestBatch = lockBatch;
	resourceLockRequest->isBlocking = false;
	lockOperation = OperationState::startOperation(new AtomicLockOperation(this->getOperationId(), resourceLockRequest, this->lockOperationResult));
	if(lockOperation == NULL){
		if(this->lockOperationResult->grantedFlag == false){
			return LoadBalancingStartOperation::finalizeLoadBalancing();
		}else{
			// move to next step
			return transfer();
		}
	}
	return this;
}
OperationState * ShardCopyOperation::handle(LockingNotification::ACK * ack){
	if((lockOperation == NULL && releaseOperation == NULL) || commitOperation != NULL){
		return this;
	}
	if(lockOperation != NULL){
		OperationState::stateTransit(lockOperation, ack);
		if(lockOperation == NULL){
			if(this->lockOperationResult->grantedFlag == false){
				return LoadBalancingStartOperation::finalizeLoadBalancing();
			}else{
				// move to next step
				return transfer();
			}
		}
		return this;
	}else if(releaseOperation != NULL){
		OperationState::stateTransit(releaseOperation, ack);
		if(releaseOperation == NULL){
			return LoadBalancingStartOperation::finalizeLoadBalancing();
		}
		return this;
	}
	return this;
}

OperationState * ShardCopyOperation::transfer(){

	if(abortedFlag){ // we should not proceed
		return release();
	}
	// start transfering the data
	// call MM to transfer the shard.
//	accessMigrationManager(shardId, replicaShardId, srcNodeId);
	CopyToMeNotification * copyToMeNotif = new CopyToMeNotification(replicaShardId, unassignedShardId);
	this->send(copyToMeNotif, NodeOperationId(srcNodeId));
	delete copyToMeNotif;
	return this;
}
OperationState * ShardCopyOperation::handle(MMNotification * mmStatus){
	if(mmStatus->getStatus().status == MM_STATUS_FAILURE){
		return release();
	}else if(mmStatus->getStatus().status == MM_STATUS_SUCCESS){

		Cluster_Writeview * writeview = ShardManager::getWriteview();

		string indexDirectory = ShardManager::getShardManager()->getConfigManager()->getShardDir(writeview->clusterName,
				writeview->cores[unassignedShardId.coreId]->getName(), &unassignedShardId);
		if(indexDirectory.compare("") == 0){
			indexDirectory = ShardManager::getShardManager()->getConfigManager()->createShardDir(writeview->clusterName,
					writeview->cores[unassignedShardId.coreId]->getName(), &unassignedShardId);
		}

		physicalShard = LocalPhysicalShard(mmStatus->getStatus().shard, indexDirectory, "");
		return commit();
	}
	return this;
}


OperationState * ShardCopyOperation::commit(){
	if(commitOperation != NULL){
		return this;
	}
	// prepare the shard change
	ShardAssignChange * shardAssignChange = new ShardAssignChange(unassignedShardId, ShardManager::getCurrentNodeId(), 0);
	shardAssignChange->setPhysicalShard(physicalShard);

	commitOperation = new AtomicCommitOperation(this->getOperationId(), vector<NodeId>(), shardAssignChange);
	commitOperation = OperationState::startOperation(commitOperation);
	if(commitOperation == NULL){
		return release();
	}
	return this;
}
OperationState * ShardCopyOperation::handle(CommitNotification::ACK * ack){
	if(commitOperation == NULL || lockOperation != NULL || releaseOperation != NULL){
		return this; // ignore.
	}
	OperationState::stateTransit(commitOperation, ack);
	if(commitOperation == NULL){
		return release();
	}
	return this;
}

OperationState * ShardCopyOperation::handle(NodeFailureNotification * nodeFailure){
	if(nodeFailure == NULL){
		ASSERT(false);
		return this;
	}
	if(nodeFailure->getFailedNodeID() == srcNodeId){
		if(lockOperation != NULL){
			// we must leave the locking operation to finish and then jump to release
			this->abortedFlag = true;
		}
		if(lockOperation == NULL && commitOperation == NULL && releaseOperation == NULL){
			// we were still in transfer
			return release();
		}
		if(commitOperation != NULL){
			// we are passed the data transfer. So just continue ...
		}
		if(releaseOperation != NULL){
			// we are releasing, so just continue ...
		}
	}
	// now pass the notification to all operations
	if(lockOperation != NULL){
		OperationState::stateTransit(lockOperation, nodeFailure);
		if(lockOperation == NULL){
			if(this->lockOperationResult->grantedFlag == false){
				return LoadBalancingStartOperation::finalizeLoadBalancing();
			}else{
				// move to next step
				return transfer();
			}
		}
		return this;
	}

	if(commitOperation != NULL){
		OperationState::stateTransit(commitOperation, nodeFailure);
		if(commitOperation == NULL){
			return release();
		}
		return this;
	}
	if(releaseOperation != NULL){
		OperationState::stateTransit(releaseOperation, nodeFailure);
		if(releaseOperation == NULL){
			return LoadBalancingStartOperation::finalizeLoadBalancing();
		}
		return this;
	}
	return this;
}

OperationState * ShardCopyOperation::handle(Notification * notification){
	switch (notification->messageType()) {
		case ShardingNodeFailureNotificationMessageType:
			return handle((NodeFailureNotification *)notification);
		case ShardingMMNotificationMessageType:
			return handle((MMNotification *)notification);
		case ShardingCommitACKMessageType:
			return handle((CommitNotification::ACK *)notification);
		case ShardingLockACKMessageType:
			return handle((LockingNotification::ACK * )notification);
		default:
			// ignore;
			return this;
	}
}

string ShardCopyOperation::getOperationName() const {
	return "shard_copy";
}
string ShardCopyOperation::getOperationStatus() const {
	stringstream ss;
	ss << "Making copy of " << replicaShardId.toString() << " to prepare " << unassignedShardId.toString() << "%";
	ss << "Src node : " << srcNodeId << "%";
	if (lockOperation != NULL) {
		ss << lockOperation->getOperationName() << "%";
		ss << lockOperation->getOperationStatus();
	} else {
		ss << "Lock result : " << lockOperationResult->grantedFlag << "%";
		if(commitOperation == NULL && releaseOperation == NULL){ // we are in transfer session
			ss << "Transferring data ...%" ;
		}else{
			if(! physicalShard.server){
				ss << "Transferring " << replicaShardId.toString() << " to prepare " << unassignedShardId.toString() << " failed." << "%";
			}else{
				ss << "Transferred " << physicalShard.server->getIndexer()->getNumberOfDocumentsInIndex() << " records.%";
			}
		}

		if(commitOperation != NULL){
			ss << commitOperation->getOperationName() << "%";
			ss << commitOperation->getOperationStatus();
		}

		if(releaseOperation != NULL){
			ss << releaseOperation->getOperationName() << "%";
			ss << releaseOperation->getOperationStatus();
		}
	}

	return ss.str();
}

OperationState * ShardCopyOperation::release(){
	if(releaseOperation != NULL){
		return this;
	}
	vector<SingleResourceLockRequest *> releaseBatch;
	SingleResourceLockRequest * releaseRequest1 = new SingleResourceLockRequest(unassignedShardId,
			NodeOperationId(ShardManager::getCurrentNodeId(),this->getOperationId()));
	SingleResourceLockRequest * releaseRequest2 = new SingleResourceLockRequest(replicaShardId,
			NodeOperationId(ShardManager::getCurrentNodeId(),this->getOperationId()));
	releaseBatch.push_back(releaseRequest1);
	releaseBatch.push_back(releaseRequest2);
	ResourceLockRequest * resourceLockRequest = new ResourceLockRequest();
	resourceLockRequest->requestBatch = releaseBatch;
	resourceLockRequest->isBlocking = true;
	releaseOperation = new AtomicLockOperation(this->getOperationId(), resourceLockRequest);
	releaseOperation = OperationState::startOperation(releaseOperation);
	if(releaseOperation == NULL){
		return LoadBalancingStartOperation::finalizeLoadBalancing();
	}
	return this;
}


}
}
