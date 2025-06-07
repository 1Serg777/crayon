#pragma once

#include "GLSL/Token.h"

#include <list>
#include <memory>
#include <optional>
#include <vector>

namespace crayon {
	namespace glsl {

		class Expr;

		struct LayoutQualifier {
			Token name;
			std::optional<int> value;
		};

		struct TypeQual {
			bool Empty() const;

			std::list<LayoutQualifier> layout;

			std::optional<Token> storage;
			std::optional<Token> precision;
			std::optional<Token> interpolation;
			std::optional<Token> invariant;
			std::optional<Token> precise;
		};

		struct TypeSpec {
			bool ArrayType() const;
			size_t ArrayDimensionCount() const;
			std::shared_ptr<Expr> ArrayDimensionSizeExpr(size_t dimension) const;
			
			// Base type.
			Token type;
			// Array dimensions.
			// Optional is used because
			// we need to differentiate between the following three states:
			// 1. No brackets after the base type.
			// 2. Empty brackets after the base type.
			// 3. Brackets with a constant integer expression inside after the base type.
			// When std::optional<size_t> has no value, we assume that the dimensions is not present.
			// When, however, it have value and the value is 0, we assume that
			// this dimension was formed by writing empty brackets like so "[]".
			// When value is present and it's > 0, the dimension is present and was sized explicitly.
			// std::optional<size_t> x;
			// std::optional<size_t> y;
			
			// Alternative way to specify dimensions.
			// std::vector<size_t> dimensions;
			std::vector<std::shared_ptr<Expr>> dimensions;
		};

		struct FullSpecType {
			TypeQual qualifier;
			TypeSpec specifier;
		};

	}
}