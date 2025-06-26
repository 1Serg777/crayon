#pragma once

#include "GLSL/Token.h"

#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace crayon {
	namespace glsl {

		class StructDecl;
		class Expr;

		// Alternative 'GlslBasicType' definition #1.
		/*
		enum class GlslBasicType {
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
		// Alternative 'GlslBasicType' definition #2.
		/*
		enum class GlslBasicType {
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
		// Changing 'GlslBasicType' enumeration may cause methods responsible for
		// type checks and type promotion operations to work incorrectly.
		// If you do that, make sure to change the methods whose functionality is affected.
		enum class GlslBasicType {
			UNDEFINED = -1,
			// Basic types.
			// Scalars.
			BOOL, INT, UINT, FLOAT, DOUBLE,
			// Vectors.
			BVEC2, IVEC2, UVEC2, VEC2, DVEC2,
			BVEC3, IVEC3, UVEC3, VEC3, DVEC3,
			BVEC4, IVEC4, UVEC4, VEC4, DVEC4,
			// Matrices.
			// MAT2, DMAT2, MAT3, DMAT3, MAT4, DMAT4 are simply aliases that
			// we're going to treat early in the semantic analysis stage.
			// MAT2, DMAT2, MAT3, DMAT3, MAT4, DMAT4,
			MAT2X2, DMAT2X2, MAT2X3, DMAT2X3, MAT2X4, DMAT2X4,
			MAT3X2, DMAT3X2, MAT3X3, DMAT3X3, MAT3X4, DMAT3X4,
			MAT4X2, DMAT4X2, MAT4X3, DMAT4X3, MAT4X4, DMAT4X4,
			// Custom (user-defined) types.
			CUSTOM,
			COUNT,
		};
		enum class OperandPos {
			LHS, RHS
		};

		struct GlslExprType {
			bool BasicType() const;
			bool CustomType() const;
			bool AnonymousCustomType() const;
			bool Array() const;

			std::vector<size_t> dimensions;
			std::string_view name;
			GlslBasicType type{GlslBasicType::UNDEFINED};
			bool constExpr{false};
		};
		bool operator==(const GlslExprType& type1, const GlslExprType& type2);
		// Strong equality.
		bool TypesEqual(const GlslExprType& type1, const GlslExprType& type2);
		// Equality with the Implicit Conversions taken into account.
		bool TypePromotable(const GlslExprType& check, const GlslExprType& promoteTo);

		GlslBasicType GetGlslExprType(TokenType tokenType);

		GlslBasicType GetAliasType(std::string_view alias);

		int GetFundamentalTypeRank(GlslBasicType type);
		GlslBasicType GetFundamentalType(GlslBasicType type);

		GlslBasicType GetTypeRowsCols(size_t rows, size_t cols);
		size_t GetNumberOfRows(GlslBasicType type, OperandPos pos);
		size_t GetNumberOfCols(GlslBasicType type, OperandPos pos);

		size_t GetVecNumberOfRows(GlslBasicType type, OperandPos pos);
		size_t GetVecNumberOfCols(GlslBasicType type, OperandPos pos);

		size_t GetColVecNumberOfRows(GlslBasicType type);
		size_t GetColVecNumberOfCols(GlslBasicType type);
		size_t GetRowVecNumberOfRows(GlslBasicType type);
		size_t GetRowVecNumberOfCols(GlslBasicType type);

		size_t GetMatNumberOfRows(GlslBasicType type);
		size_t GetMatNumberOfCols(GlslBasicType type);

		GlslBasicType PromoteType(GlslBasicType type, int rankDiff);
#if defined (_DEBUG) || defined(DEBUG)
		GlslBasicType PromoteFundamentalType(GlslBasicType type, int rankDiff);
		GlslBasicType PromoteScalarType(GlslBasicType type, int rankDiff);
		GlslBasicType PromoteVectorType(GlslBasicType type, int rankDiff);
		GlslBasicType PromoteMatrixType(GlslBasicType type, int rankDiff);
#endif

		bool FundamentalTypeBool(GlslBasicType type);
		bool FundamentalTypeInt(GlslBasicType type);
		bool FundamentalTypeUint(GlslBasicType type);
		bool FundamentalTypeFloat(GlslBasicType type);
		bool FundamentalTypeDouble(GlslBasicType type);

		bool FundamentalType(GlslBasicType type);
		bool ScalarType(GlslBasicType type);
		bool IntegralType(GlslBasicType type);
		bool FloatingType(GlslBasicType type);
		bool VectorType(GlslBasicType type);
		bool MatrixType(GlslBasicType type);

		bool AddSubDivAllowed(GlslBasicType lhs, GlslBasicType rhs);
		bool AdditionAllowed(GlslBasicType lhs, GlslBasicType rhs);
		bool SubtractionAllowed(GlslBasicType lhs, GlslBasicType rhs);
		bool MultiplicationAllowed(GlslBasicType lhs, GlslBasicType rhs);
		bool DivisionAllowed(GlslBasicType lhs, GlslBasicType rhs);

		GlslBasicType LookupExprType(GlslBasicType lhs, GlslBasicType rhs, TokenType op);
		GlslBasicType InferExprType(GlslBasicType lhs, GlslBasicType rhs, TokenType op);
		GlslBasicType InferArithmeticBinaryExprType(GlslBasicType lhs, GlslBasicType rhs, TokenType op);

		struct LayoutQualifier {
			Token name;
			std::optional<int> value;
		};

		struct TypeQual {
			bool Const() const;
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
			// Used only when the 'TypeSpec' instance is part of the declaration.
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