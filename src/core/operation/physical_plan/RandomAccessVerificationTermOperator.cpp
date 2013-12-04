
#include "PhysicalOperators.h"

using namespace std;
namespace srch2 {
namespace instantsearch {

////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PhysicalPlan Random Access Verification Term Operator ////////////////////////////

RandomAccessVerificationTermOperator::RandomAccessVerificationTermOperator() {
}

RandomAccessVerificationTermOperator::~RandomAccessVerificationTermOperator(){
}
bool RandomAccessVerificationTermOperator::open(QueryEvaluatorInternal * queryEvaluator, PhysicalPlanExecutionParameters & params){
	this->queryEvaluator = queryEvaluator;
	ASSERT(this->getPhysicalPlanOptimizationNode()->getChildrenCount() == 0);
	return true;
}
PhysicalPlanRecordItem * RandomAccessVerificationTermOperator::getNext(const PhysicalPlanExecutionParameters & params) {
	return NULL;
}
bool RandomAccessVerificationTermOperator::close(PhysicalPlanExecutionParameters & params){
	this->queryEvaluator = NULL;
	ASSERT(this->getPhysicalPlanOptimizationNode()->getChildrenCount() == 0);
	return true;
}
bool RandomAccessVerificationTermOperator::verifyByRandomAccess(PhysicalPlanRandomAccessVerificationParameters & parameters) {
	  //do the verification
	PrefixActiveNodeSet *prefixActiveNodeSet =
			this->getPhysicalPlanOptimizationNode()->getLogicalPlanNode()->stats->getActiveNodeSetForEstimation(parameters.isFuzzy);

	Term * term = this->getPhysicalPlanOptimizationNode()->getLogicalPlanNode()->exactTerm;

	unsigned termSearchableAttributeIdToFilterTermHits = term->getAttributeToFilterTermHits();
	// assume the iterator returns the ActiveNodes in the increasing order based on edit distance
	for (ActiveNodeSetIterator iter(prefixActiveNodeSet, term->getThreshold());
			!iter.isDone(); iter.next()) {
		const TrieNode *trieNode;
		unsigned distance;
		iter.getItem(trieNode, distance);

		unsigned minId = trieNode->getMinId();
		unsigned maxId = trieNode->getMaxId();
		if (term->getTermType() == srch2::instantsearch::TERM_TYPE_COMPLETE) {
			if (trieNode->isTerminalNode())
				maxId = minId;
			else
				continue;  // ignore non-terminal nodes
		}

		unsigned matchingKeywordId;
		float termRecordStaticScore;
		unsigned termAttributeBitmap;
		if (this->queryEvaluator->getForwardIndex()->haveWordInRange(parameters.recordToVerify->getRecordId(), minId, maxId,
				termSearchableAttributeIdToFilterTermHits,
				matchingKeywordId, termAttributeBitmap, termRecordStaticScore)) {
			parameters.termRecordMatchingPrefixes.push_back(trieNode);
			parameters.attributeBitmaps.push_back(termAttributeBitmap);
			parameters.prefixEditDistances.push_back(distance);
			bool isPrefixMatch = ( (!trieNode->isTerminalNode()) || (minId != matchingKeywordId) );
			parameters.runTimeTermRecordScore = DefaultTopKRanker::computeTermRecordRuntimeScore(termRecordStaticScore, distance,
						term->getKeyword()->size(),
						isPrefixMatch,
						parameters.prefixMatchPenalty , term->getSimilarityBoost() ) ;
			parameters.staticTermRecordScore = termRecordStaticScore ;
			// parameters.positionIndexOffsets ????
			return true;
		}
	}
	return false;
}
// The cost of open of a child is considered only once in the cost computation
// of parent open function.
PhysicalPlanCost RandomAccessVerificationTermOptimizationOperator::getCostOfOpen(const PhysicalPlanExecutionParameters & params){
	return PhysicalPlanCost(1); // cost O(1)
}
// The cost of getNext of a child is multiplied by the estimated number of calls to this function
// when the cost of parent is being calculated.
PhysicalPlanCost RandomAccessVerificationTermOptimizationOperator::getCostOfGetNext(const PhysicalPlanExecutionParameters & params) {
	return PhysicalPlanCost(); // cost zero
}
// the cost of close of a child is only considered once since each node's close function is only called once.
PhysicalPlanCost RandomAccessVerificationTermOptimizationOperator::getCostOfClose(const PhysicalPlanExecutionParameters & params) {
	return PhysicalPlanCost(1); // cost O(1)
}
PhysicalPlanCost RandomAccessVerificationTermOptimizationOperator::getCostOfVerifyByRandomAccess(const PhysicalPlanExecutionParameters & params){
	// base cost for one verification : 20
	unsigned cost = 20;
	unsigned estimatedNumberOfTerminalNodes = this->getLogicalPlanNode()->stats->getEstimatedNumberOfLeafNodes();
	return PhysicalPlanCost(20 * estimatedNumberOfTerminalNodes);
}
void RandomAccessVerificationTermOptimizationOperator::getOutputProperties(IteratorProperties & prop){
	// TODO
}
void RandomAccessVerificationTermOptimizationOperator::getRequiredInputProperties(IteratorProperties & prop){
	// the only requirement for input is to be directly connected to inverted index,
	// so since no operator outputs PhysicalPlanIteratorProperty_LowestLevel NULL operator will be pushed down to lowest level
	prop.addProperty(PhysicalPlanIteratorProperty_LowestLevel);
}
PhysicalPlanNodeType RandomAccessVerificationTermOptimizationOperator::getType() {
	return PhysicalPlanNode_RandomAccessTerm;
}
bool RandomAccessVerificationTermOptimizationOperator::validateChildren(){
	// shouldn't have any child
	ASSERT(getChildrenCount() == 0);
	if(getChildrenCount() > 0){
		return false;
	}
	return true;
}

}
}
