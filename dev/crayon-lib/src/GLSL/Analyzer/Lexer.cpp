#include "GLSL/Analyzer/Lexer.h"

#include <cstdlib>

namespace crayon
{
	namespace glsl
	{
		void Lexer::ScanSrcCode(const char* srcCodeData, size_t srcCodeSize, const LexerConfig& config)
		{
			this->srcCodeData = srcCodeData;
			this->srcCodeSize = srcCodeSize;

			this->config = config;

			tokens.clear();
			line = column = start = current = 0;

			while (!AtEnd())
			{
				start = current;
				ScanToken();
			}

			this->config = LexerConfig{};

			this->srcCodeSize = 0;
			this->srcCodeData = nullptr;
		}

		const std::vector<Token>& Lexer::GetTokens() const
		{
			return tokens;
		}

		void Lexer::ScanToken()
		{
			char c = Advance();
			switch (c)
			{
				case '=':
				{
					AddToken(TokenType::EQUAL);
				}
				break;

				case '+':
				{
					AddToken(TokenType::PLUS);
				}
				break;
				case '-':
				{
					AddToken(TokenType::DASH);
				}
				break;
				case '*':
				{
					AddToken(TokenType::STAR);
				}
				break;

				case '(':
				{
					AddToken(TokenType::LEFT_PAREN);
				}
				break;
				case ')':
				{
					AddToken(TokenType::RIGHT_PAREN);
				}
				break;

				case '{':
				{
					AddToken(TokenType::LEFT_BRACE);
				}
				break;
				case '}':
				{
					AddToken(TokenType::RIGHT_BRACE);
				}
				break;
				case '.':
				{
					AddToken(TokenType::DOT);
				}
				break;
				case ',':
				{
					AddToken(TokenType::COMMA);
				}
				break;
				case ';':
				{
					AddToken(TokenType::SEMICOLON);
				}
				break;

				case '/':
				{
					uint32_t commentLineStart = line;
					uint32_t commentColumnStart = column - 1;
					if (Match('/'))
					{
						while (!AtEnd() && Peek() != '\n')
							Advance();
					}
					else if (Match('*'))
					{
						while (!AtEnd())
						{
							char c = Peek();
							if (c == '\n')
								line++;
							if (c == '*' && !AtEndNext() && PeekNext() == '/')
							{
								// Advance(); Advance();
								current += 2;
								break;
							}
							Advance();
						}

						if (AtEnd())
						{
							// Report a syntax error: unterminated multiline comment!
						}
					}
					else
					{
						AddToken(TokenType::SLASH);
					}
				}
				break;

				case '\n':
				{
					column = 0;
					line++;
				}
				break;

				case ' ':
				case '\r':
				case '\t':
				{
					// do nothing
				}
				break;

				default:
				{
					if (Numeric(c))
					{
						Number();
					}
					else if (Alpha(c))
					{
						Identifier();
					}
					else
					{
						// Report a syntax error: unidentified token encountered!
						std::cerr << "Unidentified token encountered: '" << c << "'\n";
					}
				}
				break;
			}
		}

		Token Lexer::CreateToken() const
		{
			Token token{};
			token.tokenType = TokenType::UNDEFINED;
			token.lexeme = std::string_view{ srcCodeData + start, current - start };
			return token;
		}
		Token Lexer::CreateToken(TokenType tokenType) const
		{
			Token token{};
			token.tokenType = tokenType;
			token.lexeme = std::string_view{ srcCodeData + start, current - start };
			return token;
		}

		void Lexer::AddToken(TokenType tokenType)
		{
			tokens.push_back(CreateToken(tokenType));
		}
		void Lexer::AddToken(const Token& token)
		{
			tokens.push_back(token);
		}

		char Lexer::Advance()
		{
			column++;
			return srcCodeData[current++];
		}
		char Lexer::Peek() const
		{
			if (AtEnd())
				return '\0'; // none of the token types match that symbol and it shouldn't occur in the input string
			else
				return srcCodeData[current];
		}
		char Lexer::PeekNext() const
		{
			if (AtEndNext())
				return '\0'; // none of the token types match that symbol and it shouldn't occur in the input string
			else
				return srcCodeData[current + 1];
		}
		bool Lexer::Match(char c)
		{
			if (Peek() == c)
			{
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

		bool Lexer::Alpha(char c) const
		{
			return (c >= 'a' && c <= 'z' ||
				    c >= 'A' && c <= 'Z' ||
				    c == '_');
		}
		bool Lexer::Numeric(char c) const
		{
			return (c >= '0' && c <= '9');
		}
		bool Lexer::AlphaNumeric(char c) const
		{
			return Alpha(c) || Numeric(c);
		}

		bool Lexer::AtEnd() const
		{
			return current >= srcCodeSize;
		}
		bool Lexer::AtEndNext() const
		{
			return current + 1 >= srcCodeSize;
		}
	}
}