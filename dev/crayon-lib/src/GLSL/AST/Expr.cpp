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
				throw std::runtime_error{"Unknown binary operator!"};
				return ExprValue{};
			}
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

		static constexpr auto uintInt = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<unsigned int, int>(a, b, op);
			return result;
		};
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

		static constexpr auto floatInt = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<float, int>(a, b, op);
			return result;
		};
		static constexpr auto floatUint = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<float, unsigned int>(a, b, op);
			return result;
		};
		static constexpr auto floatFloat = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<float, float>(a, b, op);
			return result;
			};
		static constexpr auto floatDouble = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<float, double>(a, b, op);
			return result;
			};

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
		static constexpr auto doubleDouble = [](const ExprValue& a, const ExprValue& b, TokenType op) {
			ExprValue result = ComputeExpr<double, double>(a, b, op);
			return result;
		};

		static constexpr int intFundTypeOffset = 2;
		static std::function<ExprValue(const ExprValue&, const ExprValue&, TokenType)> exprEvalFuns[4][4] = {
			// INT,     UINT,       FLOAT,       DOUBLE
			{intInt,    intUint,    intFloat,    intDouble   }, // INT, UINT, FLOAT, DOUBLE
			{uintInt,   uintUint,   uintFloat,   uintDouble  }, // INT, UINT, FLOAT, DOUBLE
			{floatInt,  floatUint,  floatFloat,  floatDouble }, // INT, UINT, FLOAT, DOUBLE
			{doubleInt, doubleUint, doubleFloat, doubleDouble}, // INT, UINT, FLOAT, DOUBLE
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

			size_t leftTypeId = binaryExpr->GetLeftExpr()->GetExprTypeId();
			size_t rightTypeId = binaryExpr->GetRightExpr()->GetExprTypeId();
			const TypeSpec& leftType = envCtx.typeTable->GetType(leftTypeId);
			const TypeSpec& rightType = envCtx.typeTable->GetType(rightTypeId);
			int leftRank = GetFundamentalTypeRank(leftType.type.tokenType);
			int rightRank = GetFundamentalTypeRank(rightType.type.tokenType);

			const auto& exprEvalFun = exprEvalFuns[leftRank - intFundTypeOffset][rightRank - intFundTypeOffset];
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
			// 1.
			// const Token& intConst = intConstExpr->GetIntConst();
			// int value = static_cast<int>(std::strtol(intConst.lexeme.data(), nullptr, 10));
			// result = value;
			// 2.
			ConstVal intVal = envCtx.constTable->GetConstVal(intConstExpr->GetConstId());
			result = std::get<int>(intVal);
		}
		void ExprEvalVisitor::VisitUintConstExpr(UintConstExpr* uintConstExpr) {
			// 1.
			// const Token& uintConst = uintConstExpr->GetUintConst();
			// unsigned int value = static_cast<unsigned int>(std::strtoul(uintConst.lexeme.data(), nullptr, 10));
			// result = value;
			// 2.
			ConstVal uintVal = envCtx.constTable->GetConstVal(uintConstExpr->GetConstId());
			result = std::get<unsigned int>(uintVal);
		}
		void ExprEvalVisitor::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			// 1.
			// const Token& floatConst = floatConstExpr->GetFloatConst();
			// float value = std::strtof(floatConst.lexeme.data(), nullptr);
			// result = value;
			// 2.
			ConstVal floatVal = envCtx.constTable->GetConstVal(floatConstExpr->GetConstId());
			result = std::get<float>(floatVal);
		}
		void ExprEvalVisitor::VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) {
			// 1.
			// const Token& doubleConst = doubleConstExpr->GetDoubleConst();
			// double value = std::strtod(doubleConst.lexeme.data(), nullptr);
			// result = value;
			// 2.
			ConstVal doubleVal = envCtx.constTable->GetConstVal(doubleConstExpr->GetConstId());
			result = std::get<double>(doubleVal);
		}
		void ExprEvalVisitor::VisitGroupExpr(GroupExpr* groupExpr) {
			groupExpr->GetExpr()->Accept(this);
		}

		bool ExprEvalVisitor::ResultBool() const {
			return std::holds_alternative<bool>(result);
		}
		bool ExprEvalVisitor::ResultInt() const {
			return std::holds_alternative<int>(result);
		}
		bool ExprEvalVisitor::ResultUint() const {
			return std::holds_alternative<unsigned int>(result);
		}
		bool ExprEvalVisitor::ResultFloat() const {
			return std::holds_alternative<float>(result);
		}
		bool ExprEvalVisitor::ResultDouble() const {
			return std::holds_alternative<double>(result);
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

		void ExprTypeInferenceVisitor::VisitInitListExpr(InitListExpr* InitListExpr) {
			// Not supported yet!
		}
		void ExprTypeInferenceVisitor::VisitAssignExpr(AssignExpr* assignExpr) {
			// According to the specification, the assignment operators cannot create constant expressions.
			// In other words, "a = b" cannot be a constant expression!
			// So the expression is explicitly set to be a non-const expression.
			// The type of this expression is evaluated and set .

			Expr* rvalue = assignExpr->GetRvalue();
			rvalue->Accept(this);
			const TypeSpec& rvalueTypeSpec = envCtx.typeTable->GetType(rvalue->GetExprTypeId());

			Expr* lvalue = assignExpr->GetLvalue();
			lvalue->Accept(this);
			const TypeSpec& lvalueTypeSpec = envCtx.typeTable->GetType(lvalue->GetExprTypeId());

			if (IsTypePromotable(rvalueTypeSpec, lvalueTypeSpec)) {
				assignExpr->SetExprTypeId(lvalue->GetExprTypeId());
				assignExpr->SetExprConstState(false);
			} else {
				// throw an exception?
				// should I perhaps access an error handler somehow?
				std::runtime_error{"Can't assign the 'rvalue' to the specified 'lvalue'!"};
			}
		}
		void ExprTypeInferenceVisitor::VisitBinaryExpr(BinaryExpr* binaryExpr) {
			Expr* lhs = binaryExpr->GetLeftExpr();
			lhs->Accept(this);
			const TypeSpec& lhsTypeSpec = envCtx.typeTable->GetType(lhs->GetExprTypeId());

			Expr* rhs = binaryExpr->GetRightExpr();
			rhs->Accept(this);
			const TypeSpec& rhsTypeSpec = envCtx.typeTable->GetType(rhs->GetExprTypeId());

			const Token& binaryOp = binaryExpr->GetOperator();
			TypeSpec resTypeSpec = InferArithmeticBinaryExprType(lhsTypeSpec, rhsTypeSpec, binaryOp.tokenType);
			if (resTypeSpec.type.tokenType == TokenType::UNDEFINED) {
				// The binary operation is not defined for the types provided!
				// Should I throw an exception?
				// should I perhaps access an error handler somehow?
				std::runtime_error{"Can't perform 'binaryOp' on the specified types!"};
			}
			size_t resTypeId = envCtx.typeTable->GetTypeId(resTypeSpec);
			binaryExpr->SetExprTypeId(resTypeId);
			binaryExpr->SetExprConstState(lhs->IsConstExpr() && rhs->IsConstExpr());
		}
		void ExprTypeInferenceVisitor::VisitUnaryExpr(UnaryExpr* unaryExpr) {
			// Not fully supported yet!
			const Token& unaryOp = unaryExpr->GetOperator();
			Expr* exprOperand = unaryExpr->GetExpr();
			// TODO: do I need to do anything else here?
			unaryExpr->SetExprTypeId(exprOperand->GetExprTypeId());
			unaryExpr->SetExprConstState(exprOperand->IsConstExpr());
		}
		void ExprTypeInferenceVisitor::VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) {
			// Not supported yet!
		}
		void ExprTypeInferenceVisitor::VisitFunCallExpr(FunCallExpr* funCallExpr) {
			// Not supported yet!
		}
		void ExprTypeInferenceVisitor::VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) {
			TypeSpec ctorTypeSpec = ctorCallExpr->GetType();
			size_t typeId = envCtx.typeTable->GetTypeId(ctorTypeSpec);
			ctorCallExpr->SetExprTypeId(typeId);
			bool isCtorCallConstExpr{true};
			for (const std::shared_ptr<Expr>& arg : ctorCallExpr->GetArgs()) {
				if (!arg->IsConstExpr()) {
					isCtorCallConstExpr = false;
					break;
				}
			}
			ctorCallExpr->SetExprConstState(isCtorCallConstExpr);
		}
		void ExprTypeInferenceVisitor::VisitVarExpr(VarExpr* varExpr) {
			std::string_view varName = varExpr->GetVariable().lexeme;
			std::shared_ptr<VarDecl> varDecl = envCtx.currentScope->GetVarDecl(varName);
			TypeSpec varExprTypeSpec = varDecl->GetVarTypeSpec();
			size_t typeId = envCtx.typeTable->GetTypeId(varExprTypeSpec);
			varExpr->SetExprTypeId(typeId);
			if (varDecl->IsConst()) {
				varExpr->SetExprConstState(true);
			}
		}
		void ExprTypeInferenceVisitor::VisitIntConstExpr(IntConstExpr* intConstExpr) {
			TypeSpec intConstTypeSpec{};
			intConstTypeSpec.type = GenerateToken(TokenType::INT);
			size_t typeId = envCtx.typeTable->GetTypeId(intConstTypeSpec);
			intConstExpr->SetExprTypeId(typeId);
			intConstExpr->SetExprConstState(true);
		}
		void ExprTypeInferenceVisitor::VisitUintConstExpr(UintConstExpr* uintConstExpr) {
			TypeSpec uintConstTypeSpec{};
			uintConstTypeSpec.type = GenerateToken(TokenType::UINT);
			size_t typeId = envCtx.typeTable->GetTypeId(uintConstTypeSpec);
			uintConstExpr->SetExprTypeId(typeId);
			uintConstExpr->SetExprConstState(true);
		}
		void ExprTypeInferenceVisitor::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			TypeSpec floatConstTypeSpec{};
			floatConstTypeSpec.type = GenerateToken(TokenType::FLOAT);
			size_t typeId = envCtx.typeTable->GetTypeId(floatConstTypeSpec);
			floatConstExpr->SetExprTypeId(typeId);
			floatConstExpr->SetExprConstState(true);
		}
		void ExprTypeInferenceVisitor::VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) {
			TypeSpec doubleConstTypeSpec{};
			doubleConstTypeSpec.type = GenerateToken(TokenType::DOUBLE);
			size_t typeId = envCtx.typeTable->GetTypeId(doubleConstTypeSpec);
			doubleConstExpr->SetExprTypeId(typeId);
			doubleConstExpr->SetExprConstState(true);
		}
		void ExprTypeInferenceVisitor::VisitGroupExpr(GroupExpr* groupExpr) {
			groupExpr->Accept(this);
			size_t groupExprTypeId = groupExpr->GetExpr()->GetExprTypeId();
			groupExpr->SetExprTypeId(groupExprTypeId);
			groupExpr->SetExprConstState(groupExpr->GetExpr()->IsConstExpr());
		}

		void ExprTypeInferenceVisitor::SetEnvironmentContext(const EnvironmentContext& envCtx) {
			this->envCtx = envCtx;
		}
		void ExprTypeInferenceVisitor::ResetEnvironmentContext() {
			this->envCtx = EnvironmentContext();
		}

		void Expr::SetExprTypeId(size_t typeId) {
			this->typeId = typeId;
		}
		size_t Expr::GetExprTypeId() const {
			return typeId;
		}
		void Expr::SetExprConstState(bool isConst) {
			this->isConst = isConst;
		}
		bool Expr::IsConstExpr() const {
			return isConst;
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

		void CallExpr::AddArg(std::shared_ptr<Expr> arg) {
			args.push_back(arg);
		}
		bool CallExpr::HasArgs() const {
			return args.empty();
		}
		const std::vector<std::shared_ptr<Expr>>& CallExpr::GetArgs() const {
			return args;
		}

		FunCallExpr::FunCallExpr(std::shared_ptr<Expr> target)
			: target(target) {
		}
		void FunCallExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitFunCallExpr(this);
		}
		Expr* FunCallExpr::GetTarget() const {
			return target.get();
		}

		CtorCallExpr::CtorCallExpr(const TypeSpec& typeSpec)
			: typeSpec(typeSpec) {
		}
		void CtorCallExpr::Accept(ExprVisitor* exprVisitor) {
			exprVisitor->VisitCtorCallExpr(this);
		}
		const TypeSpec& CtorCallExpr::GetType() const {
			return typeSpec;
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

		IntConstExpr::IntConstExpr(const Token& intConst, ConstId intConstId)
			: intConst(intConst), intConstId(intConstId) {
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
		int IntConstExpr::GetValue() const {
			return ParseIntValue(intConst.lexeme);
		}
		ConstId IntConstExpr::GetConstId() const {
			return intConstId;
		}

		UintConstExpr::UintConstExpr(const Token& uintConst, ConstId uintConstId)
			: uintConst(uintConst), uintConstId(uintConstId) {
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
		unsigned int UintConstExpr::GetValue() const {
			return ParseUintValue(uintConst.lexeme);
		}
		ConstId UintConstExpr::GetConstId() const {
			return uintConstId;
		}

		FloatConstExpr::FloatConstExpr(const Token& floatConst, ConstId floatConstId)
			: floatConst(floatConst), floatConstId(floatConstId) {
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
		float FloatConstExpr::GetValue() const {
			return ParseFloatValue(floatConst.lexeme);
		}
		ConstId FloatConstExpr::GetConstId() const {
			return floatConstId;
		}

		DoubleConstExpr::DoubleConstExpr(const Token& doubleConst, ConstId doubleConstId)
			: doubleConst(doubleConst), doubleConstId(doubleConstId) {
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
		double DoubleConstExpr::GetValue() const {
			return ParseDoubleValue(doubleConst.lexeme);
		}
		ConstId DoubleConstExpr::GetConstId() const {
			return doubleConstId;
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