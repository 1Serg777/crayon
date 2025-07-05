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

			std::shared_ptr<ShaderProgram> GetShaderProgram() const;

			// Block vist methods
			void VisitShaderProgramBlock(ShaderProgramBlock* programBlock) override;
			void VisitFixedStagesConfigBlock(FixedStagesConfigBlock* fixedStagesConfigBlock) override;
			void VisitMaterialPropertiesBlock(MaterialPropertiesBlock* materialPropertiesBlock) override;
			void VisitVertexInputLayoutBlock(VertexInputLayoutBlock* vertexInputLayoutBlock) override;
			void VisitShaderBlock(ShaderBlock* shaderBlock) override;

		private:
			std::vector<std::shared_ptr<VarDecl>> CreateVertexAttribDecls(const VertexInputLayoutDesc& vertexInputLayout);
			std::shared_ptr<VarDecl> CreateVertexAttribDecl(const VertexAttribDesc& vertexAttrib);

			std::shared_ptr<InterfaceBlockDecl> CreateUniformInterfaceBlockDecl(const MaterialPropsDesc& matProps);
			std::shared_ptr<VarDecl> CreateInterfaceBlockVarDecl(const MaterialPropDesc& matProp);

			std::shared_ptr<ShaderProgram> shaderProgram;
			std::unique_ptr<GlslWriter> glslWriter;
		};

	}
}