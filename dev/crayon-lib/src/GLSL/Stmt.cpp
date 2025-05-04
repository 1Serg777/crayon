#include "GLSL/Stmt.h"

namespace crayon
{
	namespace glsl
	{
		QualDecl::QualDecl(const TypeQual& qualifier)
			: qualifier(qualifier)
		{
		}

		void QualDecl::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitQualDecl(this);
		}

		VarDecl::VarDecl(const FullSpecType& fullSpecType, const Token& identifier)
			: fullSpecType(fullSpecType), identifier(identifier)
		{
		}

		void VarDecl::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitVarDecl(this);
		}

		FunParam::FunParam(const FullSpecType& fullSpecType)
			: fullSpecType(fullSpecType)
		{
		}
		FunParam::FunParam(const FullSpecType& fullSpecType, const Token& identifier)
			: fullSpecType(fullSpecType), identifier(identifier)
		{
		}

		void FunParam::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitFunParam(this);
		}

		const FullSpecType& FunParam::GetFullSpecType()
		{
			return fullSpecType;
		}

		bool FunParam::HasIdentifier() const
		{
			return identifier.has_value();
		}
		const Token& FunParam::GetIdentifier() const
		{
			return identifier.value();
		}

		void FunParamList::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitFunParamList(this);
		}

		void FunParamList::AddFunParam(std::shared_ptr<FunParam> funParam)
		{
			funParams.push_back(funParam);
		}

		bool FunParamList::Empty() const
		{
			return funParams.size() == 0;
		}

		const std::list<std::shared_ptr<FunParam>>& FunParamList::GetFunParams() const
		{
			return funParams;
		}

		FunDecl::FunDecl(const FullSpecType& fullSpecType, const Token& identifier)
			: fullSpecType(fullSpecType), identifier(identifier)
		{
		}
		FunDecl::FunDecl(const FullSpecType& fullSpecType, const Token& identifier, std::shared_ptr<FunParamList> params)
			: fullSpecType(fullSpecType), identifier(identifier), params(params)
		{
		}

		void FunDecl::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitFunDecl(this);
		}

		bool FunDecl::ParamListEmpty() const
		{
			if (params) return false;
			return true;
		}

		FunDef::FunDef(const FullSpecType& fullSpecType, const Token& identifier)
			: fullSpecType(fullSpecType), identifier(identifier)
		{
		}
		FunDef::FunDef(const FullSpecType& fullSpecType, const Token& identifier, std::shared_ptr<FunParamList> params)
			: fullSpecType(fullSpecType), identifier(identifier), params(params)
		{
		}
		FunDef::FunDef(
			const FullSpecType& fullSpecType, const Token& identifier,
			std::shared_ptr<FunParamList> params, std::shared_ptr<BlockStmt> stmts)
			: fullSpecType(fullSpecType), identifier(identifier), params(params), stmts(stmts)
		{
		}

		void FunDef::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitFunDef(this);
		}
		
		bool FunDef::ParamListEmpty() const
		{
			if (params) return false;
			return true;
		}

		bool FunDef::FunBlockEmpty() const
		{
			if (stmts) return false;
			return true;
		}

		void BlockStmt::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitBlockStmt(this);
		}
		
		void BlockStmt::AddStmt(std::shared_ptr<Stmt> stmt)
		{
			stmts.push_back(stmt);
		}
	}
}