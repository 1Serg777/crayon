#include "GLSL/Analyzer/SemanticAnalyzer.h"

#include "GLSL/Error.h"

namespace crayon {
	namespace glsl {

		SemanticAnalyzer::SemanticAnalyzer() {
			exprTypeInferenceVisitor = std::make_unique<ExprTypeInferenceVisitor>();
		}

		void SemanticAnalyzer::SetEnvironmentContext(const EnvironmentContext& envCtx) {
			this->envCtx = envCtx;
			exprTypeInferenceVisitor->SetEnvironmentContext(this->envCtx);
		}
		void SemanticAnalyzer::ResetEnvironmentContext() {
			this->envCtx = EnvironmentContext();
			exprTypeInferenceVisitor->ResetEnvironmentContext();
		}

		bool SemanticAnalyzer::CheckVertexAttribDecl(std::shared_ptr<VertexAttribDecl> vertexAttribDecl) {
			return CheckVertexAttribType(vertexAttribDecl) && CheckVertexAttribChannel(vertexAttribDecl);
		}
		bool SemanticAnalyzer::CheckVertexAttribType(std::shared_ptr<VertexAttribDecl> vertexAttribDecl) {
			// Check the attribute's type specifier. The type must not be:
			// 1. A boolean type
			// 2. An opaque type
			// 3. An array type, or
			// 4. A structure
			const TypeSpec& typeSpec = vertexAttribDecl->GetTypeSpec();
			GlslBasicType glslBasicType = TokenTypeToGlslBasicType(typeSpec.type.tokenType);
			// Check #1.
			if (FundamentalTypeBool(glslBasicType)) {
				return false;
			}
			// Check #2.
			if (IsTypeOpaque(typeSpec.type.tokenType)) {
				return false;
			}
			// Check #3.
			if (typeSpec.IsArray()) {
				return false;
			}
			// Check #4.
			// if (IsTypeAggregate(typeSpec.type, currentScope)) {
			if (typeSpec.IsAggregate()) {
				return false;
			}
			return true;
		}
		bool SemanticAnalyzer::CheckVertexAttribChannel(std::shared_ptr<VertexAttribDecl> vertexAttribDecl) {
			// The channel identifier must map to one of the values in the 'VertexAttribChannel' enumeration.
			return IdentifierTokenToVertexAttribChannel(vertexAttribDecl->GetChannel()) != VertexAttribChannel::UNDEFINED;
		}

		bool SemanticAnalyzer::CheckColorAttachmentDecl(std::shared_ptr<ColorAttachmentDecl> colorAttachmentDecl) {
			return CheckColorAttachmentType(colorAttachmentDecl) && CheckColorAttachmentChannel(colorAttachmentDecl);
		}
		bool SemanticAnalyzer::CheckColorAttachmentType(std::shared_ptr<ColorAttachmentDecl> colorAttachmentDecl) {
			// Check the attachment's type specifier. The type must not be:
			// 1. A boolean type
			// 2. A double-precision scalar or vector type
			// 3. An opaque type
			// 4. A matrix type
			// 5. A structure
			// 6. (Custom, not in the specification) An array type
			const TypeSpec& typeSpec = colorAttachmentDecl->GetTypeSpec();
			GlslBasicType glslBasicType = TokenTypeToGlslBasicType(typeSpec.type.tokenType);
			// Check #1.
			if (FundamentalTypeBool(glslBasicType)) {
				return false;
			}
			// Check #2.
			if (FundamentalTypeDouble(glslBasicType)) {
				return false;
			}
			// Check #3.
			if (IsTypeOpaque(typeSpec.type.tokenType)) {
				return false;
			}
			// Check #4.
			if (IsMatrixType(glslBasicType)) {
				return false;
			}
			// Check #5.
			// if (IsTypeAggregate(typeSpec.type, currentScope)) {
			if (typeSpec.IsAggregate()) {
				return false;
			}
			// Check #6.
			if (typeSpec.IsArray()) {
				return false;
			}
			return true;
		}
		bool SemanticAnalyzer::CheckColorAttachmentChannel(std::shared_ptr<ColorAttachmentDecl> colorAttachmentDecl) {
			// The channel identifier must map to one of the values in the 'ColorAttachmentChannel' enumeration.
			return IdentifierTokenToColorAttachmentChannel(colorAttachmentDecl->GetChannel()) != ColorAttachmentChannel::UNDEFINED;
		}

		bool SemanticAnalyzer::CheckVarDecl(std::shared_ptr<VarDecl> varDecl) {
			std::shared_ptr<Expr> initializer = varDecl->GetInitializerExpr();
			if (initializer) {
				initializer->Accept(exprTypeInferenceVisitor.get());
				const GlslExprType& initializerExprType = initializer->GetExprType();
				GlslExprType varAccessExprType = exprTypeInferenceVisitor->InferVarExprType(varDecl.get());
				return TypePromotable(initializerExprType, varAccessExprType);
			}
			return true;
		}

	}
}