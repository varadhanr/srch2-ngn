/*
 * TokenOperator.h
 *
 *  Created on: 2013-5-17
 */

#ifndef __CORE_ANALYZER__TOKEN_OPERATOR_H__
#define __CORE_ANALYZER__TOKEN_OPERATOR_H__

#include <boost/shared_ptr.hpp>
#include "TokenStreamContainer.h"

namespace srch2 {
namespace instantsearch {

/*
 * TokenOperator can be a Tokenizer or TokenFilter
 */
class TokenStream {
public:
    boost::shared_ptr<TokenStreamContainer> tokenStreamContainer;
    TokenStream() {
    }
    virtual bool processToken() = 0;
    std::vector<CharType> & getProcessedToken() {
        return tokenStreamContainer->currentToken;
    }
    virtual ~TokenStream() {
    }

};
}
}
#endif /* __CORE_ANALYZER__TOKEN_OPERATOR_H__ */