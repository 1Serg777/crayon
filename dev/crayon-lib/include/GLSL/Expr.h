#pragma once

#include "GLSL/AST.h"
#include "GLSL/Token.h"

namespace crayon
{
	namespace glsl
	{
		class Binary;
		class IntConst;

		class ExprVisitor
		{
		public:

			virtual void VisitBinaryExpr(Binary* binaryExpr) = 0;
			virtual void VisitIntConstExpr(IntConst* intConstExpr) = 0;
		};

		class ExprEvalVisitor : public ExprVisitor
		{
		public:

			void VisitBinaryExpr(Binary* binaryExpr) override;
			void VisitIntConstExpr(IntConst* intConstExpr) override;

			int GetResult() const;

		private:

			int result{ 0 };
		};

		class ExprPostfixPrinterVisitor : public ExprVisitor
		{
		public:

			void VisitBinaryExpr(Binary* binaryExpr) override;
			void VisitIntConstExpr(IntConst* intConstExpr) override;
		};

		class Expr
		{
		public:

			virtual void Accept(ExprVisitor* exprVisitor) = 0;
		};

		class Binary : public Expr
		{
		public:

			Binary(std::shared_ptr<Expr> left, const Token& op, std::shared_ptr<Expr> right);

			void Accept(ExprVisitor* exprVisitor) override;

			Expr* GetLeftExpr() const;
			Expr* GetRightExpr() const;

			const Token& GetOperator() const;

		private:

			std::shared_ptr<Expr> left;
			std::shared_ptr<Expr> right;
			Token op{};
		};

		class IntConst : public Expr
		{
		public:

			IntConst(const Token& intConst);

			void Accept(ExprVisitor* exprVisitor) override;

			const Token& GetIntConst() const;

		private:

			Token intConst{ 0 };
		};
	}
}