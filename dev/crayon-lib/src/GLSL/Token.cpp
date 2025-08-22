#include "GLSL/Token.h"

#include <array>
#include <cassert>

namespace crayon {
	namespace glsl {

		constexpr std::array<std::string_view, static_cast<size_t>(TokenType::TOKEN_NUM)> tokenNames{
			// Type Qualifiers:
			// Layout Qualifiers
			"LAYOUT",
			// Storage Qualifiers
			"CONST", "IN", "OUT", "UNIFORM", "BUFFER",
			// Precision qualifiers
			"HIGH_PRECISION", "MEDIUM_PRECESION", "LOW_PRECISION",
			// Interpolation qualifiers
			"SMOOTH", "FLAT", "NOPERSPECTIVE",
			// Invariant qualifiers
			"INVARIANT",
			// Precise qualifier
			"PRECISE",

			// Types:
			"STRUCT",
			"VOID",
			// Scalar basic types:
			"BOOL", "INT", "UINT", "FLOAT", "DOUBLE",
			// Vector types:
			"BVEC2", "IVEC2", "UVEC2", "VEC2", "DVEC2",
			"BVEC3", "IVEC3", "UVEC3", "VEC3", "DVEC3",
			"BVEC4", "IVEC4", "UVEC4", "VEC4", "DVEC4",
			// Matrix types:
			"MAT2",  "MAT3",  "MAT4",
			"DMAT2", "DMAT3", "DMAT4",
			"MAT2X2", "DMAT2X2", "MAT2X3", "DMAT2X3", "MAT2X4", "DMAT2X4",
			"MAT3X2", "DMAT3X2", "MAT3X3", "DMAT3X3", "MAT3X4", "DMAT3X4",
			"MAT4X2", "DMAT4X2", "MAT4X3", "DMAT4X3", "MAT4X4", "DMAT4X4",
			// ... add more types later ...
			// Opaque types:
			"SAMPLER2D",
			"UIMAGE2DMSARRAY",

			// Operators:
			// Unary operators:
			"BANG", "TILDE",
			// Binary operators:
			// Binary arithmetic operators:
			// Multiplicative operators:
			"STAR", "SLASH", "PERCENT", // *, /, %
			// Additive operators:
			"PLUS", "DASH", // +, -
			// Shift operators:
			"LEFT_OP", "RIGHT_OP", // <<, >>
			// Relational operators:
			"LEFT_ANGLE", "RIGHT_ANGLE", "LE_OP", "GE_OP", // <, >, <=, >=
			// Equality operators:
			"EQ_OP", "NE_OP", // ==, !=
			// Bitwise operators:
			"AMPERSAND", "CARET", "VERTICAL_BAR", // &, ^, |
			// Logical operators:
			"AND_OP", "XOR_OP", "OR_OP", // &&, ^^, ||
			"EQUAL",
			"MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN",
			"ADD_ASSIGN", "SUB_ASSIGN",
			"LEFT_ASSIGN", "RIGHT_ASSIGN",
			"AND_ASSIGN", "XOR_ASSIGN", "OR_ASSIGN",

			// Constants and other "primaries":
			"IDENTIFIER",
			"INTCONSTANT", "UINTCONSTANT",
			"FLOATCONSTANT", "DOUBLECONSTANT",

			// Punctuation marks:
			"LEFT_PAREN", "RIGHT_PAREN",
			"LEFT_BRACKET", "RIGHT_BRACKET",
			"LEFT_BRACE", "RIGHT_BRACE",
			"DOT", "COMMA", "SEMICOLON",

			// GLSL language extension keywords:
			"STRING",
			"SHADER_PROGRAM_KW",
			"BEGIN", "END",
			// Graphics pipeline blocks:
			"FIXED_STAGES_CONFIG_KW", "MATERIAL_PROPERTIES_KW", "VERTEX_INPUT_LAYOUT_KW", "COLOR_ATTACHMENTS_KW",
			// Material property type keywords:
			"MAT_PROP_TYPE_INT", "MAT_PROP_TYPE_FLOAT", // Scalars
			"MAT_PROP_TYPE_VEC2", "MAT_PROP_TYPE_VEC3", "MAT_PROP_TYPE_VEC4", "MAT_PROP_TYPE_COLOR", // Vectors
			"MAT_PROP_TYPE_TEX2D", // Opaque types, i.e. Textures
			// Graphics pipeline shader stages:
			"VS_KW",
			"TCS_KW", "TES_KW", "GS_KW", "FS_KW",
			// Compute pipeline starting symbols:
			// Compute pipeline shader stages:
			"CS_KW",
			// TODO
		};
		constexpr std::array<std::string_view, static_cast<size_t>(TokenType::TOKEN_NUM)> tokenLexemes{
			// Type Qualifiers:
			// Layout Qualifiers
			"layout",
			// Storage Qualifiers
			"const", "in", "out", "uniform", "buffer",
			// Precision qualifiers
			"highp", "mediump", "lowp",
			// Interpolation qualifiers
			"smooth", "flat", "noperspective",
			// Invariant qualifiers
			"invariant",
			// Precise qualifier
			"precise",

			// Types:
			"struct",
			"void",
			// Scalar basic types:
			"bool", "int", "uint", "float", "double",
			// Vector types:
			"bvec2", "ivec2", "uvec2", "vec2", "dvec2",
			"bvec3", "ivec3", "uvec3", "vec3", "dvec3",
			"bvec4", "ivec4", "uvec4", "vec4", "dvec4",
			// Matrix types:
			"mat2",  "mat3",  "mat4",
			"dmat2", "dmat3", "dmat4",
			"mat2x2", "dmat2x2", "mat2x3", "dmat2x3", "mat2x4", "dmat2x4",
			"mat3x2", "DMAT3X2", "mat3x3", "dmat3x3", "mat3x4", "dmat3x4",
			"mat4x2", "dmat4x2", "mat4x3", "dmat4x3", "mat4x4", "dmat4x4",
			// ... add more types later ...
			// Opaque types:
			"sampler2D",
			"uimage2DMSArray",

			// Operators:
			// Unary operators:
			"!", "~",
			// Binary operators:
			// Binary arithmetic operators:
			// Multiplicative operators:
			"*", "/", "%", // *, /, %
			// Additive operators:
			"+", "-", // +, -
			// Shift operators:
			"<<", ">>", // <<, >>
			// Relational operators:
			"<", ">", "<=", ">=", // <, >, <=, >=
			// Equality operators:
			"==", "!=", // ==, !=
			// Bitwise operators:
			"&", "^", "|", // &, ^, |
			// Logical operators:
			"&&", "^^", "||", // &&, ^^, ||
			"==", // ==
			"*=", "/=", "%=", // *=, /=, %=
			"+=", "-=", // +=, -=
			"<<=", ">>=", // <<=, >>=
			"&=", "^=", "|=", // &=, ^=, |=

			// Constants and other "primaries":
			"", // IDENTIFIER
			"", "", // INTCONSTANT, UINTCONSTANT
			"", "", // FLOATCONSTANT, DOUBLECONSTANT

			// Punctuation marks:
			"(", ")",
			"[", "]",
			"{", "}",
			".", ",", ";",

			// GLSL language extension keywords:
			"", // STRING
			"ShaderProgram",
			"BEGIN", "END",
			// Graphics pipeline blocks:
			"FixedStagesConfig", "MaterialProperties", "VertexInputLayout", "ColorAttachments",
			// Material property type keywords:
			"Integer", "Float", // Scalars
			"Vector2", "Vector3", "Vector4", "Color", // Vectors
			"Texture2D", // Opaque types, i.e. Textures
			// Graphics pipeline shader stages:
			"VertexShader",
			"TessellationControlShader", "TessellationEvaluationShader",
			"GeometryShader", "FragmentShader",
			// Compute pipeline starting symbols:
			// Compute pipeline shader stages:
			"ComputeShader",
			// TODO
		};

		bool Token::HasSrcCodeRepresentation() const {
			return line == -1 && startCol == -1 && endCol == -1;
		}

		Token GenerateToken(TokenType tokenType) {
			Token token{};
			token.tokenType = tokenType;
			token.lexeme = TokenTypeToLexeme(tokenType);
			return token;
		}

		void PrintToken(std::ostream& out, const Token& token) {
			// +1 for 'line' and 'startCol' variables is because internally lines and columns are indexed starting from 0.
			out << "{"
				<< "'" << token.lexeme << "'" << ", " 
				<< "[" << token.line + 1 << ":" << token.startCol + 1 << "]"
				<< "}";
		}

		std::string_view TokenTypeToStr(TokenType tokenType) {
			if (tokenType == TokenType::UNDEFINED || tokenType == TokenType::TOKEN_NUM)
				return "UNDEFINED";
			std::string_view tokenName = tokenNames[static_cast<size_t>(tokenType)];
			return tokenName;
		}
		std::string_view TokenTypeToLexeme(TokenType tokenType) {
			if (tokenType == TokenType::UNDEFINED || tokenType == TokenType::TOKEN_NUM)
				return "";
			std::string_view tokenLexeme = tokenLexemes[static_cast<size_t>(tokenType)];
			return tokenLexeme;
		}

		std::string_view ExtractStringLiteral(const Token& token) {
			assert(token.tokenType == TokenType::STRING && "The token must be of the STRING type!");
			// - Start past the initial string delimiter character ("'" or '"')
			//   (This adds -1 to the length of the resulting lexeme)
			// - Reduce the length of the lexeme by 1 once more to avoid taking the end delimiter as part of the literal.
			//   (This adds an additional -1 to the length of the resulting lexeme)
			return std::string_view(token.lexeme.data() + 1, token.lexeme.size() - 2);
		}

	}
}