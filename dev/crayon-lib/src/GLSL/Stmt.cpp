#include "GLSL/Stmt.h"

namespace crayon
{
	namespace glsl
	{
		void VarDecl::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitVarDecl(this);
		}

		void FunDecl::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitFunDecl(this);
		}

		void FunDef::Accept(StmtVisitor* stmtVisitor)
		{
			stmtVisitor->VisitFunDef(this);
		}
	}
}