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

		enum class GlslExprType {
			UNDEFINED = -1,
			BOOL,
			// Scalars.
			INT,   UINT,
			FLOAT, DOUBLE,
			// Vectors.
			BVEC2, BVEC3, BVEC4,
			IVEC2, IVEC3, IVEC4,
			UVEC2, UVEC3, UVEC4,
			VEC2,  VEC3,  VEC4,
			DVEC2, DVEC3, DVEC4,
			// Matrices
			// 1) Single precision.
			MAT2,   MAT3,   MAT4,
			MAT2X2, MAT2X3, MAT2X4,
			MAT3X2, MAT3X3, MAT3X4,
			MAT4X2, MAT4X3, MAT4X4,
			// 2) Double precision.
			DMAT2,   DMAT3,   DMAT4,
			DMAT2X2, DMAT2X3, DMAT2X4,
			DMAT3X2, DMAT3X3, DMAT3X4,
			DMAT4X2, DMAT4X3, DMAT4X4,
			COUNT,
		};

		bool FundamentalTypeBool(GlslExprType type);
		bool FundamentalTypeInt(GlslExprType type);
		bool FundamentalTypeUint(GlslExprType type);
		bool FundamentalTypeFloat(GlslExprType type);
		bool FundamentalTypeDouble(GlslExprType type);

		bool ScalarType(GlslExprType type);
		bool VectorType(GlslExprType type);
		bool MatrixType(GlslExprType type);

		bool AddSubDivAllowed(GlslExprType lhs, GlslExprType rhs);
		bool AdditionAllowed(GlslExprType lhs, GlslExprType rhs);
		bool SubtractionAllowed(GlslExprType lhs, GlslExprType rhs);
		bool MultiplicationAllowed(GlslExprType lhs, GlslExprType rhs);
		bool DivisionAllowed(GlslExprType lhs, GlslExprType rhs);

		GlslExprType InferExprType(GlslExprType lhs, GlslExprType rhs, TokenType op);

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
			bool IsBasic() const;
			bool IsAggregate() const;
			bool IsArray() const;

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