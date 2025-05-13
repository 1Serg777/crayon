#pragma once

#include "GLSL/Type.h"

#include <memory>
#include <vector>

namespace crayon {
	namespace glsl {

		class Decl;
		class Expr;
		class BlockStmt;
		class DeclStmt;
		class ExprStmt;

		class StmtVisitor {
		public:
			virtual void VisitBlockStmt(BlockStmt* blockStmt) = 0;
			virtual void VisitDeclStmt(DeclStmt* declStmt) = 0;
			virtual void VisitExprStmt(ExprStmt* exprStmt) = 0;
		};

		class Stmt {
		public:
			Stmt() = default;
			virtual ~Stmt() = default;
			virtual void Accept(StmtVisitor* stmtVisitor) = 0;
		};

		class BlockStmt : public Stmt {
		public:
			virtual ~BlockStmt() = default;

			void Accept(StmtVisitor* stmtVisitor) override;

			void AddStmt(std::shared_ptr<Stmt> stmt);

			bool IsEmpty() const;
			const std::vector<std::shared_ptr<Stmt>>& GetStatements() const;

		private:
			std::vector<std::shared_ptr<Stmt>> stmts;
		};

		class DeclStmt : public Stmt {
		public:
			DeclStmt(std::shared_ptr<Decl> decl);
			virtual ~DeclStmt() = default;

			void Accept(StmtVisitor* stmtVisitor) override;

			std::shared_ptr<Decl> GetDeclaration() const;
		private:
			std::shared_ptr<Decl> decl;
		};
	
		class ExprStmt : public Stmt {
		public:
			ExprStmt(std::shared_ptr<Expr> expr);
			virtual ~ExprStmt() = default;

			void Accept(StmtVisitor* stmtVisitor) override;

			std::shared_ptr<Expr> GetExpression() const;
		private:
			std::shared_ptr<Expr> expr;
		};
	
	}
}