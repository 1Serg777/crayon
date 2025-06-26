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