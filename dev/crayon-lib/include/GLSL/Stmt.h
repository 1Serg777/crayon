#pragma once

namespace crayon
{
	namespace glsl
	{
		class VarDecl;
		class FunDecl;
		class FunDef;

		class StmtVisitor
		{
		public:

			virtual void VisitVarDecl(VarDecl* varDecl) = 0;
			virtual void VisitFunDecl(FunDecl* funDecl) = 0;
			virtual void VisitFunDef(FunDef* funDef) = 0;
		};

		class Stmt
		{
		public:

			virtual void Accept(StmtVisitor* stmtVisitor) = 0;
		};

		class VarDecl : public Stmt
		{
		public:

			void Accept(StmtVisitor* stmtVisitor) override;
		};

		class FunDecl : public Stmt
		{
		public:

			void Accept(StmtVisitor* stmtVisitor) override;
		};

		class FunDef : public Stmt
		{
		public:

			void Accept(StmtVisitor* stmtVisitor) override;
		};
	}
}