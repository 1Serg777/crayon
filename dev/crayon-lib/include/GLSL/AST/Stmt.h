#pragma once

#include "GLSL/Type.h"

#include <vector>

namespace crayon
{
	namespace glsl
	{
		class QualDecl;
		class VarDecl;
		class FunDecl;
		class FunDef;
		class ExtDeclList;
		class BlockStmt;

		class StmtVisitor
		{
		public:

			virtual void VisitQualDecl(QualDecl* qualDecl) = 0;
			virtual void VisitVarDecl(VarDecl* varDecl) = 0;
			virtual void VisitFunDecl(FunDecl* funDecl) = 0;
			virtual void VisitFunDef(FunDef* funDef) = 0;
			virtual void VisitExtDeclList(ExtDeclList* extDeclList) = 0;
			virtual void VisitBlockStmt(BlockStmt* blockStmt) = 0;
		};

		class Stmt
		{
		public:

			virtual void Accept(StmtVisitor* stmtVisitor) = 0;
		};

		class QualDecl : public Stmt
		{
		public:

			QualDecl(const TypeQual& qualifier);

			void Accept(StmtVisitor* stmtVisitor) override;

			const TypeQual& GetTypeQualifier() const;

		private:

			TypeQual qualifier;
		};

		class VarDecl : public Stmt
		{
		public:

			VarDecl(const FullSpecType& fullSpecType, const Token& identifier);

			void Accept(StmtVisitor* stmtVisitor) override;

			const FullSpecType& GetFullySpecifiedType() const;
			const Token& GetIdentifier();

		private:

			FullSpecType fullSpecType;
			Token identifier;
		};

		class FunParam
		{
		public:

			FunParam(const FullSpecType& fullSpecType);
			FunParam(const FullSpecType& fullSpecType, const Token& identifier);

			const FullSpecType& GetFullSpecType() const;

			bool HasIdentifier() const;
			const Token& GetIdentifier() const;

		private:

			FullSpecType fullSpecType;
			std::optional<Token> identifier;
		};

		class FunParamList
		{
		public:

			void AddFunctionParameter(const FunParam& funParam);

			bool Empty() const;

			const std::list<FunParam>& GetFunctionParameters() const;

		private:

			std::list<FunParam> funParams;
		};

		class FunProto
		{
		public:

			FunProto(const FullSpecType& fullSpecType, const Token& identifier);
			FunProto(const FullSpecType& fullSpecType, const Token& identifier, std::shared_ptr<FunParamList> params);

			const FullSpecType& GetFullySpecifiedType() const;
			const Token& GetIdentifier() const;

			bool FunctionParameterListEmpty() const;
			const FunParamList& GetFunctionParameterList() const;

		private:

			FullSpecType fullSpecType;
			Token identifier;

			std::shared_ptr<FunParamList> params;
		};

		class FunDecl : public Stmt
		{
		public:

			FunDecl(std::shared_ptr<FunProto> funProto);

			void Accept(StmtVisitor* stmtVisitor) override;

			const FunProto& GetFunctionPrototype() const;

		private:

			std::shared_ptr<FunProto> funProto;
		};

		class FunDef : public Stmt
		{
		public:

			FunDef(std::shared_ptr<FunProto> funProto);
			FunDef(std::shared_ptr<FunProto> funProto, std::shared_ptr<BlockStmt> stmts);

			void Accept(StmtVisitor* stmtVisitor) override;

			const FunProto& GetFunctionPrototype() const;

			bool FunBlockEmpty() const;
			std::shared_ptr<BlockStmt> GetBlockStatement() const;

		private:

			std::shared_ptr<FunProto> funProto;
			std::shared_ptr<BlockStmt> stmts;
		};

		class ExtDeclList : public Stmt
		{
		public:

			void Accept(StmtVisitor* stmtVisitor) override;

			// void AddQualDecl(std::shared_ptr<QualDecl> qualDecl);
			// void AddVarDecl(std::shared_ptr<VarDecl> qualDecl);
			// void AddFunDecl(std::shared_ptr<FunDecl> qualDecl);
			// void AddFunDef(std::shared_ptr<FunDef> qualDecl);

			void AddDeclaration(std::shared_ptr<Stmt> declaration);

			const std::vector<std::shared_ptr<Stmt>>& GetDeclarations();

		private:

			std::vector<std::shared_ptr<Stmt>> declarations;
		};

		class BlockStmt : public Stmt
		{
		public:

			void Accept(StmtVisitor* stmtVisitor) override;

			void AddStmt(std::shared_ptr<Stmt> stmt);

		private:

			std::vector<std::shared_ptr<Stmt>> stmts;
		};
	}
}