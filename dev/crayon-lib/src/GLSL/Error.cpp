#include "GLSL/Error.h"

#include <sstream>

namespace crayon {
	namespace glsl {

        SyntaxError::SyntaxError(const Token& errToken)
            : errToken(errToken) {
            CreateErrMsg(std::string_view{});
        }
        SyntaxError::SyntaxError(const Token& errToken, std::string_view errMsg)
            : errToken(errToken) {
            CreateErrMsg(errMsg);
        }
        SyntaxError::SyntaxError(const Token& errToken, TokenType expected)
            : errToken(errToken), expected(expected) {
            CreateErrMsg(std::string_view{});
        }
        SyntaxError::SyntaxError(const Token& errToken, TokenType expected, std::string_view errMsg)
            : errToken(errToken), expected(expected) {
            CreateErrMsg(errMsg);
        }

        const char* SyntaxError::what() const noexcept {
            return errMsg.data();
        }
        const Token& SyntaxError::GetErrorToken() const {
            return errToken;
        }
        TokenType SyntaxError::GetExpectedTokenType() const {
            return expected;
        }

        void SyntaxError::CreateErrMsg(std::string_view errMsg) {
            std::stringstream errStream;
            // +1 for 'line' and 'startCol' is because internally lines and columns are indexed starting from 0.
            errStream << "Syntax error ["
			    << errToken.line + 1 << ":" << errToken.startCol + 1
				<< "]: " << errMsg;
            errStream << "\n";
            if (expected != TokenType::UNDEFINED) {
                errStream << "Expected '" << TokenTypeToStr(expected) << "'" << ", " <<
                    "Encountered: '" << TokenTypeToStr(errToken.tokenType) << "'!";
            } else {
                errStream << "Token: '" << TokenTypeToStr(errToken.tokenType) << "'"
                    << ", " << "lexeme: '" << errToken.lexeme << "'.";
            }
            this->errMsg = std::move(errStream.str());
        }

    }
}