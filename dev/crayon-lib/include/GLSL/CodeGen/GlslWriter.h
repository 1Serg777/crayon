#pragma once

#include "GLSL/AST/Decl.h"
#include "GLSL/AST/Stmt.h"
#include "GLSL/AST/Expr.h"

#include <sstream>

namespace crayon {
	namespace glsl {

		struct GlslWriterConfig {
			// Block braces
			bool leftBraceOnSameLine{ false };
			// Indentation parameters
			int indentCount{ 4 };
			bool indentSpaces{ true }; // Tabs otherwise
		};

		class GlslWriter : public DeclVisitor, StmtVisitor, ExprVisitor {
		public:
			GlslWriter(const GlslWriterConfig& config);

			// Decl visit methods
			void VisitTransUnit(TransUnit* transUnit) override;
            void VisitFunDecl(FunDecl* funDecl) override;
            void VisitQualDecl(QualDecl* qualDecl) override;
			void VisitVarDecl(VarDecl* varDecl) override;

			// Stmt visit methods
			void VisitBlockStmt(BlockStmt* blockStmt) override;

			// Expression visit methods
			void VisitBinaryExpr(Binary* binaryExpr) override;
			void VisitIntConstExpr(IntConst* intConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;

			std::string GetSrcCodeStr() const;

		private:
			// Helper methods
			void WriteFullySpecifiedType(const FullSpecType& fullSpecType);
			void WriteTypeQualifier(const TypeQual& typeQual);
			void WriteLayoutQualifier(const std::list<LayoutQualifier>& layoutQualifiers);

			void WriteFunctionPrototype(const FunProto& funProto);
			void WriteFunctionParameterList(const FunParamList& funParamList);

			void WriteOpeningBlockBrace();

			GlslWriterConfig config;
			std::stringstream src;
		};
	}
}