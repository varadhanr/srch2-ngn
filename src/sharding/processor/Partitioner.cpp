#include "Partitioner.h"

namespace srch2is = srch2::instantsearch;
using namespace std;

using namespace srch2is;

namespace srch2 {
namespace httpwrapper {


//###########################################################################
//                       Partitioner
//###########################################################################


Partitioner::Partitioner(ConfigManager * configurationManager){
	this->configurationManager = configurationManager;
}

/*
 * Hash implementation :
 * 1. Uses getRecordValueToHash to find the value to be hashed for this record
 * 2. Uses TransportationManager to get total number of shards to know the hash space
 * 3. Uses hash(...) to choose which shard should be responsible for this record
 * 4. Returns the information of corresponding Shard (which can be discovered from SM)
 */
ShardId Partitioner::getShardIDForRecord(Record * record){

	unsigned valueToHash = getRecordValueToHash(record);

	unsigned totalNumberOfShards = 3; //routingManager->getNumberOfShards(); TODO  we need total number of shards

	return convertUnsignedToCoreShardInfo(hash(valueToHash , totalNumberOfShards));
}

ShardId Partitioner::getShardIDForRecord(string primaryKeyStringValue){
	unsigned valueToHash = getRecordValueToHash(primaryKeyStringValue);

	unsigned totalNumberOfShards = 3; // routingManager->getNumberOfShards(); TODO we need total number of shards

	return convertUnsignedToCoreShardInfo(hash(valueToHash , totalNumberOfShards));
}


/*
 * Uses Configuration file and the given expression to
 * calculate the record corresponding value to be hashed
 * for example if this value is just ID for each record we just return
 * the value of ID
 */
unsigned Partitioner::getRecordValueToHash(Record * record){

	// When the record is being parsed, configuration is used to compute the hashable value of this
	// record. It will be saved in record.
	return 0;//TEMP	Partitioner(ConfigManager * configurationManager){
	this->configurationManager = configurationManager;
}
/*
 * Uses a hash function to hash valueToHash to a value in range [0,hashSpace)
 * and returns this value
 */
unsigned Partitioner::hash(unsigned valueToHash, unsigned hashSpace){
	// use a hash function
	// now simply round robin
	return valueToHash % hashSpace;
}


ShardId Partitioner::convertUnsignedToCoreShardInfo(unsigned coreShardIndex){
	//TODO
}

}
}


