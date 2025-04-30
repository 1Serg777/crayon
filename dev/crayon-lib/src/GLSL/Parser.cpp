#include "GLSL/Parser.h"

namespace crayon
{
	namespace glsl
	{
		Parser::Parser()
		{
		}

		void Parser::Parse(const Token* tokenStream, size_t tokenStreamSize)
		{
			this->tokenStream = tokenStream;
			this->tokenStreamSize = tokenStreamSize;

			current = 0;

			// 1. Use later for lists of statements
			/*
			while (!AtEnd())
			{

			}
			*/

			rootExpr = Expression();

			this->tokenStreamSize = 0;
			this->tokenStream = nullptr;
		}

		std::shared_ptr<ASTNode> Parser::GetASTRootNode() const
		{
			return rootNode;
		}

		std::shared_ptr<Expr> Parser::GetRootExpression() const
		{
			return rootExpr;
		}

		std::shared_ptr<Expr> Parser::Expression()
		{
			std::shared_ptr<Expr> expr = Term();

			while (Match(TokenType::PLUS) || Match(TokenType::DASH))
			{
				const Token* op = Previous();

				std::shared_ptr<Expr> term = Term();

				expr = std::make_shared<Binary>(expr, *op, term);
			}

			return expr;
		}
		std::shared_ptr<Expr> Parser::Term()
		{
			std::shared_ptr<Expr> term = Primary();

			while (Match(TokenType::STAR) || Match(TokenType::SLASH))
			{
				const Token* op = Previous();

				std::shared_ptr<Expr> primary = Primary();

				term = std::make_shared<Binary>(term, *op, primary);
			}

			return term;
		}
		std::shared_ptr<Expr> Parser::Primary()
		{
			const Token* intConst = Consume(TokenType::INTCONSTANT, "Integer constant expected!");

			std::shared_ptr<Expr> primary = std::make_shared<IntConst>(*intConst);

			return primary;
		}

		const Token* Parser::Advance()
		{
			if (AtEnd())
				return Last();
			
			return tokenStream + current++;
		}

		const Token* Parser::Previous()
		{
			return tokenStream + (current - 1);
		}

		const Token* Parser::Peek() const
		{
			if (AtEnd())
				return Last();

			return tokenStream + current;
		}

		bool Parser::Match(TokenType tokenType)
		{
			if (AtEnd())
				return false;

			if (Peek()->tokenType == tokenType)
			{
				Advance();
				return true;
			}

			return false;
		}

		const Token* Parser::Consume(TokenType tokenType, std::string_view msg)
		{
			if (Match(tokenType))
				return Previous();
			
			throw std::runtime_error{ msg.data() };
		}

		bool Parser::AtEnd() const
		{
			if (current >= tokenStreamSize)
				return true;
			return false;
		}

		const Token* Parser::Last() const
		{
			return tokenStream + (tokenStreamSize - 1);
		}
	}
}