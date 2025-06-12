#pragma once

#include "GLSL/Token.h"
#include "GLSL/AST/Decl.h"
#include "GLSL/AST/Stmt.h"
#include "GLSL/AST/Expr.h"

#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>

namespace crayon {
	namespace glsl {

		class Environment {
		public:
			Environment();
			Environment(std::shared_ptr<Environment> enclosingScope);

			void AddStructDecl(std::shared_ptr<StructDecl> structDecl);
			void AddFunDecl(std::shared_ptr<FunDecl> funDecl);
			void AddVarDecl(std::shared_ptr<VarDecl> varDecl);

			bool StructDeclExist(std::string_view structName) const;
			bool FunDeclExist(std::string_view funName) const;
			bool VarDeclExist(std::string_view varName) const;

			std::shared_ptr<StructDecl> GetStructDecl(std::string_view structName) const;
			std::shared_ptr<FunDecl> GetFunDecl(std::string_view funName) const;
			std::shared_ptr<VarDecl> GetVarDecl(std::string_view varName) const;

			bool IsExternalScope() const;
			std::shared_ptr<Environment> GetEnclosingScope() const;

		private:
			std::unordered_map<std::string_view, std::shared_ptr<StructDecl>> aggregates;
			std::unordered_map<std::string_view, std::shared_ptr<FunDecl>> functions;
			std::unordered_map<std::string_view, std::shared_ptr<VarDecl>> variables;
			std::shared_ptr<Environment> enclosingScope;
		};

		class Parser {
		public:
			void Parse(const Token* tokenStream, size_t tokenStreamSize);

			std::shared_ptr<TransUnit> GetTranslationUnit() const;

		private:
			void EnterNewScope();
			void RestoreEnclosingScope();

			void TranslationUnit();

			std::shared_ptr<Decl> ExternalDeclaration();
			std::shared_ptr<Decl> DeclarationOrFunctionDefinition(DeclContext declContext);
			std::shared_ptr<Decl> Declaration(DeclContext declContext);
			std::shared_ptr<StructDecl> StructDeclaration();
			std::shared_ptr<VarDecl> StructFieldDecl();

			std::shared_ptr<FunProto> FunctionPrototype(const FullSpecType& fullSpecType, const Token& identifier);
			std::shared_ptr<FunParamList> FunctionParameterList();
			FunParam FunctionParameter();

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

			bool IsTypeBasic(TokenType tokenType) const;
			bool IsTypeAggregate(const Token& type) const;
			bool IsType(const Token& type) const;

			const Token* Advance();
			const Token* Previous();
			const Token* Peek() const;
			bool Match(TokenType tokenType);
			const Token* Consume(TokenType tokenType, std::string_view msg);

			bool AtEnd() const;
			const Token* Last() const;

			const Token* tokenStream{nullptr};
			size_t tokenStreamSize{0};
			uint32_t current{0};

			std::shared_ptr<TransUnit> transUnit;
			std::shared_ptr<Environment> currentScope;
		};
	}
}