#pragma once

#include "GLSL/Token.h"
#include "GLSL/Type.h"
#include "GLSL/AST/Decl.h"
#include "GLSL/Reflect/ReflectCommon.h"

#include <memory>
#include <string_view>
#include <vector>

namespace crayon {
	namespace glsl {

		class VertexAttribDecl;
		class MatPropDecl;

		class Block;
		class ShaderProgramBlock;
		class FixedStagesConfigBlock;
		class MaterialPropertiesBlock;
		class VertexInputLayoutBlock;
		class ShaderBlock;

		class BlockVisitor {
		public:
			virtual void VisitShaderProgramBlock(ShaderProgramBlock* programBlock) = 0;
			virtual void VisitFixedStagesConfigBlock(FixedStagesConfigBlock* fixedStagesConfigBlock) = 0;
			virtual void VisitMaterialPropertiesBlock(MaterialPropertiesBlock* materialPropertiesBlock) = 0;
			virtual void VisitVertexInputLayoutBlock(VertexInputLayoutBlock* vertexInputLayoutBlock) = 0;
			virtual void VisitShaderBlock(ShaderBlock* shaderBlock) = 0;
		};

		class Block {
		public:
			Block() = default;
			virtual ~Block() = default;
			virtual void Accept(BlockVisitor* blockVisitor) = 0;
		};

		class ShaderProgramBlock : public Block {
		public:
			ShaderProgramBlock() = default;
			ShaderProgramBlock(const Token& programName);

			void Accept(BlockVisitor* blockVisitor) override;

			void AddBlock(std::shared_ptr<Block> block);
			bool BlockListEmpty() const;
			const std::vector<std::shared_ptr<Block>> GetBlocks();

			bool ShaderProgramNameEmpty() const;
			std::string_view GetShaderProgramName() const;

		private:
			std::vector<std::shared_ptr<Block>> blocks;
			Token programName;
		};

		class FixedStagesConfigBlock : public Block {
		public:
			void Accept(BlockVisitor* blockVisitor) override;
		};

		class MaterialPropertiesBlock : public Block {
		public:
			MaterialPropertiesBlock(const Token& name);

			void Accept(BlockVisitor* blockVisitor) override;

			const Token& GetName() const;

			bool HasMatPropDecl(std::string_view matPropName);
			void AddMatPropDecl(std::shared_ptr<MatPropDecl> matPropDecl);
			std::shared_ptr<MatPropDecl> GetMatPropDecl(std::string_view matPropName) const;
			const std::vector<std::shared_ptr<MatPropDecl>>& GetMatPropDecls() const;

		private:
			Token name;
			std::vector<std::shared_ptr<MatPropDecl>> matProps;
		};

		class VertexInputLayoutBlock : public Block {
		public:
			void Accept(BlockVisitor* blockVisitor) override;

			bool HasVertexAttribDecl(std::string_view vertexAttribName);
			void AddVertexAttribDecl(std::shared_ptr<VertexAttribDecl> vertexAttribDecl);
			std::shared_ptr<VertexAttribDecl> GetVertexAttribDecl(std::string_view vertexAttribName) const;
			const std::vector<std::shared_ptr<VertexAttribDecl>>& GetAttribDecls() const;

		private:
			std::vector<std::shared_ptr<VertexAttribDecl>> vertexAttribs;
		};

		class ShaderBlock : public Block {
		public:
			ShaderBlock(std::shared_ptr<TransUnit> transUnit, ShaderType shaderType);

			void Accept(BlockVisitor* blockVisitor) override;

			std::shared_ptr<TransUnit> GetTranslationUnit() const;
			ShaderType GetShaderType() const;

		private:
			std::shared_ptr<TransUnit> transUnit;
			ShaderType shaderType;
		};

		// Block declarations.
		// Vertex input layout declrations.

		class VertexAttribDecl {
		public:
			VertexAttribDecl(const TypeSpec& typeSpec, const Token& name, const Token& channel);

			const TypeSpec& GetTypeSpec() const;
			const Token& GetName() const;
			const Token& GetChannel() const;

		private:
			TypeSpec typeSpec;
			Token name;
			Token channel;
		};

		// Material property declarations.

		class MatPropDecl {
		public:
			MatPropDecl(const Token& type, const Token& name);

			const Token& GetType() const;
			const Token& GetName() const;

		private:
			Token type;
			Token name;
		};

		// Block statements.
		// Fixed stages config statements.

		std::vector<std::shared_ptr<VarDecl>> CreateVertexAttribDecls(const VertexInputLayoutDesc& vertexInputLayout);
		std::shared_ptr<VarDecl> CreateVertexAttribDecl(const VertexAttribDesc& vertexAttrib);
		std::shared_ptr<InterfaceBlockDecl> CreateUniformInterfaceBlockDecl(const MaterialPropsDesc& matProps);
		std::shared_ptr<VarDecl> CreateInterfaceBlockVarDecl(const MaterialPropDesc& matProp);

		std::vector<std::shared_ptr<VarDecl>> CreateVertexAttribVarDecls(std::shared_ptr<VertexInputLayoutBlock> vertexInputLayout);
		std::shared_ptr<VarDecl> CreateVertexAttribVarDecl(std::shared_ptr<VertexAttribDecl> vertexAttribDecl);
		std::shared_ptr<InterfaceBlockDecl> CreateInterfaceBlockDecl(std::shared_ptr<MaterialPropertiesBlock> matPropBlock);
		std::shared_ptr<VarDecl> CreateInterfaceBlockVarDecl(std::shared_ptr<MatPropDecl> matPropDecl);

	}
}