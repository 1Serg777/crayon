#include "GLSL/Analyzer/Parser.h"

#include <iostream>
#include <sstream>

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
			// d) No type qualifier and no type specifier case should have been handled in
			//    `external-declaration'.
			// As a result, the semicolon is a syntax error,
			// until I figure out where cases (b) and (c) can be used.
			if (Match(TokenType::SEMICOLON)) {
				throw std::runtime_error{"An invalid declaration!"};
			}

			const Token* identifier =
				Consume(TokenType::IDENTIFIER, "Expected an identifier in a declaration!");

			// 4. Variable or array declarations.
			// if (Match(TokenType::LEFT_BRACKET)) {
			if (Peek()->tokenType == TokenType::LEFT_BRACKET) {
				std::shared_ptr<ArrayDecl> arrayDecl = std::make_shared<ArrayDecl>(fullSpecType, *identifier);
				for (const std::shared_ptr<Expr>& dimExpr : ArraySpecifier()) {
					arrayDecl->AddDimension(dimExpr);
				}
				if (Match(TokenType::EQUAL)) {
					// std::shared_ptr<Expr> initExpr = AssignmentExpression();
					// arrayDecl->SetInitializerExpr(initExpr);
					arrayDecl->SetInitializerExpr(Initializer());
				}
				// std::cout << "[Array decl.][Previous()] '" << Previous()->lexeme << "'\n";
				// std::cout << "[Array decl.][Peek()] '" << Peek()->lexeme << "'\n";
				Consume(TokenType::SEMICOLON, "[Array decl.] Expected a semicolon after an initializer!");
				return arrayDecl;
			} else if (Match(TokenType::SEMICOLON)) {
				// 3. Variable declaration
				std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, *identifier);
				return varDecl;
			} else if (Match(TokenType::EQUAL)) {
				// 4. Variable declaration with an initializer.
				std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, *identifier);
				varDecl->SetInitializerExpr(Initializer());
				Consume(TokenType::SEMICOLON, "[Var decl.] Expected a semicolon after an initializer!");
				return varDecl;
			}

			if (Match(TokenType::LEFT_PAREN)) {
				// 5. Function declaration or function defintion
				if (declContext != DeclContext::EXTERNAL) {
					throw std::runtime_error{
						"Function declaration or function definition is only allowed in the global scope!"
					};
				}
				std::shared_ptr<FunProto> funProto = FunctionPrototype(fullSpecType, *identifier);
				if (Match(TokenType::SEMICOLON)) {
					// 5.1 Function declaration.
					std::shared_ptr<FunDecl> funDecl = std::make_shared<FunDecl>(funProto);
					return funDecl;
				}
				if (Peek()->tokenType == TokenType::LEFT_BRACE) {
					// 5.2 Function definition
					std::shared_ptr<BlockStmt> stmts = BlockStatement();
					std::shared_ptr<FunDecl> funDef = std::make_shared<FunDecl>(funProto, stmts);
					return funDef;
				}
			} else {
				throw std::runtime_error{"Expected a function definition or function declaration!"};
			}

			// If none of the above, throw a syntax error: "Expected a declaration!"
			throw std::runtime_error{"Expected a declaration!"};
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
			while (true) {
				try {
					if (Peek()->tokenType == TokenType::LEFT_BRACE) {
						return BlockStatement();
					}
					return SimpleStatement();
				} catch (std::runtime_error& re) {
					const Token* prev = Previous();
					std::cerr << "Syntax error ["
						<< prev->line << ":" << prev->column
						<< "]: " << re.what() << "\n";

					// Synchronize();
					// if (AtEnd())
					// 	return std::shared_ptr<Stmt>{};
				}
			}
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
				// 2. It's a declaration statement.
				std::shared_ptr<Decl> decl = Declaration(DeclContext::BLOCK);
				// Consume(TokenType::SEMICOLON, "A semicolon expected after a declaration statement!");
				std::shared_ptr<DeclStmt> declStmt = std::make_shared<DeclStmt>(decl);
				return declStmt;
			} else {
				// 3. Otherwise, we parse an expression statement.
				std::shared_ptr<ExprStmt> exprStmt = std::make_shared<ExprStmt>(Expression());
				Consume(TokenType::SEMICOLON, "A semicolon expected after an expression statement!");
				return exprStmt;
			}
		}

		void Parser::Synchronize() {
			// We entered a "panic" mode where we're ensure where exactly we are in the grammar.
			// We skip over all tokens until we reach something that looks like a start of a statement.
			while (!AtEnd()) {
				if (Peek()->tokenType == TokenType::SEMICOLON) {
					Advance();
					break;
				}
			}
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
		std::shared_ptr<Expr> Parser::Initializer() {
			if (Match(TokenType::LEFT_BRACE)) {
				// Parse an initializer list.
				std::shared_ptr<Expr> initListExpr = InitializerList();
				Match(TokenType::COMMA); // We don't need to do anything special.
				Consume(TokenType::RIGHT_BRACE, "'}' expected in an initializer!");
				return initListExpr;
			} else {
				// Parse a single initializer expression.
				std::shared_ptr<Expr> initializerExpr = AssignmentExpression();
				return initializerExpr;
			}
		}
		std::shared_ptr<Expr> Parser::InitializerList() {
			std::shared_ptr<InitListExpr> initListExpr = std::make_shared<InitListExpr>();
			std::shared_ptr<Expr> expr = Initializer();
			initListExpr->AddInitExpr(expr);
			while (Match(TokenType::COMMA) && Peek()->tokenType != TokenType::RIGHT_BRACE) {
				expr = Initializer();
				initListExpr->AddInitExpr(expr);
			}
			return initListExpr;
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
			//         what happened to 'unary_expression'?
			// Essentially, we don't know whether we should call
			// the "ConditionalExpression" or "UnaryExpression" procedure
			// until after we've reached an equality sign.
			// What we do instead is call the more "broader" ConditionalExpression function
			// and then check whether the expression returned is a valid lvalue
			// among those produced by the UnaryExpression procedure and its descendants.
			std::shared_ptr<Expr> assignExpr = ConditionalExpression();
			if (Match(TokenType::EQUAL)) {
				// [TODO]: check whether the 'expr' is a valid assignment target.
				//         in other words, check if it's an lvalue.
				// if the check fails, throw a syntax error.
				std::shared_ptr<Expr> rvalue = AssignmentExpression();
				assignExpr = std::make_shared<AssignExpr>(assignExpr, rvalue);
			}
			return assignExpr;
		}
		std::shared_ptr<Expr> Parser::ConditionalExpression() {
			// TODO: implement parsing of conditional expressions.
			return AdditiveExpression();
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
			if (Match(TokenType::PLUS) || Match(TokenType::DASH)) {
				const Token* op = Previous();
				std::shared_ptr<Expr> expr = UnaryExpression();
				return std::make_shared<UnaryExpr>(*op, expr);
			} else {
				return PostfixExpression();
			}
		}
		std::shared_ptr<Expr> Parser::PostfixExpression() {
			// Parse the first part of the postfix expression.
			std::shared_ptr<Expr> expr;
			if (IsType(Peek()->tokenType)) {
				// 1. Parse a constructor call.
				TypeSpec typeSpec = TypeSpecifier();
				Consume(TokenType::LEFT_PAREN, "Constructor call must have an openning '('!");
				std::shared_ptr<FunCallArgList> args = FunctionCallArgumentList();
				Consume(TokenType::RIGHT_PAREN, "Constructor call must have a closing ')'!");
				expr = std::make_shared<CtorCallExpr>(typeSpec.type, args);
			} else {
				// 2. Parse a primary expression.
				expr = PrimaryExpression();
			}

			// Parse the rest of the postfix expression.
			// For now, only two postfix expression types are supported:
			// 1. Function calls, and
			// 2. Field selections
			while (true) {
				if (Match(TokenType::LEFT_PAREN)) {
					std::shared_ptr<FunCallArgList> args = FunctionCallArgumentList();
					Consume(TokenType::RIGHT_PAREN, "Function call must have a closing ')'!");
					expr = std::make_shared<FunCallExpr>(expr, args);
				} else if (Match(TokenType::DOT)) {
					const Token* field = Consume(
						TokenType::IDENTIFIER, "Field name must be a valid identifier!");
					expr = std::make_shared<FieldSelectExpr>(expr, *field);
				} else {
					break;
				}
			}

			return expr;
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
			} else if (Match(TokenType::UINTCONSTANT)) {
				// 4. It's an unsigned integer constant.
				const Token* uintConst = Previous();
				primary = std::make_shared<UintConstExpr>(*uintConst);
			} else if (Match(TokenType::FLOATCONSTANT)) {
				// 5. It's a single precision floating-point constant.
				const Token* floatConst = Previous();
				primary = std::make_shared<FloatConstExpr>(*floatConst);
			} else if (Match(TokenType::DOUBLECONSTANT)) {
				// 6. It's a double precision floating-point constant.
				const Token* doubleConst = Previous();
				primary = std::make_shared<DoubleConstExpr>(*doubleConst);
			} else {
				// const Token* tok = Previous();
				const Token* tok = Advance();
				std::stringstream errStream;
				errStream << "[Syntax error][" << tok->line << ":" << tok->column << "]: ";
				errStream << "Unexpected primary expression: " << tok->lexeme << "\n";
				throw std::runtime_error{errStream.str()};
				// throw std::runtime_error{"Unexpected primary expression!"};
			}
			return primary;
		}

		std::shared_ptr<FunCallArgList> Parser::FunctionCallArgumentList() {
			std::shared_ptr<FunCallArgList> args = std::make_shared<FunCallArgList>();
			// 1. No arguments
			if (Peek()->tokenType == TokenType::RIGHT_PAREN ||
			    Peek()->tokenType == TokenType::VOID) {
				Advance();
				return args;
			}
			// 2. One or more arguments
			std::shared_ptr<Expr> arg = AssignmentExpression();
			args->AddFunCallArg(arg);
			while (Match(TokenType::COMMA)) {
				arg = AssignmentExpression();
				args->AddFunCallArg(arg);
			}
			return args;
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
			TypeSpec typeSpec{};
			typeSpec.type = *Advance();
			if (!IsType(typeSpec.type.tokenType)) {
				throw std::runtime_error{"Base type in a type specifier expected!"};
			}
			if (Peek()->tokenType == TokenType::LEFT_BRACKET) {
				typeSpec.dimensions = ArraySpecifier();
			}
			return typeSpec;
		}
		std::vector<std::shared_ptr<Expr>> Parser::ArraySpecifier() {
			std::vector<std::shared_ptr<Expr>> dimensions;
			while (Peek()->tokenType == TokenType::LEFT_BRACKET) {
				Advance();
				if (Match(TokenType::RIGHT_BRACKET)) {
					dimensions.push_back(std::shared_ptr<Expr>{});
				} else {
					std::shared_ptr<Expr> constIntExpr = ConditionalExpression();
					// IntConstExpr* intConst = dynamic_cast<IntConstExpr*>(constIntExpr.get());
					// if (!intConst) {
					// 	throw std::runtime_error{"Only constant integer expressions are allowed to specify an array size!"};
					// }
					dimensions.push_back(constIntExpr);
					Consume(TokenType::RIGHT_BRACKET, "Right bracket expected after specifying array dimension size!");
				}
			}
			return dimensions;
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
			else
				throw std::runtime_error{msg.data()};
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