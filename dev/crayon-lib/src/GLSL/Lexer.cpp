#include "GLSL/Lexer.h"

#include <cstdlib>

namespace crayon
{
	namespace glsl
	{
		void Lexer::ScanSrcCode(const char* srcCodeData, size_t srcCodeSize)
		{
			this->srcCodeData = srcCodeData;
			this->srcCodeSize = srcCodeSize;

			line = column = start = current = 0;

			while (!AtEnd())
			{
				start = current;
				ScanToken();
			}

			this->srcCodeSize = 0;
			this->srcCodeData = nullptr;
		}

		const std::vector<Token>& Lexer::GetTokens()
		{
			return tokens;
		}

		void Lexer::ScanToken()
		{
			char c = Advance();
			switch (c)
			{
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
							if (c == '*' && PeekNext() == '/')
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
					else
					{
						// Report a syntax error: unidentified token encountered!
					}
				}
				break;
			}
		}

		Token Lexer::CreateToken(TokenType tokenType)
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

		char Lexer::Advance()
		{
			column++;
			return srcCodeData[current++];
		}
		char Lexer::Peek()
		{
			if (AtEnd())
				return '\0'; // none of the token types match that symbol and it shouldn't occur in the input string
			else
				return srcCodeData[current];
		}
		char Lexer::PeekNext()
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

			AddIntegerConstant();
		}

		void Lexer::AddIntegerConstant()
		{
			AddToken(TokenType::INTCONSTANT);
			Token& numTok = tokens[tokens.size() - 1];
			
			int value = static_cast<int>(std::strtol(numTok.lexeme.data(), nullptr, 10));

			// TODO
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