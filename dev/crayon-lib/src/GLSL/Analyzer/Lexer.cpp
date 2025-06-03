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
							char c = Peek();
							if (c == '\n')
								state.line++;
							else if (c == '*' && !NextAtEnd() && PeekNext() == '/') {
								// Advance(); Advance();
								state.current += 2;
								break;
							}
							Advance();
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
					state.column = 0;
					state.line++;
				break;

				case ' ':
				case '\r':
				case '\t':
				break; // do nothing

				default: {
					if (Numeric(c)) {
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
			assert(state.current != 0 && "Calling 'PutBack' before advancing once is not allowed!");
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

		void Lexer::Number()
		{
			while (Numeric(Peek()))
			{
				Advance();
			}

			// Handle as a floating-point constant
			if (Match('.'))
			{
				while (Numeric(Peek()))
				{
					Advance();
				}
				AddFloatConstant();
			}
			// Handle as an integer constant
			else
			{
				AddIntConstant();
			}
		}
		void Lexer::Identifier()
		{
			while (AlphaNumeric(Peek()))
			{
				Advance();
			}

			AddIdOrKeyword();
		}

		void Lexer::AddIntConstant()
		{
			AddToken(TokenType::INTCONSTANT);
			Token& intConst = tokens[tokens.size() - 1];
			
			int value = static_cast<int>(std::strtol(intConst.lexeme.data(), nullptr, 10));

			// TODO: add the constant to a constant table (or constant pool)?
		}
		void Lexer::AddFloatConstant()
		{
			AddToken(TokenType::FLOATCONSTANT);
			Token& floatConst = tokens[tokens.size() - 1];

			float value = std::strtof(floatConst.lexeme.data(), nullptr);

			// TODO: add the constant to a constant table (or constant pool)?
		}

		void Lexer::AddIdOrKeyword()
		{
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
		bool Lexer::Numeric(char c) const {
			return c >= '0' && c <= '9';
		}
		bool Lexer::AlphaNumeric(char c) const {
			return Alpha(c) || Numeric(c);
		}

		bool Lexer::AtEnd() const {
			return state.current >= srcSize;
		}
		bool Lexer::NextAtEnd() const {
			return state.current + 1 >= srcSize;
		}
	}
}