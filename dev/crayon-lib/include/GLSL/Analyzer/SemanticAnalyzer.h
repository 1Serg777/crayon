#pragma once

#include "GLSL/AST/Decl.h"
#include "GLSL/AST/Stmt.h"
#include "GLSL/AST/Expr.h"

namespace crayon {
	namespace glsl {

        class AssignTargetCheck : public ExprVisitor {
        public:
            void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;

        private:
            bool result{ false };
        };

        class SemanticAnalyzer {
            // [TODO]
        };
    }
}