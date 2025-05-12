#pragma once

#include "GLSL/Token.h"

#include <memory>

namespace crayon {
	namespace glsl {

		class AssignExpr;
		class BinaryExpr;
		class VarExpr; // Variable access expression
		class IntConstExpr; // Int constant access expression
		class GroupExpr;

		class ExprVisitor {
		public:
			virtual void VisitAssignExpr(AssignExpr* assignExpr) = 0;
			virtual void VisitBinaryExpr(BinaryExpr* binaryExpr) = 0;
			virtual void VisitVarExpr(VarExpr* varExpr) = 0;
			virtual void VisitIntConstExpr(IntConstExpr* intConstExpr) = 0;
			virtual void VisitGroupExpr(GroupExpr* groupExpr) = 0;
		};

		class ExprEvalVisitor : public ExprVisitor {
		public:
			void VisitAssignExpr(AssignExpr* assignExpr) override;
			void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;

			int GetResult() const;

		private:
			int result{ 0 };
		};

		class ExprPostfixPrinterVisitor : public ExprVisitor {
		public:
			void VisitAssignExpr(AssignExpr* assignExpr) override;
			void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;
		};

		class ExprParenPrinterVisitor : public ExprVisitor {
		public:
			void VisitAssignExpr(AssignExpr* assignExpr) override;
			void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;
		};

		class Expr {
		public:
			Expr() = default;
			virtual ~Expr() = default;
			virtual void Accept(ExprVisitor* exprVisitor) = 0;
		};

		class AssignExpr : public Expr {
		public:
			AssignExpr(std::shared_ptr<Expr> lvalue, std::shared_ptr<Expr> rvalue);
			virtual ~AssignExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			Expr* GetLvalue() const;
			Expr* GetRvalue() const;

		private:
			std::shared_ptr<Expr> lvalue;
			std::shared_ptr<Expr> rvalue;
		};

		class BinaryExpr : public Expr {
		public:
			BinaryExpr(std::shared_ptr<Expr> left, const Token& op, std::shared_ptr<Expr> right);
			virtual ~BinaryExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			Expr* GetLeftExpr() const;
			Expr* GetRightExpr() const;

			const Token& GetOperator() const;

		private:
			std::shared_ptr<Expr> left;
			std::shared_ptr<Expr> right;
			Token op{};
		};

		class VarExpr : public Expr {
		public:
			VarExpr(const Token& variable);
			virtual ~VarExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			const Token& GetVariable() const;

		private:
			Token variable;
		};
		
		class IntConstExpr : public Expr {
		public:
			IntConstExpr(const Token& intConst);
			virtual ~IntConstExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			const Token& GetIntConst() const;

		private:
			Token intConst;
		};

		class GroupExpr : public Expr {
		public:
			GroupExpr(std::shared_ptr<Expr> expr);
			virtual ~GroupExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			Expr* GetParenExpr() const;

		private:
			std::shared_ptr<Expr> expr;
		};
	}
}