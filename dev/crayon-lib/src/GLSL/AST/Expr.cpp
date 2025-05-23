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
		void ExprEvalVisitor::VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) {
			// TODO: implement environments first!
		}
		void ExprEvalVisitor::VisitFunCallExpr(FunCallExpr* funCallExpr) {
			// TODO: implement environments first!
		}
		void ExprEvalVisitor::VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) {
			// TODO: implement environments first!
		}
		void ExprEvalVisitor::VisitVarExpr(VarExpr* varExpr) {
			// TODO: implement environments first!
		}
		void ExprEvalVisitor::VisitIntConstExpr(IntConstExpr* intConstExpr) {
			const Token& intConst = intConstExpr->GetIntConst();
			int value = static_cast<int>(std::strtol(intConst.lexeme.data(), nullptr, 10));
			result = value;
		}
		void ExprEvalVisitor::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			const Token& floatConst = floatConstExpr->GetFloatConst();
			float value = std::strtof(floatConst.lexeme.data(), nullptr);
			// result = value;
			// TODO: think about how to return results of different types!
			//       1) A C-style union?
			//       2) A C++-style type-safe union?
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
		void ExprPostfixPrinterVisitor::VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) {
			// TODO: implement environments first!
		}
		void ExprPostfixPrinterVisitor::VisitFunCallExpr(FunCallExpr* funCallExpr) {
			// TODO: implement environments first!
		}
		void ExprPostfixPrinterVisitor::VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) {
			// TODO: implement environments first!
		}
		void ExprPostfixPrinterVisitor::VisitVarExpr(VarExpr* varExpr) {
			std::cout << varExpr->GetVariable().lexeme << " ";
		}
		void ExprPostfixPrinterVisitor::VisitIntConstExpr(IntConstExpr* intConstExpr) {
			std::cout << intConstExpr->GetIntConst().lexeme << " ";
		}
		void ExprPostfixPrinterVisitor::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			std::cout << floatConstExpr->GetFloatConst().lexeme << " ";
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
		void ExprParenPrinterVisitor::VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) {
			// TODO: implement environments first!
		}
		void ExprParenPrinterVisitor::VisitFunCallExpr(FunCallExpr* funCallExpr) {
			// TODO: implement environments first!
		}
		void ExprParenPrinterVisitor::VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) {
			// TODO: implement environments first!
		}
		void ExprParenPrinterVisitor::VisitVarExpr(VarExpr* varExpr) {
			std::cout << varExpr->GetVariable().lexeme;
		}
		void ExprParenPrinterVisitor::VisitIntConstExpr(IntConstExpr* intConstExpr) {
			std::cout << intConstExpr->GetIntConst().lexeme;
		}
		void ExprParenPrinterVisitor::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			std::cout << floatConstExpr->GetFloatConst().lexeme << " ";
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

		FieldSelectExpr::FieldSelectExpr(std::shared_ptr<Expr> target, const Token& field)
			: target(target), field(field) {
		}
		void FieldSelectExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitFieldSelectExpr(this);
		}
		Expr* FieldSelectExpr::GetTarget() const {
			return target.get();
		}
		const Token& FieldSelectExpr::GetField() const {
			return field;
		}

		void FunCallArgList::AddFunCallArg(std::shared_ptr<Expr> arg) {
			funCallArgs.push_back(arg);
		}
		bool FunCallArgList::Empty() const {
			return funCallArgs.empty();
		}
		const std::list<std::shared_ptr<Expr>>& FunCallArgList::GetArgs() const {
			return funCallArgs;
		}

		FunCallExpr::FunCallExpr(std::shared_ptr<Expr> target)
			: target(target) {
		}
		FunCallExpr::FunCallExpr(std::shared_ptr<Expr> target, std::shared_ptr<FunCallArgList> args)
			: target(target), args(args) {
		}
		void FunCallExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitFunCallExpr(this);
		}
		Expr* FunCallExpr::GetTarget() const {
			return target.get();
		}
		bool FunCallExpr::ArgsEmpty() const {
			if (args) return false;
			return true;
		}
		const FunCallArgList& FunCallExpr::GetArgs() const {
			return *args.get();
		}

		CtorCallExpr::CtorCallExpr(const Token& type)
			: type(type) {
		}
		CtorCallExpr::CtorCallExpr(const Token& type, std::shared_ptr<FunCallArgList> args)
			: type(type), args(args) {
		}
		void CtorCallExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitCtorCallExpr(this);
		}
		const Token& CtorCallExpr::GetType() const {
			return type;
		}
		bool CtorCallExpr::ArgsEmpty() const {
			if (args) return false;
			return true;
		}
		const FunCallArgList& CtorCallExpr::GetArgs() const {
			return *args.get();
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

		FloatConstExpr::FloatConstExpr(const Token& floatConst)
			: floatConst(floatConst) {
		}
		void FloatConstExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitFloatConstExpr(this);
		}
		const Token& FloatConstExpr::GetFloatConst() const {
			return floatConst;
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