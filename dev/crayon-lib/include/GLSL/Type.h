#pragma once

#include <list>

namespace crayon
{
	namespace glsl
	{
		class TypeQual
		{
			// TODO
		};

		class TypeSpec
		{
			// TODO
		};

		class FullSpecType
		{
		public:

			FullSpecType();

		private:

			std::list<TypeQual> qualifiers;
			TypeSpec specifier{};
		};
	}
}