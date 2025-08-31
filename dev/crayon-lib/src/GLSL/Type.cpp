#include "GLSL/Type.h"
#include "GLSL/Analyzer/Environment.h"

#include <algorithm>
#include <cassert>
#include <string_view>
#include <sstream>
#include <unordered_map>

namespace crayon {
	namespace glsl {

		bool IsTypeBasic(TokenType tokenType) {
			if (tokenType >= TokenType::VOID &&
				tokenType <= TokenType::UIMAGE2DMSARRAY) {
				return true;
			}
			return false;
		}
		bool IsTypeFundamental(TokenType tokenType) {
			return tokenType >= TokenType::VOID &&
			       tokenType <= TokenType::DOUBLE;
		}
		bool IsTypeScalar(TokenType tokenType) {
			if (tokenType >= TokenType::BOOL &&
				tokenType <= TokenType::DOUBLE) {
				return true;
			}
			return false;
		}
		bool IsTypeVector(TokenType tokenType) {
			if (tokenType >= TokenType::BVEC2 &&
				tokenType <= TokenType::DVEC4) {
				return true;
			}
			return false;
		}
		bool IsTypeMatrix(TokenType tokenType) {
			if (tokenType >= TokenType::MAT2 &&
				tokenType <= TokenType::DMAT4X4) {
				return true;
			}
			return false;
		}
		bool IsTypeTransparent(TokenType tokenType) {
			if (tokenType >= TokenType::VOID &&
				tokenType <= TokenType::DMAT4X4) {
				return true;
			}
			return false;
		}
		bool IsTypeOpaque(TokenType tokenType) {
			if (tokenType >= TokenType::SAMPLER2D &&
				tokenType <= TokenType::UIMAGE2DMSARRAY) {
				return true;
			}
			return false;
		}

		static std::unordered_map<std::string_view, TokenType> aliasTypeMap{
			{"mat2",  TokenType::MAT2X2 },
			{"mat3",  TokenType::MAT3X3 },
			{"mat4",  TokenType::MAT4X4 },
			{"dmat2", TokenType::DMAT2X2},
			{"dmat3", TokenType::DMAT3X3},
			{"dmat4", TokenType::DMAT4X4},
		};
		TokenType GetAliasType(std::string_view alias) {
			auto searchRes = aliasTypeMap.find(alias);
			if (searchRes == aliasTypeMap.end()) {
				return TokenType::UNDEFINED;
			}
			return searchRes->second;
		}

		static constexpr int materialPropertyTypeOffset = static_cast<int>(TokenType::MAT_PROP_TYPE_INT);
		static constexpr TokenType materialPropertyTypeToGlslType[] = {
			// Scalar types:
			TokenType::INT,       // MAT_PROP_TYPE_INT
			TokenType::FLOAT,     // MAT_PROP_TYPE_FLOAT
			// Vector types:
			TokenType::VEC2,      // MAT_PROP_TYPE_VEC2
			TokenType::VEC3,      // MAT_PROP_TYPE_VEC3
			TokenType::VEC4,      // MAT_PROP_TYPE_VEC4
			TokenType::VEC4,      // MAT_PROP_TYPE_COLOR
			// Opaque types:
			TokenType::SAMPLER2D, // MAT_PROP_TYPE_TEX2D
		};
		bool IsMaterialPropertyType(TokenType tokenType) {
			return tokenType >= TokenType::MAT_PROP_TYPE_INT &&
				   tokenType <= TokenType::MAT_PROP_TYPE_TEX2D;
		}
		TokenType MapMaterialPropertyType(TokenType tokenType) {
			assert(IsMaterialPropertyType(tokenType) && "Not a material property type provided!");
			int idx = static_cast<int>(tokenType) - materialPropertyTypeOffset;
			return materialPropertyTypeToGlslType[idx];
		}

		static constexpr int fundamentalTypeOffset = static_cast<int>(TokenType::VOID);
		static constexpr TokenType fundamentalTypeMap[] = {
			TokenType::VOID,
			// Scalars.
			TokenType::BOOL, TokenType::INT, TokenType::UINT, TokenType::FLOAT, TokenType::DOUBLE, // BOOL, INT, UINT, FLOAT, DOUBLE
			// Vectors.
			TokenType::BOOL, TokenType::INT, TokenType::UINT, TokenType::FLOAT, TokenType::DOUBLE, // BVEC2, IVEC2, UVEC2, VEC2, DVEC2
			TokenType::BOOL, TokenType::INT, TokenType::UINT, TokenType::FLOAT, TokenType::DOUBLE, // BVEC3, IVEC3, UVEC3, VEC3, DVEC3
			TokenType::BOOL, TokenType::INT, TokenType::UINT, TokenType::FLOAT, TokenType::DOUBLE, // BVEC4, IVEC4, UVEC4, VEC4, DVEC4
			// Matrices.
			TokenType::FLOAT, TokenType::DOUBLE, TokenType::FLOAT, TokenType::DOUBLE, TokenType::FLOAT, TokenType::DOUBLE, // MAT2X2, DMAT2X2, MAT2X3, DMAT2X3, MAT2X4, DMAT2X4
			TokenType::FLOAT, TokenType::DOUBLE, TokenType::FLOAT, TokenType::DOUBLE, TokenType::FLOAT, TokenType::DOUBLE, // MAT3X2, DMAT3X2, MAT3X3, DMAT3X3, MAT3X4, DMAT3X4
			TokenType::FLOAT, TokenType::DOUBLE, TokenType::FLOAT, TokenType::DOUBLE, TokenType::FLOAT, TokenType::DOUBLE, // MAT4X2, DMAT4X2, MAT4X3, DMAT4X3, MAT4X4, DMAT4X4
		};
		int GetFundamentalTypeRank(TokenType type) {
			return static_cast<int>(GetFundamentalType(type));
		}
		TokenType GetFundamentalType(TokenType type) {
			assert(IsTypeTransparent(type) && "Only transparent types have fundamental type!");
			int fundTypeIdx = static_cast<int>(type) - fundamentalTypeOffset;
			return fundamentalTypeMap[fundTypeIdx];
		}

		size_t GetDimensionCountNonArray(TokenType type) {
			assert(IsTypeTransparent(type) && "Type must be transparent!");
			if (IsTypeScalar(type)) {
				return 1;
			}
			if (IsTypeVector(type)) {
				return GetColVecNumberOfRows(type);
			}
			if (IsTypeMatrix(type)) {
				size_t rows = GetMatNumberOfRows(type);
				size_t cols = GetMatNumberOfCols(type);
				return rows * cols;
			}
			// To avoid a warning message.
			return 0;
		}

		static constexpr TokenType rowsColsFloatTypeMap[4][4] = {
			{TokenType::FLOAT, TokenType::VEC2,   TokenType::VEC3,   TokenType::VEC4  },
			{TokenType::VEC2,  TokenType::MAT2X2, TokenType::MAT2X3, TokenType::MAT2X4},
			{TokenType::VEC3,  TokenType::MAT3X2, TokenType::MAT3X3, TokenType::MAT3X4},
			{TokenType::VEC4,  TokenType::MAT4X2, TokenType::MAT4X3, TokenType::MAT4X4},
		};
		static constexpr TokenType rowsColsDoubleTypeMap[4][4] = {
			{TokenType::DOUBLE, TokenType::DVEC2,   TokenType::DVEC3,   TokenType::DVEC4  },
			{TokenType::DVEC2,  TokenType::DMAT2X2, TokenType::DMAT2X3, TokenType::DMAT2X4},
			{TokenType::DVEC3,  TokenType::DMAT3X2, TokenType::DMAT3X3, TokenType::DMAT3X4},
			{TokenType::DVEC4,  TokenType::DMAT4X2, TokenType::DMAT4X3, TokenType::DMAT4X4},
		};
		TokenType GetTypeRowsCols(size_t rows, size_t cols) {
			return rowsColsFloatTypeMap[rows - 1][cols - 1];
		}
		TokenType GetTypeRowsCols(TokenType fundamentalType, size_t rows, size_t cols) {
			assert(IsTypeFundamental(fundamentalType) && "Not a fundamental type provided!");
			assert(fundamentalType != TokenType::FLOAT &&
				   fundamentalType != TokenType::DOUBLE &&
				   "Only 'float' or 'double' matrices are supported!");
			if (fundamentalType == TokenType::FLOAT) {
				return rowsColsFloatTypeMap[rows - 1][cols - 1];
			} else /*if (fundamentalType == GlslBasicType::DOUBLE)*/ {
				return rowsColsDoubleTypeMap[rows - 1][cols - 1];
			}
		}

		size_t GetNumberOfRows(TokenType type, OperandPos pos) {
			if (IsTypeMatrix(type)) {
				return GetMatNumberOfRows(type);
			} else if (IsTypeVector(type)) {
				return GetVecNumberOfRows(type, pos);
			} else /*if (IsScalarType(type))*/ {
				return 1;
			}
		}
		size_t GetNumberOfCols(TokenType type, OperandPos pos) {
			if (IsTypeMatrix(type)) {
				return GetMatNumberOfCols(type);
			} else if (IsTypeVector(type)) {
				return GetVecNumberOfCols(type, pos);
			} else /*if (IsScalarType(type))*/ {
				return 1;
			}
		}

		size_t GetVecNumberOfRows(TokenType type, OperandPos pos) {
			// LHS vectors are treated as row vectors and
			// RHS vectors as column vectors.
			if (pos == OperandPos::LHS) {
				return GetRowVecNumberOfRows(type);
			} else /*(pos == OperandPos::RHS)*/ {
				return GetColVecNumberOfRows(type);
			}
		}
		size_t GetVecNumberOfCols(TokenType type, OperandPos pos) {
			// LHS vectors are treated as row vectors and
			// RHS vectors as column vectors.
			if (pos == OperandPos::LHS) {
				return GetRowVecNumberOfCols(type);
			} else /*(pos == OperandPos::RHS)*/ {
				return GetColVecNumberOfCols(type);
			}
		}

		// Column vectors are used by default.
		// The two maps below were created from the point of view of column vectors.
		static constexpr int vecRowsColsOffset = static_cast<int>(TokenType::BVEC2);
		static constexpr size_t colVecRowsTypeMap[] = {
			2, 2, 2, 2, 2, // BVEC2, IVEC2, UVEC2, VEC2, DVEC2
			3, 3, 3, 3, 3, // BVEC3, IVEC3, UVEC3, VEC3, DVEC3
			4, 4, 4, 4, 4, // BVEC4, IVEC4, UVEC4, VEC4, DVEC4
		};
		static constexpr size_t colVecColsTypeMap[] = {
			1, 1, 1, 1, 1, // BVEC2, IVEC2, UVEC2, VEC2, DVEC2
			1, 1, 1, 1, 1, // BVEC3, IVEC3, UVEC3, VEC3, DVEC3
			1, 1, 1, 1, 1, // BVEC4, IVEC4, UVEC4, VEC4, DVEC4
		};
		size_t GetColVecNumberOfRows(TokenType type) {
			assert(IsTypeVector(type) && "Type must be a vector type!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return colVecRowsTypeMap[idx];
		}
		size_t GetColVecNumberOfCols(TokenType type) {
			assert(IsTypeVector(type) && "Type must be vector type!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return colVecColsTypeMap[idx];
		}
		size_t GetRowVecNumberOfRows(TokenType type) {
			assert(IsTypeVector(type) && "Type must be a vector type!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return colVecColsTypeMap[idx];
		}
		size_t GetRowVecNumberOfCols(TokenType type) {
			assert(IsTypeVector(type) && "Type must be a vector type!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return colVecRowsTypeMap[idx];
		}

		static constexpr int matRowsColsOffset = static_cast<int>(TokenType::MAT2X2);
		static constexpr size_t matRowsTypeMap[] = {
			2, 2, 2, 2, 2, 2, // MAT2X2, DMAT2X2, MAT2X3, DMAT2X3, MAT2X4, DMAT2X4
			3, 3, 3, 3, 3, 3, // MAT3X2, DMAT3X2, MAT3X3, DMAT3X3, MAT3X4, DMAT3X4
			4, 4, 4, 4, 4, 4, // MAT4X2, DMAT4X2, MAT4X3, DMAT4X3, MAT4X4, DMAT4X4
		};
		static constexpr size_t matColsTypeMap[] = {
			2, 2, 3, 3, 4, 4, // MAT2X2, DMAT2X2, MAT2X3, DMAT2X3, MAT2X4, DMAT2X4
			2, 2, 3, 3, 4, 4, // MAT3X2, DMAT3X2, MAT3X3, DMAT3X3, MAT3X4, DMAT3X4
			2, 2, 3, 3, 4, 4, // MAT4X2, DMAT4X2, MAT4X3, DMAT4X3, MAT4X4, DMAT4X4
		};
		size_t GetMatNumberOfRows(TokenType type) {
			assert(IsTypeMatrix(type) && "Type must be a matrix type!");
			int idx = static_cast<int>(type) - matRowsColsOffset;
			return matRowsTypeMap[idx];
		}
		size_t GetMatNumberOfCols(TokenType type) {
			assert(IsTypeMatrix(type) && "Type must be a matrix type!");
			int idx = static_cast<int>(type) - matRowsColsOffset;
			return matColsTypeMap[idx];
		}

		static constexpr int fundamentalTypeToVectorTypeOffset = 2;
		static constexpr TokenType fundamentalTypeToVectorType[][3] = {
			{TokenType::BVEC2, TokenType::BVEC3, TokenType::BVEC4}, // BOOL
			{TokenType::IVEC2, TokenType::IVEC3, TokenType::IVEC4}, // INT
			{TokenType::UVEC2, TokenType::UVEC3, TokenType::UVEC4}, // UINT
			{TokenType::VEC2,  TokenType::VEC3,  TokenType::VEC4 }, // FLOAT
			{TokenType::DVEC2, TokenType::DVEC3, TokenType::DVEC4}, // DOUBLE
		};
		TokenType FundamentalTypeToVectorType(TokenType type, size_t dimension) {
			assert(IsTypeFundamental(type) && "Type must be a fundamental type!");
			int idx = static_cast<int>(dimension) - fundamentalTypeToVectorTypeOffset;
			return fundamentalTypeToVectorType[static_cast<int>(type)][idx];
		}
		TokenType FundamentalTypeToMatrixType(TokenType type, size_t rows, size_t cols) {
			assert(IsTypeFundamental(type) && "Type must be a fundamental type!");
			return GetTypeRowsCols(type, rows, cols);
		}

		TokenType PromoteType(TokenType type, int rankDiff) {
#if defined (_DEBUG) || defined(DEBUG)
			if (IsTypeScalar(type)) return PromoteScalarType(type, rankDiff);
			if (IsTypeVector(type)) return PromoteVectorType(type, rankDiff);
			if (IsTypeMatrix(type)) return PromoteMatrixType(type, rankDiff);
#endif
			return static_cast<TokenType>(static_cast<int>(type) + rankDiff);
		}
#if defined (_DEBUG) || defined(DEBUG)
		TokenType PromoteFundamentalType(TokenType type, int rankDiff) {
			// 3 is because to go from INT to DOUBLE we must increase the rank of INT by 3.
			// -3 is allowed in case we ever need to consider demoting types.
			assert((rankDiff >= -3 && rankDiff <= 3) &&
			        "Rank promotion number must be within the [-3, 3] range!");
			assert(IsTypeFundamental(type) && "Type must be fundamental!");
			assert(type != TokenType::BOOL && "Type must not be BOOL!"); // can't promote bool to anything.
			TokenType promotedType = static_cast<TokenType>(static_cast<int>(type) + rankDiff);
			assert(IsTypeFundamental(promotedType) && "Promoted type must stay fundamental!");
			return promotedType;
		}
		TokenType PromoteScalarType(TokenType type, int rankDiff) {
			return PromoteFundamentalType(type, rankDiff);
		}
		TokenType PromoteVectorType(TokenType type, int rankDiff) {
			// 3 is because to go from INT to DOUBLE we must increase the rank of INT by 3.
			// -3 is allowed in case we ever need to consider demoting types.
			assert((rankDiff >= -3 && rankDiff <= 3) &&
			        "Rank promotion number must be within the [-3, 3] range!");
			assert(IsTypeVector(type) && "Type must be a vector type!");
			assert(type != TokenType::BOOL && "Type must not be BOOL!"); // can't promote bool to anything.
			TokenType promotedType = static_cast<TokenType>(static_cast<int>(type) + rankDiff);
			assert(IsTypeVector(promotedType) && "Promoted type must stay a vector type!");
			return promotedType;
		}
		TokenType PromoteMatrixType(TokenType type, int rankDiff) {
			// 1 is because to go from FLOAT to DOUBLE we must increase the rank of FLOAT by 1.
			// -1 is allowed in case we ever need to consider demoting DOUBLE to FLOAT.
			assert((rankDiff >= -1 && rankDiff <= 1) &&
			        "Rank promotion number must be within the [-3, 3] range!");
			assert(IsTypeMatrix(type) && "Type must be one of the matrix types!");
			TokenType promotedType = static_cast<TokenType>(static_cast<int>(type) + rankDiff);
			assert(IsTypeMatrix(promotedType) && "Promoted type must stay a matrix type!");
			return promotedType;
		}
#endif

		bool AddSubDivAllowed(TokenType lhs, TokenType rhs) {
			if (IsTypeScalar(lhs) || IsTypeScalar(rhs)) {
				// 1) Both operands are scalars, or
				// 2) at least one of them is.
				// In both cases all arithmetic binary operations are allowed.
				return true;
			} else {
				// Both operands are non-scalar objects. (Matrices, vectors, or a mix of both)
				// The +, -, / arithmetic binary operations are allowed if
				// 1) Both operands are vectors of the same size.
				// 2) Both operands are matrices of the same size.
				// When one of the operands is a vector and the other one is a matrix, the operations are not allowed.
				if (IsTypeVector(lhs) && IsTypeVector(rhs)) {
					size_t lhsVecSize = GetColVecNumberOfRows(lhs);
					size_t rhsVecSize = GetColVecNumberOfRows(rhs);
					return lhsVecSize == rhsVecSize;
				} else if (IsTypeMatrix(lhs) && IsTypeMatrix(rhs)) {
					size_t rowsLhs = GetMatNumberOfRows(lhs);
					size_t colsLhs = GetMatNumberOfCols(lhs);
					size_t rowsRhs = GetMatNumberOfRows(rhs);
					size_t colsRhs = GetMatNumberOfCols(rhs);
					return rowsLhs == rowsRhs && colsLhs == colsRhs;
				} else {
					// One operand is a vector and the other one is a matrix.
					return false;
				}
			}
			return false;
		}
		bool AdditionAllowed(TokenType lhs, TokenType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}
		bool SubtractionAllowed(TokenType lhs, TokenType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}
		bool MultiplicationAllowed(TokenType lhs, TokenType rhs) {
			if (IsTypeScalar(lhs) || IsTypeScalar(rhs)) {
				// 1) Both operands are scalars, or
				// 2) at least one of them is.
				// In both cases multiplication is allowed.
				return true;
			} else {
				// Both operands are non-scalar objects.
				// The * arithmetic binary operations is allowed if and only if
				// The number of columns of the left operand is equal to the number of rows of the right operand.
				size_t colsLhs = GetNumberOfCols(lhs, OperandPos::LHS);
				size_t rowsRhs = GetNumberOfRows(rhs, OperandPos::RHS);
				return colsLhs == rowsRhs;
			}
			return false;
		}
		bool DivisionAllowed(TokenType lhs, TokenType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}

		TokenType InferExprType(TokenType lhs, TokenType rhs, TokenType op) {
			// First of all, we need to figure out what type of operation is applied.
			if (op == TokenType::PLUS || op == TokenType::DASH ||
				op == TokenType::STAR || op == TokenType::SLASH) {
				// 1. Arithmetic binary operation.
				return InferArithmeticBinaryExprType(lhs, rhs, op);
			}
			// TODO: implement other types of operations.
			return TokenType::UNDEFINED;
		}
		TokenType InferArithmeticBinaryExprType(TokenType lhs, TokenType rhs, TokenType op) {
			// Following the specification's explanation on p.123
			// Link: https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.pdf
			// If any of the types' fundamental type is BOOL, the result is UNDEFINED, because according to the specification
			// only integer and floating-point scalars, vectors, and matrices are allowed with the arithmetic binary operators.

			// First, we handle cases when one of the types is UNDEFINED.
			// This can happen when we call the function recursively (either directly or indirectly).
			if (lhs == TokenType::UNDEFINED || rhs == TokenType::UNDEFINED)
				return TokenType::UNDEFINED;

			TokenType lhsFundType = GetFundamentalType(lhs);
			TokenType rhsFundType = GetFundamentalType(rhs);
			if (lhsFundType == TokenType::BOOL || rhsFundType == TokenType::BOOL) {
				return TokenType::UNDEFINED;
			}
			if (IsTypeScalar(lhs) && IsTypeScalar(rhs)) {
				// 1. Both operands are scalars.
				// The operation is applied resulting in a scalar of the "biggest" type.
				// The "biggest" type means the type that has a higher rank.
				// The way the types are listed follows the implicit conversion rules outlined in the specification.
				// INT -> UINT -> FLOAT -> DOUBLE, where UINT is "bigger" than INT, for example,
				// and DOUBLE is the "biggest" type of all the fundamental types.
				// If both types are the same, the result will be of that type.
				size_t typeId = std::max(static_cast<size_t>(lhs), static_cast<size_t>(rhs));
				return static_cast<TokenType>(typeId);
			}
			if (IsTypeScalar(lhs) || IsTypeScalar(rhs)) {
				// 2. One of the types is a scalar while the other is either a vector or a matrix.
				// According to the specification the operation is applied to each component
				// of the non-scalar operand resulting in the same size vector or matrix of the bigger type.
				// So if a scalar is a "double" and a vector/matrix is "float", the result is going to be
				// a vector/matrix of the "double" type.
				// First we need to figure out which operand is a scalar and which one is a vector/matrix.
				if (IsTypeScalar(lhs)) {
					// Left-hand operand is a scalar.
					if (IsTypeVector(rhs))
						return InferScalarOpVectorExprType(lhs, rhs, op);
					else if (IsTypeMatrix(rhs))
						return InferScalarOpMatrixExprType(lhs, rhs, op);
					else
						return TokenType::UNDEFINED;
				} else /*if (IsTypeScalar(rhs))*/ {
					// Right-hand side operand is a scalar.
					if (IsTypeVector(lhs))
						return InferVectorOpScalarExprType(lhs, rhs, op);
					else if (IsTypeMatrix(lhs))
						return InferMatrixOpScalarExprType(lhs, rhs, op);
					else
						return TokenType::UNDEFINED;
				}
			}
			// At this point we know that both operands are non-scalars.
			if (IsTypeVector(lhs) && IsTypeVector(rhs)) {
				return InferVectorOpVectorExprType(lhs, rhs, op);
			}
			// One of the operands is a vector while the other is a matrix,
			// or both operands are matrices.
			if (IsTypeVector(lhs) && IsTypeMatrix(rhs))
				return InferVectorOpMatrixExprType(lhs, rhs, op);
			else if (IsTypeMatrix(lhs) && IsTypeVector(rhs))
				return InferMatrixOpVectorExprType(lhs, rhs, op);
			else if (IsTypeMatrix(lhs) && IsTypeMatrix(rhs))
				return InferMatrixOpMatrixExprType(lhs, rhs, op);
			else
				return TokenType::UNDEFINED;
		}
		TokenType InferScalarOpScalarExprType(TokenType lhs, TokenType rhs, TokenType op) {
			// 1. Both operands are scalars.
			// The operation is applied resulting in a scalar of the "biggest" type.
			// The "biggest" type means the type that has a higher rank.
			// The way the types are listed follows the implicit conversion rules outlined in the specification.
			// INT -> UINT -> FLOAT -> DOUBLE, where UINT is "bigger" than INT, for example,
			// and DOUBLE is the "biggest" type of all the fundamental types.
			// If both types are the same, the result will be of that type.
			size_t typeId = std::max(static_cast<size_t>(lhs), static_cast<size_t>(rhs));
			return static_cast<TokenType>(typeId);
		}
		TokenType InferScalarOpVectorExprType(TokenType lhs, TokenType rhs, TokenType op) {
			// Left-hand operand is a scalar.
			// The specification states that we must first match the fundamental types of both operands.
			// Or, in other words, promote the "smaller" type to the "bigger" one. (i.e., INT -> FLOAT).
			// This means that we also need to figure out the fundamental type of the vector
			// and then either promote its type to the type of the left operand or use its original type as is.
			int rankDiff = GetFundamentalTypeRank(lhs) - GetFundamentalTypeRank(rhs);
			if (rankDiff > 0) {
				// The lhs operand's type is "bigger", so the type of the vector must be promoted.
				// i.e., FLOAT * IVEC3 -> VEC3, because
				// the fundamental type of the vector is smaller than the fundamental type of the scalar.
				return PromoteType(rhs, rankDiff);
			}
			// rankDiff == 0 means the types are the same.
			return rhs;
		}
		TokenType InferVectorOpScalarExprType(TokenType lhs, TokenType rhs, TokenType op) {
			// Same algorithm as above but for the right-hand side operand.
			int rankDiff = GetFundamentalTypeRank(rhs) - GetFundamentalTypeRank(lhs);
			if (rankDiff > 0) {
				return PromoteType(lhs, rankDiff);
			}
			return lhs;
		}
		TokenType InferScalarOpMatrixExprType(TokenType lhs, TokenType rhs, TokenType op) {
			// Left-hand operand is a scalar.
			// The specification states that we must first match the fundamental types of both operands.
			// Or, in other words, promote the "smaller" type to the "bigger" one. (i.e., INT -> FLOAT).
			// This means that we also need to figure out the fundamental type of the matrix
			// and then either promote its type to the type of the left operand or use its original type as is.
			int rankDiff = GetFundamentalTypeRank(lhs) - GetFundamentalTypeRank(rhs);
			if (rankDiff > 0) {
				// The lhs operand's type is "bigger", so the type of the matrix must be promoted.
				// i.e., DOUBLE * MAT3 -> DMAT3, because
				// the fundamental type of the amtrix is smaller than the fundamental type of the scalar.
				return PromoteType(rhs, rankDiff);
			}
			// rankDiff == 0 means the types are the same.
			return rhs;
		}
		TokenType InferMatrixOpScalarExprType(TokenType lhs, TokenType rhs, TokenType op) {
			// Same algorithm as above but for the right-hand side operand.
			int rankDiff = GetFundamentalTypeRank(rhs) - GetFundamentalTypeRank(lhs);
			if (rankDiff > 0) {
				return PromoteType(lhs, rankDiff);
			}
			return lhs;
		}
		TokenType InferVectorOpVectorExprType(TokenType lhs, TokenType rhs, TokenType op) {
			// The two operands are vectors and they must be of the same size.
			// The operation is done component-wise resulting in a vector of the same size.
			// The types are promoted if necessary (i.e., IVEC2 + VEC2 -> VEC2).
			// We can check these requirements in one of the two ways:
			// 1) Check the number of rows of both operands assuming they're column vectors, or
			// 2) Check the number of columns of the operands assuming they're row vectors.
			// We're going to use the 1st since vectors are assumed to be column vectors by default.
			// (in GLSL at least (SPIR-V too?))
			size_t rowsLhs = GetColVecNumberOfRows(lhs);
			size_t rowsRhs = GetColVecNumberOfRows(rhs);
			if (rowsLhs == rowsRhs) {
				// Return the "bigger" vector type.
				// We can check the types either by using the ranks of the operands' fundamental types, or
				// by simply comparing the operands' original types.
				// The latter works, because of the way the type enumeration was set up.
				// i.e., IVEC2 / VEC2 -> VEC2
				// Alternative way:
				/*
				int rankDiff = GetFundamentalTypeRank(lhs) - GetFundamentalTypeRank(rhs);
				if (rankDiff > 0) {
					// return PromoteType(rhs, rankDiff);
					return lhs;
				}
				return rhs;
				*/
				// IVECN, UVECN, VECN, or DVECN
				// Vector types are set up in such a way so as to allow the following simple comparisons
				// to figure out whether a type promotion is needed or not:
				if (lhs > rhs) return lhs;
				else return rhs; // if rhs is > lhs or equal to it
			} else {
				return TokenType::UNDEFINED;
			}
		}
		TokenType InferVectorOpMatrixExprType(TokenType lhs, TokenType rhs, TokenType op) {
			if (op != TokenType::STAR)
				return TokenType::UNDEFINED;
			// 1) Multiplication operation.
			// Multiplications is handled according to the rules of linear algebra.
			// Here, the left-hand side operand is a vector, so it's considered to be a row vector.
			// The number of its columns must match the number of rows of the right-hand matrix.
			size_t vecColsLhs = GetRowVecNumberOfCols(lhs);
			size_t matRowsRhs = GetMatNumberOfRows(rhs);
			size_t matColsRhs = GetMatNumberOfCols(rhs);
			if (vecColsLhs == matRowsRhs) {
				// Multiplication is allowed.
				// Now we need to figure out what type should be used for the result of the multiplication.
				// Since one of the operands (rhs) is a matrix, the fundamental type of the result
				// will be at least FLOAT (matrices can only be either FLOAT or DOUBLE).
				TokenType fundamentalTypeLhs = GetFundamentalType(lhs);
				TokenType fundamentalTypeRhs = GetFundamentalType(rhs);
				// Should be either FLOAT or DOUBLE.
				TokenType resFundamentalType = std::max(fundamentalTypeLhs, fundamentalTypeRhs);
				TokenType resInferredType = GetTypeRowsCols(1, matColsRhs);
				return resInferredType;
			} else {
				return TokenType::UNDEFINED;
			}
		}
		TokenType InferMatrixOpVectorExprType(TokenType lhs, TokenType rhs, TokenType op) {
			// Same as above, but now the vector (rhs) is a column vector.
			// Everything else should be more or less the same.
			if (op != TokenType::STAR)
				return TokenType::UNDEFINED;
			// 1) Multiplication operation.
			// Multiplications is handled according to the rules of linear algebra.
			// Here, the right-hand side operand is a vector, so it's considered to be a column vector.
			// The number of its rows must match the number of columns of the left-hand matrix.
			size_t matRowsLhs = GetMatNumberOfRows(lhs);
			size_t matColsLhs = GetMatNumberOfCols(lhs);
			size_t vecRowsRhs = GetColVecNumberOfRows(rhs);
			if (matColsLhs == vecRowsRhs) {
				// Multiplication is allowed.
				// Now we need to figure out what type should be used for the result of the multiplication.
				// Since one of the operands (lhs) is a matrix, the fundamental type of the result
				// will be at least FLOAT (matrices can only be either FLOAT or DOUBLE).
				TokenType fundamentalTypeLhs = GetFundamentalType(lhs);
				TokenType fundamentalTypeRhs = GetFundamentalType(rhs);
				// Should be either FLOAT or DOUBLE.
				TokenType resFundamentalType = std::max(fundamentalTypeLhs, fundamentalTypeRhs);
				TokenType resInferredType = GetTypeRowsCols(matRowsLhs, 1);
				return resInferredType;
			} else {
				return TokenType::UNDEFINED;
			}
		}
		TokenType InferMatrixOpMatrixExprType(TokenType lhs, TokenType rhs, TokenType op) {
			size_t rowsLhs = GetMatNumberOfRows(lhs);
			size_t colsLhs = GetMatNumberOfCols(lhs);
			size_t rowsRhs = GetMatNumberOfRows(rhs);
			size_t colsRhs = GetMatNumberOfCols(rhs);
			if (op == TokenType::STAR) {
				// 1) Multiplication operation.
				// Multiplications is handled according to the rules of linear algebra
				// Both operands here are matrices.
				if (colsLhs == rowsRhs) {
					// Multiplication is allowed.
					// Now we need to figure out what type should be used for the result of the multiplication.
					// Since at least one of the operands is a matrix, the fundamental type of the result
					// will be at least FLOAT.
					TokenType fundamentalTypeLhs = GetFundamentalType(lhs);
					TokenType fundamentalTypeRhs = GetFundamentalType(rhs);
					// Should be either FLOAT or DOUBLE.
					TokenType resFundamentalType = std::max(fundamentalTypeLhs, fundamentalTypeRhs);
					TokenType resInferredType = GetTypeRowsCols(rowsLhs, colsRhs);
					return resInferredType;
				} else {
					return TokenType::UNDEFINED;
				}
			} else {
				// 2) Otherwise we're dealing with either addition, subtraction, or devision operation.
				// In any case we need both operands' dimensions to match.
				if (rowsLhs == rowsRhs && colsLhs == colsRhs) {
					// Alternative way:
					/*
					int rankDiff = GetFundamentalTypeRank(lhs) - GetFundamentalTypeRank(rhs);
					if (rankDiff > 0) {
						// return PromoteType(rhs, rankDiff);
						return lhs;
					}
					return rhs;
					*/
					// MATNXM or DMATNXM
					// Types are set up in such a way so as to allow to perform the following
					// simple comparisons to figure out whether a type promotion is needed or not.
					if (lhs > rhs) return lhs;
					else return rhs;
				} else {
					return TokenType::UNDEFINED;
				}
			}
		}

		bool ArrayDim::IsValid() const {
			return dimSize != 0;
		}
		bool ArrayDim::IsImplicit() const {
			if (!dimExpr) return true;
			return false;
		}

		bool TypeQual::Const() const {
			return storage.has_value() &&
				   storage.value().tokenType == TokenType::CONST;
		}
		bool TypeQual::Empty() const {
			return layout.empty() &&
				   !storage.has_value() &&
				   !precision.has_value() &&
				   !interpolation.has_value() &&
				   !invariant.has_value() &&
				   !precise.has_value();
		}
	
		bool TypeSpec::IsBasic() const {
			return IsTypeBasic(type.tokenType);
		}
		bool TypeSpec::IsTransparent() const {
			return IsTypeTransparent(type.tokenType);
		}
		bool TypeSpec::IsOpaque() const {
			return IsTypeOpaque(type.tokenType);
		}
		bool TypeSpec::IsStructure() const {
			return type.tokenType == TokenType::IDENTIFIER || typeDecl;
		}
		bool TypeSpec::IsStructureNamed() const {
			return type.tokenType == TokenType::IDENTIFIER && !typeDecl;
		}
		bool TypeSpec::IsStructureAnonymous() const {
			return type.tokenType != TokenType::IDENTIFIER && typeDecl;
		}
		bool TypeSpec::IsScalar() const {
			return IsTypeScalar(type.tokenType) && dimensions.empty();
		}
		bool TypeSpec::IsVector() const {
			return IsTypeVector(type.tokenType);
		}
		bool TypeSpec::IsMatrix() const {
			return IsTypeMatrix(type.tokenType);
		}
		bool TypeSpec::IsTexture() const {
			return IsTypeOpaque(type.tokenType);
		}
		bool TypeSpec::IsArray() const {
			return !dimensions.empty();
		}
		bool TypeSpec::IsAggregate() const {
			return IsStructure() || IsArray();
		}

		bool operator==(const TypeSpec& type1, const TypeSpec& type2) {
			bool typesEqual{false};
			// 1. First, let's deal with custom types.
			if (!IsTypeBasic(type1.type.tokenType) && !IsTypeBasic(type2.type.tokenType)) {
				// 1.1 Both types are custom types.
				if (type1.typeDecl && type2.typeDecl) {
					// If the types are anonymous structure types, their "typeDecl" fields
					// must point to the same object.
					typesEqual = type1.typeDecl == type2.typeDecl;
				} else {
					// If the types are named, then they are equal iff their type names are the same.
					typesEqual = type1.type.lexeme == type2.type.lexeme;
				}
			}
			// 2. One of the types is a custom type, or
			// 3. both types are basic types.
			// In any case, it's enough to check their token types.
			// (IDENTIFIER != to any of the type tokens, i.e. IDENTIFIER != VEC3, etc.)
			typesEqual = type1.type.tokenType == type2.type.tokenType;
			// 4. Check if the types are arrays.
			// If so, check the number of dimensions and their sizes, both must be equal.
			// It's enough to check whether one of the types is an array type.
			if (type1.IsArray() || type2.IsArray()) {
				if (type1.dimensions.size() == type2.dimensions.size()) {
					// Both types are arrays and their number of dimensions match.
					for (size_t i = 0; i < type1.dimensions.size(); i++) {
						if (type1.dimensions[i].dimSize != type2.dimensions[i].dimSize) {
							// The sizes of the same dimension differ in both types,
							// which means the types are not equal.
							typesEqual = false;
						}
					}
				} else {
					// Dimensions are different, the types are not equal.
					typesEqual = false;
				}
			}
			return typesEqual;
		}
		bool IsTypePromotable(const TypeSpec& check, const TypeSpec& promoteTo) {
			// 1. If the types are equal, we consider they're promotable.
			//    Think of it as a sort of a unit promotion, where any type can be promoted to itself
			//    via this "unit" promotion, so to speak.
			if (check == promoteTo)
				return true;
			// 2. If any of the types is opaque, we can't promote from or to such a type.
			if (check.IsOpaque() || promoteTo.IsOpaque())
				return false;
			// 3. If both types are transparent:
			if (check.IsTransparent() && promoteTo.IsTransparent()) {
				// First, we check what fundamental type both types have.
				TokenType checkFundType = GetFundamentalType(check.type.tokenType);
				TokenType promoteToFundType = GetFundamentalType(promoteTo.type.tokenType);
				// If any of the fundamental types is VOID, we can't promote that to or from any other type.
				// VOID is the fundamental type of itself only, so this case can only happen when we
				// "check" or try to "promote to" VOID.
				if (checkFundType == TokenType::VOID || promoteToFundType == TokenType::VOID)
					return false;
				// Similarly to the previous point, if any of the fundamental types is BOOL,
				// we can't promote that to or from any other type.
				if (checkFundType == TokenType::BOOL || promoteToFundType == TokenType::BOOL)
					return false;
				// Now we know we're dealing with either scalars, vectors, or matrices (or a mix of them).
				// First we check if the fundamental type is promotable or not, because if it's not
				// we don't care whether the types are scalars, vectors, or matrices,
				// because we still wouldn't be able to promote that.
				int checkRank = GetFundamentalTypeRank(check.type.tokenType);
				int promoteToRank = GetFundamentalTypeRank(promoteTo.type.tokenType);
				if (checkRank > promoteToRank)
					return false;
				if (IsTypeVector(check.type.tokenType) && IsTypeVector(promoteTo.type.tokenType)) {
					// Both types are vector types.
					// All we need to check for them is whether their number of components match.
					// If they don't, the types are not promotable.
					if (GetColVecNumberOfRows(check.type.tokenType) !=
					    GetColVecNumberOfRows(promoteTo.type.tokenType))
						return false;
				} else if (IsTypeMatrix(check.type.tokenType) && IsTypeMatrix(promoteTo.type.tokenType)) {
					// Both types are matrix types.
					// For matrix types to be promotable their number of rows and columns should match.
					// Otherwise they are not promotable.
					bool rowsMatch = GetMatNumberOfRows(check.type.tokenType) ==
					                 GetMatNumberOfRows(promoteTo.type.tokenType);
					bool colsMatch = GetMatNumberOfCols(check.type.tokenType) ==
						             GetMatNumberOfCols(promoteTo.type.tokenType);
					if (!rowsMatch || !colsMatch)
						return false;
				} else {
					// For scalars, the rank check is enough to see if we can promote a type, however, here
					// we also need to account for a possibility of the types' categories being different.
					// By type categories we mean a type being a scalar, a vector, or a matrix.
					// If the check below is 'true', the the types' categories don't match, which is not promotable.
					if (!IsTypeScalar(check.type.tokenType) && !IsTypeScalar(promoteTo.type.tokenType))
						// Types are "mixed", which is not promotable.
						return false;
				}
			} else if (check.IsStructure() && promoteTo.IsStructure()) {
				// Both types are user-defined structures.
				// User-defined types cannot be implicitly converted to any other type.
				// Structures can be named or anonymous.
				// In the case of anonymous structures we check if they point to the same type declaration.
				// And in the case of named structure we just simply check if they have the same names.
				if (check.IsStructureNamed() && promoteTo.IsStructureNamed()) {
					if (check.type.lexeme != promoteTo.type.lexeme)
						return false;
				} else if (check.IsStructureAnonymous() && promoteTo.IsStructureAnonymous()) {
					if (check.typeDecl != promoteTo.typeDecl)
						return false;
				} else {
					// One of the structures is named and the other is anonymous.
					// This is not promotable.
					return false;
				}
			} else {
				// 4. Types' categories don't match,
				//    can't promote a transparent type to a custom one, or the other way around.
				return false;
			}
			// Types can be arrays, so we must check if the number of dimensions and their sizes match.
			if (check.IsArray() || promoteTo.IsArray()) {
				// 5. Any of the types is an array. The reason we don't check if both types
				//    are arrays is because when one is an array and the other one is not,
				//    the non-array type's dimension count is 0 which is not going to be equal to the
				//    dimension count of the array type by definition.
				if (check.dimensions.size() != promoteTo.dimensions.size())
					return false;
				for (size_t i = 0; i < check.dimensions.size(); i++) {
					if (check.dimensions[i].dimSize != promoteTo.dimensions[i].dimSize) {
						return false;
					}
				}
			}
			// All checks were successful, so the 'check' type can be promoted to the 'promoteTo' type.
			return true;
		}
		TypeSpec PromoteType(const TypeSpec& what, const TypeSpec& promoteTo) {
			// First we check if the type "what" can be promoted to the type "promoteTo".
			if (!IsTypePromotable(what, promoteTo))
				return TypeSpec(); // empty lexeme and UNDEFINED type token.
			// If type promotion is allowed, then we just simply return the "promoteTo" type, right?
			return promoteTo;
		}

		TypeSpec InferArithmeticBinaryExprType(const TypeSpec& lhs, const TypeSpec& rhs, TokenType op) {
			if (lhs.IsStructure() || rhs.IsStructure())
				return TypeSpec();
			if (lhs.IsOpaque() || rhs.IsOpaque())
				return TypeSpec();
			if (lhs.IsArray() || rhs.IsArray())
				return TypeSpec();

			TokenType resTokType = InferArithmeticBinaryExprType(lhs.type.tokenType,
				                                                 rhs.type.tokenType,
																 op);
			TypeSpec resType{};
			resType.type = GenerateToken(resTokType);
			return resType;
		}

		std::string MangleTypeSpecName(const TypeSpec& typeSpec) {
			std::stringstream nameMangler;
			nameMangler << typeSpec.type.lexeme;
			if (typeSpec.IsArray()) {
				for (size_t i = 0; i < typeSpec.dimensions.size(); i++) {
					nameMangler << "[";
					if (typeSpec.dimensions[i].IsValid()) {
						nameMangler << typeSpec.dimensions[i].dimSize;
					}
					nameMangler << "]";
				}
			}
			return nameMangler.str();
		}

		TypeTable::TypeTable() {
			// This way, all expressions with a type id of 0 will have
			// the "unknown" type, whose type token is UNDEFINED,
			// which is guaranteed to fail any type check.
			types.push_back(TypeSpec());
			typeMap["unknown"] = 0;
		}

		const TypeSpec& TypeTable::GetType(size_t idx) {
			assert(idx < types.size() && "Type index is out of bounds!");
			return types[idx];
		}
		const TypeSpec& TypeTable::GetType(const std::string& typeName) {
			auto searchRes = typeMap.find(typeName);
			assert(searchRes != typeMap.end() && "Check if the type exists first!");
			return GetType(searchRes->second);
		}

		bool TypeTable::HasType(const TypeSpec& type) {
			std::string mangledTypeName = MangleTypeSpecName(type);
			return HasType(mangledTypeName);
		}
		bool TypeTable::HasType(const std::string& typeName) {
			auto searchRes = typeMap.find(typeName);
			return searchRes != typeMap.end();
		}

		size_t TypeTable::AddType(const TypeSpec& type) {
			std::string mangledTypeName = MangleTypeSpecName(type);
			assert(!HasType(mangledTypeName) && "Type already exists!");
			size_t idx = types.size();
			types.push_back(type);
			typeMap[mangledTypeName] = idx;
			return idx;
		}

		size_t TypeTable::GetTypeId(const TypeSpec& type) {
			std::string mangledTypeName = MangleTypeSpecName(type);
			auto searchRes = typeMap.find(mangledTypeName);
			if (searchRes == typeMap.end()) {
				return AddType(type);
			}
			return searchRes->second;
		}

	}
}