#include "GLSL/AST/Decl.h"

#include <algorithm>
#include <cassert>

namespace crayon {
    namespace glsl {

		NamedEntity::NamedEntity(const Token& name)
			: name(name) {
		}
		bool NamedEntity::HasName() const {
			// return name.tokenType == TokenType::IDENTIFIER;
			return name.tokenType != TokenType::UNDEFINED;
		}
		const Token& NamedEntity::GetName() const {
			return name;
		}

		void AggregateEntity::AddField(std::shared_ptr<VarDecl> fieldDecl) {
			fields.push_back(fieldDecl);
		}
		bool AggregateEntity::HasField(std::string_view fieldName) const {
			auto predicate = [=](const std::shared_ptr<VarDecl>& field) {
				return fieldName == field->GetVarName().lexeme;
			};
			auto searchRes = std::find_if(fields.begin(), fields.end(), predicate);
			return searchRes != fields.end();
		}
		std::shared_ptr<VarDecl> AggregateEntity::GetField(std::string_view fieldName) {
			auto predicate = [=](const std::shared_ptr<VarDecl>& field) {
				return fieldName == field->GetVarName().lexeme;
			};
			auto searchRes = std::find_if(fields.begin(), fields.end(), predicate);
			assert(searchRes != fields.end() && "Check the existence of the field first!");
			return *searchRes;
		}
		const std::vector<std::shared_ptr<VarDecl>>& AggregateEntity::GetFields() const {
			return fields;
		}

		void ArrayEntity::AddDimension(std::shared_ptr<Expr> dimExpr) {
			this->dimensions.push_back(dimExpr);
		}
		size_t ArrayEntity::GetDimensionCount() const {
			return dimensions.size();
		}
		bool ArrayEntity::IsArray() const {
			return GetDimensionCount() > 0;
		}
		const std::vector<std::shared_ptr<Expr>>& ArrayEntity::GetDimensions() const {
			return dimensions;
		}

        void TransUnit::AddDeclaration(std::shared_ptr<Decl> decl) {
			decls.push_back(decl);
		}
		void TransUnit::Accept(DeclVisitor* declVisitor) {
			declVisitor->VisitTransUnit(this);
		}
		const std::vector<std::shared_ptr<Decl>>& TransUnit::GetDeclarations() {
			return decls;
		}

		InterfaceBlockDecl::InterfaceBlockDecl(const Token& name)
			: NamedEntity(name) {
		}
		InterfaceBlockDecl::InterfaceBlockDecl(const Token& name, const TypeQual& typeQual)
			: NamedEntity(name), typeQual(typeQual) {
		}
		InterfaceBlockDecl::InterfaceBlockDecl(const Token& name, const TypeQual& typeQual, const Token& instanceName)
			: NamedEntity(name), typeQual(typeQual), instanceName(instanceName) {
		}
		void InterfaceBlockDecl::Accept(DeclVisitor* declVisitor) {
			declVisitor->VisitInterfaceBlockDecl(this);
		}
		const TypeQual& InterfaceBlockDecl::GetTypeQualifier() const {
			return typeQual;
		}

		bool InterfaceBlockDecl::HasInstanceName() const {
			return instanceName.tokenType == TokenType::IDENTIFIER &&
				   !instanceName.lexeme.empty();
		}
		const Token& InterfaceBlockDecl::GetInstanceName() const {
			return instanceName;
		}

		DeclList::DeclList(const FullSpecType& fullSpecType)
			: fullSpecType(fullSpecType) {
		}
		void DeclList::Accept(DeclVisitor* declVisitor) {
			declVisitor->VisitDeclList(this);
		}
		void DeclList::AddDecl(std::shared_ptr<VarDecl> decl) {
			decls.push_back(decl);
		}
		const std::vector<std::shared_ptr<VarDecl>>& DeclList::GetDecls() const {
			return decls;
		}
		const FullSpecType& DeclList::GetFullSpecType() const {
			return fullSpecType;
		}

		StructDecl::StructDecl(const Token& structName)
			: NamedEntity(structName) {
		}
		void StructDecl::Accept(DeclVisitor* declVisitor) {
			declVisitor->VisitStructDecl(this);
		}
		bool StructDecl::IsStructDeclAnonymous() const {
			return !HasName();
		}

        VarDecl::VarDecl(const FullSpecType& varType, const Token& varName)
			: varType(varType), varName(varName) {
		}
		void VarDecl::Accept(DeclVisitor* declVisitor) {
			declVisitor->VisitVarDecl(this);
		}
		bool VarDecl::IsVarTypeBasic() const {
			return varType.specifier.IsBasic();
		}
		bool VarDecl::IsVarTypeAggregate() const {
			return varType.specifier.IsAggregate();
		}
		bool VarDecl::IsVarTypeArray() const {
			return varType.specifier.IsArray();
		}
		bool VarDecl::IsVarArray() const {
			return dimensions.size();
		}
		bool VarDecl::IsArray() const {
			return IsVarTypeArray() || IsVarArray();
		}
		void VarDecl::AddDimension(std::shared_ptr<Expr> dimExpr) {
			this->dimensions.push_back(dimExpr);
		}
		size_t VarDecl::GetDimensionCount() const {
			return dimensions.size();
		}
		const std::vector<std::shared_ptr<Expr>>& VarDecl::GetDimensions() const {
			return dimensions;
		}
		bool VarDecl::HasInitializerExpr() const {
			if (initExpr) return true;
			else return false;
		}
		void VarDecl::SetInitializerExpr(std::shared_ptr<Expr> initExpr) {
			this->initExpr = initExpr;
		}
		std::shared_ptr<Expr> VarDecl::GetInitializerExpr() const {
			return initExpr;
		}
		GlslExprType VarDecl::GetExprType() const {
			GlslExprType exprType{};
			exprType.type = TokenTypeToGlslBasicType(varType.specifier.type.tokenType);
			exprType.name = varType.specifier.type.lexeme;
			if (IsArray()) {
				// 1. First we go over the variable dimensions.
				for (size_t i = 0; i < dimensions.size(); i++) {
					// TODO
					// Each "dimensions[i]" expression must be a constant integer expression!
					// exprType.dimensions.push_back(dimensions[i]);
				}
				// 2. The we go over the type dimensions.
				for (size_t i = 0; i < varType.specifier.dimensions.size(); i++) {
					// TODO
					// Each "varType.specifier.dimensions[i]" expression must be a constant integer expression!
					// exprType.dimensions.push_back(varType.specifier.dimensions[i]);
				}
				// This way, a variable declaration like "int[3] a[2]" has
				// the "int[2][3]" type as expected.
			}
			return exprType;
		}
		const FullSpecType& VarDecl::GetVarType() const {
			return varType;
		}
		const Token& VarDecl::GetVarName() const {
			return varName;
		}

        FunParam::FunParam(const FullSpecType& paramType)
			: VarDecl(paramType, Token{}) {
		}
		FunParam::FunParam(const FullSpecType& paramType, const Token& paramName)
			: VarDecl(paramType, paramName) {
		}
		bool FunParam::HasName() const {
			return GetVarName().tokenType == TokenType::IDENTIFIER;
		}

        FunProto::FunProto(const FullSpecType& retType, const Token& funName)
			: retType(retType), funName(funName) {
		}
		const FullSpecType& FunProto::GetReturnType() const {
			return retType;
		}
		const Token& FunProto::GetFunctionName() const {
			return funName;
		}
		void FunProto::AddFunParam(std::shared_ptr<FunParam> funParam) {
			params.push_back(funParam);
		}
		bool FunProto::FunParamListEmpty() const {
			return params.size() == 0;
		}
		const std::vector<std::shared_ptr<FunParam>>& FunProto::GetFunParamList() const {
			return params;
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
		std::shared_ptr<FunProto> FunDecl::GetFunProto() const {
			return funProto;
		}
		std::shared_ptr<BlockStmt> FunDecl::GetBlockStmt() const {
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

		std::shared_ptr<InterfaceBlockDecl> CreateInterfaceBlockDecl(TokenType storageQual, std::string_view interfaceName,
			                                                         std::vector<std::shared_ptr<VarDecl>> fieldDecls,
			                                                         std::string_view instanceName) {
			Token storageQualTok{};
			storageQualTok.tokenType = storageQual;
			storageQualTok.lexeme = TokenTypeToLexeme(storageQualTok.tokenType);

			TypeQual typeQual{};
			typeQual.storage = storageQualTok;

			Token intNameTok{};
			intNameTok.tokenType = TokenType::IDENTIFIER;
			intNameTok.lexeme = interfaceName;

			std::shared_ptr<InterfaceBlockDecl> intBlock;
			if (!instanceName.empty()) {
				Token instanceNameTok{};
				instanceNameTok.tokenType = TokenType::IDENTIFIER;
				instanceNameTok.lexeme = instanceName;

				intBlock = std::make_shared<InterfaceBlockDecl>(intNameTok, typeQual, instanceNameTok);
			} else {
				intBlock = std::make_shared<InterfaceBlockDecl>(intNameTok, typeQual);
			}

			for (const std::shared_ptr<VarDecl>& fieldDecl : fieldDecls) {
				intBlock->AddField(fieldDecl);
			}

			return intBlock;
		}

		std::shared_ptr<VarDecl> CreateNonArrayTypeNonArrayVarDecl(TokenType varType, std::string_view varName) {
			Token typeTok{};
			typeTok.tokenType = varType;
			typeTok.lexeme = TokenTypeToLexeme(typeTok.tokenType);

			FullSpecType fullSpecType{};
			fullSpecType.specifier.type = typeTok;

			Token varNameTok{};
			varNameTok.tokenType = TokenType::IDENTIFIER;
			varNameTok.lexeme = varName;

			std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, varNameTok);
			return varDecl;
		}
		std::shared_ptr<VarDecl> CreateNonArrayTypeNonArrayVarDecl(TokenType storageQual,
			                                                       TokenType varType,
			                                                       std::string_view varName) {
			Token storageQualTok{};
			storageQualTok.tokenType = storageQual;
			storageQualTok.lexeme = TokenTypeToLexeme(storageQualTok.tokenType);

			Token typeTok{};
			typeTok.tokenType = varType;
			typeTok.lexeme = TokenTypeToLexeme(typeTok.tokenType);

			FullSpecType fullSpecType{};
			fullSpecType.qualifier.storage = storageQualTok;
			fullSpecType.specifier.type = typeTok;

			Token varNameTok{};
			varNameTok.tokenType = TokenType::IDENTIFIER;
			varNameTok.lexeme = varName;

			std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, varNameTok);
			return varDecl;
		}

		std::shared_ptr<VarDecl> CreateNonArrayTypeArrayVarDecl(TokenType storageQual, TokenType varType,
			                                                    std::string_view varName,
			                                                    std::vector<std::shared_ptr<Expr>> dimensions) {
			Token storageQualTok{};
			storageQualTok.tokenType = storageQual;
			storageQualTok.lexeme = TokenTypeToLexeme(storageQualTok.tokenType);
			
			Token typeTok{};
			typeTok.tokenType = varType;
			typeTok.lexeme = TokenTypeToLexeme(typeTok.tokenType);

			FullSpecType fullSpecType{};
			fullSpecType.qualifier.storage = storageQualTok;
			fullSpecType.specifier.type = typeTok;

			Token varNameTok{};
			varNameTok.tokenType = TokenType::IDENTIFIER;
			varNameTok.lexeme = varName;

			std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, varNameTok);
			for (const std::shared_ptr<Expr>& dimension : dimensions) {
				varDecl->AddDimension(dimension);
			}

			return varDecl;
		}
		std::shared_ptr<VarDecl> CreateNonArrayTypeArrayVarDecl(TokenType varType, std::string_view varName,
			                                                    std::vector<std::shared_ptr<Expr>> dimensions) {
			Token typeTok{};
			typeTok.tokenType = varType;
			typeTok.lexeme = TokenTypeToLexeme(typeTok.tokenType);

			FullSpecType fullSpecType{};
			fullSpecType.specifier.type = typeTok;

			Token varNameTok{};
			varNameTok.tokenType = TokenType::IDENTIFIER;
			varNameTok.lexeme = varName;

			std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, varNameTok);
			for (const std::shared_ptr<Expr>& dimension : dimensions) {
				varDecl->AddDimension(dimension);
			}

			return varDecl;
		}

}
}