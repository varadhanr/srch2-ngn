#ifndef __SHARDING_PROCESSOR_SERIALIZABLE_SEARCH_RESULTS_H_
#define __SHARDING_PROCESSOR_SERIALIZABLE_SEARCH_RESULTS_H_

namespace srch2is = srch2::instantsearch;
using namespace std;

#include "sharding/configuration/ShardingConstants.h"
#include <instantsearch/QueryResults.h>
#include <core/query/QueryResultsInternal.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "core/util/SerializationHelper.h"
#include "sharding/transport/MessageAllocator.h"
#include "highlighter/Highlighter.h"

namespace srch2 {
namespace httpwrapper {


class QueryResultPtr {
    void *ptr;
    unsigned pos;

public:
    QueryResult operator*();

    //getterMethods;
    TypedValue getResultsScore() const;

    srch2::instantsearch::StoredRecordBuffer getInMemoryData() const;
};

class SearchCommandResults {
public:

    SearchCommandResults(){
        this->queryResults = new QueryResults();
        this->resultsFactory = new QueryResultFactory();
        searcherTime = 0;
    }

    ~SearchCommandResults(){
        delete this->queryResults;
        delete this->resultsFactory;
    }

    QueryResults * getQueryResults() const{
        return queryResults;
    }
    const map<string, std::pair<string, RecordSnippet> > & getInMemoryRecordStrings() const {
        return inMemoryRecordStrings;
    }
    map<string,std::pair<string, RecordSnippet> > & getInMemoryRecordStringsWrite() {
        return inMemoryRecordStrings;
    }
    QueryResultFactory * getQueryResultsFactory() const{
        return &(*resultsFactory);
    }
    void setSearcherTime(unsigned searcherTime){
        this->searcherTime = searcherTime;
    }
    void setQueryResults(QueryResults * qr){
        this->queryResults = qr ;
    }

    unsigned getSearcherTime() const{
        return searcherTime;
    }

    //serializes the object to a byte array and places array into the region
    //allocated by given allocator
    void* serialize(MessageAllocator * aloc){
        if(queryResults == NULL){
            void * buffer = aloc->allocateMessageReturnBody(sizeof(bool));
            void * bufferWritePointer = buffer;
            bufferWritePointer = srch2::util::serializeFixedTypes(false, bufferWritePointer);
            return buffer;
        }
        // first calculate the number of bytes needed
        unsigned numberOfBytes = 0;
        numberOfBytes += sizeof(bool);
        numberOfBytes += queryResults->getNumberOfBytesForSerializationForNetwork();
        numberOfBytes += getNumberOfBytesOfInMemoryRecordStrings();
        // allocate space
        void * buffer = aloc->allocateMessageReturnBody(numberOfBytes);
        // serialize
        void * bufferWritePointer = buffer;
        bufferWritePointer = srch2::util::serializeFixedTypes(true, bufferWritePointer);
        bufferWritePointer = queryResults->serializeForNetwork(bufferWritePointer);
        bufferWritePointer = serializeInMemoryRecordStrings(bufferWritePointer);

        return buffer;
    }

    //given a byte stream recreate the original object
    static SearchCommandResults * deserialize(void* buffer){

        bool isNotNull = false;
        buffer = srch2::util::deserializeFixedTypes(buffer, isNotNull);
        if(isNotNull){
            SearchCommandResults * searchResults = new SearchCommandResults();
            buffer = QueryResults::deserializeForNetwork(*(searchResults->queryResults),buffer, searchResults->resultsFactory);
            buffer = deserializeInMemoryRecordStrings(buffer,searchResults);
            return searchResults;
        }else{
            SearchCommandResults * searchResults = new SearchCommandResults();
            return searchResults;
        }
    }

    /*
     * | size of map | id1 | string1 | id2 | string2 | ...
     */
    void * serializeInMemoryRecordStrings(void * buffer){
        // serialize size of map
        buffer = srch2::util::serializeFixedTypes(((unsigned)inMemoryRecordStrings.size()), buffer);
        // serialize map
        for(map<string, std::pair<string, RecordSnippet> >::iterator recordDataItr = inMemoryRecordStrings.begin();
                recordDataItr != inMemoryRecordStrings.end() ; ++recordDataItr){
            // serialize key
            buffer = srch2::util::serializeString(recordDataItr->first, buffer);
            // serialize value
            buffer = srch2::util::serializeString(recordDataItr->second.first, buffer);
            buffer = recordDataItr->second.second.serializeForNetwork(buffer);
        }
        return buffer;
    }

    /*
     * | size of map | id1 | string1 | id2 | string2 | ...
     */
    static void * deserializeInMemoryRecordStrings(void * buffer,SearchCommandResults * searchResults){
        // serialize size of map
        unsigned sizeOfMap = 0;
        buffer = srch2::util::deserializeFixedTypes(buffer, sizeOfMap);
        // serialize map
        for(unsigned recordDataIndex = 0; recordDataIndex < sizeOfMap ; ++recordDataIndex){
            // deserialize key
            string key ;
            buffer = srch2::util::deserializeString(buffer, key);
            // deserialize value
            string value;
            RecordSnippet recSnippet;
            buffer = srch2::util::deserializeString(buffer, value);
            buffer =  RecordSnippet::deserializeForNetwork(buffer, recSnippet);
            searchResults->inMemoryRecordStrings[key] = std::make_pair(value, recSnippet);

        }
        return buffer;
    }

    /*
     * | size of map | id1 | string1 | id2 | string2 | ...
     */
    unsigned getNumberOfBytesOfInMemoryRecordStrings(){
        unsigned numberOfBytes = 0;
        // size of map
        numberOfBytes += sizeof(unsigned);
        // map
        for(map<string, std::pair<string, RecordSnippet> >::iterator recordDataItr = inMemoryRecordStrings.begin();
                recordDataItr != inMemoryRecordStrings.end() ; ++recordDataItr){
            // key
            numberOfBytes += sizeof(unsigned) + recordDataItr->first.size();
            // value
            numberOfBytes += sizeof(unsigned) + recordDataItr->second.first.size();
            numberOfBytes += recordDataItr->second.second.getNumberOfBytesOfSnippets();
        }
        return numberOfBytes;
    }

    //Returns the type of message which uses this kind of object as transport
    static ShardingMessageType messageType(){
        return SearchResultsMessageType;
    }

    std::vector<QueryResultPtr> getSortedFinalResults();

private:
    QueryResults * queryResults;
    map<string, std::pair<string, RecordSnippet> > inMemoryRecordStrings;
    QueryResultFactory * resultsFactory;
    // extra information to be added later
    unsigned searcherTime;
};


}
}

#endif // __SHARDING_PROCESSOR_SERIALIZABLE_SEARCH_RESULTS_H_