#pragma once

#include <string_view>
#include <iostream>

namespace crayon {
	namespace glsl {

		enum class TokenType {
			UNDEFINED = -1,

			// Type Qualifiers:
			// Layout Qualifiers
			LAYOUT,
			// Storage Qualifiers
			CONST, IN, OUT, UNIFORM, BUFFER,
			// Precision qualifiers
			HIGH_PRECISION, MEDIUM_PRECESION, LOW_PRECISION,
			// Interpolation qualifiers
			SMOOTH, FLAT, NOPERSPECTIVE,
			// Invariant qualifiers
			INVARIANT,
			// Precise qualifier
			PRECISE,

			// Types:
			STRUCT,
			VOID,
			// Scalar basic types:
			BOOL, INT, UINT, FLOAT, DOUBLE,
			// Vector types:
			BVEC2, IVEC2, UVEC2, VEC2, DVEC2,
			BVEC3, IVEC3, UVEC3, VEC3, DVEC3,
			BVEC4, IVEC4, UVEC4, VEC4, DVEC4,
			// Matrix types:
			MAT2,  MAT3,  MAT4,
			DMAT2, DMAT3, DMAT4,
			MAT2X2, DMAT2X2, MAT2X3, DMAT2X3, MAT2X4, DMAT2X4,
			MAT3X2, DMAT3X2, MAT3X3, DMAT3X3, MAT3X4, DMAT3X4,
			MAT4X2, DMAT4X2, MAT4X3, DMAT4X3, MAT4X4, DMAT4X4,
			// ... add more types later ...
			// Opaque types:
			SAMPLER2D,
			UIMAGE2DMSARRAY,

			// Operators:
			// Unary operators:
			BANG, TILDE,
			// Binary operators:
			// Binary arithmetic operators:
			// Multiplicative operators:
			STAR, SLASH, PERCENT, // *, /, %
			// Additive operators:
			PLUS, DASH, // +, -
			// Shift operators:
			LEFT_OP, RIGHT_OP, // <<, >>
			// Relational operators:
			LEFT_ANGLE, RIGHT_ANGLE, LE_OP, GE_OP, // <, >, <=, >=
			// Equality operators:
			EQ_OP, NE_OP, // ==, !=
			// Bitwise operators:
			AMPERSAND, CARET, VERTICAL_BAR, // &, ^, |
			// Logical operators:
			AND_OP, XOR_OP, OR_OP, // &&, ^^, ||
			// Assignment operators:
			EQUAL,
			MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,
			ADD_ASSIGN, SUB_ASSIGN,
			LEFT_ASSIGN, RIGHT_ASSIGN,
			AND_ASSIGN, XOR_ASSIGN, OR_ASSIGN,

			// Constants and other "primaries":
			IDENTIFIER,
			INTCONSTANT, UINTCONSTANT,
			FLOATCONSTANT, DOUBLECONSTANT,

			// Punctuation marks:
			LEFT_PAREN, RIGHT_PAREN,
			LEFT_BRACKET, RIGHT_BRACKET,
			LEFT_BRACE, RIGHT_BRACE,
			DOT, COMMA, SEMICOLON,

			TOKEN_NUM
		};

		struct Token {
			std::string_view lexeme;
			TokenType tokenType{TokenType::UNDEFINED};
			int line{0};
			int startCol{0};
			int endCol{0};
		};

		void PrintToken(std::ostream& out, const Token& token);
		std::string_view TokenTypeToStr(TokenType tokenType);
	
	}
}