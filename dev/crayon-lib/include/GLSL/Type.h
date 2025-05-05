#pragma once

#include "GLSL/Token.h"

#include <list>
#include <optional>

namespace crayon
{
	namespace glsl
	{
		struct LayoutQualifier
		{
			Token name;
			std::optional<int> value;
		};

		struct TypeQual
		{
			std::list<LayoutQualifier> layout;

			std::optional<Token> storage;
			std::optional<Token> precision;
			std::optional<Token> interpolation;
			std::optional<Token> invariant;
			std::optional<Token> precise;
		};

		struct TypeSpec
		{
			Token type;
		};

		struct FullSpecType
		{
			std::optional<TypeQual> qualifier;
			TypeSpec specifier;
		};
	}
}