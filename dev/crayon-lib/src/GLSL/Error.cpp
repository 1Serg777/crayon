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

        void ErrorReporter::SetSrcCodeLink(const char* srcCodeData, size_t srcCodeSize) {
            this->srcCodeData = srcCodeData;
            this->srcCodeSize = srcCodeSize;
        }

        void ErrorReporter::ReportVarDeclInitExprTypeMismatch(std::shared_ptr<VarDecl> varDecl) const {
            // We need to report the entire line containing the variable declaration.
            // To get the source code line we're going to have to use some token that
            // fully lies on that line. We're going to walk backward to determine where the line starts,
            // as well as forward to determine where the line ends.
            // 1) A variable declaration may not have a type qualifier, so we can't be sure
            //    that at least one type qualifier token exists.
            // 2) Type token might seem to be our best shot, however, if the type is an anonymous structure,
            //    we won't have a name token referred to the actual place in the source code.
            // 3) The variable name identifier token is always present, so that's what we're going to go with.
            // First we print the source code line.
            const Token& varName = varDecl->GetVarName();
            // Print the error message.
            std::cerr << "[Var. decl.] The initializer expression type doesn't match the type of the variable declaration!\n";
            // Computer the number of digits in the line number.
            size_t lineDigitCount{0};
            size_t lineNumber = static_cast<size_t>(varName.line) + 1; // Always non-zero!
            while (lineNumber != 0) {
                lineNumber = lineNumber / 10;
                lineDigitCount++;
            }
            std::cerr << static_cast<size_t>(varName.line) + 1 << ".| ";
            size_t offset = lineDigitCount + 3; // The ".| " sequence is exactly 3 characters long.
            // Now we have to check if we're at a tab boundary.
            // If not, we pad the offset number to get to the next tab boundary.
            // We assume that a single tab is 4 space ' ' characters.
            // 
            // Print the line containing the error.
            // std::cerr << srcCodeLine;
            std::string_view srcCodeLine = GetSrcCodeTokenLine(varName);
            const char* lineData = srcCodeLine.data();
            size_t trimmedSize = srcCodeLine.size();
            while (*lineData == '\t') {
                std::fill_n(std::ostream_iterator<char>(std::cerr), 4, ' ');
                lineData++;
                trimmedSize--;
            }
            std::cerr << std::string_view{lineData, trimmedSize} << "\n";
            // Highlight the violating parts:
            // 1. Variable name
            // TODO: fix col number!
            std::fill_n(std::ostream_iterator<char>(std::cerr), offset + varName.startCol, ' ');
            std::fill_n(std::ostream_iterator<char>(std::cerr), varName.endCol - varName.startCol, '^');
            // 2. Initializer expression.
            std::pair<size_t, size_t> exprColBounds = varDecl->GetInitializerExpr()->GetExprColBounds();
            std::fill_n(std::ostream_iterator<char>(std::cerr), exprColBounds.first - varName.endCol, ' ');
            std::fill_n(std::ostream_iterator<char>(std::cerr), exprColBounds.second - exprColBounds.first, '^');
            std::cerr << std::endl;
        }

        void ErrorReporter::ReportVertexAttribDeclType(std::shared_ptr<VertexAttribDecl> vertexAttribDecl) const {
            // TODO
        }
        void ErrorReporter::ReportVertexAttribDeclChannel(std::shared_ptr<VertexAttribDecl> vertexAttribDecl) const {
            // TODO
        }

        void ErrorReporter::ReportMaterialPropertyType(std::shared_ptr<MatPropDecl> matPropDecl) const {
            // TODO
        }

        std::string_view ErrorReporter::GetSrcCodeTokenLine(const Token& token) const {
            const char* srcCodePtr = token.lexeme.data();
            // 1. Walk backward to find the '\n' character that advances to our current line.
            //    Be aware of the edge case when the current line is the very first one.
            const char* start = srcCodeData;
            const char* lineStart = srcCodePtr;
            while (lineStart > start && *lineStart != '\n') {
                lineStart--;
            }
            lineStart++;
            // 2. Walk forward to find the '\n' character that advance to the next line.
            //    Be aware of the edge case when the current line is the last one.
            const char* end = srcCodeData + srcCodeSize;
            const char* lineEnd = srcCodePtr;
            while (lineEnd < end && *lineEnd != '\n') {
                lineEnd++;
            }
            lineEnd--;
            // The two pointer we've found form the beginning and end of the line we're looking for.
            size_t lineSize = lineEnd - lineStart + 1;
            return std::string_view{lineStart, lineSize};
        }

    }
}