#pragma once

#include "GLSL/Token.h"
#include "GLSL/AST/Decl.h"
#include "GLSL/AST/Stmt.h"
#include "GLSL/AST/Expr.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace crayon {
	namespace glsl {

		class Parser {
		public:
			void Parse(const Token* tokenStream, size_t tokenStreamSize);

			std::shared_ptr<TransUnit> GetTranslationUnit() const;

		private:
			void TranslationUnit();

			std::shared_ptr<Decl> ExternalDeclaration();
			std::shared_ptr<Decl> DeclarationOrFunctionDefinition(DeclContext declContext);
			std::shared_ptr<Decl> Declaration(DeclContext declContext);

			std::shared_ptr<FunProto> FunctionPrototype(const FullSpecType& fullSpecType, const Token& identifier);
			std::shared_ptr<FunParamList> FunctionParameterList();
			FunParam FunctionParameter();

			std::shared_ptr<BlockStmt> BlockStatement();

			FullSpecType FullySpecifiedType();

			TypeQual TypeQualifier();
			void SingleTypeQualifier(TypeQual& typeQual);
			void TypeQualifierRest(TypeQual& typeQual);

			void LayoutQualifierList(std::list<LayoutQualifier>& layout);
			LayoutQualifier SingleLayoutQualifier();

			TypeSpec TypeSpecifier();

			std::shared_ptr<Expr> Expression();
			std::shared_ptr<Expr> Term();
			std::shared_ptr<Expr> Primary();

			bool IsQualifier(TokenType tokenType) const;
			bool IsStorageQualifier(TokenType tokenType) const;
			bool IsPrecisionQualifier(TokenType tokenType) const;
			bool IsType(TokenType tokenType) const;

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

			std::shared_ptr<TransUnit> transUnit;
		};
	}
}