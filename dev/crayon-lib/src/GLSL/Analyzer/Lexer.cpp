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
			state.line = state.column = state.start = state.current = 0;
		}
		LexerState Lexer::GetState() const {
			return state;
		}

		void Lexer::ScanToken() {
			char c = Advance();
			switch (c) {
				case '=':
					AddToken(TokenType::EQUAL);
				break;
				case '+':
					AddToken(TokenType::PLUS);
				break;
				case '-':
					AddToken(TokenType::DASH);
				break;
				case '*':
					AddToken(TokenType::STAR);
				break;
				case '(':
					AddToken(TokenType::LEFT_PAREN);
				break;
				case ')':
					AddToken(TokenType::RIGHT_PAREN);
				break;
				case '{':
					AddToken(TokenType::LEFT_BRACE);
				break;
				case '}':
					AddToken(TokenType::RIGHT_BRACE);
				break;
				case '.':
					// Check the `current` character whether it's a digit.
					// TODO
					AddToken(TokenType::DOT);
				break;
				case ',':
					AddToken(TokenType::COMMA);
				break;
				case ';':
					AddToken(TokenType::SEMICOLON);
				break;
				case '/': {
					uint32_t commentLineStart = state.line;
					uint32_t commentColumnStart = state.column - 1; // since we've already advanced
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
					} else {
						AddToken(TokenType::SLASH);
					}
				}
				break;

				case '\n':
					HandleNewLine();
				break;

				case ' ':
				case '\r':
				case '\t':
				break; // do nothing

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
				}
				break;
			}
		}

		Token Lexer::CreateToken() const {
			Token token{};
			token.tokenType = TokenType::UNDEFINED;
			token.lexeme = std::string_view{srcData + state.start, state.current - state.start};
			return token;
		}
		Token Lexer::CreateToken(TokenType tokenType) const {
			Token token{};
			token.tokenType = tokenType;
			token.lexeme = std::string_view{srcData + state.start, state.current - state.start};
			return token;
		}

		void Lexer::AddToken(TokenType tokenType) {
			tokens.push_back(CreateToken(tokenType));
		}
		void Lexer::AddToken(const Token& token) {
			tokens.push_back(token);
		}

		char Lexer::Advance() {
			state.column++;
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
			if (!NextAtEnd()) return srcData[state.current + 1];
			else return '\0';
		}
		bool Lexer::Match(char c) {
			if (Peek() == c) {
				Advance();
				return true;
			}
			return false;
		}
		char Lexer::Previous() {
			assert(state.current > 0 && "Calling 'Previous' before advancing once is not allowed!");
			return srcData[state.current - 1];
		}

        void Lexer::HandleNewLine() {
			state.column = 0;
			state.line++;
        }

        void Lexer::Number() {
			char firstDigit = Previous();
			IntConstType intConstType{IntConstType::DEC};
			FloatConstType floatConstType{FloatConstType::NORM};
			if (firstDigit == '0') {
				// Octal or hexadecimal.
				if (Match('x') || Match('X')) {
					intConstType = IntConstType::HEX;
					HexadecimalNumber();
					return;
				} else {
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
			if (Match('e') || Match('.')) {
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
			/*
			int a1 = 00000008;
			int a2 = 00000007;
			int b = 00000008.f;
			int c = 00000000;
			int d = 0x;
			int e = 0x0;
			*/
			// int f = 00000080000900045454.;
			// float fl1 = 0x00000AB;
			// float fl2 = 0x00000AB.0;
		}
		void Lexer::DecimalNumber() {
			AddIntConstant(IntConstType::DEC);
		}
		void Lexer::OctalNumber() {
			// Check if the octal number we've scanned is valid.
			uint32_t currentSaved = state.current;
			state.current = state.start;
			while (OctalDigit(Peek())) {
				Advance();
			}
			if (state.current != currentSaved) {
				throw std::runtime_error{"Invalid octal integer constant!"};
			}
			AddIntConstant(IntConstType::OCT);
		}
		void Lexer::HexadecimalNumber() {
			// We assume that the current input pointer is pointing at
			// the next digit after '0x' or '0X'.
			while (HexadecimalDigit(Peek())) {
				Advance();
			}
			// We can improve error reporting by handling this case right here.
			// Otherwise, the error would be detected during parsing and
			// the message would only be about 'an unexpected number literal'.
			if (Peek() == '.') {
				throw std::runtime_error{"Invalid floating-point constant!"};
			}
			AddIntConstant(IntConstType::HEX);
		}
		void Lexer::FloatingPointNumber() {
			// We assume that we got here by either
			// encountering a '.' or 'e' character.
			char c = Previous();
			if (c == '.') {
				// Finish the fractional part if any.
				while (DecimalDigit(Peek())) {
					Advance();
				}
			}
			if (c == 'e') {
				// Handle the exponent.
				Advance();
				if (Match('-')) {
					// Handle the negative sign.
				}
				// Handle the exponent number.
				while (DecimalDigit(Peek())) {
					Advance();
				}
			}
			AddFloatConstant();
		}

		void Lexer::Identifier() {
			while (AlphaNumeric(Peek())) {
				Advance();
			}
			AddIdOrKeyword();
		}

		void Lexer::AddIntConstant(IntConstType intConstType) {
			AddToken(TokenType::INTCONSTANT);
			Token& intConst = tokens[tokens.size() - 1];
			int base{0};
			int value{0};
			if (intConstType == IntConstType::DEC)
				base = 10;
			else if (intConstType == IntConstType::OCT)
				base = 8;
			else if (intConstType == IntConstType::HEX)
				base = 16;
			value = static_cast<int>(std::strtol(intConst.lexeme.data(), nullptr, base));
			// TODO: add the constant to a constant table (or constant pool)?
		}
		void Lexer::AddFloatConstant() {
			AddToken(TokenType::FLOATCONSTANT);
			Token& floatConst = tokens[tokens.size() - 1];
			float value = std::strtof(floatConst.lexeme.data(), nullptr);
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

		bool Lexer::Alpha(char c) const {
			return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_';
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
		bool Lexer::NextAtEnd() const {
			return state.current + 1 >= srcSize;
		}
	}
}