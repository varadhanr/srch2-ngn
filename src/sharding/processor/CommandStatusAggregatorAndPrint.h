#ifndef __SHARDING_PROCESSOR_COMMAND_STATUS_AGGREGATOR_AND_PRINT_H_
#define __SHARDING_PROCESSOR_COMMAND_STATUS_AGGREGATOR_AND_PRINT_H_

#include "ResultsAggregatorAndPrint.h"
#include "serializables/SerializableCommandStatus.h"
#include "serializables/SerializableInsertUpdateCommandInput.h"
#include "serializables/SerializableDeleteCommandInput.h"
#include "serializables/SerializableSerializeCommandInput.h"
#include "serializables/SerializableResetLogCommandInput.h"
#include "serializables/SerializableCommitCommandInput.h"
#include "sharding/routing/PendingMessages.h"
#include <string>
#include <sstream>

namespace srch2is = srch2::instantsearch;
using namespace std;

using namespace srch2is;

namespace srch2 {
namespace httpwrapper {

template <class RequestWithStatusResponse>
class StatusAggregator : public ResponseAggregator<RequestWithStatusResponse,CommandStatus> {
public:


    StatusAggregator(ConfigManager * configurationManager, evhttp_request *req,
    		boost::shared_ptr<const Cluster> clusterReadview, unsigned coreId, unsigned multiRouteMode = 0):
    			ResponseAggregator<RequestWithStatusResponse,CommandStatus>(clusterReadview, coreId){
        this->configurationManager = configurationManager;
        this->req = req;
        this->multiRouteMode = multiRouteMode; // this is the case where aggregator is shared with multiple callbacks
        this->preProcessCalled = false;
        this->numberOfFinalizeCallsSoFar = 0;
    }

    void setMessages(std::stringstream & log_str){
        boost::unique_lock< boost::shared_mutex > lock(_access);
        this->messages << log_str.str();
    }

    std::stringstream & getMessages(){
        boost::unique_lock< boost::shared_mutex > lock(_access);
        return this->messages;
    }

    /*
     * This function is always called by RoutingManager as the first call back function
     */
    void preProcess(ResponseAggregatorMetadata metadata){
        if(multiRouteMode > 0){
            // we need to make sure preProcess is only called once
            boost::unique_lock< boost::shared_mutex > lock(_access);
            if(preProcessCalled == true){ // already called once
                return;
            }else{ // not called yet, the first time
                preProcessCalled = true;
            }
        }
        // any preprocess code must be written here :
        // right now, nothing to do in preProcess.
    }

    /*
     * This function is called by RoutingManager if a timeout happens, The call to
     * this function must be between preProcessing(...) and callBack()
     */
    /*
     * This function is called by RoutingManager if a timeout happens, The call to
     * this function must be between preProcessing(...) and callBack()
     */
    void processTimeout(PendingMessage<RequestWithStatusResponse, CommandStatus> * message,
            ResponseAggregatorMetadata metadata){

        if(message == NULL){
            return;
        }

        RequestWithStatusResponse * sentRequest = message->getRequestObject();

        if(typeid(InsertUpdateCommand *) == typeid(sentRequest)){// timeout in insert and update

            boost::unique_lock< boost::shared_mutex > lock(_access);
            InsertUpdateCommand * sentInsetUpdateRequest = (InsertUpdateCommand *)(sentRequest);
            messages << "{\"rid\":\"" << sentInsetUpdateRequest->getRecord()->getPrimaryKey()
                                        << "\",\"" << (sentInsetUpdateRequest->getInsertOrUpdate()?"insert":"update") << "\":\"failed\",\"reason\":\"Corresponging shard ("<<
                                        message->getNodeId()<<") timedout.\"}";

        }else if (typeid(DeleteCommand *) == typeid(sentRequest)){

            boost::unique_lock< boost::shared_mutex > lock(_access);
            DeleteCommand * sentDeleteRequest = (DeleteCommand *)(sentRequest);
            messages << "{\"rid\":\"" << sentDeleteRequest->getPrimaryKey()
                            << "\",\"delete\":\"failed\",\"reason\":\"Corresponging ("<<
                            message->getNodeId() << ") shard timedout.\"}";

        }else if(typeid(SerializeCommand *) == typeid(sentRequest)){

            boost::unique_lock< boost::shared_mutex > lock(_access);
            SerializeCommand * serializeRequest = (SerializeCommand *)(sentRequest);
            messages << "{\""<< (serializeRequest->getIndexOrRecord()?"save":"export") << "\":\"failed\",\"reason\":\"Corresponging (" <<
                    message->getNodeId() << ") shard timedout.\"}";

        }else if(typeid(ResetLogCommand *) == typeid(sentRequest)){

            boost::unique_lock< boost::shared_mutex > lock(_access);
            ResetLogCommand * resetRequest = (ResetLogCommand *)(sentRequest);
            messages << "{\"reset_log\":\"failed\",\"reason\":\"Corresponging (" << message->getNodeId()<<") shard timedout.\"}";

        }else if(typeid(CommitCommand *) == typeid(sentRequest)){

            boost::unique_lock< boost::shared_mutex > lock(_access);
            CommitCommand * resetRequest = (CommitCommand *)(sentRequest);
            messages << "{\"commit\":\"failed\",\"reason\":\"Corresponging (" << message->getNodeId()<<") shard timedout.\"}";

        }else{
            //TODO : what should we do here?
            ASSERT(false);
            return;
        }
    }

    /*
     * The main function responsible of aggregating status (success or failure) results
     */
    void callBack(PendingMessage<RequestWithStatusResponse, CommandStatus> * message){

        if(message == NULL){
            return;
        }
        boost::unique_lock< boost::shared_mutex > lock(_access);
        if(messages.str().compare("") != 0){ // string not empty
            messages << ",";
        }
        if(message->getResponseObject() == NULL){
            messages << "Node " << message->getNodeId() << " timed out.";
        }else{
            messages << message->getResponseObject()->getMessage();
        }

    }

    void callBack(vector<PendingMessage<RequestWithStatusResponse, CommandStatus> * > messagesArg){

        boost::unique_lock< boost::shared_mutex > lock(_access);
        //TODO shard info can be better than just an index
        unsigned shardIndex = 0;
        for(typename vector<PendingMessage<RequestWithStatusResponse, CommandStatus> * >::iterator
                messageItr = messagesArg.begin(); messageItr != messagesArg.end(); ++messageItr){
            if(*messageItr == NULL){
                continue;
            }
            if(messages.str().compare("") != 0){ // string not empty
                messages << ",";
            }
            if((*messageItr)->getResponseObject() == NULL){
                messages << "\"shard_" << shardIndex << "\": timed out.";
            }else{
                messages << "\"shard_" << shardIndex << "\":" << (*messageItr)->getResponseObject()->getMessage();
            }
            //
            shardIndex ++;
        }
    }
    /*
     * The last call back function called by RoutingManager in all cases.
     * Example of call back call order for search :
     * 1. preProcessing()
     * 2. timeoutProcessing() [only if some shard times out]
     * 3. aggregateSearchResults()
     * 4. finalize()
     */
    void finalize(ResponseAggregatorMetadata metadata){
        if(multiRouteMode > 0){
            // we need to make sure finalize is only called once
            boost::unique_lock< boost::shared_mutex > lock(_access);
            numberOfFinalizeCallsSoFar ++;
            if(multiRouteMode != numberOfFinalizeCallsSoFar){
                return;
            }
            // ready to be finalized because all requests are finalized
        }
        //... code here
        boost::unique_lock< boost::shared_mutex > lock(_access);
        Logger::info("%s", messages.str().c_str());

        bmhelper_evhttp_send_reply2(req, HTTP_OK, "OK",
                "{\"message\":\"The batch was processed successfully\",\"log\":["
                + messages.str() + "]}\n");
    }


private:



    ConfigManager * configurationManager;
    evhttp_request *req;
    mutable boost::shared_mutex _access;
    unsigned multiRouteMode;
    bool preProcessCalled;
    unsigned numberOfFinalizeCallsSoFar;
    std::stringstream messages;
};

}
}

#endif // __SHARDING_PROCESSOR_COMMAND_STATUS_AGGREGATOR_AND_PRINT_H_