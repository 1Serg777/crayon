#pragma once

#include "GLSL/Error.h"
#include "GLSL/Token.h"
#include "GLSL/Analyzer/Environment.h"
#include "GLSL/Analyzer/SemanticAnalyzer.h"
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
			void Parse(const Token* tokenStream, size_t tokenStreamSize, ErrorReporter* errorReporter);
			bool HadSyntaxError() const;
			std::shared_ptr<TransUnit> GetTranslationUnit() const;

		private:
			void InitializeExternalScope();
			void InitializeGlobalVariables();
			
			void EnterNewScope();
			void RestoreEnclosingScope();

			void TranslationUnit();

			std::shared_ptr<Decl> ExternalDeclaration();
			std::shared_ptr<Decl> DeclarationOrFunctionDefinition(DeclContext declContext);
			std::shared_ptr<Decl> Declaration(DeclContext declContext);
			void ParseVarDeclRest(std::shared_ptr<VarDecl> varDecl);
			std::shared_ptr<StructDecl> StructDeclaration();
			std::shared_ptr<VarDecl> StructFieldDecl();

			std::shared_ptr<FunProto> FunctionPrototype(const FullSpecType& fullSpecType, const Token& identifier);
			void FunctionParameterList(std::shared_ptr<FunProto> funProto);
			std::shared_ptr<FunParam> FunctionParameter();

			std::shared_ptr<BlockStmt> BlockStatement();
			std::shared_ptr<Stmt> Statement();
			std::shared_ptr<Stmt> SimpleStatement();

			void Synchronize();

			std::shared_ptr<Expr> Expression();
			std::shared_ptr<Expr> Initializer();
			std::shared_ptr<Expr> InitializerList();
			std::shared_ptr<Expr> AssignmentExpression();
			std::shared_ptr<Expr> ConditionalExpression();
			std::shared_ptr<Expr> AdditiveExpression();
			std::shared_ptr<Expr> MultiplicativeExpression();
			std::shared_ptr<Expr> UnaryExpression();
			std::shared_ptr<Expr> PostfixExpression();
			std::shared_ptr<Expr> PrimaryExpression();

			std::shared_ptr<FunCallArgList> FunctionCallArgumentList();

			FullSpecType FullySpecifiedType();

			TypeQual TypeQualifier();
			void SingleTypeQualifier(TypeQual& typeQual);
			void TypeQualifierRest(TypeQual& typeQual);

			void LayoutQualifierList(std::list<LayoutQualifier>& layout);
			LayoutQualifier SingleLayoutQualifier();

			TypeSpec TypeSpecifier();
			std::vector<std::shared_ptr<Expr>> ArraySpecifier();

			bool IsDeclaration(const Token& token) const;
			bool IsQualifier(TokenType tokenType) const;
			bool IsStorageQualifier(TokenType tokenType) const;
			bool IsPrecisionQualifier(TokenType tokenType) const;

			bool IsType(const Token& type) const;
			bool IsTypeBasic(TokenType tokenType) const;
			bool IsTypeScalar(TokenType tokenType) const;
			bool IsTypeVector(TokenType tokenType) const;
			bool IsTypeMatrix(TokenType tokenType) const;
			bool IsTypeTransparent(TokenType tokenType) const;
			bool IsTypeOpaque(TokenType tokenType) const;
			bool IsTypeAggregate(const Token& type) const;

			bool IsAssignmentOperator(TokenType tokenType) const;
			bool IsBinaryArithmeticOperator(TokenType tokenType) const;

			const Token* Advance();
			const Token* Previous();
			const Token* Peek() const;
			bool Match(TokenType tokenType);
			const Token* Consume(TokenType tokenType, std::string_view msg);

			bool AtEnd() const;
			const Token* Last() const;

			std::shared_ptr<TransUnit> transUnit;
			std::shared_ptr<Environment> currentScope;
			std::unique_ptr<SemanticAnalyzer> semanticAnalyzer;
			ErrorReporter* errorReporter{nullptr};

			const Token* tokenStream{nullptr};
			size_t tokenStreamSize{0};
			uint32_t current{0};

			bool hadSyntaxError{false};
		};
	}
}