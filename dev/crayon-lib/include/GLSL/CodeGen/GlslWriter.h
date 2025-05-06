#pragma once

#include "GLSL/AST/Expr.h"
#include "GLSL/AST/Stmt.h"

#include <sstream>

namespace crayon
{
	namespace glsl
	{
		struct GlslWriterConfig
		{
			// Block braces

			bool leftBraceOnSameLine{ false };

			// Indentation parameters

			int identationCount{ 4 };
			bool identationSpaces{ true }; // Tabs otherwise
		};

		class GlslWriter : public ExprVisitor, StmtVisitor
		{
		public:

			GlslWriter(const GlslWriterConfig& config);

			// Expression visit methods

			void VisitBinaryExpr(Binary* binaryExpr) override;
			void VisitIntConstExpr(IntConst* intConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;

			// Stmt visit methods

			void VisitQualDecl(QualDecl* qualDecl) override;
			void VisitVarDecl(VarDecl* varDecl) override;
			void VisitFunDecl(FunDecl* funDecl) override;
			void VisitFunDef(FunDef* funDef) override;
			void VisitExtDeclList(ExtDeclList* extDeclList) override;
			void VisitBlockStmt(BlockStmt* blockStmt) override;

			std::string GetSrcCodeStr() const;

		private:

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