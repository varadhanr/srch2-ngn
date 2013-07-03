/*
 * SimpleAnalyzer.cpp
 *
 *  Created on: 2013-5-18
 */

#include "SimpleAnalyzer.h"
#include "WhiteSpaceTokenizer.h"
#include "LowerCaseFilter.h"

namespace bimaple
{
namespace instantsearch
{

// create operator flow and link share pointer to the data
TokenOperator * SimpleAnalyzer::createOperatorFlow()
{
	TokenOperator *tokenOperator = new WhiteSpaceTokenizer();
	tokenOperator = new LowerCaseFilter(tokenOperator);
	this->sharedToken = tokenOperator->sharedToken;
	return tokenOperator;
}
SimpleAnalyzer::~SimpleAnalyzer() {
	// TODO Auto-generated destructor stub
}
}}

