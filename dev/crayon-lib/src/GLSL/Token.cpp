#include "GLSL/Token.h"

namespace crayon
{
	namespace glsl
	{
		void PrintToken(std::ostream& out, const Token& token)
		{
			out << "{ " << token.lexeme << " }";
		}
	}
}