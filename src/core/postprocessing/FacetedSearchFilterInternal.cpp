//$Id: ResultsPostProcessor.h 3456 2013-06-26 02:11:13Z Jamshid $

/*
 * The Software is made available solely for use according to the License Agreement. Any reproduction
 * or redistribution of the Software not in accordance with the License Agreement is expressly prohibited
 * by law, and may result in severe civil and criminal penalties. Violators will be prosecuted to the
 * maximum extent possible.
 *
 * THE SOFTWARE IS WARRANTED, IF AT ALL, ONLY ACCORDING TO THE TERMS OF THE LICENSE AGREEMENT. EXCEPT
 * AS WARRANTED IN THE LICENSE AGREEMENT, SRCH2 INC. HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS WITH
 * REGARD TO THE SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES AND CONDITIONS OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT.  IN NO EVENT SHALL SRCH2 INC. BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF SOFTWARE.

 * Copyright © 2010 SRCH2 Inc. All rights reserved
 */

#include "FacetedSearchFilterInternal.h"
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "instantsearch/Score.h"


using namespace std;
namespace srch2 {
namespace instantsearch {

void FacetedSearchFilterInternal::doAggregationCategorical(const Score & attributeValue,
        std::vector<pair<string, float> > * counts) {

    // move on computed facet results to see if this value is seen before (increment) or is new (add and initialize)
    // TODO : optimization on find and access and increment ...
    for (std::vector<pair<string, float> >::iterator p =
            counts->begin(); p != counts->end(); ++p) {
        if (p->first.compare(attributeValue.toString()) == 0) {
            p->second ++; // this value is seen before
            return;
        }
    }
    counts->push_back(make_pair(attributeValue.toString(), 1)); // add a new value
    return;
}

// Range facet
void FacetedSearchFilterInternal::doAggregationRange(const Score & attributeValue,
        const std::vector<Score> & lowerBounds,
        std::vector<pair<string, float> > * counts  , Score & start, Score & end, Score & gap) {

    unsigned intervalIndex = attributeValue.findIndexOfContainingInterval(start,end, gap);

    if(intervalIndex > counts->size()){
        intervalIndex =  counts->size() - 1;
    }
    counts->at(intervalIndex).second ++ ;
    return;

}
void FacetedSearchFilterInternal::doAggregation(const Score & attributeValue,
        const std::vector<Score> & lowerBounds,
        std::vector<pair<string, float> > * counts , unsigned indexOfField) {

    if (lowerBounds.empty()) { // Categorical facet
        // move on computed facet results to see if this value is seen before (increment) or is new (add and initialize)
        doAggregationCategorical(attributeValue, counts);
        return;
    }
    doAggregationRange(attributeValue , lowerBounds , counts ,
            rangeStartScores.at(indexOfField),
            rangeEndScores.at(indexOfField),
            rangeGapScores.at(indexOfField));


}

/*
 * Example :
 * If we have two attributes : price,model (facet type : range, categorical)
 * and start,end and gap for price are 1,100 and 10. Then this function
 * produces an empty vector for model (because it's categorical) and a vector with the following
 * values for price:
 * -large_value, 1, 11, 21, 31, 41, ..., 91, 101
 */
void FacetedSearchFilterInternal::prepareFacetInputs(IndexSearcher *indexSearcher) {

    IndexSearcherInternal * indexSearcherInternal =
            dynamic_cast<IndexSearcherInternal *>(indexSearcher);
    Schema * schema = indexSearcherInternal->getSchema();
    ForwardIndex * forwardIndex = indexSearcherInternal->getForwardIndex();

    // 1. parse the values into Score.
    unsigned fieldIndex = 0;
    for (std::vector<std::string>::iterator field = this->fields.begin();
            field != this->fields.end(); ++field) {
        if (this->facetTypes.at(fieldIndex) == FacetTypeCategorical) { // Simple facet
            Score placeHolder; // just insert placeholders
            rangeStartScores.push_back(placeHolder);
            rangeEndScores.push_back(placeHolder);
            rangeGapScores.push_back(placeHolder);
        } else { // Range facet
            FilterType attributeType = schema->getTypeOfNonSearchableAttribute(
                    schema->getNonSearchableAttributeId(*field));
            Score start;
            start.setScore(attributeType, this->rangeStarts.at(fieldIndex));
            rangeStartScores.push_back(start);

            Score end;
            end.setScore(attributeType, this->rangeEnds.at(fieldIndex));
            rangeEndScores.push_back(end);

            Score gap;
            gap.setScore(attributeType, this->rangeGaps.at(fieldIndex));
            rangeGapScores.push_back(gap);
        }

        //
        fieldIndex++;
    }

    // 2. create the lowerbound vector for each attribute

    unsigned facetTypeIndex = 0;
    for (std::vector<FacetType>::iterator facetTypeIterator = facetTypes.begin();
            facetTypeIterator != facetTypes.end(); ++facetTypeIterator) {

        std::vector<Score> lowerBounds;
        switch (*facetTypeIterator) {
        case FacetTypeCategorical: // lower bounds vector is empty, because lower bounds are not determined before results
            break;

        case FacetTypeRange:
            Score & start = rangeStartScores.at(facetTypeIndex);
            Score & end = rangeEndScores.at(facetTypeIndex);
            Score & gap = rangeGapScores.at(facetTypeIndex);
            ASSERT(start.getType() != srch2::instantsearch::ATTRIBUTE_TYPE_TEXT);

            // Example : start : 1, gap : 10 , end : 100
            // first -large_value is added as the first category
            // then 1, 11, 21, ...and 91 are added in the loop.
            // and 101 is added after loop.
            Score lowerBoundToAdd = start;
            lowerBounds.push_back(lowerBoundToAdd.minimumValue()); // to collect data smaller than start
            while (lowerBoundToAdd < end) {
                lowerBounds.push_back(lowerBoundToAdd); // data of normal categories
                lowerBoundToAdd = lowerBoundToAdd + gap;
            }
            lowerBounds.push_back(lowerBoundToAdd); // to collect data greater than end
            break;
        }
        lowerBoundsOfIntervals[fields.at(facetTypeIndex)] = lowerBounds;
        //
        facetTypeIndex++;
    }

}

}
}
