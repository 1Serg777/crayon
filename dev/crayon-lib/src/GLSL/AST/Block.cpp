#include "GLSL/AST/Block.h"

#include <algorithm>
#include <cassert>

namespace crayon {
	namespace glsl {

		ShaderProgramBlock::ShaderProgramBlock(const Token& programName)
			:programName(programName) {
		}
		void ShaderProgramBlock::Accept(BlockVisitor* blockVisitor) {
			blockVisitor->VisitShaderProgramBlock(this);
		}
		void ShaderProgramBlock::AddBlock(std::shared_ptr<Block> block) {
			blocks.push_back(block);
		}
		bool ShaderProgramBlock::BlockListEmpty() const {
			return blocks.empty();
		}
		const std::vector<std::shared_ptr<Block>> ShaderProgramBlock::GetBlocks() {
			return blocks;
		}
		bool ShaderProgramBlock::ShaderProgramNameEmpty() const {
			// return programName.lexeme.empty();
			// return programName.tokenType == TokenType::UNDEFINED; // default value
			return programName.tokenType != TokenType::STRING;
		}
		std::string_view ShaderProgramBlock::GetShaderProgramName() const {
			// return programName.lexeme; // will include the string delimiter characters (" or ')
			return ExtractStringLiteral(programName);
		}

		void FixedStagesConfigBlock::Accept(BlockVisitor* blockVisitor) {
			blockVisitor->VisitFixedStagesConfigBlock(this);
		}

		MaterialPropertiesBlock::MaterialPropertiesBlock(const Token& name)
			:name(name) {
		}
		void MaterialPropertiesBlock::Accept(BlockVisitor* blockVisitor) {
			blockVisitor->VisitMaterialPropertiesBlock(this);
		}
		const Token& MaterialPropertiesBlock::GetName() const {
			return name;
		}
		bool MaterialPropertiesBlock::HasMatPropDecl(std::string_view matPropName) {
			auto pred = [=](const std::shared_ptr<MatPropDecl>& matPropDecl) {
				return matPropName == matPropDecl->GetName().lexeme;
			};
			auto searchRes = std::find_if(matProps.begin(), matProps.end(), pred);
			if (searchRes == matProps.end()) {
				return false;
			}
			return true;
		}
		void MaterialPropertiesBlock::AddMatPropDecl(std::shared_ptr<MatPropDecl> matPropDecl) {
			matProps.push_back(matPropDecl);
		}
		std::shared_ptr<MatPropDecl> MaterialPropertiesBlock::GetMatPropDecl(std::string_view matPropName) const {
			auto pred = [=](const std::shared_ptr<MatPropDecl>& matPropDecl) {
				return matPropName == matPropDecl->GetName().lexeme;
			};
			auto searchRes = std::find_if(matProps.begin(), matProps.end(), pred);
			assert(searchRes != matProps.end() && "Check the existence of the material property declaration first!");
			if (searchRes == matProps.end()) {
				return std::shared_ptr<MatPropDecl>{};
			}
			return *searchRes;
		}
		const std::vector<std::shared_ptr<MatPropDecl>>& MaterialPropertiesBlock::GetMatPropDecls() const {
			return matProps;
		}

		void VertexInputLayoutBlock::Accept(BlockVisitor* blockVisitor) {
			blockVisitor->VisitVertexInputLayoutBlock(this);
		}
		bool VertexInputLayoutBlock::HasVertexAttribDecl(std::string_view vertexAttribName) {
			auto pred = [=](const std::shared_ptr<VertexAttribDecl>& vertexAttribDecl) {
				return vertexAttribName == vertexAttribDecl->GetName().lexeme;
				};
			auto searchRes = std::find_if(vertexAttribs.begin(), vertexAttribs.end(), pred);
			if (searchRes == vertexAttribs.end()) {
				return false;
			}
			return true;
		}
		void VertexInputLayoutBlock::AddVertexAttribDecl(std::shared_ptr<VertexAttribDecl> vertexAttribDecl) {
			vertexAttribs.push_back(vertexAttribDecl);
		}
		std::shared_ptr<VertexAttribDecl> VertexInputLayoutBlock::GetVertexAttribDecl(std::string_view vertexAttribName) const {
			auto pred = [=](const std::shared_ptr<VertexAttribDecl>& vertexAttribDecl) {
				return vertexAttribName == vertexAttribDecl->GetName().lexeme;
			};
			auto searchRes = std::find_if(vertexAttribs.begin(), vertexAttribs.end(), pred);
			assert(searchRes != vertexAttribs.end() && "Check the existence of the vertex attribute declaration first!");
			if (searchRes == vertexAttribs.end()) {
				return std::shared_ptr<VertexAttribDecl>{};
			}
			return *searchRes;
		}
		const std::vector<std::shared_ptr<VertexAttribDecl>>& VertexInputLayoutBlock::GetAttribDecls() const {
			return vertexAttribs;
		}

		ShaderBlock::ShaderBlock(std::shared_ptr<TransUnit> transUnit, ShaderType shaderType)
			: transUnit(transUnit), shaderType(shaderType) {
		}
		void ShaderBlock::Accept(BlockVisitor* blockVisitor) {
			blockVisitor->VisitShaderBlock(this);
		}
		std::shared_ptr<TransUnit> ShaderBlock::GetTranslationUnit() const {
			return transUnit;
		}
		ShaderType ShaderBlock::GetShaderType() const {
			return shaderType;
		}

		VertexAttribDecl::VertexAttribDecl(const TypeSpec& typeSpec, const Token& name, const Token& channel)
			: typeSpec(typeSpec), name(name), channel(channel) {
		}
		const TypeSpec& VertexAttribDecl::GetTypeSpec() const {
			return typeSpec;
		}
		const Token& VertexAttribDecl::GetName() const {
			return name;
		}
		const Token& VertexAttribDecl::GetChannel() const {
			return channel;
		}

		MatPropDecl::MatPropDecl(const Token& type, const Token& name)
			: type(type), name(name) {
		}
		const Token& MatPropDecl::GetType() const {
			return type;
		}
		const Token& MatPropDecl::GetName() const {
			return name;
		}

		std::vector<std::shared_ptr<VarDecl>> CreateVertexAttribDecls(const VertexInputLayoutDesc& vertexInputLayout) {
			std::vector<std::shared_ptr<VarDecl>> vertexAttribs(vertexInputLayout.GetVertexAttribCount());
			for (size_t i = 0; i < vertexInputLayout.GetVertexAttribCount(); i++) {
				vertexAttribs[i] = CreateVertexAttribDecl(vertexInputLayout.attributes[i]);
			}
			return vertexAttribs;
		}
		std::shared_ptr<VarDecl> CreateVertexAttribDecl(const VertexAttribDesc& vertexAttrib) {
			Token inTok{};
			inTok.tokenType = TokenType::IN;
			inTok.lexeme = TokenTypeToLexeme(inTok.tokenType);

			Token locationTok{};
			locationTok.tokenType = TokenType::IDENTIFIER;
			locationTok.lexeme = "location";
			LayoutQualifier locationLayoutQual{};
			locationLayoutQual.name = locationTok;
			// Be aware that when the vertex attribute type is DOUBLE and
			// the dimension is 3 or 4, the attribute will take 2 vector slots!
			// Need to come up with some logic that would handle that.
			locationLayoutQual.value = GetVertexAttribChannelNum(vertexAttrib.channel);

			std::list<LayoutQualifier> layoutQualifiers;
			layoutQualifiers.push_back(locationLayoutQual);

			Token typeTok{};
			// typeTok.tokenType = VertexAttribTypeToTokenType(vertexAttrib.type);
			/*
			// Vertex attribute type can be a matrix, in which case the dimension will exceed 4.
			// The function used below doesn't support that though, so it won't work.
			// We need to know whether the vertex attribute type is a vector or a matrix somehow.
			typeTok.tokenType = FundamentalTypeToVectorType(VertexAttribTypeToTokenType(vertexAttrib.type),
															vertexAttrib.dimension);
			*/
			// Simply use the type that the user specified in the shader.
			typeTok.tokenType = GlslBasicTypeToTokenType(vertexAttrib.typeName);
			typeTok.lexeme = TokenTypeToLexeme(typeTok.tokenType);
			FullSpecType varType{};
			varType.qualifier.layout = layoutQualifiers;
			varType.qualifier.storage = inTok;
			varType.specifier.type = typeTok;

			Token varName{};
			varName.tokenType = TokenType::IDENTIFIER;
			varName.lexeme = vertexAttrib.name;

			std::shared_ptr<VarDecl> attribVarDecl = std::make_shared<VarDecl>(varType, varName);
			return attribVarDecl;
		}
		std::shared_ptr<InterfaceBlockDecl> CreateUniformInterfaceBlockDecl(const MaterialPropsDesc& matProps) {
			Token uniformTok{};
			uniformTok.lexeme = "uniform";
			uniformTok.tokenType = TokenType::UNIFORM;
			TypeQual uniformQual{};
			uniformQual.storage = uniformTok;

			Token interfaceBlockName{};
			interfaceBlockName.lexeme = matProps.name;
			interfaceBlockName.tokenType = TokenType::IDENTIFIER;

			std::shared_ptr<InterfaceBlockDecl> uniformInterfaceBlock =
				std::make_shared<InterfaceBlockDecl>(interfaceBlockName, uniformQual);
			for (size_t i = 0; i < matProps.GetMatPropCount(); i++) {
				uniformInterfaceBlock->AddField(CreateInterfaceBlockVarDecl(matProps.matProps[i]));
			}
			return uniformInterfaceBlock;
		}
		std::shared_ptr<VarDecl> CreateInterfaceBlockVarDecl(const MaterialPropDesc& matProp) {
			Token typeTok{};
			typeTok.tokenType = MaterialPropertyTypeToTokenType(matProp.type);
			typeTok.lexeme = TokenTypeToLexeme(typeTok.tokenType);

			FullSpecType varType{};
			varType.specifier.type = typeTok;

			Token varName{};
			varName.tokenType = TokenType::IDENTIFIER;
			varName.lexeme = matProp.name;

			std::shared_ptr<VarDecl> attribVarDecl = std::make_shared<VarDecl>(varType, varName);
			return attribVarDecl;
		}

	}
}