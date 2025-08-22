#pragma once

#include "GLSL/Type.h"

#include "GLSL/AST/Block.h"
#include "GLSL/AST/Decl.h"
#include "GLSL/AST/Stmt.h"
#include "GLSL/AST/Expr.h"

#include "GLSL/Analyzer/Environment.h"

#include "SPIRV/SpvInstruction.h"

#include "GLSL/Reflect/ShaderProgram.h"

#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace crayon {
	namespace spirv {

		enum class SpvScopeContext {
			EXTERNAL,
			FUNCTION,
			BLOCK
		};

		struct SpvEnvironment {
			void Clear();
			// std::unordered_map<std::string_view, SpvInstruction> scalarTypes; // bool, int, uint, float, double
			// std::unordered_map<std::string_view, SpvInstruction> compositeTypes; // vector, matrix, and array types
			std::unordered_map<std::string, SpvInstruction> typeDecls;
			// std::vector<SpvInstruction> aggregateTypes; // structs and interfaces?
			// TODO: perhaps, use an unordered map? or should it be ordered?
			std::unordered_map<std::string, SpvInstruction> typePtrs;
			// TODO: using a map, we'd have to implement some type of name mangling scheme.
			// Here's another issues to consider.
			// Say we're dealing with several struct declarations.
			// Some of the structures use objects whose types are other structures, so
			// the order matters in this case. If we use an unordered map to store
			// the mappings from type names to the instructions that declare them, we might run
			// into issues where the SPIR-V type declaration instructions break their real order.
			// Should we also use a vector that would preserve the order, while a map would only
			// reference the instructions?
			// std::unordered_map<std::string_view, SpvInstruction> funTypes;
			std::unordered_map<std::string, SpvInstruction> functions;
			// Variable declaration instructions.
			std::unordered_map<std::string, SpvInstruction> varDecls;
			// Constants.
			std::unordered_map<std::string, SpvInstruction> constants;

			SpvScopeContext scopeCtx{SpvScopeContext::EXTERNAL};
			SpvExecutionModel execModel{SpvExecutionModel::VERTEX};
		};

		struct SpvCodeGenContext {
			std::vector<SpvInstruction> spvInsts;
			glsl::ExternalScopeEnvironment* extScopeEnv{nullptr};
		};

		struct GlslToSpvGeneratorConfig {
			SpvType type{SpvType::BINARY};
		};

		class GlslToSpvGenerator : public glsl::BlockVisitor,
			                       public glsl::DeclVisitor,
			                       public glsl::StmtVisitor,
			                       public glsl::ExprVisitor {
		public:
			GlslToSpvGenerator(const GlslToSpvGeneratorConfig& config);

			void CompileToSpv(glsl::ShaderProgramBlock* program);
			const ShaderProgram& GetShaderProgram() const;

		private:
			void GenerateTestProgram();
			void ClearState();

			std::vector<uint32_t> GenerateSpvBinary();
			std::string GenerateSpvAsmText();

			// NEW

			SpvInstruction GetTypeDeclInst(const glsl::TypeSpec& typeSpec);
			SpvInstruction GetTypePtrDeclInst(const glsl::TypeSpec& typeSpec, SpvStorageClass storageClass);

			SpvInstruction CreateTypeDeclInst(const glsl::TypeSpec& typeSpec);
			SpvInstruction CreateTypePtrDeclInst(const glsl::TypeSpec& typeSpec, SpvStorageClass storageClass);

			SpvInstruction CreateScalarTypeDeclInst(const glsl::TypeSpec& typeSpec);
			SpvInstruction CreateVectorTypeDeclInst(const glsl::TypeSpec& typeSpec);
			SpvInstruction CreateMatrixTypeDeclInst(const glsl::TypeSpec& typeSpec);
			SpvInstruction CreateStructureTypeDeclInst(const glsl::TypeSpec& typeSpec);
			SpvInstruction CreateArrayTypeDeclInst(const glsl::TypeSpec& typeSpec);

			// NEW

			SpvInstruction GetTypeFunDeclInst(const glsl::FunProto* funProto);
			SpvInstruction CreateTypeFunDeclInst(const glsl::FunProto* funProto);

			/*
			SpvInstruction GetTypePointerInstruction(glsl::GlslBasicType glslType, SpvStorageClass storageClass);
			SpvInstruction GetTypeDeclarationInstruction(glsl::GlslBasicType glslType);
			SpvInstruction GetTypeDeclarationInstruction(const glsl::VarDecl* varDecl);
			*/
			
			/*
			SpvInstruction CreateTypeDeclarationInstruction(const glsl::VarDecl* varDecl);
			SpvInstruction CreateTypeDeclarationInstruction(glsl::GlslBasicType glslType);
			SpvInstruction CreateScalarTypeDeclarationInstruction(glsl::GlslBasicType glslType);
			SpvInstruction CreateVectorTypeDeclarationInstruction(glsl::GlslBasicType glslType);
			SpvInstruction CreateMatrixTypeDeclarationInstruction(glsl::GlslBasicType glslType);
			*/

			template <typename T>
			SpvInstruction GetConstInst(T constVal) {
				std::string mangledConstName = MangleConstName(constVal);
				auto searchRes = spvEnv.constants.find(mangledConstName);
				if (searchRes == spvEnv.constants.end()) {
					SpvInstruction constDeclInst = CreateConstInst(constVal);
					return constDeclInst;
				}
				return searchRes->second;
			}
			
			SpvInstruction CreateConstInst(int constVal);
			SpvInstruction CreateConstInst(unsigned int constVal);
			SpvInstruction CreateConstInst(float constVal);
			SpvInstruction CreateConstInst(double constVal);

			void PrintExtInstructions(std::ostream& out) const;
			void PrintModeInstructions(std::ostream& out) const;
			void PrintEntryPointInstruction(std::ostream& out) const;

			void PrintDecorationInstructions(std::ostream& out) const;
			void PrintTypeInstructions(std::ostream& out) const;
			void PrintTypePtrInstructions(std::ostream& out) const;
			void PrintConstantInstructions(std::ostream& out) const;
			void PrintGlobalVariables(std::ostream& out) const;

			void PrintFunctionInstructions(std::ostream& out) const;

			void PrintInstructions(std::ostream& out, const std::vector<SpvInstruction>& instructions) const;

			// Block vist methods
			void VisitShaderProgramBlock(glsl::ShaderProgramBlock* programBlock) override;
			void VisitFixedStagesConfigBlock(glsl::FixedStagesConfigBlock* fixedStagesConfigBlock) override;
			void VisitMaterialPropertiesBlock(glsl::MaterialPropertiesBlock* materialPropertiesBlock) override;
			void VisitVertexInputLayoutBlock(glsl::VertexInputLayoutBlock* vertexInputLayoutBlock) override;
			void VisitColorAttachmentsBlock(glsl::ColorAttachmentsBlock* colorAttachmentsBlock) override;
			void VisitShaderBlock(glsl::ShaderBlock* shaderBlock) override;

			// Decl visit methods
			void VisitTransUnit(glsl::TransUnit* transUnit) override;
			void VisitStructDecl(glsl::StructDecl* structDecl) override;
			void VisitInterfaceBlockDecl(glsl::InterfaceBlockDecl* intBlockDecl) override;
			void VisitDeclList(glsl::DeclList* declList) override;
			void VisitFunDecl(glsl::FunDecl* funDecl) override;
			void VisitQualDecl(glsl::QualDecl* qualDecl) override;
			void VisitVarDecl(glsl::VarDecl* varDecl) override;

			// Stmt visit methods
			void VisitBlockStmt(glsl::BlockStmt* blockStmt) override;
			void VisitDeclStmt(glsl::DeclStmt* declStmt) override;
			void VisitExprStmt(glsl::ExprStmt* exprStmt) override;

			// Expression visit methods
			void VisitInitListExpr(glsl::InitListExpr* initListExpr) override;
			void VisitAssignExpr(glsl::AssignExpr* assignExpr) override;
			void VisitBinaryExpr(glsl::BinaryExpr* binaryExpr) override;
			void VisitUnaryExpr(glsl::UnaryExpr* unaryExpr) override;
			void VisitFieldSelectExpr(glsl::FieldSelectExpr* fieldSelectExpr) override;
			void VisitFunCallExpr(glsl::FunCallExpr* funCallExpr) override;
			void VisitCtorCallExpr(glsl::CtorCallExpr* ctorCallExpr) override;
			void VisitVarExpr(glsl::VarExpr* varExpr) override;
			void VisitIntConstExpr(glsl::IntConstExpr* intConstExpr) override;
			void VisitUintConstExpr(glsl::UintConstExpr* uintConstExpr) override;
			void VisitFloatConstExpr(glsl::FloatConstExpr* floatConstExpr) override;
			void VisitDoubleConstExpr(glsl::DoubleConstExpr* doubleConstExpr) override;
			void VisitGroupExpr(glsl::GroupExpr* groupExpr) override;

			ShaderProgram shaderProgram;
			SpvEnvironment spvEnv;
			SpvInstruction entryPointInst;

			std::vector<SpvInstruction> extInstructions;
			std::vector<SpvInstruction> modeInstructions;

			std::vector<SpvInstruction> decorations;
			std::vector<SpvInstruction> typeDeclInsts;
			std::vector<SpvInstruction> typePtrInsts;
			std::vector<SpvInstruction> constants;
			std::vector<SpvInstruction> globalVars;

			std::vector<SpvInstruction> inVars;
			std::vector<SpvInstruction> outVars;

			std::vector<SpvInstruction> instructions;

			std::string_view entryPointFunName{"main"};
			size_t idFieldWidth{0};
			GlslToSpvGeneratorConfig config;
		};

		// NEW

		std::string MangleTypeName(const glsl::TypeSpec& typeSpec);
		std::string MangleTypePtrName(const glsl::TypeSpec& fullSpecType, SpvStorageClass storageClass);

		// NEW

		std::string MangleTypeName(const glsl::VarDecl* varDecl);
		// std::string MangleTypeName(const glsl::TypeSpec& typeSpec);
		std::string MangleTypeName(glsl::GlslBasicType glslType);

		std::string MangleConstName(int intConst);
		std::string MangleConstName(unsigned int uintConst);
		std::string MangleConstName(float floatConst);
		std::string MangleConstName(double doubleConst);

		// std::string MangleTypePointerName(const glsl::TypeSpec& typeSpec, SpvStorageClass storageClass);
		std::string MangleTypePointerName(glsl::GlslBasicType glslType, SpvStorageClass storageClass);

		std::string MangleTypeFunctionName(const glsl::FunProto* funProto);

		// GLSL extension blocks.

		std::string MangleTypeName(glsl::VertexAttribDecl* vertexAttrib);
		std::string MangleTypeName(glsl::MatPropDecl* matProp);
		std::string MangleTypeName(glsl::ColorAttachmentDecl* colorAttachment);

		std::string MangleTypePointerName(glsl::VertexAttribDecl* vertexAttrib);
		std::string MangleTypePointerName(glsl::MatPropDecl* matProp);
		std::string MangleTypePointerName(glsl::ColorAttachmentDecl* colorAttachment);

	}
}