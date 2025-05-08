#include "GLSL/Analyzer/Parser.h"

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

			TranslationUnit();

			this->tokenStreamSize = 0;
			this->tokenStream = nullptr;
		}

		std::shared_ptr<ExtDeclList> Parser::GetExternalDeclarationList() const
		{
			return extDeclList;
		}

		void Parser::TranslationUnit()
		{
			extDeclList = std::make_shared<ExtDeclList>();
			while (!AtEnd())
			{
				extDeclList->AddDeclaration(ExternalDeclaration());
			}
		}

		std::shared_ptr<Stmt> Parser::ExternalDeclaration()
		{
			FullSpecType fullSpecType{};

			if (IsQualifier(Peek()->tokenType))
			{
				fullSpecType.qualifier = TypeQualifier();

				if (Match(TokenType::SEMICOLON))
				{
					// 1. Qualifier declaration

					std::shared_ptr<QualDecl> qualDecl = std::make_shared<QualDecl>(fullSpecType.qualifier);
					return qualDecl;
				}
			}

			if (IsType(Peek()->tokenType))
			{
				fullSpecType.specifier = TypeSpecifier();

				if (Match(TokenType::SEMICOLON))
				{
					// According to the grammar a fully-specified type followed by a semicolon is valid,
					// but I decided to throw a syntax error until I find a proper use case.

					throw std::runtime_error{ "Expected an identifier in a declaration!" };
				}

				const Token* identifier = Consume(TokenType::IDENTIFIER, "Expected an identifier in a declaration!" );

				if (Match(TokenType::SEMICOLON))
				{
					// 2. Variable declaration

					std::shared_ptr<VarDecl> varDecl = VariableDeclaration(fullSpecType, *identifier);
					return varDecl;
				}

				if (Match(TokenType::LEFT_PAREN))
				{
					// 3. Function declaration or function defintion

					std::shared_ptr<Stmt> function = Function(fullSpecType, *identifier);
					return function;
				}
				else
				{
					throw std::runtime_error{ "Expected a function definition or function declaration!" };
				}
			}

			// If none of the above, throw a syntax error: "Expected a declaration!"
			throw std::runtime_error{ "Expected a declaration!" };
		}

		std::shared_ptr<VarDecl> Parser::VariableDeclaration(const FullSpecType& fullSpecType, const Token& identifier)
		{
			std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, identifier);
			return varDecl;
		}

		std::shared_ptr<Stmt> Parser::Function(const FullSpecType& fullSpecType, const Token& identifier)
		{
			std::shared_ptr<FunProto> funProto = FunctionPrototype(fullSpecType, identifier);

			if (Match(TokenType::SEMICOLON))
			{
				// 3. Function declaration

				std::shared_ptr<FunDecl> funDecl = std::make_shared<FunDecl>(funProto);
				return funDecl;
			}

			if (Match(TokenType::LEFT_BRACE))
			{
				// 4. Function definition

				std::shared_ptr<BlockStmt> stmts = BlockStatement();

				Consume(TokenType::RIGHT_BRACE, "Unterminated function definition!");

				std::shared_ptr<FunDef> funDef = std::make_shared<FunDef>(funProto, stmts);
				
				return funDef;
			}
			
			throw std::runtime_error{ "Inappropriate format of a function declaration or definition!" };
		}

		std::shared_ptr<FunProto> Parser::FunctionPrototype(const FullSpecType& fullSpecType, const Token& identifier)
		{
			std::shared_ptr<FunParamList> params = FunctionParameterList();
			Consume(TokenType::RIGHT_PAREN, "Unterminated function parameter list!");
			std::shared_ptr<FunProto> funProto = std::make_shared<FunProto>(fullSpecType, identifier, params);
			return funProto;
		}

		std::shared_ptr<FunParamList> Parser::FunctionParameterList()
		{
			std::shared_ptr<FunParamList> paramList = std::make_shared<FunParamList>();

			// 1. No parameters

			if (Peek()->tokenType == TokenType::RIGHT_PAREN)
			{
				return paramList;
			}

			// 2. One or more parameters

			FunParam param = FunctionParameter();
			paramList->AddFunctionParameter(param);

			while (Match(TokenType::COMMA))
			{
				param = FunctionParameter();
				paramList->AddFunctionParameter(param);
			}

			return paramList;
		}

		FunParam Parser::FunctionParameter()
		{
			FullSpecType fullSpecType{};

			if (IsQualifier(Peek()->tokenType))
			{
				fullSpecType.qualifier = TypeQualifier();
			}

			if (IsType(Peek()->tokenType))
			{
				fullSpecType.specifier = TypeSpecifier();
			}
			else
			{
				throw std::runtime_error{ "Type specifier of a function parameter expected!" };
			}

			if (Match(TokenType::IDENTIFIER))
			{
				const Token* identifier = Previous();
				return FunParam{ fullSpecType, *identifier };
			}
			else
			{
				return FunParam{ fullSpecType };
			}
		}

		std::shared_ptr<BlockStmt> Parser::BlockStatement()
		{
			// TODO
			return std::shared_ptr<BlockStmt>();
		}

		FullSpecType Parser::FullySpecifiedType()
		{
			FullSpecType fullSpecType{};

			if (IsQualifier(Peek()->tokenType))
			{
				fullSpecType.qualifier = TypeQualifier();
			}

			if (IsType(Peek()->tokenType))
			{
				fullSpecType.specifier = TypeSpecifier();
			}
			else
			{
				throw std::runtime_error{ "Type specifier expected in a fully-specified type declaration!" };
			}

			return fullSpecType;
		}

		TypeQual Parser::TypeQualifier()
		{
			TypeQual typeQual{};

			SingleTypeQualifier(typeQual);

			while (IsQualifier(Peek()->tokenType))
			{
				SingleTypeQualifier(typeQual);
			}

			return typeQual;
		}
		void Parser::SingleTypeQualifier(TypeQual& typeQual)
		{
			const Token* qualifier = Peek();

			// 1. Is it a layout qualifier?
			if (qualifier->tokenType == TokenType::LAYOUT)
			{
				Advance();

				Consume(TokenType::LEFT_PAREN, "Expected an opening parenthesis after the layout specifier keyword!");
				LayoutQualifierList(typeQual.layout);
				Consume(TokenType::RIGHT_PAREN, "Expected a closing parenthesis after the layout specifier!");
				
			}
			// 2. Is it a storage qualifier?
			else if (IsStorageQualifier(qualifier->tokenType))
			{
				Advance();
				typeQual.storage = *qualifier;
			}
			// 3. [TODO]: add more qualifier types later
			else
			{
				throw std::runtime_error{ "Expected a type qualifier!" };
			}
		}

		void Parser::LayoutQualifierList(std::list<LayoutQualifier>& layout)
		{
			layout.push_back(SingleLayoutQualifier());

			while (Match(TokenType::COMMA))
			{
				layout.push_back(SingleLayoutQualifier());
			}
		}

		LayoutQualifier Parser::SingleLayoutQualifier()
		{
			const Token* identifier = Consume(TokenType::IDENTIFIER, "Layout specifier name expected!");

			if (Match(TokenType::EQUAL))
			{
				// A constant expression should be expected, but an int constant is ok for now.

				const Token* value = Consume(
					TokenType::INTCONSTANT,
					// TODO: show what layout name is missing an integer value!
					"An integer constant as a layout specifier value is expected!");

				int qualifierValue = static_cast<int>(std::strtol(value->lexeme.data(), nullptr, 10));

				return LayoutQualifier{ *identifier, qualifierValue };
			}
			else
			{
				return LayoutQualifier{ *identifier };
			}
		}

		TypeSpec Parser::TypeSpecifier()
		{
			return TypeSpec{ *Advance() };
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
			std::shared_ptr<Expr> primary;

			// 1. Group expression

			if (Match(TokenType::LEFT_PAREN))
			{
				primary = Expression();
				Consume(TokenType::RIGHT_PAREN, "Matching ')' parenthesis missing!");
			}
			else
			{
				// 2. Otherwise it's an integer constant

				const Token* intConst = Consume(TokenType::INTCONSTANT, "Integer constant expected!");
				primary = std::make_shared<IntConst>(*intConst);
			}

			return primary;
		}

		bool Parser::IsQualifier(TokenType tokenType) const
		{
			if (tokenType >= TokenType::LAYOUT &&
				tokenType <= TokenType::PRECISE)
				return true;

			return false;
		}
		bool Parser::IsStorageQualifier(TokenType tokenType) const
		{
			if (tokenType >= TokenType::IN &&
				tokenType <= TokenType::OUT)
				return true;

			return false;
		}
		bool Parser::IsType(TokenType tokenType) const
		{
			if (tokenType >= TokenType::VOID &&
				tokenType <= TokenType::UIMAGE2DMSARRAY)
				return true;

			return false;
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