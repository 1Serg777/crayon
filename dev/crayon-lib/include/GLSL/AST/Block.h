#pragma once

#include "GLSL/Token.h"
#include "GLSL/AST/Decl.h"
#include "GLSL/Reflect/ReflectCommon.h"

#include <memory>
#include <string_view>
#include <vector>

namespace crayon {
	namespace glsl {

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
			void Accept(BlockVisitor* blockVisitor) override;
		};

		class VertexInputLayoutBlock : public Block {
		public:
			void Accept(BlockVisitor* blockVisitor) override;
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

	}
}