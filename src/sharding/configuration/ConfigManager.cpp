
//$Id: ConfigManager.h 2013-07-5 02:11:13Z iman $

#include "ConfigManager.h"

#include <algorithm>
#include "src/server/util/xmlParser/pugixml.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <boost/program_options.hpp>
#include <assert.h>
#include "src/core/util/Logger.h"
#include <boost/algorithm/string.hpp>
#include <sys/stat.h>

#include "src/core/util/DateAndTimeHandler.h"
#include "src/core/util/ParserUtility.h"
#include "src/core/util/Assert.h"
#include "src/core/analyzer/CharSet.h"
#include "src/server/JSONRecordParser.h"

#include "boost/algorithm/string_regex.hpp"
#include "boost/filesystem/path.hpp"
#include <boost/filesystem.hpp>

#include "sharding/sharding/metadata_manager/ResourceMetadataManager.h"
#include "sharding/sharding/metadata_manager/Cluster.h"
#include "sharding/sharding/ShardManager.h"


using namespace std;
namespace srch2is = srch2::instantsearch;
using namespace pugi;
// it is related to the pgixml.hpp which is a xml parser.

using namespace srch2::instantsearch;

namespace srch2 {
namespace httpwrapper { 

const char* const ConfigManager::OAuthParam = "OAuth";
const char* const ConfigManager::authorizationKeyTag = "authorization-key";

string ConfigManager::authorizationKey = "";
// configuration file tag and attribute names for ConfigManager
// *MUST* be lowercase

const char* const ConfigManager::multicastDiscovery = "multicastdiscovery";
const char* const ConfigManager::multicastGroupAddress = "group-address";
const char* const ConfigManager::multicastIpAddress = "ipaddress";
const char* const ConfigManager::multicastPort = "port";
const char* const ConfigManager::multicastTtl = "ttl";

const char* const ConfigManager::transportNodeTag = "transport";
const char* const ConfigManager::transportIpAddress = "ipaddress";
const char* const ConfigManager::transportPort = "port";

const char* const ConfigManager::wellKnownHosts = "wellknownhosts";

const char* const ConfigManager::nodeListeningHostNameTag = "listeninghostname";
const char* const ConfigManager::nodeListeningPortTag = "internalcommunicationport";
const char* const ConfigManager::nodeCurrentTag = "this-is-me";
const char* const ConfigManager::nodeNumberOfShardsTag = "number-of-shards";
const char* const ConfigManager::nodeNameTag = "node-name";
const char* const ConfigManager::nodeMasterTag = "node-master";
const char* const ConfigManager::nodeDataTag = "node-data";
const char* const ConfigManager::nodeHomeTag = "srch2home";
const char* const ConfigManager::nodeDataDirTag = "datadir";
const char* const ConfigManager::primaryShardTag = "core-number_of_shards";
const char* const ConfigManager::replicaShardTag = "core-number_of_replicas";
const char* const ConfigManager::clusterNameTag = "cluster-name";
const int ConfigManager::DefaultNumberOfPrimaryShards = 5;
const int ConfigManager::DefaultNumberOfReplicas = 1;
const char* const ConfigManager::DefaultClusterName = "SRCH2Cluster";
const char* const ConfigManager::pingNodeTag = "ping";
const char* const ConfigManager::pingIntervalTag = "ping-interval";
const char* const ConfigManager:: pingTimeoutTag= "ping-timeout";
const char* const ConfigManager::retryCountTag = "retry-count";
const char* const ConfigManager::coreIdTag = "coreid";
static unsigned defaultCoreId;
const char* const ConfigManager::accessLogFileString = "accesslogfile";
const char* const ConfigManager::analyzerString = "analyzer";
const char* const ConfigManager::cacheSizeString = "cachesize";
const char* const ConfigManager::collectionString = "collection";
const char* const ConfigManager::configString = "config";
const char* const ConfigManager::dataDirString = "datadir";
const char* const ConfigManager::dataFileString = "datafile";
const char* const ConfigManager::multipleDataFilesTag = "multiple-data-file";
const char* const ConfigManager::dataSourceTypeString = "datasourcetype";
const char* const ConfigManager::dbString = "db";
const char* const ConfigManager::defaultString = "default";
const char* const ConfigManager::defaultQueryTermBoostString = "defaultquerytermboost";
const char* const ConfigManager::dictionaryString = "dictionary";
const char* const ConfigManager::enablePositionIndexString = "enablepositionindex";
const char* const ConfigManager::enableCharOffsetIndexString = "enablecharoffsetindex";
const char* const ConfigManager::expandString = "expand";
const char* const ConfigManager::facetEnabledString = "facetenabled";
const char* const ConfigManager::facetEndString = "facetend";
const char* const ConfigManager::facetFieldString = "facetfield";
const char* const ConfigManager::facetFieldsString = "facetfields";
const char* const ConfigManager::facetGapString = "facetgap";
const char* const ConfigManager::facetStartString = "facetstart";
const char* const ConfigManager::facetTypeString = "facettype";
const char* const ConfigManager::fieldString = "field";
const char* const ConfigManager::fieldBasedSearchString = "fieldbasedsearch";
const char* const ConfigManager::fieldBoostString = "fieldboost";
const char* const ConfigManager::fieldsString = "fields";
const char* const ConfigManager::fieldTypeString = "fieldtype";
const char* const ConfigManager::filterString = "filter";
const char* const ConfigManager::fuzzyMatchPenaltyString = "fuzzymatchpenalty";
const char* const ConfigManager::hostString = "host";
const char* const ConfigManager::indexConfigString = "indexconfig";
const char* const ConfigManager::indexedString = "indexed";
const char* const ConfigManager::multiValuedString = "multivalued";
const char* const ConfigManager::indexTypeString = "indextype";
//const char* const ConfigManager::licenseFileString = "licensefile";
const char* const ConfigManager::listenerWaitTimeString = "listenerwaittime";
const char* const ConfigManager::listeningHostStringString = "listeninghostname";
const char* const ConfigManager::listeningPortString = "listeningport";
const char* const ConfigManager::locationLatitudeString = "location_latitude";
const char* const ConfigManager::locationLongitudeString = "location_longitude";
const char* const ConfigManager::logLevelString = "loglevel";
const char* const ConfigManager::maxDocsString = "maxdocs";
const char* const ConfigManager::maxMemoryString = "maxmemory";
const char* const ConfigManager::maxRetryOnFailureString = "maxretryonfailure";
const char* const ConfigManager::maxSearchThreadsString = "maxsearchthreads";
const char* const ConfigManager::mergeEveryMWritesString = "mergeeverymwrites";
const char* const ConfigManager::mergeEveryNSecondsString = "mergeeverynseconds";
const char* const ConfigManager::mergePolicyString = "mergepolicy";
const char* const ConfigManager::mongoDbString = "mongodb";
const char* const ConfigManager::nameString = "name";
const char* const ConfigManager::portString = "port";
const char* const ConfigManager::porterStemFilterString = "PorterStemFilter";
const char* const ConfigManager::prefixMatchPenaltyString = "prefixmatchpenalty";
const char* const ConfigManager::queryString = "query";
const char* const ConfigManager::queryResponseWriterString = "queryresponsewriter";
const char* const ConfigManager::queryTermLengthBoostString = "querytermlengthboost";
const char* const ConfigManager::queryTermFuzzyTypeString = "querytermfuzzytype";
const char* const ConfigManager::queryTermSimilarityThresholdString = "querytermsimilaritythreshold";
const char* const ConfigManager::queryTermPrefixTypeString = "querytermprefixtype";
const char* const ConfigManager::rankingAlgorithmString = "rankingalgorithm";
const char* const ConfigManager::recordBoostFieldString = "recordboostfield";
const char* const ConfigManager::recordScoreExpressionString = "recordscoreexpression";
const char* const ConfigManager::refiningString = "refining";
const char* const ConfigManager::requiredString = "required";
const char* const ConfigManager::responseContentString = "responsecontent";
const char* const ConfigManager::responseFormatString = "responseformat";
const char* const ConfigManager::rowsString = "rows";
const char* const ConfigManager::schemaString = "schema";
const char* const ConfigManager::searchableString = "searchable";
const char* const ConfigManager::searcherTypeString = "searchertype";
const char* const ConfigManager::srch2HomeString = "srch2home";
const char* const ConfigManager::stopFilterString = "StopFilter";
const char* const ConfigManager::protectedWordFilterString = "protectedKeyWordsFilter";
const char* const ConfigManager::supportSwapInEditDistanceString = "supportswapineditdistance";
const char* const ConfigManager::synonymFilterString = "synonymFilter";
const char* const ConfigManager::synonymsString = "synonyms";
const char* const ConfigManager::textEnString = "text_en";
const char* const ConfigManager::typeString = "type";
const char* const ConfigManager::typesString = "types";
const char* const ConfigManager::uniqueKeyString = "uniquekey";
const char* const ConfigManager::updateHandlerString = "updatehandler";
const char* const ConfigManager::updateLogString = "updatelog";
const char* const ConfigManager::wordsString = "words";
const char* const ConfigManager::keywordPopularityThresholdString = "keywordpopularitythreshold";
const char* const ConfigManager::getAllResultsMaxResultsThreshold = "getallresultsmaxresultsthreshold";
const char* const ConfigManager::getAllResultsKAlternative = "getallresultskalternative";
const char* const ConfigManager::multipleCoresString = "cores";
const char* const ConfigManager::singleCoreString = "core";
const char* const ConfigManager::defaultCoreNameString = "defaultcorename";
const char *const ConfigManager::allowedRecordSpecialCharactersString = "allowedrecordspecialcharacters";

const char* const ConfigManager::searchPortString = "searchport";
const char* const ConfigManager::suggestPortString = "suggestport";
const char* const ConfigManager::infoPortString = "infoport";
const char* const ConfigManager::docsPortString = "docsport";
const char* const ConfigManager::updatePortString = "updateport";
const char* const ConfigManager::savePortString = "saveport";
const char* const ConfigManager::exportPortString = "exportport";
const char* const ConfigManager::resetLoggerPortString = "resetloggerport";

const char* const ConfigManager::highLightString = "highlight";
const char* const ConfigManager::highLighterString = "highlighter";
const char* const ConfigManager::exactTagPre = "exacttagpre";
const char* const ConfigManager::exactTagPost = "exacttagpost";
const char* const ConfigManager::fuzzyTagPre = "fuzzytagpre";
const char* const ConfigManager::fuzzyTagPost = "fuzzytagpost";
const char* const ConfigManager::snippetSize = "snippetsize";

const char* const ConfigManager::defaultFuzzyPreTag = "<b>";
const char* const ConfigManager::defaultFuzzyPostTag = "</b>";
const char* const ConfigManager::defaultExactPreTag = "<b>";
const char* const ConfigManager::defaultExactPostTag = "</b>";

//TODO : not used, to be deleted
////Function Definition for verifyConsistency; it checks if the port number of core is different
////from the port number being used by the node for communication with other nodes
//bool ConfigManager::verifyConsistency()
//{
//    Cluster* currentCluster = this->getCluster();
//    vector<Node>* nodes = currentCluster->getNodes();
//    Node currentNode;
//
//    //The for loop below gets the current node
//    for(int i = 0; i < nodes->size(); i++){
//        if(nodes->at(i).thisIsMe == true)
//    	    currentNode = nodes->at(i);
//    }
//
//    //The for loop below compares the current node's port number with the port number of cores
//    for(CoreInfoMap_t::iterator it = this->coreInfoIterateBegin(); it != this->coreInfoIterateEnd(); it++){
//        int num = (uint)atol(it->second->getHTTPServerListeningPort().c_str());
//        if(num == currentNode.getPortNumber()){
//    	    return false;
//        }
//    }
//    return true;
//}

ConfigManager::ConfigManager(const string& configFile)
{
    this->configFile = configFile;
    defaultCoreName = "__DEFAULTCORE__";
    defaultCoreSetFlag = false;
}

bool ConfigManager::loadConfigFile(srch2http::ResourceMetadataManager * metadataManager)
{
    Logger::debug("Reading config file: %s\n", this->configFile.c_str());
    xml_document configDoc;
    // Checks if the xml file is parsed correctly or not.
    pugi::xml_parse_result result = configDoc.load_file(this->configFile.c_str());
    // Add a comment to this line
    if (!result) {
        Logger::error("Parsing errors in XML configuration file '%s'", this->configFile.c_str());
        Logger::error("error: %s", result.description());
        exit(-1);
    }

    // make XML node names and attribute names lowercase so we are case insensitive
    lowerCaseNodeNames(configDoc);

    bool configSuccess = true;
    std::stringstream parseError;
    std::stringstream parseWarnings;
    // parse the config file and set the variables.
    this->parse(configDoc, configSuccess, parseError, parseWarnings);

    Logger::debug("WARNINGS while reading the configuration file:");
    Logger::debug("%s\n", parseWarnings.str().c_str());

    // Check if node-name is usable : we must see if there is any other instance using this name
    if(this->getCurrentNodeName().compare("") == 0){
        Logger::error("error: Node name is not usable.");
        exit(-1);
    }
    if(! tryLockNodeName()){
        Logger::error("error: Node name is not usable. Another instance is running with the same node name.");
        exit(-1);
    }


    if(metadataManager != NULL){
		metadataManager->setWriteview(new Cluster_Writeview(0, clusterNameStr, clusterCores));
    }

    if (!configSuccess) {
        Logger::error("ERRORS while reading the configuration file");
        Logger::error("%s\n", parseError.str().c_str());
        Logger::debug(parseError.str().c_str());
        return false;
    }

    return true;
}

class XmlLowerCaseWalker : public xml_tree_walker
{
public:
    virtual bool for_each(xml_node &node);
};

// iterator calls this method once for each node in tree
bool XmlLowerCaseWalker::for_each(xml_node &node)
{
    // lowercase the name of each node
    const char_t *oldName = node.name();

    if (oldName && oldName[0]) {
        unsigned int length = strlen(oldName);

	if (length > 0) {
	    // duplicate name, but in lowercase
	    char_t *newName = new char_t[length + 1];
	    for (unsigned int i = 0; i < length; i++)
	        newName[i] = tolower(oldName[i]);
	    newName[length] = '\000';

	    (void) node.set_name(newName); // discard return - no need to interrupt traversal
	    delete[] newName;
	}
    }

    // lowercase attribute names too
    for (pugi::xml_attribute_iterator attribute = node.attributes_begin(); attribute != node.attributes_end(); ++attribute) {
        oldName = attribute->name();
	unsigned int length = strlen(oldName);

	if (length > 0) {
	    // duplicate name, but in lowercase
	    char_t *newName = new char_t[length + 1];
	    for (unsigned int i = 0; i < length; i++)
	        newName[i] = tolower(oldName[i]);
	    newName[length] = '\000';

	    (void) attribute->set_name(newName);  // ignoring return value to avoid interrupting tree traversal
	    delete[] newName;
	}
    }

    return true;  // always allow tree traversal to continue uninterrupted
}

void ConfigManager::lowerCaseNodeNames(xml_node &document)
{
    XmlLowerCaseWalker nodeTraversal;

    document.traverse(nodeTraversal);
}

void ConfigManager::trimSpacesFromValue(string &fieldValue, const char *fieldName, std::stringstream &parseWarnings, const char *append)
{
    string oldValue(fieldValue);
    trim(fieldValue);
    if (fieldValue.length() != oldValue.length()) {
        parseWarnings << "Trimmed whitespace from the variable " << fieldName << " \"" << oldValue << "\"\n";
    }
    if (append != NULL) {
        fieldValue += append;
    }
}

void ConfigManager::parseIndexConfig(const xml_node &indexConfigNode, CoreInfo_t *coreInfo, map<string, unsigned> &boostsMap, bool &configSuccess, std::stringstream &parseError, std::stringstream &parseWarnings)
{
    xml_node childNode = indexConfigNode.child(indexTypeString);
    coreInfo->indexType = 0; //Default index type is 0
    if (childNode && childNode.text()) {
        string it = string(childNode.text().get());
        if (isValidIndexType(it)) {
            coreInfo->indexType = childNode.text().as_int();
        } else {
            parseError << "Index Type's value can be only 0 or 1.\n";
            configSuccess = false;
            return;
        }
    } else {
    	Logger::warn("Index Type is not set, so the engine will use the default value 0");
        return;
    }

    coreInfo->supportSwapInEditDistance = true; // by default it is true
    childNode = indexConfigNode.child(supportSwapInEditDistanceString);
    if (childNode && childNode.text()) {
        string qtmt = childNode.text().get();
        if (isValidBool(qtmt)) {
            coreInfo->supportSwapInEditDistance = childNode.text().as_bool();
        } else {
            parseError << "The provided supportSwapInEditDistance flag is not valid";
            configSuccess = false;
            return;
        }
    }

    coreInfo->enableWordPositionIndex = false; // by default it is false
    childNode = indexConfigNode.child(enablePositionIndexString);
    if (childNode && childNode.text()) {
        string configValue = childNode.text().get();
        if (isValidBooleanValue(configValue)) {
            coreInfo->enableWordPositionIndex = childNode.text().as_bool();
        } else {
            parseError << "enablePositionIndex should be either 0 or 1.\n";
            configSuccess = false;
            return;
        }
        if (coreInfo->enableWordPositionIndex) {
            Logger::debug("turning on attribute based search because position index is enabled");
            coreInfo->supportAttributeBasedSearch = true;
        } // else leave supportAttributeBasedSearch set to previous value
    }
    coreInfo->enableCharOffsetIndex = false; // by default it is false
    childNode = indexConfigNode.child(enableCharOffsetIndexString);
    if (childNode && childNode.text()) {
    	string configValue = childNode.text().get();
    	if (isValidBooleanValue(configValue)) {
    		coreInfo->enableCharOffsetIndex = childNode.text().as_bool();
    	} else {
    		parseError << "enableCharOffsetIndex should be either 0 or 1.\n";
    		configSuccess = false;
    		return;
    	}
    	if (!coreInfo->enableWordPositionIndex && coreInfo->enableCharOffsetIndex) {
    		Logger::debug("turning on attribute based search because position index is enabled");
    		coreInfo->supportAttributeBasedSearch = true;
    	} // else leave supportAttributeBasedSearch set to previous value
    }

    childNode = indexConfigNode.child(fieldBoostString);
    // splitting the field boost input and put them in boostsMap
    if (childNode && childNode.text()) {
        string boostString = string(childNode.text().get());
        boost::algorithm::trim(boostString);
        splitBoostFieldValues(boostString, boostsMap);
    }

    // recordBoostField is an optional field
    // It should be refining and of type float, otherwise engine will not run
    coreInfo->recordBoostFieldFlag = false;
    childNode = indexConfigNode.child(recordBoostFieldString);
    if (childNode && childNode.text()) {
        string recordBoostField = string(childNode.text().get());
        if(coreInfo->refiningAttributesInfo[recordBoostField].attributeType != ATTRIBUTE_TYPE_FLOAT ){
            Logger::error("Type of record boost field is invalid, it should be of type float");
            configSuccess = false;
            return;
        }
        coreInfo->recordBoostFieldFlag = true;
        coreInfo->recordBoostField = recordBoostField;
    }

    // queryTermBoost is an optional field
    coreInfo->queryTermBoost = 1; // By default it is 1
    childNode = indexConfigNode.child(defaultQueryTermBoostString);
    if (childNode && childNode.text()) {
        string qtb = childNode.text().get();
        if (isValidQueryTermBoost(qtb)) {
            coreInfo->queryTermBoost = childNode.text().as_uint();
        } else {
            configSuccess = false;
            parseError << "The value provided for queryTermBoost is not a (non-negative)number.";
            return;
        }
    }
}

void ConfigManager::parseMongoDb(const xml_node &mongoDbNode, CoreInfo_t *coreInfo, bool &configSuccess, std::stringstream &parseError, std::stringstream &parseWarnings)
{
    xml_node childNode = mongoDbNode.child(hostString);
    if (childNode && childNode.text()) {
        coreInfo->mongoHost = string(childNode.text().get());
    } else {
        parseError << "mongo host is not set.\n";
        configSuccess = false;
        return;
    }

    childNode = mongoDbNode.child(portString);
    if (childNode && childNode.text()) {
        coreInfo->mongoPort = string(childNode.text().get());
        int value = static_cast<int>(strtol(coreInfo->mongoPort.c_str(), NULL,
                10));
        if (value <= 0 || value > USHRT_MAX) {
            parseError << "mongoPort must be between 1 and " << USHRT_MAX;
            configSuccess = false;
            return;
        }
    } else {
        coreInfo->mongoPort = ""; // use default port
    }

    childNode = mongoDbNode.child(dbString);
    if (childNode && childNode.text()) {
        coreInfo->mongoDbName = string(childNode.text().get());
    } else {
        parseError << "mongo data base name is not set.\n";
        configSuccess = false;
        return;
    }

    childNode = mongoDbNode.child(collectionString);
    if (childNode && childNode.text()) {
        coreInfo->mongoCollection = string(childNode.text().get());
    } else {
        parseError << "mongo collection name is not set.\n";
        configSuccess = false;
        return;
    }

    childNode = mongoDbNode.child(listenerWaitTimeString);
    if (childNode && childNode.text()) {
        coreInfo->mongoListenerWaitTime = childNode.text().as_uint(1);
    } else {
        coreInfo->mongoListenerWaitTime = 1;
    }

    childNode = mongoDbNode.child(maxRetryOnFailureString);
    if (childNode && childNode.text()) {
        coreInfo->mongoListenerMaxRetryOnFailure = childNode.text().as_uint(3);
    } else {
        coreInfo->mongoListenerMaxRetryOnFailure = 3;
    }

    // For MongoDB as a data source , primary key must be "_id" which is a unique key generated
    // by MongoDB. It is important to set primary key to "_id" because oplog entries for inserts
    // and deletes in MongoDB can be identified by _id only.
    coreInfo->primaryKey = "_id";
}

void ConfigManager::parseQuery(const xml_node &queryNode,
                               CoreInfo_t *coreInfo,
                               bool &configSuccess,
                               std::stringstream &parseError,
                               std::stringstream &parseWarnings)
{
    // scoringExpressionString is an optional field
    coreInfo->scoringExpressionString = "1"; // By default it is 1
    xml_node childNode = queryNode.child(rankingAlgorithmString).child(recordScoreExpressionString);
    if (childNode && childNode.text()) {
        string exp = childNode.text().get();
        boost::algorithm::trim(exp);
        if (isValidRecordScoreExpession(exp)) {
            coreInfo->scoringExpressionString = exp;
        } else {
            configSuccess = false;
            parseError << "The expression provided for recordScoreExpression is not a valid.";
            return;
        }
    }

    // fuzzyMatchPenalty is an optional field
    coreInfo->fuzzyMatchPenalty = 1; // By default it is 1
    childNode = queryNode.child(fuzzyMatchPenaltyString);
    if (childNode && childNode.text()) {
        string qtsb = childNode.text().get();
        if (isValidFuzzyMatchPenalty(qtsb)) {
            coreInfo->fuzzyMatchPenalty = childNode.text().as_float();
        } else {
            configSuccess = false;
            parseError << "The expression provided for fuzzyMatchPenalty is not a valid.";
            return;
        }
    }

    // queryTermSimilarityThreshold is an optional field
    //By default it is 0.5.
    coreInfo->queryTermSimilarityThreshold = 0.5;
    childNode = queryNode.child(queryTermSimilarityThresholdString);
    if (childNode && childNode.text()) {
        string qtsb = childNode.text().get();
        if (isValidQueryTermSimilarityThreshold(qtsb)) {
            coreInfo->queryTermSimilarityThreshold = childNode.text().as_float();
            if (coreInfo->queryTermSimilarityThreshold < 0 || coreInfo->queryTermSimilarityThreshold > 1 ){
                coreInfo->queryTermSimilarityThreshold = 0.5;
                parseError << "The value provided for queryTermSimilarityThreshold is not in [0,1].";
            }
        } else {
            configSuccess = false;
            parseError << "The value provided for queryTermSimilarityThreshold is not a valid.";
            return;
        }
    }

    // queryTermLengthBoost is an optional field
    coreInfo->queryTermLengthBoost = 0.5; // By default it is 0.5
    childNode = queryNode.child(queryTermLengthBoostString);
    if (childNode && childNode.text()) {
        string qtlb = childNode.text().get();
        if (isValidQueryTermLengthBoost(qtlb)) {
            coreInfo->queryTermLengthBoost = childNode.text().as_float();
        } else {
            configSuccess = false;
            parseError << "The expression provided for queryTermLengthBoost is not a valid.";
            return;
        }
    }

    // prefixMatchPenalty is an optional field.
    coreInfo->prefixMatchPenalty = 0.95; // By default it is 0.95
    childNode = queryNode.child(prefixMatchPenaltyString);
    if (childNode && childNode.text()) {
        string pm = childNode.text().get();

        if (isValidPrefixMatch(pm)) {
            coreInfo->prefixMatchPenalty = childNode.text().as_float();
        } else {
            configSuccess = false;
            parseError << "The value provided for prefixMatch is not a valid.";
            return;
        }
    }

    // cacheSize is an optional field
    coreInfo->cacheSizeInBytes = 50 * 1048576;
    childNode = queryNode.child(cacheSizeString);
    if (childNode && childNode.text()) {
        string cs = childNode.text().get();
        if (isValidCacheSize(cs)) {
            coreInfo->cacheSizeInBytes = childNode.text().as_uint();
        } else {
            parseError << "cache size provided is not set correctly.\n";
            configSuccess = false;
            return;
        }
    }

    // rows is an optional field
    coreInfo->resultsToRetrieve = 10; // by default it is 10
    childNode = queryNode.child(rowsString);
    if (childNode && childNode.text()) {
        string row = childNode.text().get();
        if (isValidRows(row)) {
            coreInfo->resultsToRetrieve = childNode.text().as_int();
        } else {
            parseError << "rows is not set correctly.\n";
            configSuccess = false;
            return;
        }
    }

    // fieldBasedSearch is an optional field
    if (coreInfo->enableWordPositionIndex == false &&
    		coreInfo->enableCharOffsetIndex == false ) {
        coreInfo->supportAttributeBasedSearch = false; // by default it is false
        childNode = queryNode.child(fieldBasedSearchString);
        if (childNode && childNode.text()) {
            string configValue = childNode.text().get();
            if (isValidBooleanValue(configValue)) {
                coreInfo->supportAttributeBasedSearch = childNode.text().as_bool();
            } else {
                parseError << "fieldBasedSearch is not set correctly.\n";
                configSuccess = false;
                return;
            }
        }
    } else {
        // attribute based search is enabled if positional index is enabled
        childNode = queryNode.child(fieldBasedSearchString);
        string configValue = childNode.text().get();
        if(isValidBooleanValue(configValue)){
            if(configValue.compare("0") == 0){
                if(coreInfo->enableWordPositionIndex == true || coreInfo->enableCharOffsetIndex == true){
                    Logger::warn("Attribute based search is on because either character offset or word positional index is enabled");
                }
            }
        }
        coreInfo->supportAttributeBasedSearch = true;
    }

    // queryTermFuzzyType is an optional field
    coreInfo->exactFuzzy = false; // by default it is false
    childNode = queryNode.child(queryTermFuzzyTypeString);
    if (childNode && childNode.text()) {
        string qtmt = childNode.text().get();
        if (isValidQueryTermFuzzyType(qtmt)) {
            coreInfo->exactFuzzy = childNode.text().as_bool();
        } else {
            parseError << "The queryTermFuzzyType that is provided is not valid";
            configSuccess = false;
            return;
        }
    }

    // queryTermPrefixType is an optional field
    coreInfo->queryTermPrefixType = false;
    childNode = queryNode.child(queryTermPrefixTypeString);
    if (childNode && childNode.text()) {
        string qt = childNode.text().get();
        if (isValidQueryTermPrefixType(qt)) {
            coreInfo->queryTermPrefixType = childNode.text().as_bool();
        } else {
            parseError << "The queryTerm that is provided is not valid";
            configSuccess = false;
            return;
        }
    }

    // responseFormat is an optional field
    coreInfo->searchResponseJsonFormat = 0; // by default it is 0
    childNode = queryNode.child(queryResponseWriterString).child(responseFormatString);
    if (childNode && childNode.text()) {
        string rf = childNode.text().get();
        if (isValidResponseFormat(rf)) {
            coreInfo->searchResponseJsonFormat = childNode.text().as_int();
        } else {
            parseError << "The provided responseFormat is not valid";
            configSuccess = false;
            return;
        }
    }

    coreInfo->fuzzyHighlightMarkerPre = defaultFuzzyPreTag;
    coreInfo->fuzzyHighlightMarkerPost = defaultFuzzyPostTag;
    coreInfo->exactHighlightMarkerPre = defaultExactPreTag;
    coreInfo->exactHighlightMarkerPost = defaultExactPostTag;
    coreInfo->highlightSnippetLen = 150;

    childNode = queryNode.child(highLighterString).child(snippetSize);
    if (childNode && childNode.text()) {
    	coreInfo->highlightSnippetLen = childNode.text().as_int();
    }
    childNode = queryNode.child(highLighterString).child(exactTagPre);
    if (childNode) {
    	string marker = childNode.attribute("value").value();
    	boost::algorithm::trim(marker);
    	if (marker.length() > 0){
    		coreInfo->exactHighlightMarkerPre = marker;
    	} else {
    		parseError << "The highlighter pre marker is an empty string, so the engine will use the default marker";
    		return;
    	}
    }
    childNode = queryNode.child(highLighterString).child(exactTagPost);
    if (childNode) {
    	string marker = childNode.attribute("value").value();
    	boost::algorithm::trim(marker);
    	if (marker.length() > 0){
    		coreInfo->exactHighlightMarkerPost = marker;
    	} else {
    		parseError << "The highlighter post marker is an empty string, so the engine will use the default marker";
    		return;
    	}
	}
    childNode = queryNode.child(highLighterString).child(fuzzyTagPre);
    if (childNode) {
    	string marker = childNode.attribute("value").value();
    	boost::algorithm::trim(marker);
    	if (marker.length() > 0){
    		coreInfo->fuzzyHighlightMarkerPre = marker;
    	} else {
    		parseError << "The highlighter pre marker is an empty string, so the engine will use the default marker";
    		return;
    	}
    }
    childNode = queryNode.child(highLighterString).child(fuzzyTagPost);
    if (childNode) {
    	string marker = childNode.attribute("value").value();
    	boost::algorithm::trim(marker);
    	if (marker.length() > 0){
    		coreInfo->fuzzyHighlightMarkerPost = marker;
    	} else {
    		parseError << "The highlighter post marker is an empty string, so the engine will use the default marker";
    		return;
    	}
	}

    // responseContent is an optional field
    coreInfo->searchResponseContent = (ResponseType)0; // by default it is 0
    childNode = queryNode.child(queryResponseWriterString).child(responseContentString);
    if (childNode) {
        string type = childNode.attribute(typeString).value();
        if (isValidResponseContentType(type)) {
            coreInfo->searchResponseContent = (ResponseType)childNode.attribute(typeString).as_int();
        } else {
            parseError << "The type provided for responseContent is not valid";
            configSuccess = false;
            return;
        }

        if (coreInfo->searchResponseContent == 2) {
            if (childNode.text()) {
                splitString(string(childNode.text().get()), ",", coreInfo->attributesToReturn);
            } else {
                parseError << "For specified response content type, return fields should be provided.";
                configSuccess = false;
                return;
            }
        }
    }

    // maxSearchThreads used to be contained in <query> so warn if we find it here
    childNode = queryNode.child(maxSearchThreadsString);
    if (childNode && childNode.text()) {
        Logger::warn("maxSearchThreads is no longer in <query>.  Move it under <config>");
    }
}

/*
 * Only called by parseMultipleCores().  This function is specific to parsing the <core> node defining
 * a single core (data source).  However, it doesn't do much itself.  It relies on parseDataFieldSettings() to
 * parse most of the values, including schema, because those specifications can occur under <config>
 * directly as well as under <core>.
 */
void ConfigManager::parseSingleCore(const xml_node &parentNode, CoreInfo_t *coreInfo, bool &configSuccess, std::stringstream &parseError, std::stringstream &parseWarnings)
{

    string temporaryString = "";

    if (parentNode.attribute(nameString) && string(parentNode.attribute(nameString).value()).compare("") != 0) {
        coreInfo->name = parentNode.attribute(nameString).value();
    } else {
        parseError << "Core must have a name attribute";
        configSuccess = false;
        return;
    }

    xml_node childNode = parentNode.child(coreIdTag);
    if(childNode && childNode.text()){
        string temp = string(childNode.text().get());
        trimSpacesFromValue(temp, coreIdTag, parseWarnings);
        coreInfo->setCoreId((uint)atol(temp.c_str()));
    }else{
        // TODO: to be deleted in V1
        Logger::console("!!!!!CoreId is not provided in core %s, engine will use the default value!!!!!", coreInfo->name.c_str());
        coreInfo->setCoreId(defaultCoreId);
        defaultCoreId++;
    }

    // Solr compatability - dataDir can be an attribute: <core dataDir="core0/data"
    if (parentNode.attribute(dataDirString) && string(parentNode.attribute(dataDirString).value()).compare("") != 0) {
        coreInfo->dataDir = parentNode.attribute(dataDirString).value();
        coreInfo->indexPath = srch2Home + coreInfo->dataDir;
    }

    parseCoreInformationTags(parentNode, coreInfo, configSuccess, parseError, parseWarnings);
    if (configSuccess == false) {
        return;
    }
}

// only called by parseDataConfiguration()
void ConfigManager::parseMultipleCores(const xml_node &coresNode, bool &configSuccess, std::stringstream &parseError, std::stringstream &parseWarnings)
{
	defaultCoreId = 0;
    if (coresNode) {

        // <cores defaultCoreName = "foo">
        if (coresNode.attribute(defaultCoreNameString) && string(coresNode.attribute(defaultCoreNameString).value()).compare("") != 0) {
            defaultCoreName = coresNode.attribute(defaultCoreNameString).value();
            defaultCoreSetFlag = true;
        } else {
            parseWarnings << "Cores defaultCoreName not set <cores defaultCoreName=...>";
        }

        // parse zero or more individual core settings
        for (xml_node coreNode = coresNode.first_child(); coreNode; coreNode = coreNode.next_sibling()) {
            CoreInfo_t *newCore = new CoreInfo_t(this);
            parseSingleCore(coreNode, newCore, configSuccess, parseError, parseWarnings);
    	    srch2is::Schema *schema = JSONRecordParser::createAndPopulateSchema(newCore);
    	    newCore->setSchema(schema);
            if (configSuccess) {
                clusterCores.push_back(newCore);
            } else {
                delete newCore;
                return;
            }
        }
    }
}

/*
 * parentNode is either <config> or <core>.  parseDataFieldSettings() is responsible for loading the settings
 * from either <config> or <core>, but only those settings that are common to both.
 * parseDataConfiguration() calls this with <config> and parseSingleCore() calls it with <core>.
 */
void ConfigManager::parseCoreInformationTags(const xml_node &parentNode, CoreInfo_t *coreInfo, bool &configSuccess, std::stringstream &parseError, std::stringstream &parseWarnings)
{
    string temporaryString = "";
    CoreConfigParseState_t coreParseState;
    // <config><dataDir>core0/data OR <core><dataDir>

    xml_node multipleDataFile = parentNode.child("multiple-data-file");
    if(multipleDataFile && multipleDataFile.text()){
    	string paths = string(multipleDataFile.text().get());
    	vector<string> path;
		trimSpacesFromValue(paths, "multiple-data-file", parseWarnings);
		string delimiterComma = ",";
		this->splitString(paths,delimiterComma,path);
		for (int i = 0; i < path.size(); i++){
		    vector<string> temp;
			trimSpacesFromValue(path[i], "DataFilePaths", parseWarnings);
			string str = srch2Home + "/" + this->clusterNameStr + "/" +coreInfo->name + "/" + path[i];
			coreInfo->setJsonFilePaths(str);
		}
    }


    xml_node childNodeOfCores = parentNode.child(primaryShardTag);


    if(childNodeOfCores && childNodeOfCores.text()){
    	string temp = (childNodeOfCores.text().get());
    	trimSpacesFromValue(temp, primaryShardTag, parseWarnings);
    	coreInfo->numberOfPrimaryShards = (uint)atol(temp.c_str());
    }
    else{
    	coreInfo->numberOfPrimaryShards = DefaultNumberOfPrimaryShards;
        parseWarnings << "Number of primary shards is not defined. The engine will use the default value " << coreInfo->numberOfPrimaryShards << "\n";
    }

    childNodeOfCores = parentNode.child(primaryShardTag);
    xml_node primaryShardSibling = childNodeOfCores.next_sibling(primaryShardTag);
    if(primaryShardSibling){
        parseWarnings << "Duplicate definition of \"" << primaryShardTag << "\".  The engine will use the first value " << coreInfo->numberOfPrimaryShards << "\n";
    }

    childNodeOfCores = parentNode.child(replicaShardTag);

    if(childNodeOfCores && childNodeOfCores.text()){
    	string temp = (childNodeOfCores.text().get());
    	trimSpacesFromValue(temp, replicaShardTag, parseWarnings);
    	coreInfo->numberOfReplicas = (uint)atol(temp.c_str());
    }
    else{
    	coreInfo->numberOfReplicas = DefaultNumberOfReplicas;
    	parseWarnings << "Number of replicas is not defined. The engine will use the default value " << DefaultNumberOfReplicas << "\n";
    }

    xml_node replicaSibling = childNodeOfCores.next_sibling(replicaShardTag);
    if(replicaSibling){
    	parseWarnings << "Duplicate definition of \"" << replicaShardTag << "\".  The engine will use the first value " << coreInfo->numberOfReplicas << "\n";
    }

    xml_node childNode = parentNode.child(dataDirString);
    if (childNode && childNode.text()) {
        coreInfo->dataDir = string(childNode.text().get());
        coreInfo->indexPath = srch2Home + coreInfo->dataDir;
    }

    if (coreInfo->dataDir.length() == 0) {
        parseWarnings << "Core " << coreInfo->name.c_str() << " has null dataDir\n";
    }

    childNode = parentNode.child(dataSourceTypeString);
    if (childNode && childNode.text()) {
        int dataSourceValue = childNode.text().as_int(DATA_SOURCE_JSON_FILE);
        switch(dataSourceValue) {
        case 0:
            coreInfo->dataSourceType = DATA_SOURCE_NOT_SPECIFIED;
            break;
        case 1:
            coreInfo->dataSourceType = DATA_SOURCE_JSON_FILE;
            break;
        case 2:
            coreInfo->dataSourceType = DATA_SOURCE_MONGO_DB;
            break;
        default:
            // if user forgets to specify this option, we will assume data source is JSON file
            coreInfo->dataSourceType = DATA_SOURCE_JSON_FILE;
            break;
        }
    } else {
        coreInfo->dataSourceType = DATA_SOURCE_JSON_FILE;
    }

    if (coreInfo->dataSourceType == DATA_SOURCE_JSON_FILE) {
        // dataFile is a required field only if JSON file is specified as data source.
        childNode = parentNode.child(dataFileString);
        if (childNode && childNode.text()) { // checks if the config/dataFile has any text in it or not
            temporaryString = string(childNode.text().get());
            trimSpacesFromValue(temporaryString, dataFileString, parseWarnings);
            coreInfo->dataFilePath = srch2Home + string("") + coreInfo->getName() + string("/") + temporaryString;
        } else {
            parseError << (coreInfo->name.compare("") != 0 ? coreInfo->name : "default") <<
                " core path to the data file is not set. "
                "You should set it as <dataFile>path/to/data/file</dataFile> in the config file.\n";
            configSuccess = false;
            return;
        }
    }

    // map of port type enums to strings to simplify code
    struct portNameMap_t {
        enum PortType_t portType;
        const char *portName;
    };
    static portNameMap_t portNameMap[] = {
        { SearchPort, searchPortString },
        { SuggestPort, suggestPortString },
        { InfoPort, infoPortString },
        { DocsPort, docsPortString },
        { UpdatePort, updatePortString },
        { SavePort, savePortString },
        { ExportPort, exportPortString },
        { ResetLoggerPort, resetLoggerPortString },
        { EndOfPortType, NULL }
    };

    for (unsigned int i = 0; portNameMap[i].portName != NULL; i++) {
        childNode = parentNode.child(portNameMap[i].portName);
        if (childNode && childNode.text()) { // checks if the config/port has any text in it or not
            int portValue = childNode.text().as_int();
            if (portValue <= 0 || portValue > USHRT_MAX) {
                parseError << portNameMap[i].portName << " must be between 1 and " << USHRT_MAX;
                configSuccess = false;
                return;
            }
            coreInfo->setPort(portNameMap[i].portType, portValue);
        }
    }

    if (coreInfo->dataSourceType == DATA_SOURCE_MONGO_DB) {
        childNode = parentNode.child(mongoDbString);
        parseMongoDb(childNode, coreInfo, configSuccess, parseError, parseWarnings);
        if (configSuccess == false) {
            return;
        }
    }

    // uniqueKey is required
    childNode = parentNode.child(schemaString).child(uniqueKeyString);
    if (childNode && childNode.text()) {
        coreInfo->primaryKey = string(childNode.text().get());
        // For MongoDB, the primary key should always be "_id".
        if(coreInfo->dataSourceType == DATA_SOURCE_MONGO_DB && coreInfo->primaryKey.compare("_id") != 0) {
        	parseError << "The PrimaryKey in the config file for the MongoDB adapter should always be \"_id\", not "
        				<< coreInfo->primaryKey << ".";
        	configSuccess = false;
        	return;
        }
    } else {
        parseError << (coreInfo->name.compare("") != 0 ? coreInfo->name : "default") <<
            " core uniqueKey (primary key) is not set.\n";
        configSuccess = false;
        return;
    }

    coreInfo->allowedRecordTokenizerCharacters = "";
    coreInfo->attributeToSort = 0;

    // set default number of suggestions because we don't have any config options for this yet
    coreInfo->defaultNumberOfSuggestions = 5;


    // <schema>
    childNode = parentNode.child(schemaString);
    if (childNode) {
        parseSchema(childNode, &coreParseState, coreInfo, configSuccess,
                parseError, parseWarnings);
        if (configSuccess == false) {
            return;
        }
    }

    xml_node indexConfigNode = parentNode.child(indexConfigString);
    map<string, unsigned> boostsMap;
    parseIndexConfig(indexConfigNode, coreInfo, boostsMap, configSuccess, parseError, parseWarnings);
    if (configSuccess == false) {
        return;
    }

    childNode = parentNode.child(queryString);
    if (childNode) {
        parseQuery(childNode, coreInfo, configSuccess, parseError, parseWarnings);
        if (configSuccess == false) {
            return;
        }
    }

    if (coreInfo->indexType == 1) {
        // If index type is 1, it means it is geo. So both latitude and longitude should be provided
        if (!(coreParseState.hasLatitude && coreParseState.hasLongitude)) {
            parseError << "Both Geo related attributes should set together. Currently only one of them is set.\n";
            configSuccess = false;
            return;
        }
        coreInfo->searchType = 2; // GEO search
    } else if (coreInfo->indexType == 0){
        coreInfo->searchType = 0;
        coreInfo->fieldLongitude = "IGNORE"; // IN URL parser these fields are being checked with "IGNORE". We should get rid of them.
        coreInfo->fieldLatitude = "IGNORE"; // IN URL parser these fields are being checked with "IGNORE". We should get rid of them.

        childNode = parentNode.child(queryString).child(searcherTypeString);
        if (childNode && childNode.text()) {
            string st = childNode.text().get();
            if (isValidSearcherType(st)) {
                coreInfo->searchType = childNode.text().as_int();
            } else {
                parseError << "The Searcher Type only can get 0 or 1";
                configSuccess = false;
                return;
            }
        }
    }

    // <config>
    //   <indexconfig>
    //        <fieldBoost>
    // filling the searchableAttributesInfo map
    // this depends upon parseIndexConfig() to have loaded boostsMap and
    // searchableFieldsVector & company to have been loaded elsewhere from <schema>
    for (int i = 0; i < coreParseState.searchableFieldsVector.size(); i++) {
        if (boostsMap.find(coreParseState.searchableFieldsVector[i]) == boostsMap.end()) {
            coreInfo->searchableAttributesInfo[coreParseState.searchableFieldsVector[i]] =
                SearchableAttributeInfoContainer(coreParseState.searchableFieldsVector[i] ,
                								 coreParseState.searchableFieldTypesVector[i],
                                                 coreParseState.searchableAttributesRequiredFlagVector[i] ,
                                                 coreParseState.searchableAttributesDefaultVector[i] ,
                                                 0 , 1 , coreParseState.searchableAttributesIsMultiValued[i],
                                                 coreParseState.searchableAttributesHighlight[i]);
        } else {
            coreInfo->searchableAttributesInfo[coreParseState.searchableFieldsVector[i]] =
                SearchableAttributeInfoContainer(coreParseState.searchableFieldsVector[i] ,
                								 coreParseState.searchableFieldTypesVector[i],
                                                 coreParseState.searchableAttributesRequiredFlagVector[i] ,
                                                 coreParseState.searchableAttributesDefaultVector[i] ,
                                                 0 , boostsMap[coreParseState.searchableFieldsVector[i]] ,
                                                 coreParseState.searchableAttributesIsMultiValued[i],
                                                 coreParseState.searchableAttributesHighlight[i]);
        }
    }

    // give each searchable attribute an id based on the order in the info map
    // should be consistent with the id in the schema
    map<string, SearchableAttributeInfoContainer>::iterator searchableAttributeIter = coreInfo->searchableAttributesInfo.begin();
    for(unsigned idIter = 0; searchableAttributeIter != coreInfo->searchableAttributesInfo.end() ; ++searchableAttributeIter, ++idIter){
        searchableAttributeIter->second.offset = idIter;
    }

    // checks the validity of the boost fields in boostsMap
    // must occur after parseIndexConfig() AND parseSchema()
    if (!isValidBoostFields(coreInfo, boostsMap)) {
        configSuccess = false;
        parseError << "In core " << coreInfo->name << ": Fields that are provided in the boostField do not match with the defined fields.";
    
        return;
    }

    // checks the validity of the boost values in boostsMap
    if (!isValidBoostFieldValues(boostsMap)) {
        configSuccess = false;
        parseError << "Boost values that are provided in the boostField are not in the range [1 to 100].";
        return;
    }

    // <config><updateHandler> OR <core><updateHandler>
    childNode = parentNode.child(updateHandlerString);
    if (childNode) {
        parseUpdateHandler(childNode, coreInfo, configSuccess, parseError, parseWarnings);
        if (configSuccess == false) {
            return;
        }
    }
}

void ConfigManager::parseAllCoreTags(const xml_node &configNode,
                                           bool &configSuccess,
                                           std::stringstream &parseError,
                                           std::stringstream &parseWarnings)
{

	// first try to populate the default core object
    // check for data source coreInfo outside of <cores>
    if (configNode.child(dataFileString) || configNode.child(dataDirString) || configNode.child(dataSourceTypeString)) {

        // create a default core for coreInfo outside of <cores>
		CoreInfo_t * defaultCoreInfo = new CoreInfo_t(this);
		defaultCoreInfo->name = getDefaultCoreName();
		clusterCores.push_back(defaultCoreInfo);
        parseCoreInformationTags(configNode, defaultCoreInfo, configSuccess, parseError, parseWarnings);
	    srch2is::Schema *schema = JSONRecordParser::createAndPopulateSchema(defaultCoreInfo);
	    defaultCoreInfo->setSchema(schema);
        if (configSuccess == false) {
			delete defaultCoreInfo;
            return;
        }
    }

    // if there is cores tag, we must add more core objects ....
    // <cores>
    xml_node childNode = configNode.child(multipleCoresString);
    if (childNode) {
        parseMultipleCores(childNode, configSuccess, parseError, parseWarnings);
        if (configSuccess == false) {
            return;
        }
    }
}

bool ConfigManager::setCoreParseStateVector(bool isSearchable, bool isRefining, bool isMultiValued, bool isHighlightEnabled, CoreConfigParseState_t *coreParseState, CoreInfo_t *coreInfo, std::stringstream &parseError, const xml_node &field){
	string temporaryString = "";
	if(isSearchable){ // it is a searchable field
		coreParseState->searchableFieldsVector.push_back(string(field.attribute(nameString).value()));
		coreParseState->searchableAttributesHighlight.push_back(isHighlightEnabled);
		// Checking the validity of field type
		temporaryString = string(field.attribute(typeString).value());
		if (isValidFieldType(temporaryString , true)) {
			coreParseState->searchableFieldTypesVector.push_back(parseFieldType(temporaryString));
		} else {
			parseError << "Config File Error: " << temporaryString << " is not a valid field type for searchable fields.\n";
			parseError << " Note: searchable fields only accept 'text' type. Setting 'searchable' or 'indexed' to true makes a field searchable.\n";
			return false;
		}

		if (string(field.attribute(defaultString).value()).compare("") != 0){
			coreParseState->searchableAttributesDefaultVector.push_back(string(field.attribute(defaultString).value()));
		}else{
			coreParseState->searchableAttributesDefaultVector.push_back("");
		}

		temporaryString = string(field.attribute(requiredString).value());
		if (string(field.attribute(requiredString).value()).compare("") != 0 && isValidBool(temporaryString)){
			coreParseState->searchableAttributesRequiredFlagVector.push_back(field.attribute(requiredString).as_bool());
		}else{
			coreParseState->searchableAttributesRequiredFlagVector.push_back(false);
		}
		coreParseState->searchableAttributesIsMultiValued.push_back(isMultiValued);
	}
	return true;
}

bool ConfigManager::setRefiningStateVectors(const xml_node &field, bool isMultiValued, bool isRefining, vector<string> &RefiningFieldsVector, vector<srch2::instantsearch::FilterType> &RefiningFieldTypesVector, vector<bool> &RefiningAttributesRequiredFlagVector, vector<string> &RefiningAttributesDefaultVector, vector<bool> &RefiningAttributesIsMultiValued, std::stringstream &parseError){

	string temporaryString = "";
	if(isRefining){ // it is a refining field
		RefiningFieldsVector.push_back(string(field.attribute(nameString).value()));
		// Checking the validity of field type
		temporaryString = string(field.attribute(typeString).value());
		if (this->isValidFieldType(temporaryString , false)) {
			RefiningFieldTypesVector.push_back(parseFieldType(temporaryString));
		} else {
			parseError << "Config File Error: " << temporaryString << " is not a valid field type for refining fields.\n";
			parseError << " Note: refining fields only accept 'text', 'integer',"
			                            " 'long', 'float', 'double' and 'time'. Setting 'refining' "
			                            "or 'indexed' to true makes a field refining.\n";
			return false;
		}

		// Check the validity of field default value based on it's type
		if (string(field.attribute(defaultString).value()).compare("") != 0){
			temporaryString = string(field.attribute("default").value());
			if(isValidFieldDefaultValue(temporaryString , RefiningFieldTypesVector.at(RefiningFieldTypesVector.size()-1) , isMultiValued)){

				if(RefiningFieldTypesVector.at(RefiningFieldTypesVector.size()-1) == srch2::instantsearch::ATTRIBUTE_TYPE_TIME){
					if(isMultiValued == false){
						long timeValue = srch2is::DateAndTimeHandler::convertDateTimeStringToSecondsFromEpoch(temporaryString);
						std::stringstream buffer;
						buffer << timeValue;
						temporaryString = buffer.str();
					}else{ // in the case of multivalued date and time we need to convert all values and reconstruct the list
						// For example: ["01/01/1980","01/01/1980","01/01/1990","01/01/1982"]
						                 string convertedDefaultValues = "";
						                 vector<string> defaultValueTokens;
						                 splitString(temporaryString , "," , defaultValueTokens);
						                 for(vector<string>::iterator defaultValueToken = defaultValueTokens.begin() ;
						                		 defaultValueToken != defaultValueTokens.end() ; ++defaultValueToken){
						                	 long timeValue = srch2is::DateAndTimeHandler::convertDateTimeStringToSecondsFromEpoch(*defaultValueToken);
						                	 std::stringstream buffer;
						                	 buffer << timeValue;
						                	 if(defaultValueToken == defaultValueTokens.begin()){
						                		 convertedDefaultValues = buffer.str();
						                	 }else{
						                		 convertedDefaultValues = ","+buffer.str();
						                	 }
						                 }
					}
				}
			}else{
				parseError << "Config File Error: " << temporaryString << " is not compatible with the type used for this field.\n";
				temporaryString = "";
			}
			RefiningAttributesDefaultVector.push_back(temporaryString);
		}else{
			RefiningAttributesDefaultVector.push_back("");
		}

		temporaryString = string(field.attribute(requiredString).value());
		if (string(field.attribute(requiredString).value()).compare("") != 0 && isValidBool(temporaryString)){
			RefiningAttributesRequiredFlagVector.push_back(field.attribute("required").as_bool());
		}else{
			RefiningAttributesRequiredFlagVector.push_back(false);
		}

		RefiningAttributesIsMultiValued.push_back(isMultiValued);
	}

	return true;
}

void ConfigManager::parseFacetFields(const xml_node &schemaNode, CoreInfo_t *coreInfo, std::stringstream &parseError){

	xml_node childNode;
	string temporaryString;
	if(coreInfo->facetEnabled){
		childNode = schemaNode.child(facetFieldsString);
		if (childNode) {
			for (xml_node field = childNode.first_child(); field; field = field.next_sibling()) {
				if (string(field.name()).compare(facetFieldString) == 0) {
					if (string(field.attribute(nameString).value()).compare("") != 0
							&& string(field.attribute(facetTypeString).value()).compare("") != 0){

						// insert the name of the facet
						coreInfo->facetAttributes.push_back(string(field.attribute(nameString).value()));
						// insert the type of the facet
						temporaryString = string(field.attribute(facetTypeString).value());
						int facetType = parseFacetType(temporaryString);
						if(facetType == 0){ // categorical
							coreInfo->facetTypes.push_back(facetType);
							// insert place holders for start,end and gap
							coreInfo->facetStarts.push_back("");
							coreInfo->facetEnds.push_back("");
							coreInfo->facetGaps.push_back("");
						}else if(facetType == 1){ // range
							coreInfo->facetTypes.push_back(facetType);
							// insert start
							string startTextValue = string(field.attribute(facetStartString).value());
							string facetAttributeString = string(field.attribute(nameString).value());
							srch2::instantsearch::FilterType facetAttributeType ;
							if(coreInfo->refiningAttributesInfo.find(facetAttributeString) != coreInfo->refiningAttributesInfo.end()){
								facetAttributeType = coreInfo->refiningAttributesInfo.find(facetAttributeString)->second.attributeType;
							}else{
								parseError << "Facet attribute is not declared as a non-searchable attribute. Facet disabled.\n";
								coreInfo->facetEnabled = false;
								break;
							}
							if(facetAttributeType == srch2is::ATTRIBUTE_TYPE_TIME){
								if(srch2is::DateAndTimeHandler::verifyDateTimeString(startTextValue , srch2is::DateTimeTypePointOfTime)
								|| srch2is::DateAndTimeHandler::verifyDateTimeString(startTextValue , srch2is::DateTimeTypeNow) ){
									long timeValue = srch2is::DateAndTimeHandler::convertDateTimeStringToSecondsFromEpoch(startTextValue);
									std::stringstream buffer;
									buffer << timeValue;
									startTextValue = buffer.str();
								}else{
									parseError << "Facet attribute start value is in wrong format.Facet disabled.\n";
									coreInfo->facetEnabled = false;
									break;
								}
							}
							coreInfo->facetStarts.push_back(startTextValue);

							// insert end
							string endTextValue = string(field.attribute(facetEndString).value());
							if(coreInfo->refiningAttributesInfo.find(facetAttributeString) != coreInfo->refiningAttributesInfo.end()){
								facetAttributeType = coreInfo->refiningAttributesInfo.find(facetAttributeString)->second.attributeType;
							}else{
								parseError << "Facet attribute is not declared as a non-searchable attribute. Facet disabled.\n";
								coreInfo->facetEnabled = false;
								break;
							}
							if(facetAttributeType == srch2is::ATTRIBUTE_TYPE_TIME){
								if(srch2is::DateAndTimeHandler::verifyDateTimeString(endTextValue , srch2is::DateTimeTypePointOfTime)
								|| srch2is::DateAndTimeHandler::verifyDateTimeString(endTextValue , srch2is::DateTimeTypeNow) ){
									long timeValue = srch2is::DateAndTimeHandler::convertDateTimeStringToSecondsFromEpoch(endTextValue);
									std::stringstream buffer;
									buffer << timeValue;
									endTextValue = buffer.str();
								}else{
									parseError << "Facet attribute start value is in wrong format.Facet disabled.\n";
									coreInfo->facetEnabled = false;
									break;
								}
							}
							coreInfo->facetEnds.push_back(endTextValue);

							// insert gap
							string gapTextValue = string(field.attribute(facetGapString).value());
							if(coreInfo->refiningAttributesInfo.find(facetAttributeString) != coreInfo->refiningAttributesInfo.end()){
								facetAttributeType = coreInfo->refiningAttributesInfo.find(facetAttributeString)->second.attributeType;
							}else{
								parseError << "Facet attribute is not declared as a non-searchable attribute. Facet disabled.\n";
								coreInfo->facetEnabled = false;
								break;
							}
							if(facetAttributeType == srch2is::ATTRIBUTE_TYPE_TIME){
								if(!srch2is::DateAndTimeHandler::verifyDateTimeString(gapTextValue , srch2is::DateTimeTypeDurationOfTime) ){
									parseError << "Facet attribute end value is in wrong format.Facet disabled.\n";
									coreInfo->facetEnabled = false;
									break;
								}
							}
							coreInfo->facetGaps.push_back(gapTextValue);
						}else{
							parseError << "Facet type is not recognized. Facet disabled.";
							coreInfo->facetEnabled = false;
							break;
						}

					}
				}
			}
		}
	}

    if(! coreInfo->facetEnabled){
        coreInfo->facetAttributes.clear();
        coreInfo->facetTypes.clear();
        coreInfo->facetStarts.clear();
        coreInfo->facetEnds.clear();
        coreInfo->facetGaps.clear();
    }

}

void ConfigManager::parseSchemaType(const xml_node &childNode, CoreInfo_t *coreInfo, std::stringstream &parseWarnings){

	string temporaryString = "";
	if (childNode) {        // Checks if <schema><types> exists or not
		for (xml_node fieldType = childNode.first_child(); fieldType; fieldType = fieldType.next_sibling()) { // Going on the children
			if ((string(fieldType.name()).compare(fieldTypeString) == 0)) { // Finds the fieldTypes
				if (string(fieldType.attribute(nameString).value()).compare(textEnString) == 0) {
					// Checking if the values are empty or not
					xml_node childNodeTemp = fieldType.child(analyzerString); // looks for analyzer
					for (xml_node field = childNodeTemp.first_child(); field; field = field.next_sibling()) {
						if (string(field.name()).compare(filterString) == 0) {
							if (string(field.attribute(nameString).value()).compare(porterStemFilterString) == 0) { // STEMMER FILTER
								if (string(field.attribute(dictionaryString).value()).compare("") != 0) { // the dictionary for porter stemmer is set.
									coreInfo->stemmerFlag = true;
									temporaryString = string(field.attribute(dictionaryString).value());
									trimSpacesFromValue(temporaryString, porterStemFilterString, parseWarnings);
									coreInfo->stemmerFile = boost::filesystem::path(this->srch2Home + temporaryString).normalize().string();
								}else{
									Logger::warn("Dictionary file is not set for PorterStemFilter, so stemming is disabled");
								}
							} else if (string(field.attribute(nameString).value()).compare(stopFilterString) == 0) { // STOP FILTER
								if (string(field.attribute(wordsString).value()).compare("") != 0) { // the words file for stop filter is set.
									temporaryString = string(field.attribute(wordsString).value());
									trimSpacesFromValue(temporaryString, stopFilterString, parseWarnings);
									coreInfo->stopFilterFilePath = boost::filesystem::path(srch2Home + temporaryString).normalize().string();
								}else{
									Logger::warn("word parameter in StopFilter is empty, so stop word filter is disabled");
								}
							}
							else if (string(field.attribute(nameString).value()).compare(protectedWordFilterString) == 0) {
								if (string(field.attribute(wordsString).value()).compare("") != 0) { // the file for protected words filter is set.
									temporaryString = string(field.attribute(wordsString).value());
									trimSpacesFromValue(temporaryString, protectedWordFilterString, parseWarnings);
									coreInfo->protectedWordsFilePath = boost::filesystem::path(srch2Home + temporaryString).normalize().string();
								}else{
									Logger::warn("words parameter for protected keywords is empty, so protected words filter is disabled");
								}
							} else if (string(field.attribute(nameString).value()).compare(synonymFilterString) == 0) {
								if (string(field.attribute(synonymsString).value()).compare("") != 0) { // the file for synonyms filter is set.
									temporaryString = string(field.attribute(synonymsString).value());
									trimSpacesFromValue(temporaryString, synonymsString, parseWarnings);
									coreInfo->synonymFilterFilePath = boost::filesystem::path(srch2Home + temporaryString).normalize().string();
								}else{
									Logger::warn("Synonym filter is disabled because synonym parameter is empty, ");
								}
								if (string(field.attribute(expandString).value()).compare("") != 0) {
									temporaryString = string(field.attribute(expandString).value());
									if (isValidBool(temporaryString)) {
										coreInfo->synonymKeepOrigFlag = field.attribute(expandString).as_bool(true);
									}
								}else{
									Logger::warn("Synonym filter's expand attribute is missing. Using default = true");
								}
							}

						} else if (string(field.name()).compare(allowedRecordSpecialCharactersString) == 0) {
							CharSet charTyper;
							string in = field.text().get(), out; // TODO: Using type string NOT multi-lingual?

							// validate allowed characters
							for (std::string::iterator iterator = in.begin(); iterator != in.end(); iterator++) {
								switch (charTyper.getCharacterType(*iterator)) {
								case CharSet::DELIMITER_TYPE:
									out += *iterator;
									break;
								case CharSet::LATIN_TYPE:
								case CharSet::BOPOMOFO_TYPE:
								case CharSet::HANZI_TYPE:
									Logger::warn("%s character %c already included in terms, ignored", allowedRecordSpecialCharactersString, *iterator);
									break;
								case CharSet::WHITESPACE:
									Logger::warn("%s character %c is whitespace and cannot be treated as part of a term, ignored", allowedRecordSpecialCharactersString, *iterator);
									break;
								default:
									Logger::warn("%s character %c of unexpected type %d, ignored", allowedRecordSpecialCharactersString, *iterator, static_cast<int> (charTyper.getCharacterType(*iterator)));
									break;
								}
							}

							coreInfo->allowedRecordTokenizerCharacters = out;
						}else{
							Logger::error("Valid tag is not set, it can only be filter or allowedrecordspecialcharacters");
						}
					}
				}else{
					Logger::error("Not a valid fieldType name in config file, currently we only support text_en");
				}
			}
		}
	} else {
		parseWarnings << "Analyzer Filters will be disabled.\n";
	}

}


void ConfigManager::parseSchema(const xml_node &schemaNode, CoreConfigParseState_t *coreParseState, CoreInfo_t *coreInfo, bool &configSuccess, std::stringstream &parseError, std::stringstream &parseWarnings)
{
	    string temporaryString = "";
	    vector<string> RefiningFieldsVector;
	    vector<srch2::instantsearch::FilterType> RefiningFieldTypesVector;
	    vector<bool> RefiningAttributesRequiredFlagVector;
	    vector<string> RefiningAttributesDefaultVector;
	    vector<bool> RefiningAttributesIsMultiValued;

	    /*
	     * <field>  in config.xml file
	     */
	    coreInfo->isPrimSearchable = 0;

	    xml_node fieldsNode = schemaNode.child(fieldsString);
	    if (fieldsNode) {
	        for (xml_node field = fieldsNode.first_child(); field; field = field.next_sibling()) {
	            if (string(field.name()).compare(fieldString) == 0) {

	            	bool isMultiValued = false;
	            	bool isSearchable = false;
	            	bool isRefining = false;
	                bool isHighlightEnabled = false;
	                if(!setFieldFlagsFromFile(field, isMultiValued, isSearchable, isRefining, isHighlightEnabled, parseError, configSuccess)){
	                	configSuccess = false;
	                	return;
	                }

	                //This code gets executed only if the field is primary key
	                if(string(field.attribute(nameString).value()).compare(coreInfo->primaryKey) == 0){

	                	if(isMultiValued){
	                		configSuccess = false;
	                		parseError << "Config File Error: Primary Key cannot be multivalued";
	                		return;
	                	}
	                    if (string(field.attribute(typeString).value()).compare(
	                            "text") != 0) {
	                        configSuccess = false;
	                        parseError
	                                << "Config File Error: Type of the primary key must be \"text\".\n";
	                        return;
	                    }

	                	if(isSearchable){
	                		coreInfo->isPrimSearchable = 1;
	                		coreParseState->searchableFieldsVector.push_back(string(field.attribute(nameString).value()));
	                		coreParseState->searchableFieldTypesVector.push_back(ATTRIBUTE_TYPE_TEXT);
	                		// there is no need for default value for primary key
	                		coreParseState->searchableAttributesDefaultVector.push_back("");
	                		// primary key is always required.
	                		coreParseState->searchableAttributesRequiredFlagVector.push_back(true);
	                		coreParseState->searchableAttributesHighlight.push_back(isHighlightEnabled);
	                		coreParseState->searchableAttributesIsMultiValued.push_back(isMultiValued);
	                	}

	                	if(isRefining){
	                		RefiningFieldsVector.push_back(coreInfo->primaryKey);
	                		RefiningFieldTypesVector.push_back(srch2::instantsearch::ATTRIBUTE_TYPE_TEXT);
	                		RefiningAttributesDefaultVector.push_back("");
	                		RefiningAttributesRequiredFlagVector.push_back(true);
	                		RefiningAttributesIsMultiValued.push_back(isMultiValued);
	                	}
	                	continue;
	                }

	                if (string(field.attribute(nameString).value()).compare("") != 0
	                                   && string(field.attribute(typeString).value()).compare("") != 0) {

	                	if(!setCoreParseStateVector( isSearchable,  isRefining,  isMultiValued,  isHighlightEnabled,  coreParseState,  coreInfo,  parseError, field)){
	                		configSuccess = false;
	                		return;
	                	}

	                	if(!setRefiningStateVectors(field, isMultiValued, isRefining, RefiningFieldsVector,
	                			RefiningFieldTypesVector, RefiningAttributesRequiredFlagVector, RefiningAttributesDefaultVector,
	                			RefiningAttributesIsMultiValued, parseError)){
	                		configSuccess = false;
	                		return;
	                	}

	                    // Checks for geo types. location_latitude and location_longitude are geo types
						string fieldType = field.attribute(typeString).value();
						string lowerCase = fieldType;
						std::transform(lowerCase.begin(), lowerCase.end(),
								lowerCase.begin(), ::tolower);
						if (lowerCase.compare(locationLatitudeString) == 0) {
	                        coreParseState->hasLatitude = true;
	                        coreInfo->fieldLatitude = string(field.attribute(nameString).value());
	                    }
						if (lowerCase.compare(locationLongitudeString) == 0) {
	                        coreParseState->hasLongitude = true;
	                        coreInfo->fieldLongitude = string(field.attribute(nameString).value());
	                    }
	                }else { // if one of the values of name, type or indexed is empty
	                    parseError << "For the searchable fields, "
	                               << "providing values for 'name' and 'type' is required\n ";
	                    configSuccess = false;
	                    return;
	                }

	            }else {
	                parseWarnings << "Unexpected XML node " << field.name() << " within <fields>";
	            }
	        }
	    }
	    else { // No searchable fields provided.
	    	parseError << "No fields are provided.\n";
	    	configSuccess = false;
	    	return;
	    }

	    // Checking if there is any field or not.
	        if (coreParseState->searchableFieldsVector.size() == 0) {
	            parseError << "No searchable fields are provided.\n";
	            configSuccess = false;
	            return;
	        }

	        if(RefiningFieldsVector.size() != 0){
	            for (unsigned iter = 0; iter < RefiningFieldsVector.size(); iter++) {

	                coreInfo->refiningAttributesInfo[RefiningFieldsVector[iter]] =
	                    RefiningAttributeInfoContainer(RefiningFieldsVector[iter] ,
	                                                   RefiningFieldTypesVector[iter] ,
	                                                   RefiningAttributesDefaultVector[iter] ,
	                                                   RefiningAttributesRequiredFlagVector[iter],
	                                                   RefiningAttributesIsMultiValued[iter]);
	            }
	        }

	        /*
	         * <facetEnabled>  in config.xml file
	         */
	        coreInfo->facetEnabled = false; // by default it is false
	        xml_node childNode = schemaNode.child(facetEnabledString);
	        if (childNode && childNode.text()) {
	            string qtmt = childNode.text().get();
	            if (isValidBool(qtmt)) {
	                coreInfo->facetEnabled = childNode.text().as_bool();
	            } else {
	                parseError << "The facetEnabled that is provided is not valid";
	                configSuccess = false;
	                return;
	            }
	        }

	        /*
	         * <facetFields>  in config.xml file
	         */

	        parseFacetFields(schemaNode, coreInfo, parseError);

	        // Analyzer flags : Everything is disabled by default.
	        coreInfo->stemmerFlag = false;
	        coreInfo->stemmerFile = "";
	        coreInfo->stopFilterFilePath = "";
	        coreInfo->synonymFilterFilePath = "";
	        coreInfo->protectedWordsFilePath = "";
	        coreInfo->synonymKeepOrigFlag = true;

	        childNode = schemaNode.child(typesString);

	        parseSchemaType(childNode, coreInfo, parseWarnings);

	        /*
	         * <Schema/>: End
	         */
}


bool ConfigManager::setSearchableRefiningFromIndexedAttribute(const xml_node &field, bool &isSearchable, bool &isRefining, std::stringstream &parseError, bool &configSuccess){

string temporaryString = "";
temporaryString = string(field.attribute(indexedString).value());
if(isValidBool(temporaryString)){
	if(field.attribute(indexedString).as_bool()){ // indexed = true
		isSearchable = true;
		isRefining = true;
	}else{ // indexed = false
		if(string(field.attribute(searchableString).value()).compare("") != 0){
			temporaryString = string(field.attribute(searchableString).value());
			if(isValidBool(temporaryString)){
				if(field.attribute(searchableString).as_bool()){
					isSearchable = true;
				}else{
					isSearchable = false;
				}
			}else{
				parseError << "Config File Error: Unknown value for property 'searchable'.\n";
				configSuccess = false;
				return false;
			}
		}

		if(string(field.attribute(refiningString).value()).compare("") != 0){
			temporaryString = string(field.attribute(refiningString).value());
			if(isValidBool(temporaryString)){
				if(field.attribute(refiningString).as_bool()){
					isRefining = true;
				}else{
					isRefining = false;
				}
			}else{
				parseError << "Config File Error: Unknown value for property 'refining'.\n";
				configSuccess = false;
				return false;
			}
		}
	}
}else{
	parseError << "Config File Error: Unknown value for property 'indexed'.\n";
	configSuccess = false;
	return false;
}

return true;
}

bool ConfigManager::setSearchableAndRefining(const xml_node &field, bool &isSearchable, bool &isRefining, std::stringstream &parseError, bool &configSuccess){

	string temporaryString = "";
	if(string(field.attribute(searchableString).value()).compare("") != 0){
		temporaryString = string(field.attribute(searchableString).value());
		if(isValidBool(temporaryString)){
			if(field.attribute(searchableString).as_bool()){
				isSearchable = true;
			}else{
				isSearchable = false;
			}
		}else{
			parseError << "Config File Error: Unknown value for property 'searchable'.\n";
			configSuccess = false;
			return false;
		}
	}

	if(string(field.attribute(refiningString).value()).compare("") != 0){
		temporaryString = string(field.attribute(refiningString).value());
		if(isValidBool(temporaryString)){
			if(field.attribute(refiningString).as_bool()){
				isRefining = true;
			}else{
				isRefining = false;
			}
		}else{
			parseError << "Config File Error: Unknown value for property 'refining'.\n";
			configSuccess = false;
			return false;
		}
	}
	return true;
}

//bool ConfigManager::setCoreParseState()

bool ConfigManager::setFieldFlagsFromFile(const xml_node &field, bool &isMultiValued, bool &isSearchable, bool &isRefining, bool &isHighlightEnabled, std::stringstream &parseError, bool &configSuccess){
	string temporaryString = "";
	if(string(field.attribute(multiValuedString).value()).compare("") != 0){
		temporaryString = string(field.attribute(multiValuedString).value());
	    if(isValidBool(temporaryString)){
	    	isMultiValued = field.attribute(multiValuedString).as_bool();
	    }else{
            parseError << "Config File Error: Unknown value for property '"<< multiValuedString <<"'.\n";
	    	return false;
	    }
	}

        if(string(field.attribute(indexedString).value()).compare("") != 0){
			if(!setSearchableRefiningFromIndexedAttribute(field,  isSearchable,  isRefining,  parseError,  configSuccess)){
				configSuccess = false;
				return false;
			}
        }else{
        	if(!setSearchableAndRefining(field,  isSearchable,  isRefining,  parseError,  configSuccess)){
        		configSuccess = false;
        		return false;
        	}
        }

        //set highlight flags
        if(string(field.attribute(highLightString).value()).compare("") != 0){
        	temporaryString = string(field.attribute(highLightString).value());
        	if (isValidBool(temporaryString)){
        		if(field.attribute(highLightString).as_bool()){
        			isHighlightEnabled = true;
        		}
        	}
        }
        return true;
	}


void ConfigManager::parseUpdateHandler(const xml_node &updateHandlerNode, CoreInfo_t *coreInfo, bool &configSuccess, std::stringstream &parseError, std::stringstream &parseWarnings)
{
    string temporaryString = "";

    //By default the maximum number of document is 15000000.
    xml_node childNode = updateHandlerNode.child(maxDocsString);
    bool mdflag = false;
    coreInfo->documentLimit = 15000000;
    if (childNode && childNode.text()) {
        string md = childNode.text().get();
        if (this->isValidMaxDoc(md)) {
            coreInfo->documentLimit = childNode.text().as_uint();
            mdflag = true;
        }
    }
    if (!mdflag) {
    	 Logger::warn("MaxDoc is not set, so the engine will use the default value 15,000,000");
    }

    coreInfo->memoryLimit = 100000;
    bool mmflag = false;
    childNode = updateHandlerNode.child(maxMemoryString);
    if (childNode && childNode.text()) {
        string mm = childNode.text().get();
        if (this->isValidMaxMemory(mm)) {
            coreInfo->memoryLimit = childNode.text().as_uint();
            mmflag = true;
        }
    }
    if (!mmflag) {
    	Logger::warn("Maximum memory limit is not set, so the engine will use the default value 1GB");
    }

    // mergeEveryNSeconds
    //If the tag is not set the engine will assume default value of 1
    childNode = updateHandlerNode.child(mergePolicyString).child(mergeEveryNSecondsString);
    coreInfo->mergeEveryNSeconds = 10;
    bool mensflag = false;
    if (childNode && childNode.text()) {
        string mens = childNode.text().get();
        if (this->isValidMergeEveryNSeconds(mens)) {
            coreInfo->mergeEveryNSeconds = childNode.text().as_uint();
            mensflag = true;
        }
    }
    if (!mensflag) {
    	Logger::warn("mergeEveryNSeconds is not set correctly, so the engine will use the default value 10");
    }

    // mergeEveryMWrites
    //If the tag is not set the engine will assume default value of 1
    childNode = updateHandlerNode.child(mergePolicyString).child(mergeEveryMWritesString);
    coreInfo->mergeEveryMWrites = 100;
    bool memwflag = false;
    if (childNode && childNode.text()) {
        string memw = childNode.text().get();

        if (this->isValidMergeEveryMWrites(memw)) {
            coreInfo->mergeEveryMWrites = childNode.text().as_uint();
            memwflag = true;
        }
    }
    if (!memwflag) {
    	Logger::warn("mergeEveryMWrites is not set correctly, so the engine will use the default value 100");
    }

    // set default value for updateHistogramEveryPSeconds and updateHistogramEveryQWrites because there
    // is no option in xml for this one yet
    float updateHistogramWorkRatioOverTime = 0.1; // 10 percent of background thread process is spent for updating histogram
    coreInfo->updateHistogramEveryPMerges = (unsigned)
        ( 1.0 / updateHistogramWorkRatioOverTime) ; // updateHistogramEvery 10 Merges
    coreInfo->updateHistogramEveryQWrites =
        (unsigned)((coreInfo->mergeEveryMWrites * 1.0 ) / updateHistogramWorkRatioOverTime); // 10000 for mergeEvery 1000 Writes

    // TODO - logging per core
    // logLevel is optional. To make loglevel optional the llflag's initial value has been set to false.
    // llflag is false, if log level is not set in config file or wrong value is given by the user, otherwise llflag remains true.
    this->loglevel = Logger::SRCH2_LOG_INFO;
    childNode = updateHandlerNode.child(updateLogString).child(logLevelString);
    bool llflag = false;
    if (childNode && childNode.text()) {
        string ll = childNode.text().get();
        if (this->isValidLogLevel(ll)) {
            this->loglevel = static_cast<Logger::LogLevel>(childNode.text().as_int());
            llflag = true;
        } else {
            llflag = false;
        }
    }
    if (!llflag) {
        Logger::warn("Log Level is either not set or not set correctly, so the engine will use the"
                        " default value 3");
    }

    // accessLogFile is required
    childNode = updateHandlerNode.child(updateLogString).child(accessLogFileString);
    if (childNode && childNode.text()) {
        temporaryString = string(childNode.text().get());
        trimSpacesFromValue(temporaryString, updateLogString, parseWarnings);
        this->httpServerAccessLogFile = this->srch2Home + "/" + coreInfo->getName() + "/" + temporaryString;
    } else {
        parseError << "httpServerAccessLogFile is not set.\n";
        configSuccess = false;
        return;
    }
}

bool ConfigManager::isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}


bool checkValidity(string &parameter){

	for(int i = 0; i< parameter.length(); i++){
		if(!std::isalnum(parameter[i])){
			return false;
		}
	}
	return true;
}

void ConfigManager::parse(const pugi::xml_document& configDoc,
                          bool &configSuccess,
                          std::stringstream &parseError,
                          std::stringstream &parseWarnings)
{
    string temporaryString = ""; // This is just for temporary use.
    vector<string> ipAddressOfKnownHost;

    int flag_cluster = 0;

    xml_node configNode = configDoc.child(configString);

    xml_node wellKnownHost = configNode.child(wellKnownHosts);
    if(wellKnownHost && wellKnownHost.text()){

    	temporaryString = string(wellKnownHost.text().get());
		trimSpacesFromValue(temporaryString, "WellKnownHosts", parseWarnings);
    	string delimiterComma = ",";
    	this->splitString(temporaryString,delimiterComma,ipAddressOfKnownHost);

    	for (int i = 0; i < ipAddressOfKnownHost.size(); i++){
    	    vector<string> temp;
    		trimSpacesFromValue(ipAddressOfKnownHost[i], "WellKnownHosts", parseWarnings);
    		this->splitString(ipAddressOfKnownHost[i], ":", temp);
    		if (temp.size() > 1) {
    			this->setWellKnownHost(pair<string, unsigned>(temp[0],(uint)atol(temp[1].c_str())));
    		}
    		else if (temp.size() == 1) {
    			this->setWellKnownHost(pair<string, unsigned>(temp[0], -1));
    		}
    	}
    }

    xml_node transportNode = configNode.child(transportNodeTag);
    if(transportNode){
    	xml_node transportIpAddressNode = transportNode.child(transportIpAddress);
    	if(transportIpAddressNode && transportIpAddressNode.text()){
    		temporaryString = string(transportIpAddressNode.text().get());
    		trimSpacesFromValue(temporaryString, "transport-IpAddress", parseWarnings);
    		transport.setIpAddress(temporaryString);
    	}

    	 xml_node transportPortNode = transportNode.child(transportPort);
    	 if(transportPortNode && transportPortNode.text()){
    		 temporaryString = string(transportPortNode.text().get());
    	   	trimSpacesFromValue(temporaryString, "transport-port", parseWarnings);
    	   	if(isNumber(temporaryString))
    	   		transport.setPort((uint)atol(temporaryString.c_str()));
    	   	else
    	   		parseWarnings << "port number specified within transport is not valid, engine will use the default value 8088";
    	 }

    }

    xml_node multicastDiscoveryNode = configNode.child(multicastDiscovery);
    if(multicastDiscoveryNode){
    	xml_node groupAddressNode = multicastDiscoveryNode.child(multicastGroupAddress);
    	if(groupAddressNode && groupAddressNode.text()){
    		temporaryString = string(groupAddressNode.text().get());
    		trimSpacesFromValue(temporaryString, "group-address", parseWarnings);
    		mDiscovery.setGroupAddress(temporaryString);
    	}

        xml_node multicastIpAddressNode = multicastDiscoveryNode.child(multicastIpAddress);
        if(multicastIpAddressNode && multicastIpAddressNode.text()){
        	temporaryString = string(multicastIpAddressNode.text().get());
       		trimSpacesFromValue(temporaryString, "multicast-IpAddress", parseWarnings);
       		mDiscovery.setIpAddress(temporaryString);
       	}

        xml_node multicastPortNode = multicastDiscoveryNode.child(multicastPort);
        if(multicastPortNode && multicastPortNode.text()){
        	temporaryString = string(multicastPortNode.text().get());
        	trimSpacesFromValue(temporaryString, "multicast-port", parseWarnings);
        	if(isNumber(temporaryString))
        		mDiscovery.setPort((uint)atol(temporaryString.c_str()));
        	else
        		parseWarnings << "port number specified within multicast is not valid, engine will use the default value 8088";
        }

        xml_node multicastTtlNode = multicastDiscoveryNode.child(multicastTtl);
        if(multicastTtlNode && multicastTtlNode.text()){
        	temporaryString = string(multicastTtlNode.text().get());
        	trimSpacesFromValue(temporaryString, "multicast-ttl", parseWarnings);
        	if(isNumber(temporaryString))
        		mDiscovery.setTtl((uint)atol(temporaryString.c_str()));
        	else
        		parseWarnings << "ttl specified within multicast is not valid, engine will use the default value of 5";
        }
    }



    xml_node pingNode = configNode.child(pingNodeTag);
    if(pingNode){
        xml_node pingInterval = pingNode.child(pingIntervalTag);
        if(pingInterval && pingInterval.text()){
        	temporaryString = string(pingInterval.text().get());
           trimSpacesFromValue(temporaryString, "pingInterval", parseWarnings);
           if(isNumber(temporaryString))
               ping.setPingInterval((uint)atol(temporaryString.c_str()));
           else{
        	   parseWarnings<<"Ping interval specified is not valid, engine will use the default value 1";
           }
        }

        xml_node pingTimeout = pingNode.child(pingTimeoutTag);
        if(pingTimeout && pingTimeout.text()){
        	temporaryString = string(pingTimeout.text().get());
            trimSpacesFromValue(temporaryString, "pingTimeout", parseWarnings);
            if(isNumber(temporaryString))
                ping.setPingTimeout((uint)atol(temporaryString.c_str()));
            else{
         	   parseWarnings<<"Ping timeout specified is not valid, engine will use the default value 1";
            }
        }

        xml_node retryCount = pingNode.child(retryCountTag);
        if(retryCount && retryCount.text()){
        	temporaryString = string(retryCount.text().get());
            trimSpacesFromValue(temporaryString, "retryCount", parseWarnings);
            if(isNumber(temporaryString))
                ping.setRetryCount((uint)atol(temporaryString.c_str()));
            else{
                parseWarnings<<"Retry count specified is not valid, engine will use the default value 1";
            }
         }
    }

    xml_node clusterName = configNode.child(clusterNameTag);
    if (clusterName && clusterName.text()) {
    	temporaryString = string(clusterName.text().get());
    	trimSpacesFromValue(temporaryString, clusterNameTag, parseWarnings);
    	if (temporaryString != "") {
    		this->clusterNameStr = temporaryString;
    	} else {
    		this->clusterNameStr = string(DefaultClusterName);
    		parseWarnings << "Cluster name is not specified. Engine will use the default value " << this->clusterNameStr << "\n";
    	}
    }else{
    	this->clusterNameStr = string(DefaultClusterName);
    	parseWarnings << "Cluster name is not specified. Engine will use the default value " << this->clusterNameStr << "\n";
    }

    xml_node clusterNameSibling = clusterName.next_sibling(clusterNameTag);
    if (clusterNameSibling && clusterNameSibling.text()){
          parseWarnings << "Duplicate definition of \"" << clusterNameTag << "\".  The engine will use the first value: " << this->clusterNameStr << "\n";    }

    temporaryString = "";

    std::string nodeName = "srch2-node";
    //  node-name -- optional
    xml_node xmlnodeNameTag = configNode.child(nodeNameTag);
    if (xmlnodeNameTag && xmlnodeNameTag.text()) { // checks if the config/node-name has any text in it or not
    	temporaryString = string(xmlnodeNameTag.text().get());
        trimSpacesFromValue(temporaryString, nodeNameTag, parseWarnings);
        nodeName = temporaryString;
    } else {
    	parseWarnings << "Node name is not defined in the config file. Using a default value = " << nodeName << endl;
    }
    this->nodeNameStr = nodeName;




    //  ports -- operation specific ports -- optional --- default listening port.

    // srch2Home is a required field
    xml_node childNode = configNode.child(srch2HomeString);
    if (childNode && childNode.text()) { // checks if the config/srch2Home has any text in it or not
        temporaryString = string(childNode.text().get());
        trimSpacesFromValue(temporaryString, srch2HomeString, parseWarnings, "/");
        srch2Home = temporaryString;
    } else {
        parseError << "srch2Home is not set.\n";
        configSuccess = false;
        return;
    }


	string authKey = "";
	//Check for authorization key
	xml_node authorizationNode = configNode.child(authorizationKeyTag);
	if(authorizationNode && authorizationNode.text()){
		authKey = string(authorizationNode.text().get());
		trimSpacesFromValue(authKey,authorizationKeyTag, parseWarnings);
		if(checkValidity(authKey)){
			ConfigManager::setAuthorizationKey(authKey);
		}else{
			parseWarnings << "Authorization Key is invalid string, so it will not be used by the engine! ";
			Logger::console("Authorization Key is invalid string, so it will not be used by the engine! ");
		}
	}

    // maxSearchThreads is an optional field
    numberOfThreads = 1; // by default it is 1
    childNode = configNode.child(maxSearchThreadsString);
    if (childNode && childNode.text()) {
        string mst = childNode.text().get();
        if (isValidMaxSearchThreads(mst)) {
            numberOfThreads = childNode.text().as_int();
        } else {
            parseError << "maxSearchThreads is not set correctly.\n";
            configSuccess = false;
            return;
        }
    }

    // parse all data source settings - no core (default core) and multiples core handled
    // requires indexType to have been loaded by parseIndexConfig()
    parseAllCoreTags(configNode, configSuccess, parseError, parseWarnings);
    if (configSuccess == false) {
        return;
    }

    CoreInfo_t * defaultCoreInfo = this->getCoreByName(getDefaultCoreName());
    if (defaultCoreInfo == NULL) {
        parseError << "Default core " << getDefaultCoreName() << " not found\n";
        configSuccess = false;
        return;
    }

    /*
     * <Config> in config.xml file
     */
    // licenseFile is a required field
    // Note: Due to freemium project, we are disabling license key check.
    //
    //    childNode = configNode.child(licenseFileString);
    //    if (childNode && childNode.text()) { // checks if config/licenseFile exists and have any text value or not
    //        tempUse = string(childNode.text().get());
    //        trimSpacesFromValue(tempUse, licenseFileString, parseWarnings);
    //        this->licenseKeyFile = this->srch2Home + tempUse;
    //    } else {
    //        parseError << "License key is not set.\n";
    //        configSuccess = false;
    //        return;
    //    }

    // listeningHostname is a required field
    childNode = configNode.child(listeningHostStringString);
    if (childNode && childNode.text()) { // checks if config/listeningHostname exists and have any text value or not
        this->httpServerListeningHostname = string(childNode.text().get());
    } else {
        parseError << "listeningHostname is not set.\n";
        configSuccess = false;
        return;
    }

    // listeningPort is a required field
    childNode = configNode.child(listeningPortString);
    if (childNode && childNode.text()) { // checks if the config/listeningPort has any text in it or not
        this->httpServerListeningPortStr = string(childNode.text().get());
        int value = static_cast<int>(strtol(httpServerListeningPortStr.c_str(),
                NULL, 10));
        if (value <= 0 || value > USHRT_MAX) {
            parseError << listeningPortString << " must be between 1 and " << USHRT_MAX;
            configSuccess = false;
            return;
        }

        // TODO : we must change the parsing code to parse all ports ...
        for(srch2http::PortType_t portType = (srch2http::PortType_t) 0;
    			portType < srch2http::EndOfPortType; portType = srch2http::incrementPortType(portType)){
        	httpServerListeningPorts[portType] = value;
        }
    } else {
        parseError << "listeningPort is not set.\n";
        configSuccess = false;
        return;
    }

    if (defaultCoreInfo->supportAttributeBasedSearch && defaultCoreInfo->searchableAttributesInfo.size() > 31) {
        parseError
            << "To support attribute-based search, the number of searchable attributes cannot be bigger than 31.\n";
        configSuccess = false;
        return;
    }

    // TODO - move to individual cores?
    this->ordering = 0;

    // setting default values for getAllResults optimization parameters
    // <getAllResultsMaxResultsThreshold>10000</getAllResultsMaxResultsThreshold>
    childNode = configNode.child(getAllResultsMaxResultsThreshold);
    if (childNode && childNode.text()) {
        string kpt = childNode.text().get();

        if (this->isValidGetAllResultsMaxResultsThreshold(kpt)) {
            this->getAllResultsNumberOfResultsThreshold = childNode.text().as_uint();
        }else{
            parseError << "getAllResultsMaxResultsThreshold has an invalid format. Use 10000 as its value.\n";
            this->getAllResultsNumberOfResultsThreshold = 10000;
        }
    }else{
        this->getAllResultsNumberOfResultsThreshold = 10000;
    }

    // <getAllResultsKAlternative>2000</getAllResultsKAlternative>
    childNode = configNode.child(getAllResultsKAlternative);
    if (childNode && childNode.text()) {
        string kpt = childNode.text().get();

        if (this->isValidGetAllResultsKAlternative(kpt)) {
            this->getAllResultsNumberOfResultsToFindInEstimationMode = childNode.text().as_uint();
        }else{
            parseError << "getAllResultsKAlternative has an invalid format. Use 2000 as its value.\n";
            this->getAllResultsNumberOfResultsToFindInEstimationMode = 2000;
        }
    }else{
        this->getAllResultsNumberOfResultsToFindInEstimationMode = 2000;
    }


    childNode = configNode.child(keywordPopularityThresholdString);
    if (childNode && childNode.text()) {
        string kpt = childNode.text().get();

        if (this->isValidKeywordPopularityThreshold(kpt)) {
            this->keywordPopularityThreshold = childNode.text().as_uint();
        }else{
            parseError << "keywordPopularityThreshold has an invalid format. Use 50000 as its value. \n";
            keywordPopularityThreshold = 50000;
        }
    }else{
        keywordPopularityThreshold = 50000;
    }
}



void ConfigManager::_setDefaultSearchableAttributeBoosts(const string &coreName, const vector<string> &searchableAttributesVector)
{
    CoreInfo_t *coreInfo = NULL;
    if (coreName.compare("") != 0) {
        coreInfo = this->getCoreByName(coreName);
    } else {
        coreInfo = this->getCoreByName(this->getDefaultCoreName());
    }

    for (unsigned iter = 0; iter < searchableAttributesVector.size(); iter++) {
        coreInfo->searchableAttributesInfo[searchableAttributesVector[iter]] =
            SearchableAttributeInfoContainer(searchableAttributesVector[iter] ,
            		srch2::instantsearch::ATTRIBUTE_TYPE_TEXT,
            		false, "" , iter , 1 , false);
    }
}

ConfigManager::~ConfigManager()
{
    for(unsigned coreIdx = 0 ; coreIdx < clusterCores.size() ; ++coreIdx){
    	if(clusterCores.at(coreIdx) != NULL){
    		delete clusterCores.at(coreIdx);
    	}
    }

    unlockNodeName();
}

unsigned ConfigManager::getKeywordPopularityThreshold() const {
    return keywordPopularityThreshold;
}

int ConfigManager::getIndexType(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getIndexType();

}

bool ConfigManager::getSupportSwapInEditDistance(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getSupportSwapInEditDistance();
}

const string& ConfigManager::getAttributeLatitude(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getFieldLatitude();
}

const string& ConfigManager::getAttributeLongitude(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getFieldLongitude();
}

float ConfigManager::getDefaultSpatialQueryBoundingBox() const {
    return defaultSpatialQueryBoundingBox;
}

unsigned int ConfigManager::getNumberOfThreads() const
{
    return numberOfThreads;
}

const string& ConfigManager::getIndexPath(const string &coreName) const {
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getIndexPath();
}

const string& ConfigManager::getPrimaryKey(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getPrimaryKey();
}

const map<string, SearchableAttributeInfoContainer > * ConfigManager::getSearchableAttributes(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getSearchableAttributes();
}

const map<string, RefiningAttributeInfoContainer > * ConfigManager::getRefiningAttributes(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getRefiningAttributes();
}

bool ConfigManager::isFacetEnabled(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->isFacetEnabled();
}

const vector<string> * ConfigManager::getFacetAttributes(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getFacetAttributes();
}

const vector<int> * ConfigManager::getFacetTypes(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getFacetTypes();
}

const vector<string> * ConfigManager::getFacetStarts(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getFacetStarts();
}

const vector<string> * ConfigManager::getFacetEnds(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getFacetEnds();
}

const vector<string> * ConfigManager::getFacetGaps(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getFacetGaps();
}


const string &ConfigManager::getSrch2Home() const {
    return srch2Home;
}

bool ConfigManager::getStemmerFlag(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getStemmerFlag();
}

const string &ConfigManager::getStemmerFile(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getStemmerFile();
}

const string &ConfigManager::getSynonymFilePath(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getSynonymFilePath();
}

const string &ConfigManager::getProtectedWordsFilePath(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getProtectedWordsFilePath();
}

bool ConfigManager::getSynonymKeepOrigFlag(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getSynonymKeepOrigFlag();
}

const string &ConfigManager::getStopFilePath(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getStopFilePath();
}

const string& ConfigManager::getAttributeRecordBoostName(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getrecordBoostField();
}

const string& ConfigManager::getRecordAllowedSpecialCharacters(const string &coreName) const {
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getRecordAllowedSpecialCharacters();
}

int ConfigManager::getSearchType(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getSearchType();
}

int ConfigManager::getIsPrimSearchable(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getIsPrimSearchable();
}

unsigned ConfigManager::getQueryTermBoost(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getQueryTermBoost();
}

bool ConfigManager::getSupportAttributeBasedSearch(const string &coreName) const
{
	string coreNameTmp = coreName;
    if (coreNameTmp.compare("") == 0) {
        coreNameTmp = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreNameTmp)->getSupportAttributeBasedSearch();
}


const string& ConfigManager::getLicenseKeyFileName() const {
    return licenseKeyFile;
}

const string& ConfigManager::getHTTPServerListeningHostname() const {
    return httpServerListeningHostname;
}

const string& ConfigManager::getHTTPServerDefaultListeningPort() const {
    return httpServerListeningPortStr;
}

const map<enum srch2http::PortType_t, unsigned short int>& ConfigManager::getHTTPServerListeningPorts() const {
	return httpServerListeningPorts;
}

int ConfigManager::getOrdering() const {
    return ordering;
}

bool ConfigManager::isRecordBoostAttributeSet(const string &coreName) const
{
	string coreName2 = coreName;
    if (coreName2.compare("") == 0) {
    	coreName2 = getDefaultCoreName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreName2)->getRecordBoostFieldFlag();
}

const string& ConfigManager::getHTTPServerAccessLogFile() const {
    return httpServerAccessLogFile;
}

const Logger::LogLevel& ConfigManager::getHTTPServerLogLevel() const {
    return loglevel;
}


string ConfigManager::getAuthorizationKey(){
	return ConfigManager::authorizationKey;
}

void ConfigManager::setAuthorizationKey(string &key){
	ConfigManager::authorizationKey = key;
}



unsigned CoreInfo_t::getCacheSizeInBytes() const
{
    return cacheSizeInBytes;
}

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////// Validate & Helper functions
///////////////////////////////////////////////////////////////////////////////////////////////

// splitString gets a string as its input and a delimiter. It splits the string based on the delimiter and pushes back the values to the result
void ConfigManager::splitString(string str, const string& delimiter, vector<string>& result) {
    size_t pos = 0;
    string token;
    while ((pos = str.find(delimiter)) != string::npos) {
        result.push_back(str.substr(0, pos));
        str.erase(0, pos + delimiter.length());
    }
    result.push_back(str);
}

void ConfigManager::splitBoostFieldValues(string boostString, map<string, unsigned>& boosts) {
    vector<string> boostTokens;
    this->splitString(boostString, " ", boostTokens);
    for (int i = 0; i < boostTokens.size(); i++) {
        size_t pos = boostTokens[i].find("^");
        if (pos != string::npos) {
            string field = boostTokens[i].substr(0, pos);
            string boost = boostTokens[i].substr(pos + 1, boostTokens[i].length());
            boosts[field] = static_cast<unsigned int>(strtoul(boost.c_str(),
                    NULL, 10));
            if (boosts[field] < 1 || boosts[field] > 100) {
                boosts[field] = 1;
            }
        } else {
            boosts[boostTokens[i]] = 1;
        }
    }
}

bool ConfigManager::isOnlyDigits(string& str) {
    string::const_iterator it = str.begin();
    while (it != str.end() && std::isdigit(*it)) {
        ++it;
    }
    return !str.empty() && it == str.end();
}

bool ConfigManager::isFloat(string str) {
    std::size_t found = str.find(".");
    if (found != std::string::npos) {
        str.erase(found, 1);
        if (str.find(".") != string::npos) {
            return false;
        }
    }
    return this->isOnlyDigits(str);
}

bool ConfigManager::isValidFieldType(string& fieldType , bool isSearchable) {
    string lowerCase = fieldType;
    std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(),
            ::tolower);

    if(isSearchable){
        // supported types are: text, location_latitude, location_longitude
        if ((lowerCase.compare("text") == 0)
                || (lowerCase.compare(locationLatitudeString) == 0)
                || (lowerCase.compare("location_longitude") == 0)) {
            return true;
        }
        return false;
    }else{
        // supported types are: text, integer, float, time
        if ((lowerCase.compare("text") == 0)
                || (lowerCase.compare("integer") == 0)
                || (lowerCase.compare("long") == 0)
                || (lowerCase.compare("float") == 0)
                || (lowerCase.compare("double") == 0)
                || (lowerCase.compare("time") == 0)) {
            return true;
        }
        return false;
    }
}

bool ConfigManager::isValidFieldDefaultValue(string& defaultValue, srch2::instantsearch::FilterType fieldType , bool isMultiValued)
{
    if(isMultiValued == false){
	return validateValueWithType(fieldType , defaultValue);
    }

    // if it is a multi-valued attribute, default value is a comma separated list of default values. example : "tag1,tag2,tag3"
    vector<string> defaultValueTokens;
    splitString(defaultValue , "," , defaultValueTokens);
    for(vector<string>::iterator defaultValueToken = defaultValueTokens.begin() ; defaultValueToken != defaultValueTokens.end() ; ++defaultValueToken){
	if( validateValueWithType(fieldType , *defaultValueToken) == false){
	    return false;
	}
    }
    return true;
}

bool ConfigManager::isValidBool(string& fieldType) {
    // supported types are: text, location_latitude, location_longitude
    if ((fieldType.compare("true") == 0) || (fieldType.compare("True") == 0) || (fieldType.compare("TRUE") == 0)
            || (fieldType.compare("false") == 0) || (fieldType.compare("False") == 0)
            || (fieldType.compare("FALSE") == 0)) {
        return true;
    }
    return false;
}

// validates if all the fields from boosts Fields are in the Triple or not.
bool ConfigManager::isValidBoostFields(const CoreInfo_t *coreInfo,
				       map<string, unsigned>& boostFields)
{
    map<string, unsigned>::iterator iter;
    for (iter = boostFields.begin(); iter != boostFields.end(); ++iter) {
        if (coreInfo->searchableAttributesInfo.count(iter->first) > 0) {
            continue;
        }
        return false;
    }
    return true;
}

bool ConfigManager::isValidBoostFieldValues(map<string, unsigned>& boostMap) {
    map<string, unsigned>::iterator iter;
    for (iter = boostMap.begin(); iter != boostMap.end(); ++iter) {
        if (iter->second <= 100 && iter->second >= 1) {
            continue;
        }
        return false;
    }
    return true;
}

bool ConfigManager::isValidQueryTermBoost(string& queryTermBoost) {
    return this->isFloat(queryTermBoost);
}

bool ConfigManager::isValidIndexCreateOrLoad(string& indexCreateLoad) {
    if (indexCreateLoad.compare("0") == 0 || indexCreateLoad.compare("1") == 0) {
        return true;
    }
    return false;
}

bool ConfigManager::isValidRecordScoreExpession(string& recordScoreExpression) {
    // We should validate this too.
    return true;
}

bool ConfigManager::isValidFuzzyMatchPenalty(string& fuzzyMatchPenalty) {
    return this->isFloat(fuzzyMatchPenalty);
}

bool ConfigManager::isValidQueryTermSimilarityThreshold(string & qTermSimilarityThreshold){
    return this->isFloat(qTermSimilarityThreshold);
}

bool ConfigManager::isValidQueryTermLengthBoost(string& queryTermLengthBoost) {
    if (this->isFloat(queryTermLengthBoost)) {
        float val = static_cast<float>(strtod(queryTermLengthBoost.c_str(),NULL));
        if (val >= 0 && val <= 1) {
            return true;
        }
    } else {
        return false;
    }
    return true;
}

bool ConfigManager::isValidPrefixMatch(string& prefixmatch) {
    if (this->isFloat(prefixmatch)) {
        float val = static_cast<float>(strtod(prefixmatch.c_str(),NULL));
        if (val >= 0 && val <= 1) {
            return true;
        }
    }
    return false;
}

bool ConfigManager::isValidCacheSize(string& cacheSize) {
    unsigned minCacheSize = 50 * 1048576;     // 50MB
    unsigned maxCacheSize = 500 * 1048576;    // 500MB
    if (this->isOnlyDigits(cacheSize)) {
        int cs = static_cast<int>(strtol(cacheSize.c_str(),NULL,10));
        if (cs >= minCacheSize && cs <= maxCacheSize) {
            return true;
        }
    }
    return false;
}

bool ConfigManager::isValidRows(string& rows) {
    return (this->isOnlyDigits(rows) && (strtol(rows.c_str(),NULL,10) > 0)); // should be number and greater that 1
}

bool ConfigManager::isValidMaxSearchThreads(string& maxSearchThreads) {
    return (this->isOnlyDigits(maxSearchThreads) && (strtol(maxSearchThreads.c_str(),NULL,10) > 0)); // should be number and greater that 1
}

bool ConfigManager::isValidBooleanValue(string& fieldValue) {
    if (fieldValue.compare("0") == 0 || fieldValue.compare("1") == 0) {
        return true;
    }
    return false;
}

bool ConfigManager::isValidQueryTermFuzzyType(string& queryTermFuzzyType) {
    if (queryTermFuzzyType.compare("0") == 0 || queryTermFuzzyType.compare("1") == 0) {
        return true;
    }
    return false;
}

bool ConfigManager::isValidQueryTermPrefixType(string& queryTermPrefixType) {
    if (queryTermPrefixType.compare("0") == 0 || queryTermPrefixType.compare("1") == 0) {
        return true;
    }
    return false;
}

bool ConfigManager::isValidResponseFormat(string& responseFormat) {
    if (responseFormat.compare("0") == 0 || responseFormat.compare("1") == 0) {
        return true;
    }
    return false;
}

bool ConfigManager::isValidResponseContentType(string responseContentType) {
    if (responseContentType.compare("0") == 0 || responseContentType.compare("1") == 0
    		|| responseContentType.compare("2") == 0){
        return true;
    }
    return false;
}

bool ConfigManager::isValidMaxDoc(string& maxDoc) {
    return this->isOnlyDigits(maxDoc);
}

bool ConfigManager::isValidMaxMemory(string& maxMemory) {
    return this->isOnlyDigits(maxMemory);
}

bool ConfigManager::isValidMergeEveryNSeconds(string& mergeEveryNSeconds) {
    if (this->isOnlyDigits(mergeEveryNSeconds)) {
        if (strtol(mergeEveryNSeconds.c_str(),NULL,10) >= 1) {
            return true;
        }
    }
    return false;
}

bool ConfigManager::isValidMergeEveryMWrites(string& mergeEveryMWrites) {
    if (this->isOnlyDigits(mergeEveryMWrites)) {
        if (strtol(mergeEveryMWrites.c_str(),NULL,10) >= 1) {
            return true;
        }
    }
    return false;
}

bool ConfigManager::isValidKeywordPopularityThreshold(string kpt){
    if (this->isOnlyDigits(kpt)) {
        if (strtol(kpt.c_str(),NULL,10) >= 1) {
            return true;
        }
    }
    return false;
}

bool ConfigManager::isValidGetAllResultsMaxResultsThreshold(string kpt){
    if (this->isOnlyDigits(kpt)) {
        if (strtol(kpt.c_str(),NULL,10) >= 1) {
            return true;
        }
    }
    return false;
}

bool ConfigManager::isValidGetAllResultsKAlternative(string kpt){
    if (this->isOnlyDigits(kpt)) {
        if (strtol(kpt.c_str(),NULL,10) >= 1) {
            return true;
        }
    }
    return false;
}

// Note: For release mode compiles, logLevel can still be 4 (debug), but debug output will be suppressed.
bool ConfigManager::isValidLogLevel(string& logLevel) {
    if (logLevel.compare("0") == 0 || logLevel.compare("1") == 0 || logLevel.compare("2") == 0
        || logLevel.compare("3") == 0 || logLevel.compare("4") == 0) {
        return true;
    }
    return false;
}

bool ConfigManager::isValidIndexType(string& indexType) {
    if (indexType.compare("0") == 0 || indexType.compare("1") == 0 ){
        return true;
    }
    return false;
}

bool ConfigManager::isValidSearcherType(string& searcherType) {
    if (searcherType.compare("0") == 0 || searcherType.compare("1") == 0 ){
        return true;
    }
    return false;
}

srch2::instantsearch::FilterType ConfigManager::parseFieldType(string& fieldType){
    string lowerCase = fieldType;
    std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(),
            ::tolower);
    if (lowerCase.compare("integer") == 0)
        return srch2::instantsearch::ATTRIBUTE_TYPE_INT;
    else if (lowerCase.compare("long") == 0)
            return srch2::instantsearch::ATTRIBUTE_TYPE_LONG;
    else if (lowerCase.compare("float") == 0)
        return srch2::instantsearch::ATTRIBUTE_TYPE_FLOAT;
    else if (lowerCase.compare("double") == 0)
            return srch2::instantsearch::ATTRIBUTE_TYPE_DOUBLE;
    else if (lowerCase.compare("text") == 0)
        return srch2::instantsearch::ATTRIBUTE_TYPE_TEXT;
    else if (lowerCase.compare("time") == 0)
        return srch2::instantsearch::ATTRIBUTE_TYPE_TIME;

    Logger::warn("\"%s\" is not a supported type. The following are supported "\
            "types: text, integer, long, float, double, time, "\
            "location_longitude (for geo search), "\
            "and location_latitude (for geo search).",fieldType.c_str());
    //The only possibility this function throws an exception is
    //the programmer forgets to call isValidFieldType() before using this function

    ASSERT(false);
    return srch2::instantsearch::ATTRIBUTE_TYPE_INT;
}

int ConfigManager::parseFacetType(string& facetType){
    string facetTypeLowerCase = boost::to_lower_copy(facetType);
    if(facetTypeLowerCase.compare("categorical") == 0){
        return 0;
    }else if(facetTypeLowerCase.compare("range") == 0){
        return 1;
    }else{
        return -1;
    }
}

bool ConfigManager::isPositionIndexEnabled(const string &coreName) const
{
    if (coreName.compare("") == 0) {
        return (getCoreByName(defaultCoreName)->isPositionIndexWordEnabled() ||
        		getCoreByName(defaultCoreName)->isPositionIndexCharEnabled());
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreName)->isPositionIndexWordEnabled() ||
    		clusterReadview->getCoreByName(coreName)->isPositionIndexCharEnabled();
}

const string& ConfigManager::getMongoServerHost(const string &coreName) const
{
    if (coreName.compare("") == 0) {
        return getCoreByName(defaultCoreName)->getMongoServerHost();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreName)->getMongoServerHost();
}

const string& ConfigManager::getMongoServerPort(const string &coreName) const
{
    if (coreName.compare("") == 0) {
        return getCoreByName(defaultCoreName)->getMongoServerPort();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreName)->getMongoServerPort();
}

const string& ConfigManager::getMongoDbName(const string &coreName) const
{
    if (coreName.compare("") == 0) {
        return getCoreByName(defaultCoreName)->getMongoDbName();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreName)->getMongoDbName();
}

const string& ConfigManager::getMongoCollection (const string &coreName) const
{
    if (coreName.compare("") == 0) {
        return getCoreByName(defaultCoreName)->getMongoCollection();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreName)->getMongoCollection();
}

const unsigned ConfigManager::getMongoListenerWaitTime (const string &coreName) const
{
    if (coreName.compare("") == 0) {
        return getCoreByName(defaultCoreName)->getMongoListenerWaitTime();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreName)->getMongoListenerWaitTime();
}

const unsigned ConfigManager::getMongoListenerMaxRetryCount(const string &coreName) const
{
    if (coreName.compare("") == 0) {
        return getCoreByName(defaultCoreName)->getMongoListenerMaxRetryOnFailure();
    }
    boost::shared_ptr<const ClusterResourceMetadata_Readview> clusterReadview;
    ShardManager::getReadview(clusterReadview);
    return clusterReadview->getCoreByName(coreName)->getMongoListenerMaxRetryOnFailure();
}

string ConfigManager::createSRCH2Home()
{
	boost::filesystem::path dir = this->getSrch2Home();
	boost::filesystem::create_directory(dir);
	return this->getSrch2Home();
}

string ConfigManager::createClusterDir(const string& clusterName)
{
	string path = this->getSrch2Home() +clusterName;
	createSRCH2Home();
	boost::filesystem::create_directory(path);
	return path;
}

string ConfigManager::createNodeDir(const string& clusterName)
{
	string path = this->getSrch2Home() + clusterName + "/" + this->getCurrentNodeName();
	createClusterDir(clusterName);
	boost::filesystem::create_directory(path);
	return path;
}

string ConfigManager::createCoreDir(const string& clusterName, const string& coreName)
{
	string path = this->getSrch2Home() + clusterName + "/" + this->getCurrentNodeName() + "/" + coreName;
	createNodeDir(clusterName);
	boost::filesystem::create_directory(path);
	return path;
}

string ConfigManager::createShardDir(const string& clusterName, const string& coreName, const ShardId * shardId)
{
	string path = this->getSrch2Home() + clusterName + "/" + this->getCurrentNodeName() + "/" + coreName + "/" + shardId->toString();
	createCoreDir(clusterName, coreName);
	boost::filesystem::create_directory(path);
	return path;
}

string ConfigManager::getClusterName() {
	return clusterNameStr;
}

string ConfigManager::getSRCH2HomeDir()
{
	string path = this->getSrch2Home();
	if(boost::filesystem::is_directory(path))
		return path;
	else
		return "";
}

string ConfigManager::getClusterDir(const string& clusterName)
{
	string path = this->getSRCH2HomeDir() + clusterName;
	if(boost::filesystem::is_directory(path))
		return path;
	else
		return "";

}

string ConfigManager::getNodeDir(const string& clusterName)
{
	string path = getClusterDir(clusterName) + "/" + this->getCurrentNodeName();
	if(boost::filesystem::is_directory(path))
		return path;
	else
		return "";

}

string ConfigManager::getCoreDir(const string& clusterName, const string& coreName)
{
	string path = getNodeDir(clusterName) + "/" + coreName;
	if(boost::filesystem::is_directory(path))
		return path;
	else
		return "";
}

string ConfigManager::getShardDir(const string& clusterName, const string& coreName, const ShardId * shardId)
{
	string path = getCoreDir(clusterName, coreName) + "/" + shardId->toString();
	if(boost::filesystem::is_directory(path))
		return path;
	else
		return "";
}


bool ConfigManager::tryLockNodeName(){
	string nodeDirPath = this->getNodeDir(this->clusterNameStr);
	if(nodeDirPath.compare("") == 0){
		nodeDirPath = this->createNodeDir(this->clusterNameStr);
	}
	string lockFilePath = nodeDirPath + "/." + this->getCurrentNodeName() + ".lock";
	ifstream lockFile(lockFilePath.c_str());
	if(lockFile.good()){
		lockFile.close();
		return false;
	}else{
		ofstream newLockFile(lockFilePath.c_str());
		newLockFile.write("locked", 6);
		newLockFile.close();
		return true;
	}
	return false;
}
void ConfigManager::unlockNodeName(){
	string nodeDirPath = this->getNodeDir(this->clusterNameStr);
	if(nodeDirPath.compare("") == 0){
		return;
	}
	string lockFilePath = nodeDirPath + "/." + this->getCurrentNodeName() + ".lock";
	std::remove(lockFilePath.c_str());
	return;
}

void ConfigManager::renameDir(const string & src, const string & target){
	boost::filesystem::path dirSrc = src;
	boost::filesystem::path dirTrg = target;
	boost::filesystem::rename(dirSrc, dirTrg);
}

uint ConfigManager::removeDir(const string& path)
{
	uint numberOfFilesDeleted = boost::filesystem::remove_all(path);
	return numberOfFilesDeleted;
}

string ConfigManager::getCurrentNodeName() const{
	return this->nodeNameStr;
}

string Transport::getIpAddress(){
	return this->ipAddress;
}

unsigned Transport::getPort(){
	return this->port;
}

void Transport::setIpAddress(const string& ipAddress){
	this->ipAddress = ipAddress;
}

void Transport::setPort(unsigned port){
	this->port = port;
}

void MulticastDiscovery::setIpAddress(string& ipAddress){
	this->ipAddress = ipAddress;
}

void MulticastDiscovery::setGroupAddress(string& groupAddress){
	this->groupAddress = groupAddress;
}

void MulticastDiscovery::setPort(unsigned port){
	this->port = port;
}

void MulticastDiscovery::setTtl(unsigned ttl){
	this->ttl = ttl;
}

string MulticastDiscovery::getIpAddress(){
	return this->ipAddress;
}

string MulticastDiscovery::getGroupAddress(){
	return this->groupAddress;
}

unsigned MulticastDiscovery::getPort(){
	return this->port;
}

unsigned MulticastDiscovery::getTtl(){
	return this->ttl;
}

// end of namespaces
}
}