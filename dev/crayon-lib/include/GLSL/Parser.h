#pragma once

#include "GLSL/AST.h"
#include "GLSL/Token.h"
#include "GLSL/Expr.h"
#include "GLSL/Stmt.h"

#include <memory>
#include <vector>

namespace crayon
{
	namespace glsl
	{
		class Parser
		{
		public:

			Parser();

			void Parse(const Token* tokenStream, size_t tokenStreamSize);

			const std::vector<std::shared_ptr<Stmt>>& GetStatements() const;

		private:

			void TranslationUnit();

			std::shared_ptr<Stmt> ExternalDeclaration();

			std::shared_ptr<Expr> Expression();
			std::shared_ptr<Expr> Term();
			std::shared_ptr<Expr> Primary();

			const Token* Advance();
			const Token* Previous();
			const Token* Peek() const;
			bool Match(TokenType tokenType);

			const Token* Consume(TokenType tokenType, std::string_view msg);

			bool AtEnd() const;
			const Token* Last() const;

			const Token* tokenStream{ nullptr };
			size_t tokenStreamSize{ 0 };

			uint32_t current{ 0 };

			std::vector<std::shared_ptr<Stmt>> stmts;
		};
	}
}