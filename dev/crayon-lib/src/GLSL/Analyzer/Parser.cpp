#include "GLSL/Analyzer/Parser.h"

namespace crayon {
	namespace glsl {

		void Parser::Parse(const Token* tokenStream, size_t tokenStreamSize) {
			this->tokenStream = tokenStream;
			this->tokenStreamSize = tokenStreamSize;
			current = 0;
			TranslationUnit();
			this->tokenStreamSize = 0;
			this->tokenStream = nullptr;
		}

		std::shared_ptr<TransUnit> Parser::GetTranslationUnit() const {
			return transUnit;
		}

		void Parser::TranslationUnit() {
			transUnit = std::make_shared<TransUnit>();
			while (!AtEnd()) {
				std::shared_ptr<Decl> decl = ExternalDeclaration();
				if (decl) transUnit->AddDeclaration(decl);
			}
		}

		std::shared_ptr<Decl> Parser::ExternalDeclaration() {
			// 1. A single semicolon, ignore it.
			if (Match(TokenType::SEMICOLON)) {
				return std::shared_ptr<Decl>{};
			}
			// 2. Declaration or function definition.
			return DeclarationOrFunctionDefinition(DeclContext::EXTERNAL);
		}
		std::shared_ptr<Decl> Parser::DeclarationOrFunctionDefinition(DeclContext declContext) {
			FullSpecType fullSpecType{};
			if (IsQualifier(Peek()->tokenType)) {
				// Parse type qualifiers.
				if (Match(TokenType::PRECISE)) {
					// 1. Could be a precision statement (which is parsed in 'declaration')
					//    Example: precise lowp float;
					fullSpecType.qualifier.precise = *Previous();
					if (IsPrecisionQualifier(Peek()->tokenType)) {
						fullSpecType.qualifier.precision = *Advance();
						if (IsType(Peek()->tokenType)) {
							fullSpecType.specifier = TypeSpecifier();
							// At this point we've parsed the type qualifiers and a type specifier.
							// For a valid precision statement we must match a semicolon.
							// If not, then it's probably a variable declaration,
							// which is going to be handled later.
							if (Match(TokenType::COMMA)) {
								// [TODO]: return a new parse statement declaration
							}
						}
					}
					// 1.1 Parse TypeQualifierRest (if any) since there might be more of them
					// depending on where we stopped in the code above.
					TypeQualifierRest(fullSpecType.qualifier);
				} else {
					fullSpecType.qualifier = TypeQualifier();
				}

				// Type qualifiers followed by a semicolon.
				if (Match(TokenType::SEMICOLON)) {
					// 2. Qualifier declaration
					std::shared_ptr<QualDecl> qualDecl =
						std::make_shared<QualDecl>(fullSpecType.qualifier);
					return qualDecl;
				}
			}
			if (IsType(Peek()->tokenType)) {
				fullSpecType.specifier = TypeSpecifier();
			}

			// a) Type qualifiers that end with a semicolon were handled before.
			// b) Single type specifier followed by a semicolon is valid according to the grammar:
			//    - Through the second production with 'init-declarator-list'
			//    but I don't know any use case for that.
			// c) Fully-specified type followed by a semicolon is the same thing as (b):
			//    - Valid through the second production, but use cases for that are unknown.
			// d) No type qualifiers and no type specifier case should have been handled in
			//    `external-declaration'.
			// As a result, the semicolon is a syntax error,
			// until I figure out where cases (b) and (c) can be used.
			if (Match(TokenType::SEMICOLON)) {
				throw std::runtime_error{ "An invalid declaration!" };
			}

			const Token* identifier =
				Consume(TokenType::IDENTIFIER, "Expected an identifier in a declaration!");

			if (Match(TokenType::SEMICOLON)) {
				// 3. Variable declaration
				std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, *identifier);
				return varDecl;
			}

			if (Match(TokenType::LEFT_PAREN)) {
				// 4. Function declaration or function defintion
				if (declContext != DeclContext::EXTERNAL) {
					throw std::runtime_error{
						"Function declaration or function definition is only allowed in the global scope!"
					};
				}
				std::shared_ptr<FunProto> funProto = FunctionPrototype(fullSpecType, *identifier);
				if (Match(TokenType::SEMICOLON)) {
					// 4.1 Function declaration.
					std::shared_ptr<FunDecl> funDecl = std::make_shared<FunDecl>(funProto);
					return funDecl;
				}
				if (Peek()->tokenType == TokenType::LEFT_BRACE) {
					// 4.2 Function definition
					std::shared_ptr<BlockStmt> stmts = BlockStatement();
					std::shared_ptr<FunDecl> funDef = std::make_shared<FunDecl>(funProto, stmts);
					return funDef;
				}
			} else {
				throw std::runtime_error{ "Expected a function definition or function declaration!" };
			}

			// If none of the above, throw a syntax error: "Expected a declaration!"
			throw std::runtime_error{ "Expected a declaration!" };
		}
		std::shared_ptr<Decl> Parser::Declaration(DeclContext declContext) {
            return DeclarationOrFunctionDefinition(declContext);
        }

		std::shared_ptr<FunProto> Parser::FunctionPrototype(const FullSpecType& fullSpecType, const Token& identifier) {
			std::shared_ptr<FunParamList> params = FunctionParameterList();
			Consume(TokenType::RIGHT_PAREN, "Unterminated function parameter list!");
			std::shared_ptr<FunProto> funProto =
				std::make_shared<FunProto>(fullSpecType, identifier, params);
			return funProto;
		}
		// Parse optional function declaration or function definition parameters.
		std::shared_ptr<FunParamList> Parser::FunctionParameterList() {
			std::shared_ptr<FunParamList> paramList = std::make_shared<FunParamList>();
			// 1. No parameters
			if (Peek()->tokenType == TokenType::RIGHT_PAREN) {
				return paramList;
			}
			// 2. One or more parameters
			FunParam param = FunctionParameter();
			paramList->AddFunctionParameter(param);
			while (Match(TokenType::COMMA)) {
				param = FunctionParameter();
				paramList->AddFunctionParameter(param);
			}
			return paramList;
		}
		// Parse a single function parameter.
		// function_header_with_parameters:
		//     function_header parameter_declaration
		//     function_header_with_parameters COMMA parameter_declaration
		//     
		// parameter_declarator:
		//     type_specifier IDENTIFIER
		//     type_specifier IDENTIFIER array_specifier
		// 
		// parameter_declaration:
		//     type_qualifier parameter_declarator
		//     parameter_declarator
		//     type_qualifier parameter_type_specifier
		//     parameter_type_specifier
		// 
		// parameter_type_specifier:
		//     type_specifier
		FunParam Parser::FunctionParameter() {
			// Reusing the FullySpecifiedType function here,
			// even though that's in violation of the grammar.
			// I'm not quite sure why they decided to reuse type_qualifier and type_specifier
			// but not fully_specified_type. Is that for better error reporting?
			// C99 grammar, if I'm not mistaken, reuses declaration-specifiers,
			// which is basically our fully_specified_type.
			/*
			FullSpecType fullSpecType{};
			if (IsQualifier(Peek()->tokenType)) {
				fullSpecType.qualifier = TypeQualifier();
			}
			if (IsType(Peek()->tokenType)) {
				fullSpecType.specifier = TypeSpecifier();
			} else {
				// throw std::runtime_error{ "Type specifier of a function parameter expected!" };
				throw std::runtime_error{ "A function parameter must have a type specifier!" };
			}
			*/
			FullSpecType fullSpecType{};
			try {
				fullSpecType = FullySpecifiedType();
			} catch (const std::runtime_error& re) {
				std::string errMsg{ "An invalid function parameter declaration!" };
				errMsg.append("\n");
				errMsg.append(re.what());
				throw std::runtime_error{ errMsg };
			}
			// Both function declarations and function definitions
			// can have unnamed parameters.
			if (Match(TokenType::IDENTIFIER)) {
				const Token* identifier = Previous();
				return FunParam{ fullSpecType, *identifier };
			} else {
				return FunParam{ fullSpecType };
			}
		}

		// Also called `compound_statement` in the GLSL language grammar.
		// There's also `compound_statement_no_new_scope' nonterminal,
		// which has practically the same productions.
		// The differences clearly have something to do with scopes,
		// but I'm not quite sure. For example, why wouldn't we want
		// to create a new scope for new function definition?
		// Until I encounter a use case where this would be important,
		// the BlockStatement() procedure is used for both nonterminals.
		std::shared_ptr<BlockStmt> Parser::BlockStatement() {
			Consume(TokenType::LEFT_BRACE, "Openning brace in a block statement expected!");
			std::shared_ptr<BlockStmt> stmts = std::make_shared<BlockStmt>();
			if (Match(TokenType::RIGHT_BRACE)) {
				// 1. An empty block.
				return stmts;
			}
			while (!Match(TokenType::RIGHT_BRACE)) {
				stmts->AddStmt(Statement());
			}
			return stmts;
		}
		std::shared_ptr<Stmt> Parser::Statement() {
			if (Peek()->tokenType == TokenType::LEFT_BRACE) {
				return BlockStatement();
			}
			return SimpleStatement();
		}
		std::shared_ptr<Stmt> Parser::SimpleStatement() {
			// 1. First we check whether the current lookahead token
			//    belongs to any of the terminals from the first sets
			//    of the following productions:
			//    'selection_statement'
			//    'switch_statement'
			//    'case_label'
			//    'iteration_statement'
			//    'jump_statement'
			// [TODO]: they are not supported yet
			// 2. Check the current lookahead token to see if it's either
			//    a type qualifier or a type specifier. If so, then we
			//    parse a declaration.
			if (IsQualifier(Peek()->tokenType) || IsType(Peek()->tokenType)) {
				std::shared_ptr<DeclStmt> declStmt =
					std::make_shared<DeclStmt>(
						Declaration(DeclContext::BLOCK));
			}
			// 3. Otherwise, we parse an expression statement.
			std::shared_ptr<ExprStmt> exprStmt = std::make_shared<ExprStmt>(Expression());
			Consume(TokenType::SEMICOLON, "A semicolon expected after an expression statement!");
			return exprStmt;
		}

		// Parse an expression starting with the lowest-precedence expression,
		// which is an assignment expression.
		// The grammar is
		// 'expression':
		//     'assignment_expression'
		//     'expression' COMMA 'assignment_expression'
		// The last production is ignored for now.
		std::shared_ptr<Expr> Parser::Expression() {
			return AssignmentExpression();
		}
		// Parse an assignment expression.
		// 'assignment_expression':
		//     'conditional_expression'
		//     'unary_expression' 'assignment_operator' 'assignment_expression'
		// Currently, there are following limitations:
		// a) We don't have a method that parses conditional expressions, so
		//    we start with additive expressions.
		// b) The only assignment operator supported right now is the ASSIGN terminal.
		std::shared_ptr<Expr> Parser::AssignmentExpression() {
			// [TODO]: explain how the grammar is parsed.
			std::shared_ptr<Expr> assignExpr = AdditiveExpression();
			if (Match(TokenType::EQUAL)) {
				// [TODO]: check whether the 'expr' is a valid assignment target.
				//         in other words, check if it's an lvalue.
				// if the check fails, throw a syntax error.
				std::shared_ptr<Expr> rvalue = AssignmentExpression();
				assignExpr = std::make_shared<AssignExpr>(assignExpr, rvalue);
			}
			return assignExpr;
		}
		std::shared_ptr<Expr> Parser::AdditiveExpression() {
			std::shared_ptr<Expr> expr = MultiplicativeExpression();
			while (Match(TokenType::PLUS) || Match(TokenType::DASH)) {
				const Token* op = Previous();
				std::shared_ptr<Expr> term = MultiplicativeExpression();
				expr = std::make_shared<BinaryExpr>(expr, *op, term);
			}
			return expr;
		}
		std::shared_ptr<Expr> Parser::MultiplicativeExpression() {
			std::shared_ptr<Expr> term = UnaryExpression();
			while (Match(TokenType::STAR) || Match(TokenType::SLASH)) {
				const Token* op = Previous();
				std::shared_ptr<Expr> primary = UnaryExpression();
				term = std::make_shared<BinaryExpr>(term, *op, primary);
			}
			return term;
		}
		std::shared_ptr<Expr> Parser::UnaryExpression() {
			// [TODO]: implementation of the 'unary_expression' nonterminal.
			return PrimaryExpression();
		}
		std::shared_ptr<Expr> Parser::PrimaryExpression() {
			std::shared_ptr<Expr> primary;
			if (Match(TokenType::LEFT_PAREN)) {
				// 1. Group expression.
				primary = Expression();
				Consume(TokenType::RIGHT_PAREN, "Matching ')' parenthesis missing!");
			} else if (Match(TokenType::IDENTIFIER)) {
				// 2. It's a variable identifier.
				const Token* var = Previous();
				primary = std::make_shared<VarExpr>(*var);
			} else if (Match(TokenType::INTCONSTANT)) {
				// 3. It's an integer constant.
				const Token* intConst = Previous();
				primary = std::make_shared<IntConstExpr>(*intConst);
			} else {
				// const Token* tok = Previous();
				const Token* tok = Advance();
				std::string errMsg{ "Unexpected primary expression: " };
				errMsg.append(tok->lexeme);
				throw std::runtime_error{ errMsg };
				// throw std::runtime_error{ "Unexpected primary expression!" };
			}
			return primary;
		}

		FullSpecType Parser::FullySpecifiedType() {
			FullSpecType fullSpecType{};
			if (IsQualifier(Peek()->tokenType)) {
				fullSpecType.qualifier = TypeQualifier();
			} if (IsType(Peek()->tokenType)) {
				fullSpecType.specifier = TypeSpecifier();
			} else {
				throw std::runtime_error{
					"Type specifier expected in a fully-specified type declaration!"
				};
			}
			return fullSpecType;
		}

		TypeQual Parser::TypeQualifier() {
			TypeQual typeQual{};
			SingleTypeQualifier(typeQual);
			TypeQualifierRest(typeQual);
			return typeQual;
		}
		void Parser::SingleTypeQualifier(TypeQual& typeQual) {
			const Token* qualifier = Peek();
			if (qualifier->tokenType == TokenType::LAYOUT) {
				// 1. Handle a layout qualifier.
				Advance();
				Consume(
					TokenType::LEFT_PAREN,
					"Expected an opening parenthesis after the layout specifier keyword!");
				LayoutQualifierList(typeQual.layout);
				Consume(
					TokenType::RIGHT_PAREN,
					"Expected a closing parenthesis after the layout specifier!");
			} else if (IsStorageQualifier(qualifier->tokenType)) {
				// 2. Handle a storage qualifier.
				Advance();
				typeQual.storage = *qualifier;
			} else {
				// 3. [TODO]: add more qualifier types later
				throw std::runtime_error{ "Expected a type qualifier!" };
			}
		}
		void Parser::TypeQualifierRest(TypeQual& typeQual) {
			// One or more qualifiers has already been parsed, so it's fine if there's no more of them.
			// If there are more, however, we parse all of them.
			while (IsQualifier(Peek()->tokenType)) {
				SingleTypeQualifier(typeQual);
			}
		}

		void Parser::LayoutQualifierList(std::list<LayoutQualifier>& layout) {
			layout.push_back(SingleLayoutQualifier());
			while (Match(TokenType::COMMA)) {
				layout.push_back(SingleLayoutQualifier());
			}
		}
		LayoutQualifier Parser::SingleLayoutQualifier() {
			const Token* identifier = Consume(TokenType::IDENTIFIER, "Layout specifier name expected!");
			if (Match(TokenType::EQUAL)) {
				// A constant expression should be expected, but an int constant is ok for now.
				const Token* value = Consume(
					TokenType::INTCONSTANT,
					// TODO: show what layout name is missing an integer value!
					"An integer constant as a layout specifier value is expected!");
				int qualifierValue = static_cast<int>(std::strtol(value->lexeme.data(), nullptr, 10));
				return LayoutQualifier{ *identifier, qualifierValue };
			} else {
				return LayoutQualifier{ *identifier };
			}
		}

		TypeSpec Parser::TypeSpecifier() {
			return TypeSpec{ *Advance() };
		}

		bool Parser::IsQualifier(TokenType tokenType) const {
			if (tokenType >= TokenType::LAYOUT &&
				tokenType <= TokenType::PRECISE)
				return true;
			return false;
		}
		bool Parser::IsStorageQualifier(TokenType tokenType) const {
			if (tokenType >= TokenType::IN &&
				tokenType <= TokenType::OUT)
				return true;
			return false;
		}
        bool Parser::IsPrecisionQualifier(TokenType tokenType) const {
            if (tokenType >= TokenType::HIGH_PRECISION &&
				tokenType <= TokenType::LOW_PRECISION)
				return true;
			return false;
        }
		bool Parser::IsType(TokenType tokenType) const {
			if (tokenType >= TokenType::VOID &&
				tokenType <= TokenType::UIMAGE2DMSARRAY)
				return true;
			return false;
		}

		const Token* Parser::Advance() {
			if (AtEnd())
				return Last();
			return tokenStream + current++;
		}
		const Token* Parser::Previous() {
			return tokenStream + (current - 1);
		}
		const Token* Parser::Peek() const {
			if (AtEnd())
				return Last();
			return tokenStream + current;
		}
		bool Parser::Match(TokenType tokenType) {
			if (AtEnd())
				return false;
			if (Peek()->tokenType == tokenType)	{
				Advance();
				return true;
			}
			return false;
		}
		const Token* Parser::Consume(TokenType tokenType, std::string_view msg) {
			if (Match(tokenType))
				return Previous();
			throw std::runtime_error{ msg.data() };
		}

		bool Parser::AtEnd() const {
			if (current >= tokenStreamSize)
				return true;
			return false;
		}
		const Token* Parser::Last() const {
			return tokenStream + (tokenStreamSize - 1);
		}
	}
}