#include "SPIRV/CodeGen/GlslToSpv.h"

#include <iomanip>
#include <sstream>

namespace crayon {
	namespace spirv {

		GlslToSpvGenerator::GlslToSpvGenerator(const GlslToSpvGeneratorConfig& config)
			: config(config) {
		}

		void GlslToSpvGenerator::CompileToSpv(glsl::ShaderProgramBlock* program) {
			// TODO

			// TEST
			// 1. OpNop
			SpvInstruction nop = OpNop();
			spvInstructions.push_back(nop);
			// 2. OpTypeXXX
			// 2.1 OpTypeVoid
			SpvInstruction opTypeVoid = OpTypeVoid();
			spvInstructions.push_back(opTypeVoid);
			// 2.2 OpTypeFloat
			SpvInstruction opTypeFloat = OpTypeFloat(32);
			spvInstructions.push_back(opTypeFloat);
			// 2.3 OpTypeVector
			// Declaring a vec4 type.
			SpvInstruction opTypeVector = OpTypeVector(opTypeFloat, 4);
			spvInstructions.push_back(opTypeVector);
		}

		std::vector<uint32_t> GlslToSpvGenerator::GenerateSpvBinary() {
			std::vector<uint32_t> spvBinary;
			// TODO
			return spvBinary;
		} 
		std::string GlslToSpvGenerator::GenerateSpvAsmText() {
			std::stringstream spvAsmText;
			size_t idWidth = CalcDigitCount(GetLastGeneratedSpvId()) + 1; // 1 is from the "%" character
			for (const SpvInstruction& spvInstruction : spvInstructions) {
				if (spvInstruction.HasResultId()) {
					spvAsmText << std::setw(idWidth);
				} else {
					size_t spvInstructionShift = idWidth + 3; // 3 is from the " = " sequence of characters
					std::fill_n(
						std::ostream_iterator<char>(spvAsmText),
						spvInstructionShift, ' ');
				}
				PrintSpvInstruction(spvAsmText, spvInstruction);
				spvAsmText << "\n";
			}
			return spvAsmText.str();
		}

		void GlslToSpvGenerator::VisitShaderProgramBlock(glsl::ShaderProgramBlock* programBlock) {
			// TODO
		}
		void GlslToSpvGenerator::VisitFixedStagesConfigBlock(glsl::FixedStagesConfigBlock* fixedStagesConfigBlock) {
			// TODO
		}
		void GlslToSpvGenerator::VisitMaterialPropertiesBlock(glsl::MaterialPropertiesBlock* materialPropertiesBlock) {
			// TODO
		}
		void GlslToSpvGenerator::VisitVertexInputLayoutBlock(glsl::VertexInputLayoutBlock* vertexInputLayoutBlock) {
			// TODO
		}
		void GlslToSpvGenerator::VisitColorAttachmentsBlock(glsl::ColorAttachmentsBlock* colorAttachmentsBlock) {
			// TODO
		}
		void GlslToSpvGenerator::VisitShaderBlock(glsl::ShaderBlock* shaderBlock) {
			// TODO
		}

		void GlslToSpvGenerator::VisitTransUnit(glsl::TransUnit* transUnit) {
			// TODO
		}
		void GlslToSpvGenerator::VisitStructDecl(glsl::StructDecl* structDecl) {
			// TODO
		}
		void GlslToSpvGenerator::VisitInterfaceBlockDecl(glsl::InterfaceBlockDecl* intBlockDecl) {
			// TODO
		}
		void GlslToSpvGenerator::VisitDeclList(glsl::DeclList* declList) {
			// TODO
		}
		void GlslToSpvGenerator::VisitFunDecl(glsl::FunDecl* funDecl) {
			// TODO
		}
		void GlslToSpvGenerator::VisitQualDecl(glsl::QualDecl* qualDecl) {
			// TODO
		}
		void GlslToSpvGenerator::VisitVarDecl(glsl::VarDecl* varDecl) {
			// TODO
		}

		void GlslToSpvGenerator::VisitBlockStmt(glsl::BlockStmt* blockStmt) {
			// TODO
		}
		void GlslToSpvGenerator::VisitDeclStmt(glsl::DeclStmt* declStmt) {
			// TODO
		}
		void GlslToSpvGenerator::VisitExprStmt(glsl::ExprStmt* exprStmt) {
			// TODO
		}

		void GlslToSpvGenerator::VisitInitListExpr(glsl::InitListExpr* initListExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitAssignExpr(glsl::AssignExpr* assignExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitBinaryExpr(glsl::BinaryExpr* binaryExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitUnaryExpr(glsl::UnaryExpr* unaryExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitFieldSelectExpr(glsl::FieldSelectExpr* fieldSelectExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitFunCallExpr(glsl::FunCallExpr* funCallExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitCtorCallExpr(glsl::CtorCallExpr* ctorCallExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitVarExpr(glsl::VarExpr* varExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitIntConstExpr(glsl::IntConstExpr* intConstExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitUintConstExpr(glsl::UintConstExpr* uintConstExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitFloatConstExpr(glsl::FloatConstExpr* floatConstExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitDoubleConstExpr(glsl::DoubleConstExpr* doubleConstExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitGroupExpr(glsl::GroupExpr* groupExpr) {
			// TODO
		}

	}
}