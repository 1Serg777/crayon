#include "GLSL/AST/Stmt.h"

namespace crayon {
	namespace glsl {

		void BlockStmt::Accept(StmtVisitor* stmtVisitor) {
			stmtVisitor->VisitBlockStmt(this);
		}
		
		void BlockStmt::AddStmt(std::shared_ptr<Stmt> stmt) {
			stmts.push_back(stmt);
		}
	}
}