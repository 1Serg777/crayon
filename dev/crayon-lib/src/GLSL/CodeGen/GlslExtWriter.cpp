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
				GlslBasicType glslBasicType = TokenTypeToGlslBasicType(typeSpec.type.tokenType);

				VertexAttribDesc vertexAttrib{};
				vertexAttrib.name = name.lexeme;
				vertexAttrib.typeName = TokenTypeToGlslBasicType(typeSpec.type.tokenType);
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
		
	}
}