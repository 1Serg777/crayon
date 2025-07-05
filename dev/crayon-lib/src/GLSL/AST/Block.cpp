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

		MaterialPropertiesBlock::MaterialPropertiesBlock(const Token& name)
			:name(name) {
		}
		void MaterialPropertiesBlock::Accept(BlockVisitor* blockVisitor) {
			blockVisitor->VisitMaterialPropertiesBlock(this);
		}
		const Token& MaterialPropertiesBlock::GetName() const {
			return name;
		}
		void MaterialPropertiesBlock::AddMatPropDecl(std::shared_ptr<MatPropDecl> matPropDecl) {
			matPropDecls.push_back(matPropDecl);
		}
		const std::vector<std::shared_ptr<MatPropDecl>>& MaterialPropertiesBlock::GetMatPropDecls() const {
			return matPropDecls;
		}

		void VertexInputLayoutBlock::Accept(BlockVisitor* blockVisitor) {
			blockVisitor->VisitVertexInputLayoutBlock(this);
		}
		void VertexInputLayoutBlock::AddAttribDecl(std::shared_ptr<VertexAttribDecl> attribDecl) {
			attribDecls.push_back(attribDecl);
		}
		const std::vector<std::shared_ptr<VertexAttribDecl>>& VertexInputLayoutBlock::GetAttribDecls() const {
			return attribDecls;
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

	}
}