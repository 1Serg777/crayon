#include "GLSL/AST/Expr.h"

#include <cstdlib>
#include <iostream>

namespace crayon {
	namespace glsl {

		void ExprEvalVisitor::VisitInitListExpr(InitListExpr* initListExpr) {
			// [TODO]: implement environments first!
		}
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
		void ExprEvalVisitor::VisitUnaryExpr(UnaryExpr* unaryExpr) {
			unaryExpr->GetExpr()->Accept(this);
			int exprRes = result;

			switch (unaryExpr->GetOperator().tokenType) {
				case TokenType::PLUS: {
					result = +exprRes;
				}
				break;
				case TokenType::DASH: {
					result = -exprRes;
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
		void ExprEvalVisitor::VisitUintConstExpr(UintConstExpr* uintConstExpr) {
			// TODO
		}
		void ExprEvalVisitor::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			const Token& floatConst = floatConstExpr->GetFloatConst();
			float value = std::strtof(floatConst.lexeme.data(), nullptr);
			// result = value;
			// TODO: think about how to return results of different types!
			//       1) A C-style union?
			//       2) A C++-style type-safe union?
		}
		void ExprEvalVisitor::VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) {
			// TODO
		}
		void ExprEvalVisitor::VisitGroupExpr(GroupExpr* groupExpr) {
			groupExpr->GetExpr()->Accept(this);
		}

		int ExprEvalVisitor::GetResult() const {
			return result;
		}

		void ExprPostfixPrinterVisitor::VisitInitListExpr(InitListExpr* initListExpr) {
			// TODO:
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
		void ExprPostfixPrinterVisitor::VisitUnaryExpr(UnaryExpr* unaryExpr) {
			unaryExpr->GetExpr()->Accept(this);
			std::cout << unaryExpr->GetOperator().lexeme << " ";
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
		void ExprPostfixPrinterVisitor::VisitUintConstExpr(UintConstExpr* uintConstExpr) {
			std::cout << uintConstExpr->GetUintConst().lexeme << " ";
		}
		void ExprPostfixPrinterVisitor::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			std::cout << floatConstExpr->GetFloatConst().lexeme << " ";
		}
		void ExprPostfixPrinterVisitor::VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) {
			std::cout << doubleConstExpr->GetDoubleConst().lexeme << " ";
		}
		void ExprPostfixPrinterVisitor::VisitGroupExpr(GroupExpr* groupExpr) {
			groupExpr->GetExpr()->Accept(this);
		}
		
		void ExprParenPrinterVisitor::VisitInitListExpr(InitListExpr* InitListExpr) {
			// TODO:
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
		void ExprParenPrinterVisitor::VisitUnaryExpr(UnaryExpr* unaryExpr) {
			std::cout << "( ";
			std::cout << unaryExpr->GetOperator().lexeme << " ";
			unaryExpr->GetExpr()->Accept(this);
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
		void ExprParenPrinterVisitor::VisitUintConstExpr(UintConstExpr* uintConstExpr) {
			std::cout << uintConstExpr->GetUintConst().lexeme;
		}
		void ExprParenPrinterVisitor::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			std::cout << floatConstExpr->GetFloatConst().lexeme;
		}
		void ExprParenPrinterVisitor::VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) {
			std::cout << doubleConstExpr->GetDoubleConst().lexeme;
		}
		void ExprParenPrinterVisitor::VisitGroupExpr(GroupExpr* groupExpr) {
			groupExpr->GetExpr()->Accept(this);
		}

		void ExprTypeInferenceVisitor::VisitInitListExpr(InitListExpr* InitListExpr) {
			// TODO: implement scopes and environments first!s
		}
		void ExprTypeInferenceVisitor::VisitAssignExpr(AssignExpr* assignExpr) {
			// TODO: implement scopes and environments first!
		}
		void ExprTypeInferenceVisitor::VisitBinaryExpr(BinaryExpr* binaryExpr) {
			// TODO: implement scopes and environments first!
		}
		void ExprTypeInferenceVisitor::VisitUnaryExpr(UnaryExpr* unaryExpr) {
			unaryExpr->SetType(unaryExpr->GetExpr()->GetType());
		}
		void ExprTypeInferenceVisitor::VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) {
			// TODO: implement scopes and environments first!
		}
		void ExprTypeInferenceVisitor::VisitFunCallExpr(FunCallExpr* funCallExpr) {
			// TODO: implement scopes and environments first!
		}
		void ExprTypeInferenceVisitor::VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) {
			// TODO: implement scopes and environments first!
		}
		void ExprTypeInferenceVisitor::VisitVarExpr(VarExpr* varExpr) {
			// TODO: implement scopes and environments first!
		}
		void ExprTypeInferenceVisitor::VisitIntConstExpr(IntConstExpr* intConstExpr) {
			intConstExpr->SetType(GlslExprType::INT);
		}
		void ExprTypeInferenceVisitor::VisitUintConstExpr(UintConstExpr* uintConstExpr) {
			uintConstExpr->SetType(GlslExprType::UINT);
		}
		void ExprTypeInferenceVisitor::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			floatConstExpr->SetType(GlslExprType::FLOAT);
		}
		void ExprTypeInferenceVisitor::VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) {
			doubleConstExpr->SetType(GlslExprType::DOUBLE);
		}
		void ExprTypeInferenceVisitor::VisitGroupExpr(GroupExpr* groupExpr) {
			groupExpr->SetType(groupExpr->GetExpr()->GetType());
		}

		Expr::Expr(GlslExprType type)
				: type(type) {
		}
		void Expr::SetType(GlslExprType type) {
			this->type = type;
		}
		GlslExprType Expr::GetType() const {
			return type;
		}

		void InitListExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitInitListExpr(this);
		}
		void InitListExpr::AddInitExpr(std::shared_ptr<Expr> initExpr) {
			initExprs.push_back(initExpr);
		}
		bool InitListExpr::IsEmpty() const {
			return initExprs.empty();
		}
		const std::vector<std::shared_ptr<Expr>>& InitListExpr::GetInitExprs() const {
			return initExprs;
		}

		AssignExpr::AssignExpr(std::shared_ptr<Expr> lvalue, std::shared_ptr<Expr> rvalue, const Token& assignOp)
			: lvalue(lvalue), rvalue(rvalue), assignOp(assignOp) {
		}
		void AssignExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitAssignExpr(this);
		}
		const Token& AssignExpr::GetAssignOp() const {
			return assignOp;
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

		UnaryExpr::UnaryExpr(const Token& op, std::shared_ptr<Expr> expr)
							 : op(op), expr(expr) {
		}
		void UnaryExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitUnaryExpr(this);
		}
		Expr* UnaryExpr::GetExpr() const {
			return expr.get();
		}
		const Token& UnaryExpr::GetOperator() const {
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
				: Expr(GlslExprType::INT), intConst(intConst) {
		}
		void IntConstExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitIntConstExpr(this);
		}
		const Token& IntConstExpr::GetIntConst() const {
			return intConst;
		}

		UintConstExpr::UintConstExpr(const Token& uintConst)
				: Expr(GlslExprType::UINT), uintConst(uintConst) {
		}
		void UintConstExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitUintConstExpr(this);
		}
		const Token& UintConstExpr::GetUintConst() const {
			return uintConst;
		}

		FloatConstExpr::FloatConstExpr(const Token& floatConst)
				: Expr(GlslExprType::FLOAT), floatConst(floatConst) {
		}
		void FloatConstExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitFloatConstExpr(this);
		}
		const Token& FloatConstExpr::GetFloatConst() const {
			return floatConst;
		}

		DoubleConstExpr::DoubleConstExpr(const Token& doubleConst)
				: Expr(GlslExprType::DOUBLE), doubleConst(doubleConst) {
		}
		void DoubleConstExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitDoubleConstExpr(this);
		}
		const Token& DoubleConstExpr::GetDoubleConst() const {
			return doubleConst;
		}

		GroupExpr::GroupExpr(std::shared_ptr<Expr> expr)
			: expr(expr) {
		}
		void GroupExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitGroupExpr(this);
		}
		Expr* GroupExpr::GetExpr() const {
			return expr.get();
		}
	
	}
}