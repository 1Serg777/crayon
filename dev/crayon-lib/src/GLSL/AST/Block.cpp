#include "GLSL/AST/Block.h"

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

		void MaterialPropertiesBlock::Accept(BlockVisitor* blockVisitor) {
			blockVisitor->VisitMaterialPropertiesBlock(this);
		}

		void VertexInputLayoutBlock::Accept(BlockVisitor* blockVisitor) {
			blockVisitor->VisitVertexInputLayoutBlock(this);
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

	}
}