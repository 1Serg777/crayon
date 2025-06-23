#include "GLSL/Analyzer/Parser.h"
#include "GLSL/Error.h"

#include <cassert>
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
		bool Parser::HadSyntaxError() const {
			return hadSyntaxError;
		}
		std::shared_ptr<TransUnit> Parser::GetTranslationUnit() const {
			return transUnit;
		}

		void Parser::InitializeExternalScope() {
			EnterNewScope(); // Create the external scope.
			InitializeGlobalVariables();
		}
		void Parser::InitializeGlobalVariables() {
			// TODO
		}

		void Parser::EnterNewScope() {
			currentScope = std::make_shared<Environment>(currentScope);
		}
		void Parser::RestoreEnclosingScope() {
			assert(currentScope && "The external scope doesn't have the enclosing scope!");
			currentScope = currentScope->GetEnclosingScope();
		}

		void Parser::TranslationUnit() {
			InitializeExternalScope();
			transUnit = std::make_shared<TransUnit>();
			exprTypeInferenceVisitor = std::make_unique<ExprTypeInferenceVisitor>();
			while (!AtEnd()) {
				std::shared_ptr<Decl> decl = ExternalDeclaration();
				if (decl) transUnit->AddDeclaration(decl);
			}
		}

		std::shared_ptr<Decl> Parser::ExternalDeclaration() {
			while (!AtEnd()) {
				try {
					// Parse external declaration.
					// 1. A single semicolon, ignore it.
					if (Match(TokenType::SEMICOLON)) {
						return std::shared_ptr<Decl>{};
					}
					// 2. Declaration or function definition.
					return DeclarationOrFunctionDefinition(DeclContext::EXTERNAL);
				} catch (SyntaxError& se) {
					// Synchronize.
					hadSyntaxError = true;
					std::cerr << se.what() << std::endl;
					if (se.GetExpectedTokenType() != TokenType::SEMICOLON) {
						Synchronize();
					}
				}
			}
			return std::shared_ptr<Decl>{};
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
						const Token* token = Peek();
						if (token->tokenType == TokenType::FLOAT ||
							token->tokenType == TokenType::INT ||
							IsTypeOpaque(token->tokenType)) {
							// At this point we've parsed precision qualifiers
							// and a valid type specifier. To be 100% sure that this
							// a default precision qualifier statement
							// we need to match a semicolon.
							// If not, then it's probably a variable declaration,
							// which is going to be handled later.
							// 
							// The reason I chose to use the 'TypeSpecifier' method
							// to parse the type is because this should improve our
							// error reporting if the type had an array specifier, for example.
							fullSpecType.specifier = TypeSpecifier();
							if (fullSpecType.specifier.IsArray()) {
								throw std::runtime_error{"Default Precision Qualifier statement is not allowed for array types!"};
							}
							// if (Match(TokenType::COMMA)) {
								// [TODO]: return a new parse statement declaration?
							// }
						}
					}
					// 1.1 Parse TypeQualifierRest (if any) since there might be more of them
					// depending on where we stopped in the code above.
					// a) If we couldn't match the right type,
					//    the call below will simply return.
					// b) If we didn't match a precision qualifier, it can mean that either
					//    1) the qualifier was of a different type, or
					//    2) the token wasn't a qualifier to begin with
					//    In any case we attempt to capture more qualifiers before moving on.
					TypeQualifierRest(fullSpecType.qualifier);
				} else {
					fullSpecType.qualifier = TypeQualifier();
				}

				// At this point only qualifiers have been consumed.
				// We could've either stopped at a type token or a semicolon.
				// In case of an invalid input, the type of the token at this point can be different.
				// 
				// Our goal here is to try to catch type qualifiers followed by a semicolon.
				// For example, the following qualifier declaration
				// layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
				// is common in compute shaders.
				// Things like that will be handled in the if branch below.
				// (Default Precision Qualifiers were handled before).
				if (Match(TokenType::SEMICOLON)) {
					// 2. Qualifier declaration
					std::shared_ptr<QualDecl> qualDecl =
						std::make_shared<QualDecl>(fullSpecType.qualifier);
					return qualDecl;
				}
			}
			// Qualifiers weren't followed by a semicolon, so the construct we're dealing with
			// is not a qualifier declaration.
			// At this point it can be many things but we can't pick one since we need more information.
			// Instead, we continue consuming tokens until we have more information to make a decision.
			// According to the GLSL grammar in the speicifcation, qualifiers can also
			// be followed by identifiers, not necessarily types. This is the case with
			// interface blocks. Otherwise, we try to match a type token before moving forward.
			// 3.1 Interface blocks.
			if (Peek()->tokenType == TokenType::IDENTIFIER) {
				const Token* interfaceBlockName = Advance();
				// 1st Check: storage qualifiers. Must be one of:
				// "in", "out", "uniform", or "buffer".
				if (!fullSpecType.qualifier.storage.has_value()) {
					throw SyntaxError{
						*interfaceBlockName,
						"An interface block declaration must have a storage qualifier!"
					};
				}
				TokenType storageQual = fullSpecType.qualifier.storage.value().tokenType;
				if (storageQual != TokenType::IN &&
					storageQual != TokenType::OUT &&
					storageQual != TokenType::UNIFORM &&
					storageQual != TokenType::BUFFER) {
					throw SyntaxError{
						*interfaceBlockName,
						"An interface block declaration must have an IN, OUT, UNIFORM, or BUFFER storage qualifier!"
					};
				}
				// TODO: add more checks according to the specification!
				// Parse interface block fields.
				Consume(TokenType::LEFT_BRACE, "Interface block field declarations must start with a '{'!");
				std::shared_ptr<InterfaceBlockDecl> interfaceBlockDecl =
					std::make_shared<InterfaceBlockDecl>(*interfaceBlockName, fullSpecType.qualifier);
				while (Peek()->tokenType != TokenType::RIGHT_BRACE) {
					std::shared_ptr<VarDecl> fieldDecl = StructFieldDecl();
					interfaceBlockDecl->AddField(fieldDecl);
					Consume(TokenType::SEMICOLON, "Missing ';' after a struct field declaration!");
				}
				currentScope->AddInterfaceBlockDecl(interfaceBlockDecl);
				Consume(TokenType::RIGHT_BRACE, "Matching '}' at the end of the interface block declaration is not found!");
				return interfaceBlockDecl;
			}
			// 3.2 Other declarations: variable and function declarations (function definitions included).
			//     Additionally, the 'TypeSpecifier()' method is used to parse structure declarations.
			//     Both named and anonymous struct declarations are accepted.
			//     Variables of user-defined types (structs) are parsed later.
			//     The struct declaration (named or anonymous), if it's declared together with the variables,
			//     is going to be attached to the 'fullSpectype.specifier' instance ('typeDecl' field).
			if (IsType(*Peek())) {
				fullSpecType.specifier = TypeSpecifier();
			} else {
				throw SyntaxError{*Peek(), "Use of undeclared type!"};
			}
			// a) Type qualifiers that end with a semicolon were handled before.
			// b) Single type specifier followed by a semicolon is valid according to the grammar:
			//    - Through the second production with 'init-declarator-list'
			//    It's valid if the type specifier is a struct declaration!
			//    However, I'm not sure it's going to be valid if it's any of the basic types.
			// c) Fully-specified type (type qualifiers followed by a type specifier)
			//    followed by a semicolon is the same thing as (b):
			//    - Valid through the second production, but use cases for that are unknown.
			// d) "No type qualifier" and "no type specifier" cases should have been handled in the
			//    `external-declaration' productions.
			// As a result, structure declarations are the only use case.
			if (Match(TokenType::SEMICOLON)) {
				// Must be a struct declaration.
				if (!fullSpecType.specifier.typeDecl) {
					throw std::runtime_error{"Invalid declaration!"};
				}
				return fullSpecType.specifier.typeDecl;
			}
			// At this point we already have a fully specified type.
			// (Optional type qualifiers and a type specifier).
			// We continue on since the next token wasn't a semicolon.
			// The only token allowed at this step is an identifier.
			const Token* identifier =
				Consume(TokenType::IDENTIFIER, "Expected an identifier in a declaration!");
			// 4. Variable declarations.
			if (Peek()->tokenType == TokenType::LEFT_BRACKET) {
				// Array variable declarations, regardless of whether the type is an array type or not.
				// In other words, we don't really care whether the type is 'int' or 'int[]', we only care
				// about the brackets after the variable identifier. For example, 'int a[]'.
				std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, *identifier);
				for (const std::shared_ptr<Expr>& dimExpr : ArraySpecifier()) {
					varDecl->AddDimension(dimExpr);
				}
				if (Match(TokenType::EQUAL)) {
					varDecl->SetInitializerExpr(Initializer());
				}
				Consume(TokenType::SEMICOLON, "[Array var. decl.] Expected a semicolon after an initializer!");
				currentScope->AddVarDecl(varDecl);
				return varDecl;
			} else if (Match(TokenType::SEMICOLON)) {
				// 3. Variable declaration
				std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, *identifier);
				currentScope->AddVarDecl(varDecl);
				return varDecl;
			} else if (Match(TokenType::EQUAL)) {
				// 4. Variable declaration with an initializer.
				std::shared_ptr<VarDecl> varDecl = std::make_shared<VarDecl>(fullSpecType, *identifier);
				std::shared_ptr<Expr> initializer = Initializer();
				Consume(TokenType::SEMICOLON, "[Var decl.] Expected a semicolon after an initializer!");
				// initializer->Accept(exprTypeInferenceVisitor.get());
				varDecl->SetInitializerExpr(initializer);
				currentScope->AddVarDecl(varDecl);
				return varDecl;
			}
			// 5. Function declaration or function defintion
			if (Match(TokenType::LEFT_PAREN)) {
				if (declContext != DeclContext::EXTERNAL) {
					throw std::runtime_error{
						"Function declarations and function definitions are only allowed in the global scope!"
					};
				}
				std::shared_ptr<FunProto> funProto = FunctionPrototype(fullSpecType, *identifier);
				if (Match(TokenType::SEMICOLON)) {
					// 5.1 Function declaration.
					std::shared_ptr<FunDecl> funDecl = std::make_shared<FunDecl>(funProto);
					currentScope->AddFunDecl(funDecl);
					return funDecl;
				} else if (Peek()->tokenType == TokenType::LEFT_BRACE) {
					// 5.2 Function definition
					std::shared_ptr<BlockStmt> stmts = BlockStatement();
					std::shared_ptr<FunDecl> funDef = std::make_shared<FunDecl>(funProto, stmts);
					currentScope->AddFunDecl(funDef);
					return funDef;
				} else {
					throw std::runtime_error{"[Fun. decl.] Invalid function declaration!"};
				}
			}
			// If none of the above, throw a syntax error: "Expected a declaration!"
			// throw std::runtime_error{"Expected a function definition or function declaration!"};
			// throw std::runtime_error{"Expected a declaration!"};
			throw std::runtime_error{"Invalid declaration syntax!"};
		}
		std::shared_ptr<Decl> Parser::Declaration(DeclContext declContext) {
            return DeclarationOrFunctionDefinition(declContext);
        }
		std::shared_ptr<StructDecl> Parser::StructDeclaration() {
			Consume(TokenType::STRUCT, "A structure declaration must start with the 'struct' keyword!");
			std::shared_ptr<StructDecl> structDecl;
			if (Match(TokenType::LEFT_BRACE)) {
				// Unnamed struct.
				structDecl = std::make_shared<StructDecl>();
			} else if (Peek()->tokenType == TokenType::IDENTIFIER) {
				// New struct with a name. Add it to the environment.
				const Token* structId = Advance();
				structDecl = std::make_shared<StructDecl>(*structId);
			} else {
				throw std::runtime_error{"Invalid struct declaration! Struct name or '{' is expected!"};
			}
			Consume(TokenType::LEFT_BRACE, "Struct field declarations must start with a '{'!");
			while (Peek()->tokenType != TokenType::RIGHT_BRACE) {
				std::shared_ptr<VarDecl> fieldDecl = StructFieldDecl();
				structDecl->AddField(fieldDecl);
				Consume(TokenType::SEMICOLON, "Missing ';' after a struct field declaration!");
			}
			Consume(TokenType::RIGHT_BRACE, "Matching '}' at the end of the struct declaration is not found!");
			return structDecl;
		}
		std::shared_ptr<VarDecl> Parser::StructFieldDecl() {
			FullSpecType fullSpecType = FullySpecifiedType();
			const Token* identifier =
				Consume(TokenType::IDENTIFIER, "Anonymous struct members aren't supported!");
			std::shared_ptr<VarDecl> fieldDecl =
				std::make_shared<VarDecl>(fullSpecType, *identifier);
			if (Peek()->tokenType == TokenType::LEFT_BRACKET) {
				for (const std::shared_ptr<Expr>& dimExpr : ArraySpecifier()) {
					fieldDecl->AddDimension(dimExpr);
				}
			}
			return fieldDecl;
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
			Consume(TokenType::LEFT_BRACE, "Openning brace at the start of a block statement expected!");
			EnterNewScope();
			std::shared_ptr<BlockStmt> stmts = std::make_shared<BlockStmt>();
			if (Match(TokenType::RIGHT_BRACE)) {
				// 1. An empty block.
				return stmts;
			}
			while (Peek()->tokenType != TokenType::RIGHT_BRACE) {
				stmts->AddStmt(Statement());
			}
			Consume(TokenType::RIGHT_BRACE, "Closing brace at the end of a block statement expected!");
			RestoreEnclosingScope();
			return stmts;
		}
		std::shared_ptr<Stmt> Parser::Statement() {
			while (!AtEnd()) {
				try {
					if (Peek()->tokenType == TokenType::LEFT_BRACE) {
						return BlockStatement();
					}
					return SimpleStatement();
				} catch (SyntaxError& se) {
					// Synchronize.
					hadSyntaxError = true;
					std::cerr << se.what() << std::endl;
					if (se.GetExpectedTokenType() != TokenType::SEMICOLON) {
						Synchronize();
					}
				}
			}
			return std::shared_ptr<Stmt>{};
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
			if (IsDeclaration(*Peek())) {
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
			// We entered the "panic" mode where we're ensure where exactly we are in the grammar.
			// We skip over all tokens until we reach something
			// that looks like the start of a statement. (or a declaration)
			// In most of the cases (all?) a new declaration or statement
			// begins after a semicolon. Our goal is to match a semicolon,
			// move the pointer to the next token and return from the function.
			while (!AtEnd()) {
				if (Advance()->tokenType == TokenType::SEMICOLON) {
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
			// if (Match(TokenType::EQUAL)) {
			if (IsAssignmentOperator(Peek()->tokenType)) {
				// [TODO]: check whether the 'expr' is a valid assignment target.
				//         in other words, check if it's an lvalue.
				// if the check fails, throw a syntax error.
				const Token* assignOp = Advance();
				std::shared_ptr<Expr> rvalue = AssignmentExpression();
				assignExpr = std::make_shared<AssignExpr>(assignExpr, rvalue, *assignOp);
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
			if (IsType(*Peek())) {
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
				if (!currentScope->VarDeclExist(var->lexeme)) {
					throw SyntaxError{*var, "Identifier is not defined!"};
				}
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
				throw SyntaxError{*Peek(), "Unexpected primary expression!"};
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
			const Token* token = Peek();
			FullSpecType fullSpecType{};
			if (IsQualifier(token->tokenType)) {
				fullSpecType.qualifier = TypeQualifier();
			} if (IsType(*token)) {
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
			const Token* token = Peek();
			TypeSpec typeSpec{};
			if (IsTypeBasic(token->tokenType)) {
				// 1. Basic type.
				typeSpec.type = *token;
				Advance();
			} else if (IsTypeAggregate(*token)) {
				// 2. User-defined aggregate type.
				typeSpec.type = *token;
				// typeSpec.typeDecl = currentScope->GetStructDecl(token->lexeme);
				Advance();
			} else if (token->tokenType == TokenType::STRUCT) {
				// 3. New struct declaration (either named or unnamed).
				// std::cout << "Ok, at least that part is correct...\n";
				// typeSpec.type = *token;
				std::shared_ptr<StructDecl> structDecl = StructDeclaration();
				currentScope->AddStructDecl(structDecl);
				typeSpec.type = structDecl->GetStructName(); // Can be empty if the struct is anonymous!
				typeSpec.typeDecl = structDecl;
			} else {
				if (token->tokenType == TokenType::IDENTIFIER) {
					// std::shared_ptr<StructDecl> structDecl = currentScope->GetStructDecl(token->lexeme);
					if (!currentScope->StructDeclExist(token->lexeme)) {
						throw std::runtime_error{"Use of undeclared type!"};
					}
				} else {
					throw std::runtime_error{"Unknown type specifier encountered!"};
				}
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

		bool Parser::IsDeclaration(const Token& token) const {
			if (IsQualifier(token.tokenType) || IsType(token)) {
				return true;
			}
			return false;
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
		
		bool Parser::IsType(const Token& token) const {
			if (IsTypeBasic(token.tokenType) ||
				IsTypeAggregate(token) ||
				token.tokenType == TokenType::STRUCT) {
				return true;
			}
			return false;
		}
		bool Parser::IsTypeBasic(TokenType tokenType) const {
			if (tokenType >= TokenType::VOID &&
				tokenType <= TokenType::UIMAGE2DMSARRAY) {
				return true;
			}
			return false;
		}
		bool Parser::IsTypeScalar(TokenType tokenType) const {
			if (tokenType >= TokenType::BOOL &&
				tokenType <= TokenType::DOUBLE) {
				return true;
			}
			return false;
		}
		bool Parser::IsTypeVector(TokenType tokenType) const {
			if (tokenType >= TokenType::BVEC2 &&
				tokenType <= TokenType::DVEC4) {
				return true;
			}
			return false;
		}
		bool Parser::IsTypeMatrix(TokenType tokenType) const {
			if (tokenType >= TokenType::MAT2 &&
				tokenType <= TokenType::DMAT4X4) {
				return true;
			}
			return false;
		}
		bool Parser::IsTypeTransparent(TokenType tokenType) const {
			if (tokenType >= TokenType::VOID &&
				tokenType <= TokenType::DOUBLE) {
				return true;
			}
			return false;
		}
		bool Parser::IsTypeOpaque(TokenType tokenType) const {
			if (tokenType >= TokenType::SAMPLER2D &&
				tokenType <= TokenType::UIMAGE2DMSARRAY) {
				return true;
			}
			return false;
		}
		bool Parser::IsTypeAggregate(const Token& type) const {
			if (type.tokenType == TokenType::IDENTIFIER) {
				// Check if the user-defined type is already declared!
				if (currentScope->StructDeclExist(type.lexeme)) {
					return true;
				}
			}	
			return false;
		}

		bool Parser::IsAssignmentOperator(TokenType tokenType) const {
			return tokenType >= TokenType::EQUAL && tokenType <= TokenType::OR_ASSIGN;
		}
		bool Parser::IsBinaryArithmeticOperator(TokenType tokenType) const {
			return tokenType >= TokenType::STAR && tokenType <= TokenType::OR_OP;
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
			if (Match(tokenType)) {
				return Previous();
			} else {
				throw SyntaxError{*Peek(), tokenType, msg};
				// throw std::runtime_error{msg.data()};
			}
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