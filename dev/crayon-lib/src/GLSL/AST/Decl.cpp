#include "GLSL/AST/Decl.h"

#include <cassert>

namespace crayon {
    namespace glsl {

        void TransUnit::AddDeclaration(std::shared_ptr<Decl> decl) {
			decls.push_back(decl);
		}
		void TransUnit::Accept(DeclVisitor* declVisitor) {
			declVisitor->VisitTransUnit(this);
		}
		const std::vector<std::shared_ptr<Decl>>& TransUnit::GetDeclarations() {
			return decls;
		}

        VarDecl::VarDecl(const FullSpecType& varType, const Token& varName)
			: varType(varType), varName(varName) {
		}
		VarDecl::VarDecl(const FullSpecType& varType, const Token& varName,
						 std::shared_ptr<Expr> initializerExpr)
			: varType(varType), varName(varName), initializerExpr(initializerExpr) {
		}
		void VarDecl::Accept(DeclVisitor* declVisitor) {
			declVisitor->VisitVarDecl(this);
		}
		
		const FullSpecType& VarDecl::GetVariableType() const {
			return varType;
		}
		const Token& VarDecl::GetVariableName() const {
			return varName;
		}

		bool VarDecl::HasInitializerExpr() const {
			if (initializerExpr) return true;
			else return false;
		}
		std::shared_ptr<Expr> VarDecl::GetInitializerExpr() const {
			return initializerExpr;
		}

        FunParam::FunParam(const FullSpecType& paramType)
			: VarDecl(paramType, Token{}) {
		}
		FunParam::FunParam(const FullSpecType& paramType, const Token& paramName)
			: VarDecl(paramType, paramName) {
		}
		bool FunParam::HasName() const {
			return GetVariableName().tokenType == TokenType::IDENTIFIER;
		}

		void FunParamList::AddFunctionParameter(const FunParam& funParam) {
			funParams.push_back(funParam);
		}
		bool FunParamList::Empty() const {
			return funParams.empty();
		}
        const std::list<FunParam>& FunParamList::GetParams() const {
			return funParams;
		}

        FunProto::FunProto(const FullSpecType& retType, const Token& funName)
			: retType(retType), funName(funName) {
		}
		FunProto::FunProto(const FullSpecType& retType, const Token& funName, std::shared_ptr<FunParamList> params)
			: retType(retType), funName(funName), params(params) {
		}
		
		const FullSpecType& FunProto::GetReturnType() const {
			return retType;
		}
		const Token& FunProto::GetFunctionName() const {
			return funName;
		}
		
		bool FunProto::FunctionParameterListEmpty() const {
			if (params) return false;
			return true;
		}
		const FunParamList& FunProto::GetFunctionParameterList() const {
			return *params.get();
		}

		FunDecl::FunDecl(std::shared_ptr<FunProto> funProto)
			: funProto(funProto) {
		}
		FunDecl::FunDecl(std::shared_ptr<FunProto> funProto, std::shared_ptr<BlockStmt> stmts)
			: funProto(funProto), stmts(stmts) {
		}

        void FunDecl::Accept(DeclVisitor* declVisitor) {
			declVisitor->VisitFunDecl(this);
		}
        
		bool FunDecl::IsFunDecl() const {
			if (stmts) return false;
			else return true;
		}
        bool FunDecl::IsFunDef() const {
			return !IsFunDecl();
		}
        bool FunDecl::IsFunBlockEmpty() const {
			assert(IsFunDef() && "Not a function definition! Check this with IsFunDef() first!");
			return stmts->IsEmpty();
		}

		const FunProto& FunDecl::GetFunctionPrototype() const {
			return *funProto.get();
		}
		std::shared_ptr<BlockStmt> FunDecl::GetBlockStatement() const {
			return stmts;
		}

        QualDecl::QualDecl(const TypeQual& qualifier)
			: qualifier(qualifier) {
		}
		void QualDecl::Accept(DeclVisitor* declVisitor) {
			declVisitor->VisitQualDecl(this);
		}
		const TypeQual& QualDecl::GetTypeQualifier() const {
			return qualifier;
		}
    
	}
}