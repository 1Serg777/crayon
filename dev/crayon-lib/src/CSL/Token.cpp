#include "CSL/Token.h"

namespace crayon
{
	namespace csl
	{
		void PrintToken(std::ostream& out, const Token& token)
		{
			out << "{ " << token.lexeme << " }";
		}
	}
}