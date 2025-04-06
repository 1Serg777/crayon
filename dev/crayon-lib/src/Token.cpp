#include "Token.h"

namespace csl
{
	void PrintToken(std::ostream& out, const Token& token)
	{
		out << "{ " << token.lexeme << " }";
	}
}