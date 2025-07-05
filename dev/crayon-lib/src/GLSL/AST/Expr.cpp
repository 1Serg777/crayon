#include "GLSL/AST/Expr.h"

#include <array>
#include <cstdlib>
#include <iostream>
#include <functional>

namespace crayon {
	namespace glsl {
		
		using ExprValue = ExprEvalVisitor::ExprValue;
		using ExprFun = std::function<ExprValue(const ExprValue&, const ExprValue&, TokenType op)>;

		template <typename T, typename U>
		static ExprValue ComputeExpr(const ExprValue& a, const ExprValue& b, TokenType op) {
			if (op == TokenType::PLUS) {
				return ExprValue{std::get<T>(a) + std::get<U>(b)};
			} else if (op == TokenType::DASH) {
				return ExprValue{std::get<T>(a) - std::get<U>(b)};
			} else if (op == TokenType::STAR) {
				return ExprValue{std::get<T>(a) * std::get<U>(b)};
			} else if (op == TokenType::SLASH) {
				return ExprValue{std::get<T>(a) / std::get<U>(b)};
			} else {
				throw std::runtime_error{"Unknown operator!"};
				return ExprValue{};
			}
		}

		template <typename T>
		static void PromoteExprValue(ExprValue& a, int rankDiff) {
			/*
			const GlslExprType& leftType = binaryExpr->GetLeftExpr()->GetExprType();
			const GlslExprType& rightType = binaryExpr->GetRightExpr()->GetExprType();
			int leftRank = GetFundamentalTypeRank(leftType.type);
			int rightRank = GetFundamentalTypeRank(rightType.type);
			if (leftRank < rightRank) {
				// 1. The left-hand side operand must be promoted.
				// TODO
			} else if (rightRank < leftRank) {
				// 2. The right-hand side operand must be promoted.
				// TODO
			}
			*/
		}

		static constexpr auto intInt = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<int, int>(a, b, op);
			return result;
		};
		static constexpr auto intUint = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<int, unsigned int>(a, b, op);
			return result;
			};
		static constexpr auto intFloat = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<int, float>(a, b, op);
			return result;
			};
		static constexpr auto intDouble = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<int, double>(a, b, op);
			return result;
			};

		// Not allowed.
		/*
		static constexpr auto uintInt = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<unsigned int, int>(a, b, op);
			return result;
		};
		*/
		static constexpr auto uintUint = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<unsigned int, unsigned int>(a, b, op);
			return result;
			};
		static constexpr auto uintFloat = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<unsigned int, float>(a, b, op);
			return result;
			};
		static constexpr auto uintDouble = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<unsigned int, double>(a, b, op);
			return result;
			};

		// Not allowed.
		/*
		static constexpr auto floatInt = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<float, int>(a, b, op);
			return result;
		};
		static constexpr auto floatUint = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<float, unsigned int>(a, b, op);
			return result;
		};
		*/
		static constexpr auto floatFloat = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<float, float>(a, b, op);
			return result;
			};
		static constexpr auto floatDouble = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<float, double>(a, b, op);
			return result;
			};

		// Not allowed.
		/*
		static constexpr auto doubleInt = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<double, int>(a, b, op);
			return result;
		};
		static constexpr auto doubleUint = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<double, unsigned int>(a, b, op);
			return result;
		};
		static constexpr auto doubleFloat = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<double, float>(a, b, op);
			return result;
		};
		*/
		static constexpr auto doubleDouble = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<double, double>(a, b, op);
			return result;
		};

		static std::function<ExprValue(const ExprValue&, const ExprValue&, TokenType)> exprEvalFuns[4][4] = {
			// INT,       UINT,      FLOAT,       DOUBLE
			{intInt,    intUint,   intFloat,   intDouble   }, // INT, UINT, FLOAT, DOUBLE
			{ExprFun{}, uintUint,  uintFloat,  uintDouble  }, // INT, UINT, FLOAT, DOUBLE
			{ExprFun{}, ExprFun{}, floatFloat, floatDouble }, // INT, UINT, FLOAT, DOUBLE
			{ExprFun{}, ExprFun{}, ExprFun{},  doubleDouble}, // INT, UINT, FLOAT, DOUBLE
		};

		void ExprEvalVisitor::VisitInitListExpr(InitListExpr* initListExpr) {
			// [TODO]: implement environments first!
		}
		void ExprEvalVisitor::VisitAssignExpr(AssignExpr* assignExpr) {
			// [TODO]: implement environments first!
		}
		void ExprEvalVisitor::VisitBinaryExpr(BinaryExpr* binaryExpr) {
			binaryExpr->GetLeftExpr()->Accept(this);
			ExprValue left = result;
			binaryExpr->GetRightExpr()->Accept(this);
			ExprValue right = result;

			const GlslExprType& leftType = binaryExpr->GetLeftExpr()->GetExprType();
			const GlslExprType& rightType = binaryExpr->GetRightExpr()->GetExprType();
			int leftRank = GetFundamentalTypeRank(leftType.type);
			int rightRank = GetFundamentalTypeRank(rightType.type);

			const auto& exprEvalFun = exprEvalFuns[static_cast<size_t>(leftType.type)][static_cast<size_t>(rightType.type)];
			result = exprEvalFun(left, right, binaryExpr->GetOperator().tokenType);
		}
		void ExprEvalVisitor::VisitUnaryExpr(UnaryExpr* unaryExpr) {
			// TODO
			/*
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
			*/
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

		GlslBasicType ExprEvalVisitor::GetExprType() const {
			return static_cast<GlslBasicType>(result.index());
		}
		bool ExprEvalVisitor::ExprConstant() const {
			return exprConstant;
		}

		bool ExprEvalVisitor::ResultBool() const {
			return std::holds_alternative<bool>(result);
		}
		bool ExprEvalVisitor::ResultInt() const {
			return std::holds_alternative<bool>(result);
		}
		bool ExprEvalVisitor::ResultUint() const {
			return std::holds_alternative<bool>(result);
		}
		bool ExprEvalVisitor::ResultFloat() const {
			return std::holds_alternative<bool>(result);
		}
		bool ExprEvalVisitor::ResultDouble() const {
			return std::holds_alternative<bool>(result);
		}
		bool ExprEvalVisitor::ResultUndefined() const {
			return resultUndefined;
		}

		bool ExprEvalVisitor::GetBoolResult() const {
			return std::get<bool>(result);
		}
		int ExprEvalVisitor::GetIntResult() const {
			return std::get<int>(result);
		}
		unsigned int ExprEvalVisitor::GetUintResult() const {
			return std::get<unsigned int>(result);
		}
		float ExprEvalVisitor::GetFloatResult() const {
			return std::get<float>(result);
		}
		double ExprEvalVisitor::GetDoubleResult() const {
			return std::get<double>(result);
		}

		// Additional expression visitors.
		/*
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
		*/

		void ExprTypeInferenceVisitor::VisitInitListExpr(InitListExpr* InitListExpr) {
			// Not supported yet!
		}
		void ExprTypeInferenceVisitor::VisitAssignExpr(AssignExpr* assignExpr) {
			// According to the specification, the assignment operators cannot create constant expressions.
			// However, the type of this expression must still be evaluated.
			Expr* lvalue = assignExpr->GetLvalue(); // Must have its type evaluated.
			Expr* rvalue = assignExpr->GetRvalue(); // Rvalue's expression type should've already been inferred.

			// Do we also assume that the lvalue operand's type has been inferred already?
			// lvalue->Accept(this);
			GlslExprType assignExprType{};
			if (TypePromotable(rvalue->GetExprType(), lvalue->GetExprType())) {
				assignExprType = lvalue->GetExprType();
				assignExprType.constExpr = false;
			}
			assignExpr->SetExprType(assignExprType);
		}
		void ExprTypeInferenceVisitor::VisitBinaryExpr(BinaryExpr* binaryExpr) {
			// Not supported yet!
		}
		void ExprTypeInferenceVisitor::VisitUnaryExpr(UnaryExpr* unaryExpr) {
			// Not supported yet!
		}
		void ExprTypeInferenceVisitor::VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) {
			// Not supported yet!
		}
		void ExprTypeInferenceVisitor::VisitFunCallExpr(FunCallExpr* funCallExpr) {
			// Not supported yet!
		}
		void ExprTypeInferenceVisitor::VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) {
			// Not supported yet!
		}
		void ExprTypeInferenceVisitor::VisitVarExpr(VarExpr* varExpr) {
			std::shared_ptr<VarDecl> varDecl = environment->GetVarDecl(varExpr->GetVariable().lexeme);
			varExpr->SetExprType(InferVarExprType(varDecl.get()));
		}
		void ExprTypeInferenceVisitor::VisitIntConstExpr(IntConstExpr* intConstExpr) {
			GlslExprType intExprType{};
			intExprType.type = GlslBasicType::INT;
			intExprType.constExpr = true;
			intConstExpr->SetExprType(intExprType);
		}
		void ExprTypeInferenceVisitor::VisitUintConstExpr(UintConstExpr* uintConstExpr) {
			GlslExprType uintExprType{};
			uintExprType.type = GlslBasicType::UINT;
			uintExprType.constExpr = true;
			uintConstExpr->SetExprType(uintExprType);
		}
		void ExprTypeInferenceVisitor::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			GlslExprType floatExprType{};
			floatExprType.type = GlslBasicType::FLOAT;
			floatExprType.constExpr = true;
			floatConstExpr->SetExprType(floatExprType);
		}
		void ExprTypeInferenceVisitor::VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) {
			GlslExprType doubleExprType{};
			doubleExprType.type = GlslBasicType::DOUBLE;
			doubleExprType.constExpr = true;
			doubleConstExpr->SetExprType(doubleExprType);
		}
		void ExprTypeInferenceVisitor::VisitGroupExpr(GroupExpr* groupExpr) {
			groupExpr->SetExprType(groupExpr->GetExpr()->GetExprType());
		}

		void ExprTypeInferenceVisitor::SetEnvironment(const Environment* environment) {
			this->environment = environment;
		}
		void ExprTypeInferenceVisitor::ResetEnvironment() {
			environment = nullptr;
		}

		GlslExprType ExprTypeInferenceVisitor::InferVarExprType(VarDecl* varDecl) {
			// TODO: handle array variable access expressions such as "a[0]" or "a[0][2]", etc.
			// First we retrieve the corresponding variable declaration.
			// We assume that the initial check of whether such a variable exists has already been done before.
			const FullSpecType& varType = varDecl->GetVarType();
			const Token& varName = varDecl->GetVarName();
			// Now we can infer the type of the expression that is accessing the variable.
			GlslExprType exprType{};
			exprType.type = GetGlslBasicType(varType.specifier.type.tokenType);
			exprType.name = varType.specifier.type.lexeme;
			if (varType.qualifier.Const()) {
				if (varDecl->HasInitializerExpr()) {
					exprType.constExpr = varDecl->GetInitializerExpr()->GetExprType().constExpr;
				} else {
					exprType.constExpr = true;
				}
				// The variable access expression is also a constant expression according to the specification.
				// 4.3.3 Constant Expresssions, p.51
			}
			if (varDecl->IsArray()) {
				// A variable declaration like "int[3] a[2]" would have the "int[2][3]" type.
				// 1. First we go over the variable dimensions.
				const std::vector<std::shared_ptr<Expr>>& dimensions = varDecl->GetDimensions();
				for (size_t i = 0; i < dimensions.size(); i++) {
					// Each "dimensions[i]" expression must be a constant integer expression!
					dimensions[i]->Accept(&exprEvalVisitor);
					if (!exprEvalVisitor.ExprConstant()) {
						throw std::runtime_error{"Array size expression must be a constant expression!"};
					}
					if (exprEvalVisitor.ResultInt()) {
						int result = exprEvalVisitor.GetIntResult();
						if (result < 0) {
							throw std::runtime_error{"Array size expression must be a non-negative integral expression!"};
						}
						exprType.dimensions.push_back(static_cast<size_t>(result));
					} else if (exprEvalVisitor.ResultUint()) {
						exprType.dimensions.push_back(exprEvalVisitor.GetUintResult());
					} else {
						throw std::runtime_error{"Array size expression must be an integral expression!"};
					}
				}
				// 2. Then we go over the type dimensions.
				for (size_t i = 0; i < varType.specifier.dimensions.size(); i++) {
					// TODO
					// Each "varType.specifier.dimensions[i]" expression must be a constant integer expression!
					// exprType.dimensions.push_back(varType.specifier.dimensions[i]);
					// TODO
					// Each "dimensions[i]" expression must be a constant integer expression!
					varType.specifier.dimensions[i]->Accept(&exprEvalVisitor);
					if (!exprEvalVisitor.ExprConstant()) {
						throw std::runtime_error{"Array size expression must be a constant expression!"};
					}
					if (exprEvalVisitor.ResultInt()) {
						int result = exprEvalVisitor.GetIntResult();
						if (result < 0) {
							throw std::runtime_error{"Array size expression must be a non-negative integral expression!"};
						}
						exprType.dimensions.push_back(static_cast<size_t>(result));
					}
					else if (exprEvalVisitor.ResultUint()) {
						exprType.dimensions.push_back(exprEvalVisitor.GetUintResult());
					}
					else {
						throw std::runtime_error{"Array size expression must be an integral expression!"};
					}
				}
			}
			return exprType;
		}

		void Expr::SetExprType(const GlslExprType& exprType) {
			this->exprType = exprType;
		}
		const GlslExprType& Expr::GetExprType() const {
			return exprType;
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
		std::string_view AssignExpr::ToString() const {
			std::string_view lvalueStr = lvalue->ToString();
			std::string_view rvalueStr = rvalue->ToString();
			size_t exprStrSize = rvalueStr.end() - lvalueStr.begin();
			return std::string_view(lvalueStr.data(), exprStrSize);
		}
		std::pair<size_t, size_t> AssignExpr::GetExprColBounds() const {
			std::pair<size_t, size_t> lvalueColBounds = lvalue->GetExprColBounds();
			std::pair<size_t, size_t> rvalueColBounds = rvalue->GetExprColBounds();
			return std::pair<size_t, size_t>(lvalueColBounds.first, rvalueColBounds.second);
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
		std::string_view VarExpr::ToString() const {
			// TODO: add array specifier.
			return variable.lexeme;
		}
		std::pair<size_t, size_t> VarExpr::GetExprColBounds() const {
			// TODO: add array specifier.
			return std::pair<size_t, size_t>(variable.startCol, variable.endCol);
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
		std::string_view IntConstExpr::ToString() const {
			return intConst.lexeme;
		}
		std::pair<size_t, size_t> IntConstExpr::GetExprColBounds() const {
			return std::pair<size_t, size_t>(intConst.startCol, intConst.endCol);
		}
		const Token& IntConstExpr::GetIntConst() const {
			return intConst;
		}

		UintConstExpr::UintConstExpr(const Token& uintConst)
			: uintConst(uintConst) {
		}
		void UintConstExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitUintConstExpr(this);
		}
		std::string_view UintConstExpr::ToString() const {
			return uintConst.lexeme;
		}
		std::pair<size_t, size_t> UintConstExpr::GetExprColBounds() const {
			return std::pair<size_t, size_t>(uintConst.startCol, uintConst.endCol);
		}
		const Token& UintConstExpr::GetUintConst() const {
			return uintConst;
		}

		FloatConstExpr::FloatConstExpr(const Token& floatConst)
			: floatConst(floatConst) {
		}
		void FloatConstExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitFloatConstExpr(this);
		}
		std::string_view FloatConstExpr::ToString() const {
			return floatConst.lexeme;
		}
		std::pair<size_t, size_t> FloatConstExpr::GetExprColBounds() const {
			return std::pair<size_t, size_t>(floatConst.startCol, floatConst.endCol);
		}
		const Token& FloatConstExpr::GetFloatConst() const {
			return floatConst;
		}

		DoubleConstExpr::DoubleConstExpr(const Token& doubleConst)
			: doubleConst(doubleConst) {
		}
		void DoubleConstExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitDoubleConstExpr(this);
		}
		std::string_view DoubleConstExpr::ToString() const {
			return doubleConst.lexeme;
		}
		std::pair<size_t, size_t> DoubleConstExpr::GetExprColBounds() const {
			return std::pair<size_t, size_t>(doubleConst.startCol, doubleConst.endCol);
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
		std::string_view GroupExpr::ToString() const {
			std::string_view exprStr = expr->ToString();
			size_t exprSize = exprStr.size();
			// Include the openning parenthesis "(" and the closing parenthesis ")".
			return std::string_view(exprStr.data() - 1, exprSize + 2);
		}
		std::pair<size_t, size_t> GroupExpr::GetExprColBounds() const {
			std::pair<size_t, size_t> exprColBounds = expr->GetExprColBounds();
			return std::pair<size_t, size_t>(exprColBounds.first - 1, exprColBounds.second + 1);
		}
		Expr* GroupExpr::GetExpr() const {
			return expr.get();
		}
	
	}
}