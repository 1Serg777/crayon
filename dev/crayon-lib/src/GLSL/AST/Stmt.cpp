#include "GLSL/AST/Stmt.h"

namespace crayon {
	namespace glsl {

		void BlockStmt::Accept(StmtVisitor* stmtVisitor) {
			stmtVisitor->VisitBlockStmt(this);
		}
		void BlockStmt::AddStmt(std::shared_ptr<Stmt> stmt) {
			stmts.push_back(stmt);
		}
		
		bool BlockStmt::IsEmpty() const {
			return stmts.empty();
		}
		const std::vector<std::shared_ptr<Stmt>>& BlockStmt::GetStatements() const {
			return stmts;
		}

		DeclStmt::DeclStmt(std::shared_ptr<Decl> decl)
			: decl(decl) {
		}
		void DeclStmt::Accept(StmtVisitor* stmtVisitor) {
			stmtVisitor->VisitDeclStmt(this);
		}
		std::shared_ptr<Decl> DeclStmt::GetDeclaration() const {
			return decl;
		}

		ExprStmt::ExprStmt(std::shared_ptr<Expr> expr)
			: expr(expr) {
		}
		void ExprStmt::Accept(StmtVisitor* stmtVisitor) {
			stmtVisitor->VisitExprStmt(this);
		}
		std::shared_ptr<Expr> ExprStmt::GetExpression() const {
			return expr;
		}
	
	}
}