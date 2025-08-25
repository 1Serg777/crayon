#include "GLSL/CodeGen/GlslExtWriter.h"

namespace crayon {
	namespace glsl {

		GlslExtWriter::GlslExtWriter(const GlslWriterConfig& config) {
			glslWriter = std::make_unique<GlslWriter>(config);
		}

		std::shared_ptr<ShaderProgram> GlslExtWriter::CompileToGlsl(ShaderProgramBlock* program) {
			std::string_view name = program->GetShaderProgramName();
			shaderProgram = std::make_shared<ShaderProgram>(name);
			program->Accept(this);
			return shaderProgram;
		}

		void GlslExtWriter::VisitShaderProgramBlock(ShaderProgramBlock* programBlock) {
			for (const std::shared_ptr<Block>& block : programBlock->GetBlocks()) {
				block->Accept(this);
			}
		}
		void GlslExtWriter::VisitFixedStagesConfigBlock(FixedStagesConfigBlock* fixedStagesConfigBlock) {
			// TODO
		}
		void GlslExtWriter::VisitMaterialPropertiesBlock(MaterialPropertiesBlock* materialPropertiesBlock) {
			MaterialProps matPropsDesc{};
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
			shaderProgram->SetVertexInputLayout(GenerateVertexInputLayoutDesc(vertexInputLayoutBlock));
		}
		void GlslExtWriter::VisitColorAttachmentsBlock(ColorAttachmentsBlock* colorAttachmentsBlock) {
			shaderProgram->SetColorAttachments(GenerateColorAttachments(colorAttachmentsBlock));
		}
		void GlslExtWriter::VisitShaderBlock(ShaderBlock* shaderBlock) {
			glslWriter->ResetInternalState();
			glslWriter->PrintGlslVersionLine();
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
					const MaterialProps& matProps = shaderProgram->GetMaterialProps();
					if (!matProps.IsEmpty()) {
						std::shared_ptr<InterfaceBlockDecl> matPropsIntBlock = CreateUniformInterfaceBlockDecl(matProps);
						matPropsIntBlock->Accept(glslWriter.get());
						glslWriter->PrintNewLine();
					}
					// TEST

					std::shared_ptr<TransUnit> transUnit = shaderBlock->GetTranslationUnit();
					shaderProgram->SetShaderModuleGlsl(ShaderType::VS,
						                               glslWriter->CompileTranslationUnitToGlsl(transUnit.get()));
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
					// Print color attachments.
					const ColorAttachments& colorAttachments = shaderProgram->GetColorAttachments();
					for (std::shared_ptr<VarDecl>& vertexAttribDecl : CreateColorAttachmentVarDecls(colorAttachments)) {
						vertexAttribDecl->Accept(glslWriter.get());
						glslWriter->PrintNewLine();
					}
					// Print material properties (uniform interface block is used).
					const MaterialProps& matProps = shaderProgram->GetMaterialProps();
					if (!matProps.IsEmpty()) {
						std::shared_ptr<InterfaceBlockDecl> matPropsIntBlock = CreateUniformInterfaceBlockDecl(matProps);
						matPropsIntBlock->Accept(glslWriter.get());
						glslWriter->PrintNewLine();
					}
					// Print the rest of the code:
					std::shared_ptr<TransUnit> transUnit = shaderBlock->GetTranslationUnit();
					shaderProgram->SetShaderModuleGlsl(ShaderType::FS,
						                               glslWriter->CompileTranslationUnitToGlsl(transUnit.get()));
					break;
				}
			}
		}
		
	}
}