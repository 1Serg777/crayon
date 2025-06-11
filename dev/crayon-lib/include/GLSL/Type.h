#pragma once

#include "GLSL/Token.h"

#include <list>
#include <memory>
#include <optional>
#include <vector>

namespace crayon {
	namespace glsl {

		class StructDecl;
		class Expr;

		enum class ExprType {
			UNDEFINED = -1,
			BOOL,
			INT,
			UINT,
			FLOAT,
			DOUBLE,
		};

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
			bool IsBasicType() const;
			bool IsAggregateType() const;
			bool IsArrayType() const;

			size_t ArrayDimensionCount() const;
			std::shared_ptr<Expr> ArrayDimensionSizeExpr(size_t dimension) const;
			
			// Basic or Aggregate type identifier.
			Token type;
			// StructDecl pointer to a named or an unnamed structure.
			// 1) struct {...} s1, s2; // unnamed structure declaration.
			// 2) struct S {...} s1, s2; // named structure declaration.
			std::shared_ptr<StructDecl> typeDecl;
			// Array dimensions (if it's an array type, i.e. int[]).
			std::vector<std::shared_ptr<Expr>> dimensions;
		};

		struct FullSpecType {
			TypeQual qualifier;
			TypeSpec specifier;
		};

	}
}