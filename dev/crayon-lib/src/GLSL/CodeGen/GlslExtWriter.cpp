#include "GLSL/CodeGen/GlslExtWriter.h"

namespace crayon {
	namespace glsl {

		GlslExtWriter::GlslExtWriter(const GlslWriterConfig& config) {
			glslWriter = std::make_unique<GlslWriter>(config);
		}

		std::shared_ptr<ShaderProgram> GlslExtWriter::GetShaderProgram() const {
			return shaderProgram;
		}

		void GlslExtWriter::VisitShaderProgramBlock(ShaderProgramBlock* programBlock) {
			std::string_view name = programBlock->GetShaderProgramName();
			shaderProgram = std::make_shared<ShaderProgram>(name);
			for (const std::shared_ptr<Block>& block : programBlock->GetBlocks()) {
				block->Accept(this);
			}
		}
		void GlslExtWriter::VisitFixedStagesConfigBlock(FixedStagesConfigBlock* fixedStagesConfigBlock) {
			// TODO
		}
		void GlslExtWriter::VisitMaterialPropertiesBlock(MaterialPropertiesBlock* materialPropertiesBlock) {
			MaterialPropsDesc matPropsDesc{};
			matPropsDesc.name = ExtractStringLiteral(materialPropertiesBlock->GetName());
			for (const std::shared_ptr<MatPropDecl>& matPropDecl : materialPropertiesBlock->GetMatPropDecls()) {
				const Token& type = matPropDecl->GetType();
				const Token& name = matPropDecl->GetName();
				TokenType tokenType = MapMaterialPropertyType(type.tokenType);

				MaterialPropDesc matPropDesc{};
				matPropDesc.name = name.lexeme;
				// matPropDesc.visibleName; // TODO
				matPropDesc.type = TokenTypeToMaterialPropertyType(tokenType);
				// matPropDesc.showInEditor; // TODO

				matPropsDesc.AddMatProp(matPropDesc);
			}
			// TODO: more than one MaterialProperties block is allowed!
			shaderProgram->SetMaterialProps(matPropsDesc);
		}
		void GlslExtWriter::VisitVertexInputLayoutBlock(VertexInputLayoutBlock* vertexInputLayoutBlock) {
			VertexInputLayoutDesc vertexInputLayout{};
			uint32_t offset{0};
			for (const std::shared_ptr<VertexAttribDecl>& vertexAttribDecl : vertexInputLayoutBlock->GetAttribDecls()) {
				const TypeSpec& typeSpec = vertexAttribDecl->GetTypeSpec();
				const Token& name = vertexAttribDecl->GetName();
				const Token& channel = vertexAttribDecl->GetChannel();
				GlslBasicType glslBasicType = GetGlslBasicType(typeSpec.type.tokenType);

				VertexAttribDesc vertexAttrib{};
				vertexAttrib.name = name.lexeme;
				vertexAttrib.dimension = static_cast<uint32_t>(GetDimensionCountNonArray(glslBasicType));
				vertexAttrib.offset = offset;
				vertexAttrib.channel = IdentifierTokenToVertexAttribChannel(channel);
				vertexAttrib.type = TokenTypeToVertexAttribType(typeSpec.type.tokenType);

				vertexInputLayout.AddVertexAttrib(vertexAttrib);
			}
			shaderProgram->SetVertexInputLayout(vertexInputLayout);
		}
		void GlslExtWriter::VisitShaderBlock(ShaderBlock* shaderBlock) {
			glslWriter->ResetInternalState();
			ShaderType shaderType = shaderBlock->GetShaderType();
			switch (shaderType) {
				case ShaderType::VS: {
					// Print vertex input layout (variable declarations are used).
					const VertexInputLayoutDesc& vertexInputLayout = shaderProgram->GetVertexInputLayout();
					for (std::shared_ptr<VarDecl>& vertexAttribDecl : CreateVertexAttribDecls(vertexInputLayout)) {
						vertexAttribDecl->Accept(glslWriter.get());
						glslWriter->PrintNewLine();
					}

					// TEST
					std::shared_ptr<InterfaceBlockDecl> matPropsInterfaceBlock =
						CreateUniformInterfaceBlockDecl(shaderProgram->GetMaterialProps());
					matPropsInterfaceBlock->Accept(glslWriter.get());
					glslWriter->PrintNewLine();
					// TEST

					std::shared_ptr<TransUnit> transUnit = shaderBlock->GetTranslationUnit();
					transUnit->Accept(glslWriter.get());
					shaderProgram->SetVertexShaderSrc(glslWriter->GetSrcCodeStr());
					break;
				}
				case ShaderType::TCS:
					// TODO
					break;
				case ShaderType::TES:
					// TODO
					break;
				case ShaderType::GS:
					// TODO
					break;
				case ShaderType::FS: {
					// Print material properties (uniform interface block is used).
					std::shared_ptr<InterfaceBlockDecl> matPropsInterfaceBlock =
						CreateUniformInterfaceBlockDecl(shaderProgram->GetMaterialProps());
					matPropsInterfaceBlock->Accept(glslWriter.get());
					glslWriter->PrintNewLine();
					// Print the rest of the code:
					std::shared_ptr<TransUnit> transUnit = shaderBlock->GetTranslationUnit();
					transUnit->Accept(glslWriter.get());
					shaderProgram->SetFragmentShaderSrc(glslWriter->GetSrcCodeStr());
					break;
				}
			}
		}
		
		std::vector<std::shared_ptr<VarDecl>> GlslExtWriter::CreateVertexAttribDecls(const VertexInputLayoutDesc& vertexInputLayout) {
			std::vector<std::shared_ptr<VarDecl>> vertexAttribDecls(vertexInputLayout.GetVertexAttribCount());
			for (size_t i = 0; i < vertexInputLayout.GetVertexAttribCount(); i++) {
				vertexAttribDecls[i] = CreateVertexAttribDecl(vertexInputLayout.attributes[i]);
			}
			return vertexAttribDecls;
		}
		std::shared_ptr<VarDecl> GlslExtWriter::CreateVertexAttribDecl(const VertexAttribDesc& vertexAttrib) {
			Token typeTok{};
			typeTok.tokenType = VertexAttribTypeToTokenType(vertexAttrib.type);
			typeTok.lexeme = TokenTypeToLexeme(typeTok.tokenType);
			
			Token inTok{};
			inTok.tokenType = TokenType::IN;
			inTok.lexeme = TokenTypeToLexeme(inTok.tokenType);

			Token locationTok{};
			locationTok.tokenType = TokenType::IDENTIFIER;
			locationTok.lexeme = "location";
			LayoutQualifier locationLayoutQual{};
			locationLayoutQual.name = locationTok;
			locationLayoutQual.value = 0; // FIXME: automatically assign the location value!

			std::list<LayoutQualifier> layoutQualifiers;
			layoutQualifiers.push_back(locationLayoutQual);

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
		std::shared_ptr<InterfaceBlockDecl> GlslExtWriter::CreateUniformInterfaceBlockDecl(const MaterialPropsDesc& matProps) {
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
		std::shared_ptr<VarDecl> GlslExtWriter::CreateInterfaceBlockVarDecl(const MaterialPropDesc& matProp) {
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