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

		ColorAttachmentDecl::ColorAttachmentDecl(const TypeSpec& typeSpec, const Token& name, const Token& channel)
			: typeSpec(typeSpec), name(name), channel(channel) {
		}
		const TypeSpec& ColorAttachmentDecl::GetTypeSpec() const {
			return typeSpec;
		}
		const Token& ColorAttachmentDecl::GetName() const {
			return name;
		}
		const Token& ColorAttachmentDecl::GetChannel() const {
			return channel;
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

		void ColorAttachmentsBlock::Accept(BlockVisitor* blockVisitor) {
			blockVisitor->VisitColorAttachmentsBlock(this);
		}
		bool ColorAttachmentsBlock::HasColorAttachmentDecl(std::string_view colorAttachmentName) {
			auto pred = [=](const std::shared_ptr<ColorAttachmentDecl>& colorAttachmentDecl) {
				return colorAttachmentName == colorAttachmentDecl->GetName().lexeme;
			};
			auto searchRes = std::find_if(colorAttachments.begin(), colorAttachments.end(), pred);
			if (searchRes == colorAttachments.end()) {
				return false;
			}
			return true;
		}
		void ColorAttachmentsBlock::AddColorAttachmentDecl(std::shared_ptr<ColorAttachmentDecl> colorAttachmentDecl) {
			colorAttachments.push_back(colorAttachmentDecl);
		}
		std::shared_ptr<ColorAttachmentDecl> ColorAttachmentsBlock::GetColorAttachmentDecl(std::string_view colorAttachmentName) const {
			auto pred = [=](const std::shared_ptr<ColorAttachmentDecl>& colorAttachmentDecl) {
				return colorAttachmentName == colorAttachmentDecl->GetName().lexeme;
			};
			auto searchRes = std::find_if(colorAttachments.begin(), colorAttachments.end(), pred);
			assert(searchRes != colorAttachments.end() && "Check the existence of the color attachment declaration first!");
			if (searchRes == colorAttachments.end()) {
				return std::shared_ptr<ColorAttachmentDecl>{};
			}
			return *searchRes;
		}
		const std::vector<std::shared_ptr<ColorAttachmentDecl>>& ColorAttachmentsBlock::GetColorAttachments() const {
			return colorAttachments;
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

		VertexInputLayoutDesc GenerateVertexInputLayoutDesc(VertexInputLayoutBlock* vertexInputLayoutBlock) {
			VertexInputLayoutDesc vertexInputLayout{};
			uint32_t offset{0};
			for (const std::shared_ptr<VertexAttribDecl>& vertexAttribDecl : vertexInputLayoutBlock->GetAttribDecls()) {
				VertexAttribDesc vertexAttrib = GenerateVertexAttribDesc(vertexAttribDecl.get(), offset);
				vertexInputLayout.AddVertexAttrib(vertexAttrib);
				offset += vertexAttrib.GetVertexAttributeSize();
			}
			return vertexInputLayout;
		}
		VertexAttribDesc GenerateVertexAttribDesc(VertexAttribDecl* vertexAttribDecl, uint32_t offset) {
			const TypeSpec& typeSpec = vertexAttribDecl->GetTypeSpec();
			const Token& name = vertexAttribDecl->GetName();
			const Token& channel = vertexAttribDecl->GetChannel();

			VertexAttribDesc vertexAttrib{};
			vertexAttrib.name = name.lexeme;
			vertexAttrib.dimension = static_cast<uint32_t>(GetComponentCount(typeSpec.type.tokenType));
			vertexAttrib.offset = offset;
			vertexAttrib.channel = IdentifierTokenToVertexAttribChannel(channel);
			vertexAttrib.type = TokenTypeToVertexAttribType(typeSpec.type.tokenType);
			return vertexAttrib;
		}

		ColorAttachments GenerateColorAttachments(ColorAttachmentsBlock* colorAttachmentsBlock) {
			ColorAttachments colorAttachments{};
			for (const std::shared_ptr<ColorAttachmentDecl>& colorAttachment : colorAttachmentsBlock->GetColorAttachments()) {
				ColorAttachmentDesc colorAttachmentDesc = GenerateColorAttachmentDesc(colorAttachment.get());
				colorAttachments.AddColorAttachment(colorAttachmentDesc);
			}
			return colorAttachments;
		}
		ColorAttachmentDesc GenerateColorAttachmentDesc(ColorAttachmentDecl* colorAttachmentDecl) {
			const TypeSpec& typeSpec = colorAttachmentDecl->GetTypeSpec();
			const Token& name = colorAttachmentDecl->GetName();
			const Token& channel = colorAttachmentDecl->GetChannel();

			ColorAttachmentDesc colorAttachmentDesc{};
			colorAttachmentDesc.name = name.lexeme;
			colorAttachmentDesc.channel = IdentifierTokenToColorAttachmentChannel(channel);
			colorAttachmentDesc.type = TokenTypeToColorAttachmentType(typeSpec.type.tokenType);
			return colorAttachmentDesc;
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
			typeTok.tokenType = VertexAttribTypeToTokenType(vertexAttrib.type);
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
		std::shared_ptr<InterfaceBlockDecl> CreateUniformInterfaceBlockDecl(const MaterialProps& matProps) {
			Token uniformTok{};
			uniformTok.tokenType = TokenType::UNIFORM;
			uniformTok.lexeme = TokenTypeToLexeme(uniformTok.tokenType);
			TypeQual uniformQual{};
			uniformQual.storage = uniformTok;

			Token interfaceBlockNameTok{};
			interfaceBlockNameTok.tokenType = TokenType::IDENTIFIER;
			interfaceBlockNameTok.lexeme = matProps.name;

			std::shared_ptr<InterfaceBlockDecl> uniformInterfaceBlock =
				std::make_shared<InterfaceBlockDecl>(interfaceBlockNameTok, uniformQual);

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

		std::vector<std::shared_ptr<VarDecl>> CreateVertexAttribVarDecls(std::shared_ptr<VertexInputLayoutBlock> vertexInputLayout) {
			const std::vector<std::shared_ptr<VertexAttribDecl>> attribs = vertexInputLayout->GetAttribDecls();
			size_t vertexAttribCount = attribs.size();
			std::vector<std::shared_ptr<VarDecl>> vertexAttribs(vertexAttribCount);
			for (size_t i = 0; i < vertexAttribCount; i++) {
				vertexAttribs[i] = CreateVertexAttribVarDecl(attribs[i]);
			}
			return vertexAttribs;
		}
		std::shared_ptr<VarDecl> CreateVertexAttribVarDecl(std::shared_ptr<VertexAttribDecl> vertexAttribDecl) {
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
			const Token& channelTok = vertexAttribDecl->GetChannel();
			VertexAttribChannel vertexAttribChannel = IdentifierTokenToVertexAttribChannel(channelTok);
			locationLayoutQual.value = GetVertexAttribChannelNum(vertexAttribChannel);

			std::list<LayoutQualifier> layoutQualifiers;
			layoutQualifiers.push_back(locationLayoutQual);

			const TypeSpec& typeSpec = vertexAttribDecl->GetTypeSpec();
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
			typeTok.tokenType = typeSpec.type.tokenType;
			typeTok.lexeme = TokenTypeToLexeme(typeTok.tokenType);
			FullSpecType varType{};
			varType.qualifier.layout = layoutQualifiers;
			varType.qualifier.storage = inTok;
			varType.specifier.type = typeTok;

			const Token& nameTok = vertexAttribDecl->GetName();
			Token varName{};
			varName.tokenType = TokenType::IDENTIFIER;
			varName.lexeme = nameTok.lexeme;

			std::shared_ptr<VarDecl> attribVarDecl = std::make_shared<VarDecl>(varType, varName);
			return attribVarDecl;
		}
		std::shared_ptr<InterfaceBlockDecl> CreateInterfaceBlockDecl(std::shared_ptr<MaterialPropertiesBlock> matPropBlock) {
			Token uniformTok{};
			uniformTok.tokenType = TokenType::UNIFORM;
			uniformTok.lexeme = TokenTypeToLexeme(uniformTok.tokenType);
			TypeQual uniformQual{};
			uniformQual.storage = uniformTok;

			Token interfaceBlockNameTok{};
			interfaceBlockNameTok.tokenType = TokenType::IDENTIFIER;
			interfaceBlockNameTok.lexeme = matPropBlock->GetName().lexeme;

			std::shared_ptr<InterfaceBlockDecl> uniformInterfaceBlock =
				std::make_shared<InterfaceBlockDecl>(interfaceBlockNameTok, uniformQual);

			for (const std::shared_ptr<MatPropDecl>& matPropDecl : matPropBlock->GetMatPropDecls()) {
				uniformInterfaceBlock->AddField(CreateInterfaceBlockVarDecl(matPropDecl));
			}
			return uniformInterfaceBlock;
		}
		std::shared_ptr<VarDecl> CreateInterfaceBlockVarDecl(std::shared_ptr<MatPropDecl> matPropDecl) {
			Token typeTok{};
			typeTok.tokenType = MapMaterialPropertyType(matPropDecl->GetType().tokenType);
			typeTok.lexeme = TokenTypeToLexeme(typeTok.tokenType);

			FullSpecType varType{};
			varType.specifier.type = typeTok;

			Token varName{};
			varName.tokenType = TokenType::IDENTIFIER;
			varName.lexeme = matPropDecl->GetName().lexeme;

			std::shared_ptr<VarDecl> attribVarDecl = std::make_shared<VarDecl>(varType, varName);
			return attribVarDecl;
		}

		std::vector<std::shared_ptr<VarDecl>> CreateColorAttachmentVarDecls(std::shared_ptr<ColorAttachmentsBlock> colorAttachments) {
			const std::vector<std::shared_ptr<ColorAttachmentDecl>> colorAttachmentDescs = colorAttachments->GetColorAttachments();
			std::vector<std::shared_ptr<VarDecl>> varDecls(colorAttachmentDescs.size());
			size_t i{0};
			for (const std::shared_ptr<ColorAttachmentDecl> colorAttachment : colorAttachmentDescs) {
				const TypeSpec& typeSpec = colorAttachment->GetTypeSpec();
				const Token& name = colorAttachment->GetName();
				const Token& channel = colorAttachment->GetChannel();
				
				ColorAttachmentDesc colorAttachmentDesc{};
				colorAttachmentDesc.name = name.lexeme;
				colorAttachmentDesc.channel = IdentifierTokenToColorAttachmentChannel(channel);
				colorAttachmentDesc.type = TokenTypeToColorAttachmentType(typeSpec.type.tokenType);

				varDecls[i] = CreateColorAttachmentVarDecl(colorAttachmentDesc);
			}
			return varDecls;
		}
		std::vector<std::shared_ptr<VarDecl>> CreateColorAttachmentVarDecls(const ColorAttachments& colorAttachments) {
			size_t colorAttachmentCount = colorAttachments.GetColorAttachmentCount();
			std::vector<std::shared_ptr<VarDecl>> attachments(colorAttachmentCount);
			std::vector<ColorAttachmentDesc> colorAttachmentDescs = colorAttachments.GetColorAttachments();
			for (size_t i = 0; i < colorAttachmentCount; i++) {
				attachments[i] = CreateColorAttachmentVarDecl(colorAttachmentDescs[i]);
			}
			return attachments;
		}
		std::shared_ptr<VarDecl> CreateColorAttachmentVarDecl(const ColorAttachmentDesc& colorAttachmentDesc) {
			Token outTok{};
			outTok.tokenType = TokenType::OUT;
			outTok.lexeme = TokenTypeToLexeme(outTok.tokenType);

			Token locationTok{};
			locationTok.tokenType = TokenType::IDENTIFIER;
			locationTok.lexeme = "location";
			LayoutQualifier locationLayoutQual{};
			locationLayoutQual.name = locationTok;
			locationLayoutQual.value = GetColorAttachmentChannelNum(colorAttachmentDesc.channel);

			std::list<LayoutQualifier> layoutQualifiers;
			layoutQualifiers.push_back(locationLayoutQual);

			Token typeTok{};
			typeTok.tokenType = ColorAttachmentTypeToTokenType(colorAttachmentDesc.type);
			typeTok.lexeme = TokenTypeToLexeme(typeTok.tokenType);

			FullSpecType varType{};
			varType.qualifier.layout = layoutQualifiers;
			varType.qualifier.storage = outTok;
			varType.specifier.type = typeTok;

			Token varName{};
			varName.tokenType = TokenType::IDENTIFIER;
			varName.lexeme = colorAttachmentDesc.name;

			std::shared_ptr<VarDecl> attribVarDecl = std::make_shared<VarDecl>(varType, varName);
			return attribVarDecl;
		}

	}
}