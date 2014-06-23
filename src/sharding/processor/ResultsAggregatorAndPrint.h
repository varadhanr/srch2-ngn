#ifndef __SHARDING_PROCESSOR_RESULTS_AGGREGATOR_AND_PRINT_H_
#define __SHARDING_PROCESSOR_RESULTS_AGGREGATOR_AND_PRINT_H_


#include "sharding/configuration/ConfigManager.h"

#include <instantsearch/Record.h>
#include "wrapper/ParsedParameterContainer.h"
#include "util/CustomizableJsonWriter.h"
#include "util/Logger.h"

#include "core/highlighter/Highlighter.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include "sharding/processor/ProcessorUtil.h"

namespace srch2is = srch2::instantsearch;
using namespace std;

using namespace srch2is;

namespace srch2 {
namespace httpwrapper {

template <class Request, class Response>
class PendingMessage;

struct ResponseAggregatorMetadata{

};

template <class Request, class Response>
class ResponseAggregator {
public:


	ResponseAggregator(boost::shared_ptr<const Cluster> clusterReadview, unsigned coreId){
		this->clusterReadview = clusterReadview;
		this->coreId = coreId;
	}


	boost::shared_ptr<const Cluster> getClusterReadview(){
		return this->clusterReadview;
	}

	unsigned getCoreId(){
		return this->coreId;
	}
    /*
     * This function is always called by RoutingManager as the first call back function
     */
    virtual void preProcess(ResponseAggregatorMetadata metadata){};
    /*
     * This function is called by RoutingManager if a timeout happens, The call to
     * this function must be between preProcessing(...) and callBack()
     */
    virtual void processTimeout(PendingMessage<Request, Response> * message,ResponseAggregatorMetadata metadata){};

    /*
     * The callBack function used by routing manager
     */
    virtual void callBack(PendingMessage<Request, Response> * message){};
    virtual void callBack(vector<PendingMessage<Request, Response> * > messages){};

    /*
     * The last call back function called by RoutingManager in all cases.
     * Example of call back call order for search :
     * 1. preProcessing()
     * 2. timeoutProcessing() [only if some shard times out]
     * 3. aggregateSearchResults()
     * 4. finalize()
     */
    virtual void finalize(ResponseAggregatorMetadata metadata){};


    virtual ~ResponseAggregator(){};


private:
    boost::shared_ptr<const Cluster> clusterReadview;
    unsigned coreId;
};

}
}


#endif // __SHARDING_PROCESSOR_RESULTS_AGGREGATOR_AND_PRINT_H_