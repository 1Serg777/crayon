#include "GLSL/Expr.h"

#include <cstdlib>
#include <iostream>

namespace crayon
{
	namespace glsl
	{
		void ExprEvalVisitor::VisitBinaryExpr(Binary* binaryExpr)
		{
			binaryExpr->GetLeftExpr()->Accept(this);
			int left = result;
			binaryExpr->GetRightExpr()->Accept(this);
			int right = result;

			switch (binaryExpr->GetOperator().tokenType)
			{
				case TokenType::PLUS:
				{
					result = left + right;
				}
				break;
				case TokenType::DASH:
				{
					result = left - right;
				}
				break;
				case TokenType::STAR:
				{
					result = left * right;
				}
				break;
				case TokenType::SLASH:
				{
					result = left / right;
				}
				break;
			}
		}
		void ExprEvalVisitor::VisitIntConstExpr(IntConst* intConstExpr)
		{
			const Token& intConst = intConstExpr->GetIntConst();
			int value = static_cast<int>(std::strtol(intConst.lexeme.data(), nullptr, 10));
			result = value;
		}

		int ExprEvalVisitor::GetResult() const
		{
			return result;
		}

		void ExprPostfixPrinterVisitor::VisitBinaryExpr(Binary* binaryExpr)
		{
			binaryExpr->GetLeftExpr()->Accept(this);
			binaryExpr->GetRightExpr()->Accept(this);
			std::cout << binaryExpr->GetOperator().lexeme << " ";
		}
		void ExprPostfixPrinterVisitor::VisitIntConstExpr(IntConst* intConstExpr)
		{
			std::cout << intConstExpr->GetIntConst().lexeme << " ";
		}

		Binary::Binary(std::shared_ptr<Expr> left, const Token& op, std::shared_ptr<Expr> right)
			: left(left), op(op), right(right)
		{
		}

		void Binary::Accept(ExprVisitor* exprVisitor)
		{
			exprVisitor->VisitBinaryExpr(this);
		}

		Expr* Binary::GetLeftExpr() const
		{
			return left.get();
		}
		Expr* Binary::GetRightExpr() const
		{
			return right.get();
		}

		const Token& Binary::GetOperator() const
		{
			return op;
		}

		IntConst::IntConst(const Token& intConst)
			: intConst(intConst)
		{
		}

		void IntConst::Accept(ExprVisitor* exprVisitor)
		{
			exprVisitor->VisitIntConstExpr(this);
		}

		const Token& IntConst::GetIntConst() const
		{
			return intConst;
		}
	}
}