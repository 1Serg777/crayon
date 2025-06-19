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

		// Alternative 'GlslExprType' definition #1.
		/*
		enum class GlslExprType {
			UNDEFINED = -1,
			BOOL,
			// Scalars.
			INT, UINT, FLOAT, DOUBLE,
			// Vectors.
			BVEC2, IVEC2, UVEC2, VEC2, DVEC2,
			BVEC3, IVEC3, UVEC3, VEC3, DVEC3,
			BVEC4, IVEC4, UVEC4, VEC4, DVEC4,
			// Matrices
			// 1) Single precision.
			MAT2, DMAT2, MAT2X2, DMAT2X2,
			MAT3, DMAT3, MAT3X3, DMAT3X3,
			MAT4, DMAT4, MAT4X4, DMAT4X4,

			MAT2X3, DMAT2X3, MAT2X4, DMAT2X4,
			MAT3X2, DMAT3X2, MAT3X4, DMAT3X4,
			MAT4X2, DMAT4X2, MAT4X3, DMAT4X3,
			COUNT,
		};
		*/
		// Alternative 'GlslExprType' definition #2.
		/*
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
		*/
		// Changing 'GlslExprType' enumeration may cause methods responsible for
		// type checks and type promotion operations to work incorrectly.
		// If you do that, make sure to change the methods whose functionality is affected.
		enum class GlslExprType {
			UNDEFINED = -1,
			// Scalars.
			BOOL, INT, UINT, FLOAT, DOUBLE,
			// Vectors.
			BVEC2, IVEC2, UVEC2, VEC2, DVEC2,
			BVEC3, IVEC3, UVEC3, VEC3, DVEC3,
			BVEC4, IVEC4, UVEC4, VEC4, DVEC4,
			// Matrices
			// 1) Single precision.
			MAT2, MAT3, MAT4,
			MAT2X2, MAT2X3, MAT2X4,
			MAT3X2, MAT3X3, MAT3X4,
			MAT4X2, MAT4X3, MAT4X4,
			// 2) Double precision.
			DMAT2, DMAT3, DMAT4,
			DMAT2X2, DMAT2X3, DMAT2X4,
			DMAT3X2, DMAT3X3, DMAT3X4,
			DMAT4X2, DMAT4X3, DMAT4X4,
			COUNT,
		};

		GlslExprType GetFundamentalType(GlslExprType type);
		int GetFundamentalTypeRank(GlslExprType type);

		int GetNumberOfRows(GlslExprType type);
		int GetNumberOfCols(GlslExprType type);

		GlslExprType PromoteType(GlslExprType type, int rank);
		GlslExprType PromoteFundamentalType(GlslExprType type, int rank);
		GlslExprType PromoteScalarType(GlslExprType type, int rank);
		GlslExprType PromoteVectorType(GlslExprType type, int rank);
		GlslExprType PromoteMatrixType(GlslExprType type, int rank);

		bool FundamentalTypeBool(GlslExprType type);
		bool FundamentalTypeInt(GlslExprType type);
		bool FundamentalTypeUint(GlslExprType type);
		bool FundamentalTypeFloat(GlslExprType type);
		bool FundamentalTypeDouble(GlslExprType type);

		bool FundamentalType(GlslExprType type);
		bool ScalarType(GlslExprType type);
		bool IntegralType(GlslExprType type);
		bool FloatingType(GlslExprType type);
		bool VectorType(GlslExprType type);
		bool MatrixType(GlslExprType type);

		bool TypesEqual(GlslExprType type1, GlslExprType type2);

		bool AddSubDivAllowed(GlslExprType lhs, GlslExprType rhs);
		bool AdditionAllowed(GlslExprType lhs, GlslExprType rhs);
		bool SubtractionAllowed(GlslExprType lhs, GlslExprType rhs);
		bool MultiplicationAllowed(GlslExprType lhs, GlslExprType rhs);
		bool DivisionAllowed(GlslExprType lhs, GlslExprType rhs);

		GlslExprType LookupExprType(GlslExprType lhs, GlslExprType rhs, TokenType op);
		GlslExprType InferExprType(GlslExprType lhs, GlslExprType rhs, TokenType op);
		GlslExprType InferArithmeticBinaryExprType(GlslExprType lhs, GlslExprType rhs, TokenType op);

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