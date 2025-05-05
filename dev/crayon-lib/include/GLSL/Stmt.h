#pragma once

#include "GLSL/Type.h"

#include <vector>

namespace crayon
{
	namespace glsl
	{
		class QualDecl;
		class VarDecl;
		class FunParam;
		class FunParamList;
		class FunDecl;
		class FunDef;
		class BlockStmt;

		class StmtVisitor
		{
		public:

			virtual void VisitQualDecl(QualDecl* qualDecl) = 0;
			virtual void VisitVarDecl(VarDecl* varDecl) = 0;
			virtual void VisitFunParam(FunParam* funParam) = 0;
			virtual void VisitFunParamList(FunParamList* funParamList) = 0;
			virtual void VisitFunDecl(FunDecl* funDecl) = 0;
			virtual void VisitFunDef(FunDef* funDef) = 0;
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

		private:

			TypeQual qualifier;
		};

		class VarDecl : public Stmt
		{
		public:

			VarDecl(const FullSpecType& fullSpecType, const Token& identifier);

			void Accept(StmtVisitor* stmtVisitor) override;

		private:

			FullSpecType fullSpecType;
			Token identifier;
		};

		class FunParam : public Stmt
		{
		public:

			FunParam(const FullSpecType& fullSpecType);
			FunParam(const FullSpecType& fullSpecType, const Token& identifier);

			void Accept(StmtVisitor* stmtVisitor) override;

			const FullSpecType& GetFullSpecType();

			bool HasIdentifier() const;
			const Token& GetIdentifier() const;

		private:

			FullSpecType fullSpecType;
			std::optional<Token> identifier;
		};

		class FunParamList : public Stmt
		{
		public:

			void Accept(StmtVisitor* stmtVisitor) override;

			void AddFunParam(std::shared_ptr<FunParam> funParam);

			bool Empty() const;

			const std::list<std::shared_ptr<FunParam>>& GetFunParams() const;

		private:

			std::list<std::shared_ptr<FunParam>> funParams;
		};

		class FunProto : public Stmt
		{
		public:

			FunProto(const FullSpecType& fullSpecType, const Token& identifier);
			FunProto(const FullSpecType& fullSpecType, const Token& identifier, std::shared_ptr<FunParamList> params);

			void Accept(StmtVisitor* stmtVisitor) override;

			bool ParamListEmpty() const;

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

		private:

			std::shared_ptr<FunProto> funProto;
		};

		class FunDef : public Stmt
		{
		public:

			FunDef(std::shared_ptr<FunProto> funProto);
			FunDef(std::shared_ptr<FunProto> funProto, std::shared_ptr<BlockStmt> stmts);

			void Accept(StmtVisitor* stmtVisitor) override;

			bool FunBlockEmpty() const;

		private:

			std::shared_ptr<FunProto> funProto;
			std::shared_ptr<BlockStmt> stmts;
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