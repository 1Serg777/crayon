#pragma once

#include "GLSL/AST/Decl.h"
#include "GLSL/AST/Stmt.h"
#include "GLSL/AST/Expr.h"

#include "GLSL/Analyzer/Environment.h"

namespace crayon {
	namespace glsl {

        class SemanticAnalyzer {
        public:
            SemanticAnalyzer();

            void SetEnvironment(const Environment* scope);
            void ResetEnvironment();

            bool CheckVarDecl(std::shared_ptr<VarDecl> varDecl);

        private:
            const Environment* currentScope{nullptr};
            std::unique_ptr<ExprTypeInferenceVisitor> exprTypeInferenceVisitor;
        };

    }
}