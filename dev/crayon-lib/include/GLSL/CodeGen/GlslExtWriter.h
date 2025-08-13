#pragma once

#include "GLSL/AST/Block.h"
#include "GLSL/AST/Decl.h"
#include "GLSL/CodeGen/GlslWriter.h"
#include "GLSL/Reflect/ShaderProgram.h"

#include <memory>
#include <string>

namespace crayon {
	namespace glsl {

		class GlslExtWriter : public BlockVisitor {
		public:
			GlslExtWriter(const GlslWriterConfig& config);

			std::shared_ptr<ShaderProgram> CompileToGlsl(ShaderProgramBlock* program);

		private:
			// Block vist methods
			void VisitShaderProgramBlock(ShaderProgramBlock* programBlock) override;
			void VisitFixedStagesConfigBlock(FixedStagesConfigBlock* fixedStagesConfigBlock) override;
			void VisitMaterialPropertiesBlock(MaterialPropertiesBlock* materialPropertiesBlock) override;
			void VisitVertexInputLayoutBlock(VertexInputLayoutBlock* vertexInputLayoutBlock) override;
			void VisitColorAttachmentsBlock(ColorAttachmentsBlock* colorAttachmentsBlock) override;
			void VisitShaderBlock(ShaderBlock* shaderBlock) override;

			std::shared_ptr<ShaderProgram> shaderProgram;
			std::unique_ptr<GlslWriter> glslWriter;
		};

	}
}