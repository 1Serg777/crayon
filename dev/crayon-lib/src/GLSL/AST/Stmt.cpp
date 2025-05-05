#include "GLSL/AST/Stmt.h"

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

		FunProto::FunProto(const FullSpecType& fullSpecType, const Token& identifier)
			: fullSpecType(fullSpecType), identifier(identifier)
		{
		}
		FunProto::FunProto(const FullSpecType& fullSpecType, const Token& identifier, std::shared_ptr<FunParamList> params)
			: fullSpecType(fullSpecType), identifier(identifier), params(params)
		{
		}

		void FunProto::Accept(StmtVisitor* stmtVisitor)
		{
			// TODO
		}

		bool FunProto::ParamListEmpty() const
		{
			if (params) return false;
			return true;
		}

		FunDecl::FunDecl(std::shared_ptr<FunProto> funProto)
			: funProto(funProto)
		{
		}

		void FunDecl::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitFunDecl(this);
		}

		FunDef::FunDef(std::shared_ptr<FunProto> funProto)
			: funProto(funProto)
		{
		}
		FunDef::FunDef(std::shared_ptr<FunProto> funProto, std::shared_ptr<BlockStmt> stmts)
			: funProto(funProto), stmts(stmts)
		{
		}

		void FunDef::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitFunDef(this);
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