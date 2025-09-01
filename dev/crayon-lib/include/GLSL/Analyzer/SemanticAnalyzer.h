#pragma once

#include "GLSL/Type.h"

#include "GLSL/AST/Block.h"
#include "GLSL/AST/Decl.h"
#include "GLSL/AST/Stmt.h"
#include "GLSL/AST/Expr.h"

#include "GLSL/Analyzer/Environment.h"

#include "GLSL/Reflect/ReflectCommon.h"

namespace crayon {
	namespace glsl {

        class SemanticAnalyzer {
        public:
            void SetEnvironmentContext(const EnvironmentContext& envCtx);
            void ResetEnvironmentContext();

            bool CheckVertexAttribDecl(std::shared_ptr<VertexAttribDecl> vertexAttribDecl);
            bool CheckVertexAttribType(std::shared_ptr<VertexAttribDecl> vertexAttribDecl);
            bool CheckVertexAttribChannel(std::shared_ptr<VertexAttribDecl> vertexAttribDecl);

            bool CheckColorAttachmentDecl(std::shared_ptr<ColorAttachmentDecl> colorAttachmentDecl);
            bool CheckColorAttachmentType(std::shared_ptr<ColorAttachmentDecl> colorAttachmentDecl);
            bool CheckColorAttachmentChannel(std::shared_ptr<ColorAttachmentDecl> colorAttachmentDecl);

            bool CheckVarDecl(std::shared_ptr<VarDecl> varDecl,
                              DeclContext declContext,
                              ShaderType shaderType);

            bool CheckTypeSpec(TypeSpec& typeSpec);

        private:
            EnvironmentContext envCtx;
            ExprTypeInferenceVisitor exprTypeInferenceVisitor;
            ExprEvalVisitor exprEvalVisitor;
        };

    }
}