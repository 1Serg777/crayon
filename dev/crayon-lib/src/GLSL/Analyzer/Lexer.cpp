#include "GLSL/Analyzer/Lexer.h"

#include <cassert>
#include <cstdlib>

namespace crayon {
	namespace glsl {

		void Lexer::Scan(const char* srcData, size_t srcSize, const LexerConfig& config) {
			this->srcData = srcData;
			this->srcSize = srcSize;
			this->config = config;
			ClearState();
			tokens.clear();
			while (!AtEnd()) {
				state.start = state.current;
				state.startCol = state.currentCol;
				ScanToken();
			}
			this->config = LexerConfig{};
			this->srcSize = 0;
			this->srcData = nullptr;
		}

		const Token* Lexer::GetTokenData() const {
			return tokens.data();
		}
		size_t Lexer::GetTokenSize() const {
			return tokens.size();
		}

		void Lexer::ClearState() {
			state.line = state.currentCol = state.startCol;
			state.current = state.start = 0;
		}
		LexerState Lexer::GetState() const {
			return state;
		}

		void Lexer::ScanToken() {
			char c = Advance();
			switch (c) {
				case '*':
					if (Match('=')) {
						AddToken(TokenType::MUL_ASSIGN);
					} else {
						AddToken(TokenType::STAR);
					}
					break;
				case '/': {
					if (Match('/')) {
						while (!AtEnd() && Peek() != '\n')
							Advance();
					} else if (Match('*')) {
						while (!AtEnd()) {
							char c = Advance();
							if (c == '\n') {
								HandleNewLine();
							} else if (c == '*' && Peek() == '/') {
								Advance();
								break;
							}
						}
						if (AtEnd()) {
							// Report the lexical error: unterminated multiline comment!
						}
					} else if (Match('=')) {
						AddToken(TokenType::DIV_ASSIGN);
					} else {
						AddToken(TokenType::SLASH);
					}
					break;
				}
				case '%':
					if (Match('=')) {
						AddToken(TokenType::MOD_ASSIGN);
					} else {
						AddToken(TokenType::PERCENT);
					}
					break;
				case '+':
					if (Match('=')) {
						AddToken(TokenType::ADD_ASSIGN);
					} else {
						AddToken(TokenType::PLUS);
					}
					break;
				case '-':
					if (Match('=')) {
						AddToken(TokenType::SUB_ASSIGN);
					} else {
						AddToken(TokenType::DASH);
					}
					break;
				case '<':
					if (Match('<')) {
						AddToken(TokenType::LEFT_OP);
					} else if (Match('=')) {
						AddToken(TokenType::LEFT_ASSIGN);
					} else {
						AddToken(TokenType::LEFT_ANGLE);
					}
					break;
				case '>':
					if (Match('>')) {
						AddToken(TokenType::RIGHT_OP);
					} else if (Match('=')) {
						AddToken(TokenType::RIGHT_ASSIGN);
					} else {
						AddToken(TokenType::RIGHT_ANGLE);
					}
					break;
				case '=':
					if (Match('=')) {
						AddToken(TokenType::EQ_OP);
					} else {
						AddToken(TokenType::EQUAL);
					}
					break;
				case '!':
					if (Match('=')) {
						AddToken(TokenType::NE_OP);
					} else {
						AddToken(TokenType::BANG);
					}
					break;
				case '~':
					AddToken(TokenType::TILDE);
					break;
				case '&':
					if (Match('&')) {
						AddToken(TokenType::AND_OP);
					} else if (Match('=')) {
						AddToken(TokenType::AND_ASSIGN);
					} else {
						AddToken(TokenType::AMPERSAND);
					}
					break;
				case '^':
					if (Match('^')) {
						AddToken(TokenType::XOR_OP);
					} else if (Match('=')) {
						AddToken(TokenType::XOR_ASSIGN);
					} else {
						AddToken(TokenType::CARET);
					}
					break;
				case '|':
					if (Match('|')) {
						AddToken(TokenType::OR_OP);
					} else if (Match('=')) {
						AddToken(TokenType::OR_ASSIGN);
					} else {
						AddToken(TokenType::VERTICAL_BAR);
					}
					break;
				case '(':
					AddToken(TokenType::LEFT_PAREN);
					break;
				case ')':
					AddToken(TokenType::RIGHT_PAREN);
					break;
				case '[':
					AddToken(TokenType::LEFT_BRACKET);
					break;
				case ']':
					AddToken(TokenType::RIGHT_BRACKET);
					break;
				case '{':
					AddToken(TokenType::LEFT_BRACE);
					break;
				case '}':
					AddToken(TokenType::RIGHT_BRACE);
					break;
				case '.':
					if (DecimalDigit(Peek())) {
						FloatingPointNumber();
					} else {
						AddToken(TokenType::DOT);
					}
					break;
				case ',':
					AddToken(TokenType::COMMA);
					break;
				case ';':
					AddToken(TokenType::SEMICOLON);
					break;

				case '\'':
					String('\'');
					break;
				case '"':
					String('"');
					break;

				case ':':
					AddToken(TokenType::COLON);
					break;

				case '\n':
					HandleNewLine();
					break;
				case ' ':
					// state.column++;
					break;
				case '\r':
					state.currentCol = 0;
					break;
				case '\t':
					// That should depend on the editor's configuration somehow, right?
					// state.currentCol += 4;
					state.currentCol += 3; // +3 because the Advance call has already move 1 character forward.
					break;

				default: {
					if (DecimalDigit(c)) {
						Number();
					} else if (Alpha(c)) {
						Identifier();
					} else {
						// Report the lexical error: unidentified token encountered!
						Token unidentified = CreateToken();
						std::cerr << "Unidentified token encountered: '" << c << "'\n";
					}
					break;
				}
			}
		}

		Token Lexer::CreateToken() const {
			Token token{};
			token.tokenType = TokenType::UNDEFINED;
			token.lexeme = std::string_view{srcData + state.start, state.current - state.start};
			token.line = state.line;
			token.startCol = state.startCol;
			token.endCol = state.currentCol;
			return token;
		}
		Token Lexer::CreateToken(TokenType tokenType) const {
			Token token{};
			token.tokenType = tokenType;
			token.lexeme = std::string_view{srcData + state.start, state.current - state.start};
			token.line = state.line;
			token.startCol = state.startCol;
			token.endCol = state.currentCol;
			return token;
		}

		void Lexer::AddToken(TokenType tokenType) {
			tokens.push_back(CreateToken(tokenType));
		}
		void Lexer::AddToken(const Token& token) {
			tokens.push_back(token);
		}

		char Lexer::Advance() {
			state.currentCol++;
			return srcData[state.current++];
		}
		void Lexer::PutBack() {
			assert(state.current > 0 && "Calling 'PutBack' before advancing once is not allowed!");
			state.current--;
		}
		char Lexer::Peek() const {
			// The '\0' character is used because none of the token types match it
			// and it shouldn't occur in the input.
			if (!AtEnd()) return srcData[state.current];
			else return '\0';
		}
		char Lexer::PeekNext() const {
			// The '\0' character is used because none of the token types match it
			// and it shouldn't occur in the input.
			if (!AtEndNext()) return srcData[state.current + 1];
			else return '\0';
		}
		char Lexer::Previous() {
			assert(state.current > 0 && "Calling 'Previous' before advancing once is not allowed!");
			return srcData[state.current - 1];
		}
		bool Lexer::Match(char c) {
			if (Peek() == c) {
				Advance();
				return true;
			}
			return false;
		}
		char Lexer::Consume(char c, std::string_view errMsg) {
			if (Match(c))
				return Previous();
			else
				throw std::runtime_error{errMsg.data()};
		}

        void Lexer::HandleNewLine() {
			state.currentCol = 0;
			state.line++;
        }

        void Lexer::Number() {
			char firstDigit = Previous();
			IntConstType intConstType{IntConstType::DEC};
			if (firstDigit == '0') {
				// Octal or hexadecimal.
				if (Match('x') || Match('X')) {
					intConstType = IntConstType::HEX;
					HexadecimalNumber();
					return;
				} else if (DecimalDigit(Peek())) {
					// At this point we don't check if the next digit after '0'
					// is a valid octal digit or not. This is because the resulting constant
					// might actually be a floating-point constant, such as '0000.5'.
					intConstType = IntConstType::OCT;
				}
			}
			// Proceed to handle a number that at this point
			// can be one of the following three:
			// 1) Decimal integer number.
			// 2) Octal integer number.
			// 3) Floating-point number.
			// The loop works with decimal digits so if the constant
			// is an octal integer, we would have to check its validity later.
			while (DecimalDigit(Peek())) {
				Advance();
			}
			if (Match('.') || Match('e')) {
				// Handle as a floating-point constant.
				FloatingPointNumber();
			} else {
				// Handle as an integer constant. It can be one of the two types:
				// 1) Decimal
				// 2) Octal
				if (intConstType == IntConstType::DEC) {
					DecimalNumber();
				} else if (intConstType == IntConstType::OCT) {
					OctalNumber();
				}
			}
		}
		void Lexer::DecimalNumber() {
			// Handle integer suffix.
			FinishIntegerNumber(IntConstType::DEC);
		}
		void Lexer::OctalNumber() {
			// Check if the octal number we've scanned is valid.
			uint32_t currentSaved = state.current;
			uint32_t currentColSaved = state.currentCol;
			state.current = state.start;
			state.currentCol = state.startCol;
			while (OctalDigit(Peek())) {
				Advance();
			}
			if (state.current != currentSaved) {
				throw std::runtime_error{"Octal integer constant contains invalid digits!"};
			}
			FinishIntegerNumber(IntConstType::OCT);
		}
		void Lexer::HexadecimalNumber() {
			// We assume that the current input pointer is pointing at
			// the next digit after '0x' or '0X'. If the first character after
			// that sequence is not a hexadecimal number, then that's a syntax error.
			if (!HexadecimalDigit(Peek())) {
				throw std::runtime_error{"At least one hexadecimal digit must be present after '0x' or '0X'!"};
			}
			Advance();
			while (HexadecimalDigit(Peek())) {
				Advance();
			}
			// We can improve error reporting by handling the case when
			// a floating-point number has a hexadecimal integer part.
			if (Peek() == '.') {
				throw std::runtime_error{"Floating-point number can't have a hexadecimal integer part!"};
			}
			FinishIntegerNumber(IntConstType::HEX);
		}
		void Lexer::FinishIntegerNumber(IntConstType intConstType) {
			// The integer suffix is optional.
			// 'INTCONST' is the default type if it's not present.
			switch (HandleIntSuffix()) {
				case TokenType::UINTCONSTANT:
					AddUintConstant(intConstType);
				break;
				case TokenType::INTCONSTANT:
					AddIntConstant(intConstType);
				break;
				default:
					assert(false && "Unrecognized integer constant type!");
				break;
			}
		}
		void Lexer::FloatingPointNumber() {
			// We assume that we got here by either
			// encountering a '.' or 'e' character.
			// FloatConstType floatConstType{FloatConstType::NORM};
			char c = Previous();
			if (c == '.') {
				// Finish the fractional part if any.
				while (DecimalDigit(Peek())) {
					Advance();
				}
				c = Peek();
			}
			if (c == 'e') {
				// Handle the exponent.
				Advance();
				if (Match('-') || Match('+')) {
					// Handle the positive or negative sign when present.
				}
				// Handle the exponent number.
				while (DecimalDigit(Peek())) {
					Advance();
				}
			}
			// The floating suffix is optional.
			// 'FLOATCONST' is the default type if it's not present.
			switch (HandleFloatSuffix()) {
				case TokenType::DOUBLECONSTANT:
					AddDoubleConstant();
				break;
				case TokenType::FLOATCONSTANT:
					AddFloatConstant();
				break;
				default:
					assert(false && "Unrecognized floating-point constant type!");
				break;
			}
		}

		TokenType Lexer::HandleIntSuffix() {
			// Handle integer suffix.
			if (Letter(Peek())) {
				if (Match('u') || Match('U')) {
					return TokenType::UINTCONSTANT;
				} else {
					throw std::runtime_error{"Unrecognized integer suffix found!"};
				}
			} else {
				return TokenType::INTCONSTANT;
			}
		}
		TokenType Lexer::HandleFloatSuffix() {
			// Handle floating suffix.
			if (Letter(Peek())) {
				if (Match('l')) {
					Consume('f', "Unknown floating suffix found! Was 'lf' intended?");
					return TokenType::DOUBLECONSTANT;
				} else if (Match('L')) {
					Consume('F', "Unknown floating suffix found! Was 'LF' intended?");
					return TokenType::DOUBLECONSTANT;
				} else if (Match('f') || Match('F')) {
					// Single precision floating-point constant.
					// 'float' is the default type used for floats, so
					// the suffix 'f' or 'F' is optional.
					return TokenType::FLOATCONSTANT;
				} else {
					throw std::runtime_error{"Unrecognized floating suffix found!"};
				}
			} else {
				// No suffix present. Assume single precision floating-point constant.
				return TokenType::FLOATCONSTANT;
			}
		}

		void Lexer::String(char delim) {
			while (!AtEnd() && Peek() != delim) {
				Advance();
			}
			if (AtEnd()) {
				throw std::runtime_error{"Unterminated string literal!"};
			}
			Consume(delim, "Unterminated string literal!");
			AddToken(TokenType::STRING);
		}
		void Lexer::Identifier() {
			while (AlphaNumeric(Peek())) {
				Advance();
			}
			AddIdOrKeyword();
		}

		void Lexer::AddIntConstant(IntConstType intConstType) {
			AddToken(TokenType::INTCONSTANT);
			// Token& intConst = tokens[tokens.size() - 1];
			// int base = static_cast<int>(intConstType);
			// int64_t value = static_cast<int64_t>(std::strtol(intConst.lexeme.data(), nullptr, base));
			// TODO: add the constant to a constant table (or constant pool)?
		}
		void Lexer::AddUintConstant(IntConstType intConstType) {
			AddToken(TokenType::UINTCONSTANT);
			// Token& intConst = tokens[tokens.size() - 1];
			// int base = static_cast<int>(intConstType);
			// uint64_t value = static_cast<uint64_t>(std::strtoul(intConst.lexeme.data(), nullptr, base));
			// TODO: add the constant to a constant table (or constant pool)?
		}
		void Lexer::AddFloatConstant() {
			AddToken(TokenType::FLOATCONSTANT);
			// Token& floatConst = tokens[tokens.size() - 1];
			// float value = std::strtof(floatConst.lexeme.data(), nullptr);
			// TODO: add the constant to a constant table (or constant pool)?
		}
		void Lexer::AddDoubleConstant() {
			AddToken(TokenType::DOUBLECONSTANT);
			// Token& floatConst = tokens[tokens.size() - 1];
			// double value = std::strtod(floatConst.lexeme.data(), nullptr);
			// TODO: add the constant to a constant table (or constant pool)?
		}

		void Lexer::AddIdOrKeyword() {
			Token token = CreateToken();
			auto searchRes = config.keywords->find(token.lexeme);
			if (searchRes == config.keywords->end())
				token.tokenType = TokenType::IDENTIFIER;
			else
				token.tokenType = searchRes->second;
			AddToken(token);
		}

		bool Lexer::Letter(char c) const {
			return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z';
		}
		bool Lexer::Alpha(char c) const {
			return Letter(c) || c == '_';
		}
		bool Lexer::DecimalDigit(char c) const {
			return c >= '0' && c <= '9';
		}
		bool Lexer::OctalDigit(char c) const {
			return c >= '0' && c <= '7';
		}
		bool Lexer::HexadecimalDigit(char c) const {
			return c >= '0' && c <= '9' ||
				   c >= 'A' && c <= 'F' ||
				   c >= 'a' && c <= 'f';
		}
		bool Lexer::AlphaNumeric(char c) const {
			return Alpha(c) || DecimalDigit(c);
		}

		bool Lexer::AtEnd() const {
			return state.current >= srcSize;
		}
		bool Lexer::AtEndNext() const {
			return state.current + 1 >= srcSize;
		}
	}
}