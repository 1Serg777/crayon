#pragma once

#include "GLSL/Type.h"

#include "GLSL/AST/Block.h"
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

            bool CheckVertexAttribDecl(std::shared_ptr<VertexAttribDecl> vertexAttribDecl);
            bool CheckVertexAttribType(std::shared_ptr<VertexAttribDecl> vertexAttribDecl);
            bool CheckVertexAttribChannel(std::shared_ptr<VertexAttribDecl> vertexAttribDecl);

            bool CheckVarDecl(std::shared_ptr<VarDecl> varDecl);

        private:
            const Environment* currentScope{nullptr};
            std::unique_ptr<ExprTypeInferenceVisitor> exprTypeInferenceVisitor;
        };

    }
}