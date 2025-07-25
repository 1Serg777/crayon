#pragma once

#include "GLSL/Token.h"
#include "GLSL/AST/Block.h"
#include "GLSL/AST/Decl.h"
#include "GLSL/AST/Expr.h"

#include <exception>
#include <memory>
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

        class ErrorReporter {
        public:
            void SetSrcCodeLink(const char* srcCodeData, size_t srcCodeSize);

            void ReportVarDeclInitExprTypeMismatch(std::shared_ptr<VarDecl> varDecl);

            void ReportVertexAttribDeclType(std::shared_ptr<VertexAttribDecl> vertexAttribDecl);
            void ReportVertexAttribDeclChannel(std::shared_ptr<VertexAttribDecl> vertexAttribDecl);

            void ReportMaterialPropertyType(std::shared_ptr<MatPropDecl> matPropDecl);

        private:
            std::string_view GetSrcCodeTokenLine(const Token& token) const;

            const char* srcCodeData{nullptr};
            size_t srcCodeSize{0};
        };

    }
}