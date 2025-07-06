#include "GLSL/Analyzer/SemanticAnalyzer.h"

#include "GLSL/Error.h"

namespace crayon {
	namespace glsl {

		SemanticAnalyzer::SemanticAnalyzer() {
			exprTypeInferenceVisitor = std::make_unique<ExprTypeInferenceVisitor>();
		}

		void SemanticAnalyzer::SetEnvironment(const Environment* scope) {
			this->currentScope = scope;
			exprTypeInferenceVisitor->SetEnvironment(currentScope);
		}
		void SemanticAnalyzer::ResetEnvironment() {
			this->currentScope = nullptr;
			exprTypeInferenceVisitor->ResetEnvironment();
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