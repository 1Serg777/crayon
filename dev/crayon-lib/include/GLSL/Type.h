#pragma once

#include "GLSL/Token.h"

#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace crayon {
	namespace glsl {

		class StructDecl;
		class Expr;

		enum class OperandPos {
			LHS, RHS
		};

		bool IsTypeBasic(TokenType tokenType);
		bool IsTypeFundamental(TokenType tokenType);
		bool IsTypeScalar(TokenType tokenType);
		bool IsTypeVector(TokenType tokenType);
		bool IsTypeMatrix(TokenType tokenType);
		bool IsTypeTransparent(TokenType tokenType);
		bool IsTypeOpaque(TokenType tokenType);

		TokenType GetAliasType(std::string_view alias);

		bool IsMaterialPropertyType(TokenType tokenType);
		TokenType MapMaterialPropertyType(TokenType tokenType);

		int GetFundamentalTypeRank(TokenType type);
		TokenType GetFundamentalType(TokenType type);

		size_t GetDimensionCountNonArray(TokenType type);

		TokenType GetTypeRowsCols(size_t rows, size_t cols);
		TokenType GetTypeRowsCols(TokenType fundamentalType, size_t rows, size_t cols);

		size_t GetNumberOfRows(TokenType type, OperandPos pos);
		size_t GetNumberOfCols(TokenType type, OperandPos pos);

		size_t GetVecNumberOfRows(TokenType type, OperandPos pos);
		size_t GetVecNumberOfCols(TokenType type, OperandPos pos);

		size_t GetColVecNumberOfRows(TokenType type);
		size_t GetColVecNumberOfCols(TokenType type);
		size_t GetRowVecNumberOfRows(TokenType type);
		size_t GetRowVecNumberOfCols(TokenType type);

		size_t GetMatNumberOfRows(TokenType type);
		size_t GetMatNumberOfCols(TokenType type);

		TokenType FundamentalTypeToVectorType(TokenType tokenType, size_t dimension);
		TokenType FundamentalTypeToMatrixType(TokenType tokenType, size_t rows, size_t cols);

		TokenType PromoteType(TokenType type, int rankDiff);
#if defined (_DEBUG) || defined(DEBUG)
		TokenType PromoteFundamentalType(TokenType type, int rankDiff);
		TokenType PromoteScalarType(TokenType type, int rankDiff);
		TokenType PromoteVectorType(TokenType type, int rankDiff);
		TokenType PromoteMatrixType(TokenType type, int rankDiff);
#endif

		bool AddSubDivAllowed(TokenType lhs, TokenType rhs);
		bool AdditionAllowed(TokenType lhs, TokenType rhs);
		bool SubtractionAllowed(TokenType lhs, TokenType rhs);
		bool MultiplicationAllowed(TokenType lhs, TokenType rhs);
		bool DivisionAllowed(TokenType lhs, TokenType rhs);

		TokenType InferExprType(TokenType lhs, TokenType rhs, TokenType op);
		TokenType InferArithmeticBinaryExprType(TokenType lhs, TokenType rhs, TokenType op);
		TokenType InferScalarOpScalarExprType(TokenType lhs, TokenType rhs, TokenType op);
		TokenType InferScalarOpVectorExprType(TokenType lhs, TokenType rhs, TokenType op);
		TokenType InferVectorOpScalarExprType(TokenType lhs, TokenType rhs, TokenType op);
		TokenType InferScalarOpMatrixExprType(TokenType lhs, TokenType rhs, TokenType op);
		TokenType InferMatrixOpScalarExprType(TokenType lhs, TokenType rhs, TokenType op);
		TokenType InferVectorOpVectorExprType(TokenType lhs, TokenType rhs, TokenType op);
		TokenType InferVectorOpMatrixExprType(TokenType lhs, TokenType rhs, TokenType op);
		TokenType InferMatrixOpVectorExprType(TokenType lhs, TokenType rhs, TokenType op);
		TokenType InferMatrixOpMatrixExprType(TokenType lhs, TokenType rhs, TokenType op);

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

		struct ArrayDim {
			bool IsValid() const;
			bool IsImplicit() const;

			std::shared_ptr<Expr> dimExpr;
			size_t dimSize{0};
		};

		struct TypeSpec {
			bool IsBasic() const;
			bool IsTransparent() const;
			bool IsOpaque() const;
			bool IsStructure() const;
			bool IsStructureNamed() const;
			bool IsStructureAnonymous() const;
			bool IsScalar() const;
			bool IsVector() const;
			bool IsMatrix() const;
			bool IsTexture() const;
			bool IsArray() const;
			bool IsAggregate() const;
			
			// Basic or User-defined type identifier.
			Token type;
			// StructDecl pointer to a named or an unnamed structure.
			// 1) struct {...} s1, s2; // unnamed structure declaration.
			// 2) struct S {...} s1, s2; // named structure declaration.
			// Used only when the 'TypeSpec' instance is part of the declaration.
			std::shared_ptr<StructDecl> typeDecl;
			// Array dimensions (if it's an array type, i.e. int[]).
			std::vector<ArrayDim> dimensions;
		};
		
		bool operator==(const TypeSpec& type1, const TypeSpec& type2);
		bool operator!=(const TypeSpec& type1, const TypeSpec& type2);
		// Equality with the Implicit Conversions taken into account.
		bool IsTypePromotable(const TypeSpec& check, const TypeSpec& promoteTo);
		TypeSpec PromoteType(const TypeSpec& what, const TypeSpec& promoteTo);

		TypeSpec InferArithmeticBinaryExprType(const TypeSpec& lhs, const TypeSpec& rhs, TokenType op);

		struct FullSpecType {
			TypeQual qualifier;
			TypeSpec specifier;
		};

		// TODO: type table

		std::string MangleTypeSpecName(const TypeSpec& typeSpec);

		class TypeTable {
		public:
			TypeTable();

			const TypeSpec& GetType(size_t idx);
			const TypeSpec& GetType(const std::string& typeName);

			bool HasType(const TypeSpec& type);
			bool HasType(const std::string& typeName);

			size_t AddType(const TypeSpec& type);

			size_t GetTypeId(const TypeSpec& type);

		private:
			std::vector<TypeSpec> types;
			std::unordered_map<std::string, size_t> typeMap;
		};

	}
}