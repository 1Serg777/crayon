#pragma once

#include "GLSL/AST/Decl.h"
#include "GLSL/AST/Stmt.h"
#include "GLSL/AST/Expr.h"

#include <sstream>

namespace crayon {
	namespace glsl {

		struct GlslWriterConfig {
			// Indentation parameters
			int indentCount{4};
			char indentChar{' '};
			// Block braces
			bool leftBraceOnSameLine{false};
		};

		class GlslWriter : public DeclVisitor, StmtVisitor, ExprVisitor {
		public:
			GlslWriter(const GlslWriterConfig& config);

			// Decl visit methods
			void VisitTransUnit(TransUnit* transUnit) override;
			void VisitStructDecl(StructDecl* structDecl) override;
			void VisitInterfaceBlockDecl(InterfaceBlockDecl* intBlockDecl) override;
            void VisitFunDecl(FunDecl* funDecl) override;
            void VisitQualDecl(QualDecl* qualDecl) override;
			void VisitVarDecl(VarDecl* varDecl) override;

			// Stmt visit methods
			void VisitBlockStmt(BlockStmt* blockStmt) override;
			void VisitDeclStmt(DeclStmt* declStmt) override;
			void VisitExprStmt(ExprStmt* exprStmt) override;

			// Expression visit methods
			void VisitInitListExpr(InitListExpr* initListExpr) override;
			void VisitAssignExpr(AssignExpr* assignExpr) override;
			void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitUnaryExpr(UnaryExpr* unaryExpr) override;
			void VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) override;
			void VisitFunCallExpr(FunCallExpr* funCallExpr) override;
			void VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitUintConstExpr(UintConstExpr* uintConstExpr) override;
			void VisitFloatConstExpr(FloatConstExpr* floatConstExpr) override;
			void VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;

			std::string GetSrcCodeStr() const;

		private:
			void ResetInternalState();
			// Helper methods
			void WriteFullySpecifiedType(const FullSpecType& fullSpecType);
			void WriteArrayDimensions(const std::vector<std::shared_ptr<Expr>>& dimensions);
			void WriteTypeQualifier(const TypeQual& typeQual);
			void WriteLayoutQualifier(const std::list<LayoutQualifier>& layoutQualifiers);

			void WriteStructDecl(StructDecl* structDecl);
			
			void WriteInitListFirst(InitListExpr* initListExpr);
			void WriteInitListRest(InitListExpr* initListExpr);

			void WriteFunctionPrototype(const FunProto& funProto);
			void WriteFunctionParameterList(const FunParamList& funParamList);
			void WriteFunctionCallArgList(const FunCallArgList& funCallArgList);

			void WriteOpeningBlockBrace();

			void WriteIndentation();

			void RemoveFromOutput(int count);

			GlslWriterConfig config;
			std::stringstream src;
			int indentLvl{0};
			int initListLvl{0};
		};
	
	}
}