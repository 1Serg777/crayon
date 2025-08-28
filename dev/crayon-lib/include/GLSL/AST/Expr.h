#pragma once

#include "GLSL/Token.h"
#include "GLSL/Type.h"
#include "GLSL/Value.h"

#include "GLSL/Analyzer/Environment.h"

#include <cstdint>
#include <list>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

namespace crayon {
	namespace glsl {

		class InitListExpr;
		class AssignExpr;
		class BinaryExpr;
		class UnaryExpr;
		class FieldSelectExpr;
		class FunCallExpr;
		class CtorCallExpr;
		class VarExpr;
		class IntConstExpr;
		class UintConstExpr;
		class FloatConstExpr;
		class DoubleConstExpr;
		class GroupExpr;

		class ExprVisitor {
		public:
			virtual void VisitInitListExpr(InitListExpr* initListExpr) = 0;
			virtual void VisitAssignExpr(AssignExpr* assignExpr) = 0;
			virtual void VisitBinaryExpr(BinaryExpr* binaryExpr) = 0;
			virtual void VisitUnaryExpr(UnaryExpr* unaryExpr) = 0;
			virtual void VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) = 0;
			virtual void VisitFunCallExpr(FunCallExpr* funCallExpr) = 0;
			virtual void VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) = 0;
			virtual void VisitVarExpr(VarExpr* varExpr) = 0;
			virtual void VisitIntConstExpr(IntConstExpr* intConstExpr) = 0;
			virtual void VisitUintConstExpr(UintConstExpr* uintConstExpr) = 0;
			virtual void VisitFloatConstExpr(FloatConstExpr* floatConstExpr) = 0;
			virtual void VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) = 0;
			virtual void VisitGroupExpr(GroupExpr* groupExpr) = 0;
		};

		class ExprEvalVisitor : public ExprVisitor {
		public:
			using ExprValue = std::variant<bool, int, unsigned int, float, double>;

			void VisitInitListExpr(InitListExpr* initListExpr) override;
			void VisitAssignExpr(AssignExpr* assignExpr) override;
			void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitUnaryExpr(UnaryExpr* unaryExpr) override;
			void VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) override;
			void VisitFunCallExpr(FunCallExpr* funCallExpr) override;
			void VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitUintConstExpr(UintConstExpr* uintConstExpr) override;
			void VisitFloatConstExpr(FloatConstExpr* floatConstExpr) override;
			void VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;

			GlslBasicType GetExprType() const;
			bool ExprConstant() const;

			bool ResultBool() const;
			bool ResultInt() const;
			bool ResultUint() const;
			bool ResultFloat() const;
			bool ResultDouble() const;
			bool ResultUndefined() const;

			bool GetBoolResult() const;
			int GetIntResult() const;
			unsigned int GetUintResult() const;
			float GetFloatResult() const;
			double GetDoubleResult() const;

		private:
			ExprValue result;
			bool exprConstant{false};
			bool resultUndefined{false};
		};

		// Additional expr visitors.
		/*
		class ExprPostfixPrinterVisitor : public ExprVisitor {
		public:
			void VisitInitListExpr(InitListExpr* initListExpr) override;
			void VisitAssignExpr(AssignExpr* assignExpr) override;
			void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitUnaryExpr(UnaryExpr* unaryExpr) override;
			void VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) override;
			void VisitFunCallExpr(FunCallExpr* funCallExpr) override;
			void VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitUintConstExpr(UintConstExpr* uintConstExpr) override;
			void VisitFloatConstExpr(FloatConstExpr* floatConstExpr) override;
			void VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;
		};

		class ExprParenPrinterVisitor : public ExprVisitor {
		public:
			void VisitInitListExpr(InitListExpr* initListExpr) override;
			void VisitAssignExpr(AssignExpr* assignExpr) override;
			void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitUnaryExpr(UnaryExpr* unaryExpr) override;
			void VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) override;
			void VisitFunCallExpr(FunCallExpr* funCallExpr) override;
			void VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitUintConstExpr(UintConstExpr* uintConstExpr) override;
			void VisitFloatConstExpr(FloatConstExpr* floatConstExpr) override;
			void VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;
		};
		*/

		class ExprTypeInferenceVisitor : public ExprVisitor {
		public:
			void VisitInitListExpr(InitListExpr* initListExpr) override;
			void VisitAssignExpr(AssignExpr* assignExpr) override;
			void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitUnaryExpr(UnaryExpr* unaryExpr) override;
			void VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) override;
			void VisitFunCallExpr(FunCallExpr* funCallExpr) override;
			void VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitUintConstExpr(UintConstExpr* uintConstExpr) override;
			void VisitFloatConstExpr(FloatConstExpr* floatConstExpr) override;
			void VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;

			// Returns the type of an expression where the variable is used directly.
			// i.e., if we have a variable declared as "int[3] a[2]", then if it's used
			// in an expression by specifying its name directly as "a", its type will be "int[2][3]".
			// GlslExprType InferVarExprType(VarExpr* varExpr);
			GlslExprType InferVarExprType(VarDecl* varDecl);

			void SetEnvironmentContext(const EnvironmentContext& envCtx);
			void ResetEnvironmentContext();

		private:
			ExprEvalVisitor exprEvalVisitor;
			EnvironmentContext envCtx;
		};

		class Expr {
		public:
			Expr() = default;
			virtual ~Expr() = default;

			virtual void Accept(ExprVisitor* exprVisitor) = 0;
			
			void SetExprType(const GlslExprType& exprType);
			const GlslExprType& GetExprType() const;

			virtual std::string_view ToString() const {return std::string_view();}
			virtual std::pair<size_t, size_t> GetExprColBounds() const {return std::pair<size_t, size_t>();}

		protected:
			GlslExprType exprType;
		};

		class InitListExpr : public Expr {
		public:
			virtual ~InitListExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			void AddInitExpr(std::shared_ptr<Expr> initExpr);

			bool IsEmpty() const;
			const std::vector<std::shared_ptr<Expr>>& GetInitExprs() const;

		private:
			std::vector<std::shared_ptr<Expr>> initExprs;
		};

		class AssignExpr : public Expr {
		public:
			AssignExpr(std::shared_ptr<Expr> lvalue, std::shared_ptr<Expr> rvalue, const Token& assignOp);
			virtual ~AssignExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;
			std::string_view ToString() const override;
			std::pair<size_t, size_t> GetExprColBounds() const override;

			const Token& GetAssignOp() const;

			Expr* GetLvalue() const;
			Expr* GetRvalue() const;

		private:
			Token assignOp;
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
			Token op;
		};

		class UnaryExpr : public Expr {
		public:
			UnaryExpr(const Token& op, std::shared_ptr<Expr> expr);
			virtual ~UnaryExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			Expr* GetExpr() const;
			const Token& GetOperator() const;

		private:
			Token op;
			std::shared_ptr<Expr> expr;
		};

		class FieldSelectExpr : public Expr {
		public:
			FieldSelectExpr(std::shared_ptr<Expr> target, const Token& field);
			virtual ~FieldSelectExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			Expr* GetTarget() const;
			const Token& GetField() const;

		private:
			std::shared_ptr<Expr> target;
			Token field;
		};

		class FunCallArgList {
		public:
			void AddFunCallArg(std::shared_ptr<Expr> arg);
			bool Empty() const;
			const std::vector<std::shared_ptr<Expr>>& GetArgs() const;

		private:
			std::vector<std::shared_ptr<Expr>> funCallArgs;
		};

		class FunCallExpr : public Expr {
		public:
			FunCallExpr(std::shared_ptr<Expr> target);
			FunCallExpr(std::shared_ptr<Expr> target, std::shared_ptr<FunCallArgList> args);
			virtual ~FunCallExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			Expr* GetTarget() const;

			bool ArgsEmpty() const;
			const FunCallArgList& GetArgs() const;

		private:
			std::shared_ptr<Expr> target;
			std::shared_ptr<FunCallArgList> args;
		};

		class CtorCallExpr : public Expr {
		public:
			CtorCallExpr(const TypeSpec& typeSpec);
			CtorCallExpr(const TypeSpec& typeSpec, std::shared_ptr<FunCallArgList> args);
			virtual ~CtorCallExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			const TypeSpec& GetType() const;

			bool ArgsEmpty() const;
			const FunCallArgList& GetArgs() const;

		private:
			// TODO: use a TypeSpec structure to represent a constructo call's target.
			// float[3]() or vec4[2]() are apparently constructor calls too.
			// Besides, when generating SPIR-V we commonly use a TypeSpec to mangle the type's name
			// to be able to easily access the corresponding instruciton in the environment.
			TypeSpec typeSpec;
			std::shared_ptr<FunCallArgList> args;
		};

		class VarExpr : public Expr {
		public:
			VarExpr(const Token& variable);
			virtual ~VarExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;
			std::string_view ToString() const override;
			std::pair<size_t, size_t> GetExprColBounds() const override;

			const Token& GetVariable() const;

		private:
			Token variable;
		};
		
		class IntConstExpr : public Expr {
		public:
			IntConstExpr(const Token& intConst, ConstId intConstId);
			virtual ~IntConstExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;
			std::string_view ToString() const override;
			std::pair<size_t, size_t> GetExprColBounds() const override;

			const Token& GetIntConst() const;
			int GetValue() const;
			ConstId GetConstId() const;

		private:
			Token intConst;
			ConstId intConstId;
		};
		class UintConstExpr : public Expr {
		public:
			UintConstExpr(const Token& uintConst, ConstId uintConstId);
			virtual ~UintConstExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;
			std::string_view ToString() const override;
			std::pair<size_t, size_t> GetExprColBounds() const override;

			const Token& GetUintConst() const;
			unsigned int GetValue() const;
			ConstId GetConstId() const;

		private:
			Token uintConst;
			ConstId uintConstId;
		};

		class FloatConstExpr : public Expr {
		public:
			FloatConstExpr(const Token& floatConst, ConstId floatConstId);
			virtual ~FloatConstExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;
			std::string_view ToString() const override;
			std::pair<size_t, size_t> GetExprColBounds() const override;

			const Token& GetFloatConst() const;
			float GetValue() const;
			ConstId GetConstId() const;

		private:
			Token floatConst;
			ConstId floatConstId;
		};
		class DoubleConstExpr : public Expr {
		public:
			DoubleConstExpr(const Token& doubleConst, ConstId doubleConstId);
			virtual ~DoubleConstExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;
			std::string_view ToString() const override;
			std::pair<size_t, size_t> GetExprColBounds() const override;

			const Token& GetDoubleConst() const;
			double GetValue() const;
			ConstId GetConstId() const;

		private:
			Token doubleConst;
			ConstId doubleConstId;
		};

		class GroupExpr : public Expr {
		public:
			GroupExpr(std::shared_ptr<Expr> expr);
			virtual ~GroupExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;
			std::string_view ToString() const override;
			std::pair<size_t, size_t> GetExprColBounds() const override;

			Expr* GetExpr() const;

		private:
			std::shared_ptr<Expr> expr;
		};
	
	}
}