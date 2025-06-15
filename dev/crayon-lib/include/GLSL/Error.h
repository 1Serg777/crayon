#pragma once

#include "GLSL/Token.h"

#include <exception>
#include <string>
#include <string_view>

namespace crayon {
	namespace glsl {

        class SyntaxError : public std::exception {
        public:
            SyntaxError(const Token& errToken);
            SyntaxError(const Token& errToken, std::string_view errMsg);
            SyntaxError(const Token& errToken, TokenType expected);
            SyntaxError(const Token& errToken, TokenType expected, std::string_view errMsg);

            const char* what() const noexcept override;
            const Token& GetErrorToken() const;
            TokenType GetExpectedTokenType() const;

        private:
            void CreateErrMsg(std::string_view errMsg);

            Token errToken;
            std::string errMsg;
            TokenType expected{TokenType::UNDEFINED};
        };

    }
}