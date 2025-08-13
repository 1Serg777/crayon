#pragma once

#include "GLSL/AST/Block.h"
#include "GLSL/AST/Decl.h"
#include "GLSL/AST/Stmt.h"
#include "GLSL/AST/Expr.h"

#include <cstdint>
#include <vector>

namespace crayon {
	namespace spirv {

		struct GlslToSpvGeneratorConfig {
			// TODO
		};

		class GlslToSpvGenerator : public glsl::BlockVisitor,
			                       public glsl::DeclVisitor,
			                       public glsl::StmtVisitor,
			                       public glsl::ExprVisitor {
		public:
			GlslToSpvGenerator(const GlslToSpvGeneratorConfig& config);

			std::vector<uint32_t> CompileToSpvBinary(glsl::ShaderProgramBlock* program);
			std::string CompileToSpvAsmText(glsl::ShaderProgramBlock* program);

		private:
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

			GlslToSpvGeneratorConfig config;
		};

	}
}