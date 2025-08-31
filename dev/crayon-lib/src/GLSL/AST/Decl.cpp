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
		std::shared_ptr<VarDecl> AggregateEntity::GetField(std::string_view fieldName, size_t& fieldIdx) {
			for (size_t i = 0; i < fields.size(); i++) {
				if (fields[i]->GetVarName().lexeme == fieldName) {
					fieldIdx = i;
					return fields[i];
				}
			}
			return std::shared_ptr<VarDecl>();
		}
		size_t AggregateEntity::GetFieldCount() const {
			return fields.size();
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
			return varType.specifier.IsStructure();
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
		void VarDecl::AddDimension(const ArrayDim& dim) {
			dimensions.push_back(dim);
		}
		size_t VarDecl::GetDimensionCount() const {
			return dimensions.size();
		}
		const std::vector<ArrayDim>& VarDecl::GetDimensions() const {
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
		TypeSpec VarDecl::GetVarExprType() const {
			TypeSpec typeSpec{};
			typeSpec.type = varType.specifier.type;
			typeSpec.typeDecl = varType.specifier.typeDecl;
			typeSpec.dimensions.resize(dimensions.size() + varType.specifier.dimensions.size());
			// 1. First we go over the variable dimensions.
			size_t i{0};
			for (; i < dimensions.size(); i++) {
				// Each "dimensions[i]" expression must be a constant integer expression!
				typeSpec.dimensions[i] = dimensions[i];
			}
			// 2. Then we go over the type dimensions.
			for (size_t j = 0; j < varType.specifier.dimensions.size(); j++, i++) {
				// Each "varType.specifier.dimensions[i]" expression must be a constant integer expression!
				typeSpec.dimensions[i] = varType.specifier.dimensions[j];
			}
			return typeSpec;
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

		static constexpr std::string_view glPosition_varName{"gl_Position"};
		static constexpr std::string_view glPointSize_varName{"gl_PointSize"};
		static constexpr std::string_view glClipDistance_varName{"gl_ClipDistance"};
		static constexpr std::string_view glCullDistance_varName{"gl_CullDistance"};
		static constexpr std::string_view glPerVertex_intBlockName{"gl_PerVertex"};

		std::shared_ptr<InterfaceBlockDecl> CreatePerVertexIntBlockDecl() {
			std::vector<std::shared_ptr<VarDecl>> perVertexFields(4);
			perVertexFields[0] = CreateNonArrayTypeNonArrayVarDecl(TokenType::VEC4, glPosition_varName);
			perVertexFields[1] = CreateNonArrayTypeNonArrayVarDecl(TokenType::FLOAT, glPointSize_varName);

			// Dimensions of the gl_ClipDistance and gl_CullDistance variables vary per user's request/choice.
			// In case we don't access the varriables at all,
			// the "glslc" compiler seems to create just one dimension of size 1.
			std::vector<ArrayDim> dimensions(1);
			dimensions[0].dimSize = 1;

			perVertexFields[2] = CreateNonArrayTypeArrayVarDecl(TokenType::FLOAT, glClipDistance_varName, dimensions);
			perVertexFields[3] = CreateNonArrayTypeArrayVarDecl(TokenType::FLOAT, glCullDistance_varName, dimensions);

			std::shared_ptr<InterfaceBlockDecl> glPerVertex = CreateInterfaceBlockDecl(TokenType::OUT,
					                                                                   glPerVertex_intBlockName,
					                                                                   perVertexFields);
			return glPerVertex;
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
			                                                    std::vector<ArrayDim> dimensions) {
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
			for (const ArrayDim& dimension : dimensions) {
				varDecl->AddDimension(dimension);
			}

			return varDecl;
		}
		std::shared_ptr<VarDecl> CreateNonArrayTypeArrayVarDecl(TokenType varType, std::string_view varName,
			                                                    std::vector<ArrayDim> dimensions) {
			Token typeTok{};
			typeTok.tokenType = varType;
			typeTok.lexeme = TokenTypeToLexeme(typeTok.tokenType);

			FullSpecType fullSpecType{};
			fullSpecType.specifier.type = typeTok;

			Token varNameTok{};
			varNameTok.tokenType = TokenType::IDENTIFIER;
			varNameTok.lexeme = varName;

			std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, varNameTok);
			for (const ArrayDim& dimension : dimensions) {
				varDecl->AddDimension(dimension);
			}

			return varDecl;
		}

}
}