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

		const TypeQual& QualDecl::GetTypeQualifier() const
		{
			return qualifier;
		}

		VarDecl::VarDecl(const FullSpecType& fullSpecType, const Token& identifier)
			: fullSpecType(fullSpecType), identifier(identifier)
		{
		}

		void VarDecl::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitVarDecl(this);
		}

		const FullSpecType& VarDecl::GetFullySpecifiedType() const
		{
			return fullSpecType;
		}
		const Token& VarDecl::GetIdentifier()
		{
			return identifier;
		}

		FunParam::FunParam(const FullSpecType& fullSpecType)
			: fullSpecType(fullSpecType)
		{
		}
		FunParam::FunParam(const FullSpecType& fullSpecType, const Token& identifier)
			: fullSpecType(fullSpecType), identifier(identifier)
		{
		}

		const FullSpecType& FunParam::GetFullSpecType() const
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

		void FunParamList::AddFunctionParameter(const FunParam& funParam)
		{
			funParams.push_back(funParam);
		}

		bool FunParamList::Empty() const
		{
			return funParams.size() == 0;
		}

		const std::list<FunParam>& FunParamList::GetFunctionParameters() const
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

		const FullSpecType& FunProto::GetFullySpecifiedType() const
		{
			return fullSpecType;
		}
		const Token& FunProto::GetIdentifier() const
		{
			return identifier;
		}

		bool FunProto::FunctionParameterListEmpty() const
		{
			if (params) return false;
			return true;
		}

		const FunParamList& FunProto::GetFunctionParameterList() const
		{
			return *params.get();
		}

		FunDecl::FunDecl(std::shared_ptr<FunProto> funProto)
			: funProto(funProto)
		{
		}

		void FunDecl::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitFunDecl(this);
		}

		const FunProto& FunDecl::GetFunctionPrototype() const
		{
			return *funProto.get();
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

		const FunProto& FunDef::GetFunctionPrototype() const
		{
			return *funProto.get();
		}
	
		bool FunDef::FunBlockEmpty() const
		{
			if (stmts) return false;
			return true;
		}

		std::shared_ptr<BlockStmt> FunDef::GetBlockStatement() const
		{
			return stmts;
		}

		void ExtDeclList::AddDeclaration(std::shared_ptr<Stmt> declaration)
		{
			declarations.push_back(declaration);
		}

		void ExtDeclList::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitExtDeclList(this);
		}

		const std::vector<std::shared_ptr<Stmt>>& ExtDeclList::GetDeclarations()
		{
			return declarations;
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