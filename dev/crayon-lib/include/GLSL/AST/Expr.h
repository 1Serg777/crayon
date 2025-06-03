#pragma once

#include "GLSL/Token.h"

#include <list>
#include <memory>

namespace crayon {
	namespace glsl {

		class AssignExpr;
		class BinaryExpr;
		class FieldSelectExpr;
		class FunCallExpr;
		class CtorCallExpr;
		class VarExpr;
		class IntConstExpr;
		class FloatConstExpr;
		class GroupExpr;

		class ExprVisitor {
		public:
			virtual void VisitAssignExpr(AssignExpr* assignExpr) = 0;
			virtual void VisitBinaryExpr(BinaryExpr* binaryExpr) = 0;
			virtual void VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) = 0;
			virtual void VisitFunCallExpr(FunCallExpr* funCallExpr) = 0;
			virtual void VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) = 0;
			virtual void VisitVarExpr(VarExpr* varExpr) = 0;
			virtual void VisitIntConstExpr(IntConstExpr* intConstExpr) = 0;
			virtual void VisitFloatConstExpr(FloatConstExpr* floatConstExpr) = 0;
			virtual void VisitGroupExpr(GroupExpr* groupExpr) = 0;
		};

		class ExprEvalVisitor : public ExprVisitor {
		public:
			void VisitAssignExpr(AssignExpr* assignExpr) override;
			void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) override;
			void VisitFunCallExpr(FunCallExpr* funCallExpr) override;
			void VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitFloatConstExpr(FloatConstExpr* floatConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;

			int GetResult() const;

		private:
			int result{ 0 };
		};

		class ExprPostfixPrinterVisitor : public ExprVisitor {
		public:
			void VisitAssignExpr(AssignExpr* assignExpr) override;
			void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) override;
			void VisitFunCallExpr(FunCallExpr* funCallExpr) override;
			void VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitFloatConstExpr(FloatConstExpr* floatConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;
		};

		class ExprParenPrinterVisitor : public ExprVisitor {
		public:
			void VisitAssignExpr(AssignExpr* assignExpr) override;
			void VisitBinaryExpr(BinaryExpr* binaryExpr) override;
			void VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) override;
			void VisitFunCallExpr(FunCallExpr* funCallExpr) override;
			void VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) override;
			void VisitVarExpr(VarExpr* varExpr) override;
			void VisitIntConstExpr(IntConstExpr* intConstExpr) override;
			void VisitFloatConstExpr(FloatConstExpr* floatConstExpr) override;
			void VisitGroupExpr(GroupExpr* groupExpr) override;
		};

		class Expr {
		public:
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

			const std::list<std::shared_ptr<Expr>>& GetArgs() const;

		private:
			std::list<std::shared_ptr<Expr>> funCallArgs;
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
			CtorCallExpr(const Token& type);
			CtorCallExpr(const Token& type, std::shared_ptr<FunCallArgList> args);
			virtual ~CtorCallExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			const Token& GetType() const;

			bool ArgsEmpty() const;
			const FunCallArgList& GetArgs() const;

		private:
			Token type;
			std::shared_ptr<FunCallArgList> args;
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

		class FloatConstExpr : public Expr {
		public:
			FloatConstExpr(const Token& floatConst);
			virtual ~FloatConstExpr() = default;

			void Accept(ExprVisitor* exprVisitor) override;

			const Token& GetFloatConst() const;

		private:
			Token floatConst;
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