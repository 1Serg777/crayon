#include "GLSL/AST/Expr.h"

#include <cstdlib>
#include <iostream>

namespace crayon {
	namespace glsl {

		void ExprEvalVisitor::VisitAssignExpr(AssignExpr* assignExpr) {
			// [TODO]: implement environments first!
		}
		void ExprEvalVisitor::VisitBinaryExpr(BinaryExpr* binaryExpr) {
			binaryExpr->GetLeftExpr()->Accept(this);
			int left = result;
			binaryExpr->GetRightExpr()->Accept(this);
			int right = result;

			switch (binaryExpr->GetOperator().tokenType) {
				case TokenType::PLUS: {
					result = left + right;
				}
				break;
				case TokenType::DASH: {
					result = left - right;
				}
				break;
				case TokenType::STAR: {
					result = left * right;
				}
				break;
				case TokenType::SLASH: {
					result = left / right;
				}
				break;
			}
		}
		void ExprEvalVisitor::VisitVarExpr(VarExpr* varExpr) {
			// [TODO]: implement environments first!
			// const Token& intConst = intConstExpr->GetIntConst();
			// int value = static_cast<int>(std::strtol(intConst.lexeme.data(), nullptr, 10));
			// result = value;
		}
		void ExprEvalVisitor::VisitIntConstExpr(IntConstExpr* intConstExpr) {
			const Token& intConst = intConstExpr->GetIntConst();
			int value = static_cast<int>(std::strtol(intConst.lexeme.data(), nullptr, 10));
			result = value;
		}
		void ExprEvalVisitor::VisitGroupExpr(GroupExpr* groupExpr) {
			groupExpr->GetParenExpr()->Accept(this);
		}

		int ExprEvalVisitor::GetResult() const {
			return result;
		}

		void ExprPostfixPrinterVisitor::VisitAssignExpr(AssignExpr* assignExpr) {
			assignExpr->GetLvalue()->Accept(this);
			assignExpr->GetRvalue()->Accept(this);
			std::cout << "=" << " "; // [TODO]: add more assignment operators later.
		}
		void ExprPostfixPrinterVisitor::VisitBinaryExpr(BinaryExpr* binaryExpr) {
			binaryExpr->GetLeftExpr()->Accept(this);
			binaryExpr->GetRightExpr()->Accept(this);
			std::cout << binaryExpr->GetOperator().lexeme << " ";
		}
		void ExprPostfixPrinterVisitor::VisitVarExpr(VarExpr* varExpr) {
			std::cout << varExpr->GetVariable().lexeme << " ";
		}
		void ExprPostfixPrinterVisitor::VisitIntConstExpr(IntConstExpr* intConstExpr) {
			std::cout << intConstExpr->GetIntConst().lexeme << " ";
		}
		void ExprPostfixPrinterVisitor::VisitGroupExpr(GroupExpr* groupExpr) {
			groupExpr->GetParenExpr()->Accept(this);
		}

		void ExprParenPrinterVisitor::VisitAssignExpr(AssignExpr* assignExpr) {
			std::cout << "( ";
			assignExpr->GetLvalue()->Accept(this);
			std::cout << " = ";
			assignExpr->GetRvalue()->Accept(this);
			std::cout << " )";
		}
		void ExprParenPrinterVisitor::VisitBinaryExpr(BinaryExpr* binaryExpr) {
			std::cout << "( ";
			binaryExpr->GetLeftExpr()->Accept(this);
			std::cout << " " << binaryExpr->GetOperator().lexeme << " ";
			binaryExpr->GetRightExpr()->Accept(this);
			std::cout << " )";
		}
		void ExprParenPrinterVisitor::VisitVarExpr(VarExpr* varExpr) {
			std::cout << varExpr->GetVariable().lexeme;
		}
		void ExprParenPrinterVisitor::VisitIntConstExpr(IntConstExpr* intConstExpr) {
			std::cout << intConstExpr->GetIntConst().lexeme;
		}
		void ExprParenPrinterVisitor::VisitGroupExpr(GroupExpr* groupExpr) {
			groupExpr->GetParenExpr()->Accept(this);
		}

		AssignExpr::AssignExpr(std::shared_ptr<Expr> lvalue, std::shared_ptr<Expr> rvalue)
			: lvalue(lvalue), rvalue(rvalue) {
		}

		void AssignExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitAssignExpr(this);
		}

		Expr* AssignExpr::GetLvalue() const {
			return lvalue.get();
		}
		Expr* AssignExpr::GetRvalue() const {
			return rvalue.get();
		}

		BinaryExpr::BinaryExpr(std::shared_ptr<Expr> left, const Token& op, std::shared_ptr<Expr> right)
			: left(left), op(op), right(right) {
		}

		void BinaryExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitBinaryExpr(this);
		}

		Expr* BinaryExpr::GetLeftExpr() const {
			return left.get();
		}
		Expr* BinaryExpr::GetRightExpr() const {
			return right.get();
		}

		const Token& BinaryExpr::GetOperator() const {
			return op;
		}

		VarExpr::VarExpr(const Token& variable)
			: variable(variable) {
		}

		void VarExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitVarExpr(this);
		}

		const Token& VarExpr::GetVariable() const {
			return variable;
		}

		IntConstExpr::IntConstExpr(const Token& intConst)
			: intConst(intConst) {
		}

		void IntConstExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitIntConstExpr(this);
		}

		const Token& IntConstExpr::GetIntConst() const {
			return intConst;
		}

		GroupExpr::GroupExpr(std::shared_ptr<Expr> expr)
			: expr(expr) {
		}

		void GroupExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitGroupExpr(this);
		}

		Expr* GroupExpr::GetParenExpr() const {
			return expr.get();
		}
	}
}