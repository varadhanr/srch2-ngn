#ifndef __SHARDING_SHARDING_ATOMIC_LOCK_OPERATION_H__
#define __SHARDING_SHARDING_ATOMIC_LOCK_OPERATION_H__

#include "NodeIteratorOperation.h"
#include "../state_machine/State.h"
#include "../state_machine/node_iterators/NodeIteratorOperation.h"
#include "../notifications/Notification.h"
#include "../metadata_manager/ResourceLocks.h"
#include "../notifications/LockingNotification.h"
#include "../../configuration/ShardingConstants.h"

namespace srch2is = srch2::instantsearch;
using namespace srch2is;
using namespace std;
namespace srch2 {
namespace httpwrapper {

/*
 * LockRequester must provide
 * void lockResult(bool granted);
 * and in case of primaryKey locking
 * void lockResult(vector<string> rejectedPrimaryKeys);
 * which will have rejectedPrimaryKeys empty if all primaryKeys can be locked successfully
 */


class AtomicLock : public OrderedNodeIteratorListenerInterface, public ProducerInterface{
public:

	/// copy
	AtomicLock(const ClusterShardId & srcShardId,
			const ClusterShardId & destShardId,
			const NodeOperationId & copyAgent,
			ConsumerInterface * lockRequester);

	/// move
	AtomicLock(const ClusterShardId & shardId,
			const NodeOperationId & srcMoveAgent,
			const NodeOperationId & destMoveAgent,
			ConsumerInterface * lockRequester);

	/// node arrival , or metadata lock in general
	AtomicLock(const NodeOperationId & newNodeOpId,
			ConsumerInterface * lockRequester,
			const vector<NodeId> & listOfOlderNodes = vector<NodeId>(),
			const LockLevel & lockLevel = LockLevel_X,
			const bool blocking = true);

	/// record locking
	AtomicLock(const vector<string> & primaryKeys,
			const NodeOperationId & writerAgent,
			ConsumerInterface * lockRequester);


	/// general purpose cluster shard locking
	AtomicLock(const ClusterShardId & shardId,
			const NodeOperationId & agent, const LockLevel & lockLevel,
			ConsumerInterface * lockRequester);

	~AtomicLock();

	void produce();

	/*
	 * This method is called after receiving the response from each participant
	 */
	bool condition(ShardingNotification * req, ShardingNotification * res);

	bool shouldAbort(const NodeId & failedNode);

	/*
	 * Lock request must be successful (or partially successful in case of primarykeys)
	 * if we reach to this function.
	 */
	void end(map<NodeId, ShardingNotification * > & replies);


	// TODO if we solve the problem of primary keys, we can remove this API
	void setParticipants(const vector<NodeId> & participants);


private:
	ConsumerInterface * consumer;

	LockingNotification * lockNotification;
	LockingNotification::LockRequestType lockType;
	OrderedNodeIteratorOperation * locker; // it will be deleted by state-machine when it returns NULL

	LockingNotification * releaseNotification;

	vector<NodeId> participants;
	int participantIndex;
	// rejectedPrimaryKey => participant index
	map<string, unsigned> rejectedPrimaryKeys;
	NodeOperationId writeAgent; // only valid and meaningful value if primary key lock

	bool finalzedFlag ;

	void recover();

	void init();


	void finalize(bool result);
	void finalize(const vector<string> & rejectedPKs);


};


}
}


#endif // __SHARDING_SHARDING_ATOMIC_LOCK_OPERATION_H__
