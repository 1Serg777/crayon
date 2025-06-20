#include "GLSL/Type.h"

#include <algorithm>
#include <cassert>
#include <string_view>
#include <unordered_map>

namespace crayon {
	namespace glsl {

		static constexpr int tokenTypeOffset = static_cast<int>(TokenType::BOOL);
		static constexpr GlslExprType tokenTypeToGlslExprTypeMap[] = {
			// Scalar basic types:
			GlslExprType::BOOL, GlslExprType::INT, GlslExprType::UINT, GlslExprType::FLOAT, GlslExprType::DOUBLE,
			// Vector types:
			GlslExprType::BVEC2, GlslExprType::IVEC2, GlslExprType::UVEC2, GlslExprType::VEC2, GlslExprType::DVEC2,
			GlslExprType::BVEC3, GlslExprType::IVEC3, GlslExprType::UVEC3, GlslExprType::VEC3, GlslExprType::DVEC3,
			GlslExprType::BVEC4, GlslExprType::IVEC4, GlslExprType::UVEC4, GlslExprType::VEC4, GlslExprType::DVEC4,
			// Matrix types:
			GlslExprType::MAT2X2,  GlslExprType::MAT3X3,  GlslExprType::MAT4X4,
			GlslExprType::DMAT2X2, GlslExprType::DMAT3X3, GlslExprType::DMAT4X4,
			GlslExprType::MAT2X2,  GlslExprType::DMAT2X2, GlslExprType::MAT2X3, GlslExprType::DMAT2X3, GlslExprType::MAT2X4, GlslExprType::DMAT2X4,
			GlslExprType::MAT3X2,  GlslExprType::DMAT3X2, GlslExprType::MAT3X3, GlslExprType::DMAT3X3, GlslExprType::MAT3X4, GlslExprType::DMAT3X4,
			GlslExprType::MAT4X2,  GlslExprType::DMAT4X2, GlslExprType::MAT4X3, GlslExprType::DMAT4X3, GlslExprType::MAT4X4, GlslExprType::DMAT4X4,
		};

		// "asmd" are the first letters of the names of the following binary operators:
		// a = add (+), s = subtract (-), m = multiply (*), d = divide (/).
		// "asmdBinaryOpType"
		// The table is formed for the alternative definition of the 'GlslExprType' enumeration #2.
		/*
		constexpr GlslExprType binaryOpType[][static_cast<size_t>(GlslExprType::COUNT)] = {
			// 1.  BOOL,
			// 2.  INT,     UINT,
			// 3.  FLOAT,   DOUBLE,
			// 4.  BVEC2,   BVEC3,   BVEC4,
			// 5.  IVEC2,   IVEC3,   IVEC4,
			// 6.  UVEC2,   UVEC3,   UVEC4,
			// 7.  VEC2,    VEC3,    VEC4,
			// 8.  DVEC2,   DVEC3,   DVEC4,
			// 9.  MAT2,    MAT3,    MAT4,
			// 10. MAT2X2,  MAT2X3,  MAT2X4,
			// 11. MAT3X2,  MAT3X3,  MAT3X4,
			// 12. MAT4X2,  MAT4X3,  MAT4X4,
			// 13. DMAT2,   DMAT3,   DMAT4,
			// 14. DMAT2X2, DMAT2X3, DMAT2X4,
			// 15. DMAT3X2, DMAT3X3, DMAT3X4,
			// 16. DMAT4X2, DMAT4X3, DMAT4X4,
			
			// 1. BOOL
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED,                          // 2 INT,   UINT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED,                          // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 2. INT
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::INT,       GlslExprType::UINT,                               // 2 INT,   UINT
				GlslExprType::FLOAT,     GlslExprType::DOUBLE,                             // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::IVEC2,     GlslExprType::IVEC3,     GlslExprType::IVEC4,     // 5 IVEC
				GlslExprType::UVEC2,     GlslExprType::UVEC3,     GlslExprType::UVEC4,     // 6 UVEC
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::MAT2,      GlslExprType::MAT3,      GlslExprType::MAT4,      // 9 MAT
				GlslExprType::MAT2X2,    GlslExprType::MAT2X3,    GlslExprType::MAT2X4,    // 10 MAT2X
				GlslExprType::MAT3X2,    GlslExprType::MAT3X3,    GlslExprType::MAT3X4,    // 11 MAT3X
				GlslExprType::MAT4X2,    GlslExprType::MAT4X3,    GlslExprType::MAT4X4,    // 12 MAT4X
				GlslExprType::DMAT2,     GlslExprType::DMAT3,	  GlslExprType::DMAT4,     // 13 DMAT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 14 DMAT2X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 15 DMAT3X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 16 DMAT4X
			},
			// 3. UINT
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::UINT,      GlslExprType::UINT,                               // 2 INT,   UINT
				GlslExprType::FLOAT,     GlslExprType::DOUBLE,                             // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UVEC2,     GlslExprType::UVEC3,     GlslExprType::UVEC4,     // 5 IVEC
				GlslExprType::UVEC2,     GlslExprType::UVEC3,     GlslExprType::UVEC4,     // 6 UVEC
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::MAT2,      GlslExprType::MAT3,      GlslExprType::MAT4,      // 9 MAT
				GlslExprType::MAT2X2,    GlslExprType::MAT2X3,    GlslExprType::MAT2X4,    // 10 MAT2X
				GlslExprType::MAT3X2,    GlslExprType::MAT3X3,    GlslExprType::MAT3X4,    // 11 MAT3X
				GlslExprType::MAT4X2,    GlslExprType::MAT4X3,    GlslExprType::MAT4X4,    // 12 MAT4X
				GlslExprType::DMAT2,     GlslExprType::DMAT3,	  GlslExprType::DMAT4,     // 13 DMAT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 14 DMAT2X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 15 DMAT3X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 16 DMAT4X
			},
			// 4. FLOAT
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::FLOAT,     GlslExprType::FLOAT,                              // 2 INT,   UINT
				GlslExprType::FLOAT,     GlslExprType::DOUBLE,                             // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 5 IVEC
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 6 UVEC
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::MAT2,      GlslExprType::MAT3,      GlslExprType::MAT4,      // 9 MAT
				GlslExprType::MAT2X2,    GlslExprType::MAT2X3,    GlslExprType::MAT2X4,    // 10 MAT2X
				GlslExprType::MAT3X2,    GlslExprType::MAT3X3,    GlslExprType::MAT3X4,    // 11 MAT3X
				GlslExprType::MAT4X2,    GlslExprType::MAT4X3,    GlslExprType::MAT4X4,    // 12 MAT4X
				GlslExprType::DMAT2,     GlslExprType::DMAT3,	  GlslExprType::DMAT4,     // 13 DMAT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 14 DMAT2X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 15 DMAT3X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 16 DMAT4X
			},
			// 5. DOUBLE
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DOUBLE,    GlslExprType::DOUBLE,                             // 2 INT,   UINT
				GlslExprType::DOUBLE,    GlslExprType::DOUBLE,                             // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 5 IVEC
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 6 UVEC
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::DMAT2,     GlslExprType::DMAT3,     GlslExprType::DMAT4,     // 9 MAT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 10 MAT2X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 11 MAT3X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 12 MAT4X
				GlslExprType::DMAT2,     GlslExprType::DMAT3,	  GlslExprType::DMAT4,     // 13 DMAT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 14 DMAT2X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 15 DMAT3X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 16 DMAT4X
			},
			// 6. BVEC2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED,                          // 2 INT,   UINT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED,                          // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 7. BVEC3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED,                          // 2 INT,   UINT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED,                          // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 8. BVEC4
			{
			    GlslExprType::UNDEFINED,                                                   // 1 BOOL,
			    GlslExprType::UNDEFINED, GlslExprType::UNDEFINED,                          // 2 INT,   UINT
			    GlslExprType::UNDEFINED, GlslExprType::UNDEFINED,                          // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
		    },
			// 9. IVEC2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::IVEC2,     GlslExprType::UVEC2,                              // 2 INT,   UINT
				GlslExprType::VEC2,      GlslExprType::DVEC2,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::IVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 10. IVEC3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::IVEC3,     GlslExprType::UVEC3,                              // 2 INT,   UINT
				GlslExprType::VEC3,      GlslExprType::DVEC3,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::IVEC3,     GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UVEC3,     GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 11. IVEC4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::IVEC4,     GlslExprType::UVEC4,                              // 2 INT,   UINT
				GlslExprType::VEC4,      GlslExprType::DVEC4,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::IVEC4,     // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UVEC4,     // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 16 DMAT4X
			},
			// 12. UVEC2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::UVEC2,     GlslExprType::UVEC2,                              // 2 INT,   UINT
				GlslExprType::VEC2,      GlslExprType::DVEC2,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 13. UVEC3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::UVEC3,     GlslExprType::UVEC3,                              // 2 INT,   UINT
				GlslExprType::VEC3,      GlslExprType::DVEC3,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UVEC3,     GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UVEC3,     GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 14. UVEC4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::UVEC4,     GlslExprType::UVEC4,                              // 2 INT,   UINT
				GlslExprType::VEC4,      GlslExprType::DVEC4,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UVEC4,     // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UVEC4,     // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 16 DMAT4X
			},
			// 15. VEC2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::VEC2,      GlslExprType::VEC2,                               // 2 INT,   UINT
				GlslExprType::VEC2,      GlslExprType::DVEC2,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 16. VEC3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::VEC3,      GlslExprType::VEC3,                               // 2 INT,   UINT
				GlslExprType::VEC3,      GlslExprType::DVEC3,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 17. VEC4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::VEC4,      GlslExprType::VEC4,                               // 2 INT,   UINT
				GlslExprType::VEC4,      GlslExprType::DVEC4,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::VEC2,      GlslExprType::VEC3,      GlslExprType::VEC4,      // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 16 DMAT4X
			},
			// 18. DVEC2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DVEC2,     GlslExprType::DVEC2,                              // 2 INT,   UINT
				GlslExprType::DVEC2,     GlslExprType::DVEC2,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 19. DVEC3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DVEC3,     GlslExprType::DVEC3,                              // 2 INT,   UINT
				GlslExprType::DVEC3,     GlslExprType::DVEC3,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 20. DVEC4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DVEC4,      GlslExprType::DVEC4,                             // 2 INT,   UINT
				GlslExprType::DVEC4,      GlslExprType::DVEC4,                             // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DVEC2,     GlslExprType::DVEC3,     GlslExprType::DVEC4,     // 16 DMAT4X
			},
			// 21. MAT2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT2,      GlslExprType::MAT2,                               // 2 INT,   UINT
				GlslExprType::MAT2,      GlslExprType::DMAT2,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::MAT2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::MAT2X2,    GlslExprType::MAT2X3,    GlslExprType::MAT2X4,    // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DMAT2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 22. MAT3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT3,      GlslExprType::MAT3,                               // 2 INT,   UINT
				GlslExprType::MAT3,      GlslExprType::DMAT3,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::MAT3,      GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::MAT3X2,    GlslExprType::MAT3X3,    GlslExprType::MAT3X4,    // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DMAT3,     GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 23. MAT4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT4,      GlslExprType::MAT4,                               // 2 INT,   UINT
				GlslExprType::MAT4,      GlslExprType::DMAT4,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::MAT4,      // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::MAT4X2,    GlslExprType::MAT4X3,    GlslExprType::MAT4X4,    // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT4,     // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 16 DMAT4X
			},
			// 24. MAT2X2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT2X2,    GlslExprType::MAT2X2,                             // 2 INT,   UINT
				GlslExprType::MAT2X2,    GlslExprType::DMAT2X2,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::VEC2,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::MAT2X2,    GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::MAT2X2,    GlslExprType::MAT2X3,    GlslExprType::MAT2X4,    // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DMAT2X2,   GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 25. MAT2X3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT2X3,    GlslExprType::MAT2X3,                             // 2 INT,   UINT
				GlslExprType::MAT2X3,    GlslExprType::DMAT2X3,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC2,      GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC2,      GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC2,      GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC2,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::MAT2X3,    GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::MAT2X2,    GlslExprType::MAT2X3,    GlslExprType::MAT2X4,    // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DMAT2X3,   GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 25. MAT2X4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT2X4,    GlslExprType::MAT2X4,                             // 2 INT,   UINT
				GlslExprType::MAT2X4,    GlslExprType::DMAT2X4,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC2,      // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC2,      // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC2,      // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC2,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::MAT2X4,    // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::MAT2X2,    GlslExprType::MAT2X3,    GlslExprType::MAT2X4,    // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT2X4,   // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 16 DMAT4X
			},
			// 26. MAT3X2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT3X2,    GlslExprType::MAT3X2,                             // 2 INT,   UINT
				GlslExprType::MAT3X2,    GlslExprType::DMAT3X2,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::VEC3,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::VEC3,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::VEC3,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC3,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::MAT3X2,    GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::MAT3X2,    GlslExprType::MAT3X3,    GlslExprType::MAT3X4,    // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DMAT3X2,   GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 27. MAT3X3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT3X3,    GlslExprType::MAT3X3,                             // 2 INT,   UINT
				GlslExprType::MAT3X3,    GlslExprType::DMAT3X3,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC3,      GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::MAT3X3,    GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::MAT3X2,    GlslExprType::MAT3X3,    GlslExprType::MAT3X4,    // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DMAT3X3,   GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 28. MAT3X4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT3X4,    GlslExprType::MAT3X4,                             // 2 INT,   UINT
				GlslExprType::MAT3X4,    GlslExprType::DMAT3X4,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC3,      // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC3,      // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC3,      // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC3,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::MAT3X4,    // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::MAT3X2,    GlslExprType::MAT3X3,    GlslExprType::MAT3X4,    // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT3X4,   // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 16 DMAT4X
			},
			// 29. MAT4X2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT4X2,    GlslExprType::MAT4X2,                             // 2 INT,   UINT
				GlslExprType::MAT4X2,    GlslExprType::DMAT4X2,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::VEC4,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::VEC4,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::VEC4,      GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC4,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::MAT4X2,    GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::MAT4X2,    GlslExprType::MAT4X3,    GlslExprType::MAT4X4,    // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DMAT4X2,   GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 30. MAT4X3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT4X3,    GlslExprType::MAT4X3,                             // 2 INT,   UINT
				GlslExprType::MAT4X3,    GlslExprType::DMAT4X3,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC4,      GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC4,      GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::VEC4,      GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC4,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::MAT4X3,    GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::MAT4X2,    GlslExprType::MAT4X3,    GlslExprType::MAT4X4,    // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DMAT4X3,   GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT4X4,   // 14 DMAT2X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 31. MAT4X4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::MAT4X4,    GlslExprType::MAT4X4,                             // 2 INT,   UINT
				GlslExprType::MAT4X4,    GlslExprType::DMAT4X4,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::VEC4,      // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::MAT4X4,    // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::MAT4X2,    GlslExprType::MAT4X3,    GlslExprType::MAT4X4,    // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT4X4,   // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 16 DMAT4X
			},
			// 32. DMAT2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT2,     GlslExprType::DMAT2,                              // 2 INT,   UINT
				GlslExprType::DMAT2,     GlslExprType::DMAT2,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::DMAT2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DMAT2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 33. DMAT3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT3,     GlslExprType::DMAT3,                              // 2 INT,   UINT
				GlslExprType::DMAT3,     GlslExprType::DMAT3,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::DMAT3,     GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DMAT3,     GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 34. DMAT4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT4,     GlslExprType::DMAT4,                              // 2 INT,   UINT
				GlslExprType::DMAT4,     GlslExprType::DMAT4,                              // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT4,     // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT4,     // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 16 DMAT4X
			},
			// 35. DMAT2X2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X2,                            // 2 INT,   UINT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X2,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC2,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::DMAT2X2,   GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DMAT2X2,   GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 36. DMAT2X3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT2X3,   GlslExprType::DMAT2X3,                            // 2 INT,   UINT
				GlslExprType::DMAT2X3,   GlslExprType::DMAT2X3,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC2,     GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC2,     GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC2,     GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC2,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::DMAT2X3,   GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DMAT2X3,   GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 37. DMAT2X4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT2X4,   GlslExprType::DMAT2X4,                            // 2 INT,   UINT
				GlslExprType::DMAT2X4,   GlslExprType::DMAT2X4,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC2,     // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC2,     // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC2,     // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC2,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT2X4,   // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT2X4,   // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DMAT2X2,   GlslExprType::DMAT2X3,   GlslExprType::DMAT2X4,   // 16 DMAT4X
			},
			// 38. DMAT3X2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X2,                            // 2 INT,   UINT
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X2,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::DVEC3,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::DVEC3,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::DVEC3,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC3,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::DMAT3X2,   GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DMAT3X2,   GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 39. DMAT3X3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT3X3,   GlslExprType::DMAT3X3,                            // 2 INT,   UINT
				GlslExprType::DMAT3X3,   GlslExprType::DMAT3X3,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC3,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::DMAT3X3,   GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DMAT3X3,   GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 40. DMAT3X4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT3X4,   GlslExprType::DMAT3X4,                            // 2 INT,   UINT
				GlslExprType::DMAT3X4,   GlslExprType::DMAT3X4,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC3,     // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC3,     // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC3,     // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC3,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT3X4,   // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT3X4,   // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DMAT3X2,   GlslExprType::DMAT3X3,   GlslExprType::DMAT3X4,   // 16 DMAT4X
			},
			// 41. DMAT4X2
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X2,                            // 2 INT,   UINT
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X2,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::DVEC4,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::DVEC4,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::DVEC4,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::DVEC4,     GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::DMAT4X2,   GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::DMAT4X2,   GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 42. DMAT4X3
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT4X3,   GlslExprType::DMAT4X3,                            // 2 INT,   UINT
				GlslExprType::DMAT4X3,   GlslExprType::DMAT4X3,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC4,     GlslExprType::UNDEFINED, // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC4,     GlslExprType::UNDEFINED, // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC4,     GlslExprType::UNDEFINED, // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::DVEC4,     GlslExprType::UNDEFINED, // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::DMAT4X3,   GlslExprType::UNDEFINED, // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 11 MAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::DMAT4X3,   GlslExprType::UNDEFINED, // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT4X4,   // 14 DMAT2X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 15 DMAT3X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 16 DMAT4X
			},
			// 43. DMAT4X4
			{
				GlslExprType::UNDEFINED,                                                   // 1 BOOL,
				GlslExprType::DMAT4X4,   GlslExprType::DMAT4X4,                            // 2 INT,   UINT
				GlslExprType::DMAT4X4,   GlslExprType::DMAT4X4,                            // 3 FLOAT, DOUBLE
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 4 BVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 5 IVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 6 UVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 7 VEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DVEC4,     // 8 DVEC
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT4X4,   // 9 MAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 10 MAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 11 MAT3X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 12 MAT4X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::DMAT4X4,   // 13 DMAT
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 14 DMAT2X
				GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, GlslExprType::UNDEFINED, // 15 DMAT3X
				GlslExprType::DMAT4X2,   GlslExprType::DMAT4X3,   GlslExprType::DMAT4X4,   // 16 DMAT4X
			},
		};
		*/

		//constexpr GlslExprType rowsColsTypeMap[][] = {
		//	{MAT1X1, MAT1X2, MAT1X3, MAT1X4},
		//	{MAT2X1, MAT2X2, MAT2X3, MAT2X4},
		//	{MAT3X1, MAT3X2, MAT3X3, MAT3X4},
		//	{MAT4X1, MAT4X2, MAT4X3, MAT4X4},
		//};
		static std::unordered_map<std::string_view, GlslExprType> aliasTypeMap{
			{"mat2",  GlslExprType::MAT2X2 },
			{"mat3",  GlslExprType::MAT3X3 },
			{"mat4",  GlslExprType::MAT4X4 },
			{"dmat2", GlslExprType::DMAT2X2},
			{"dmat3", GlslExprType::DMAT3X3},
			{"dmat4", GlslExprType::DMAT4X4},
		};

		static constexpr GlslExprType fundamentalTypeMap[] = {
			// Scalars.
			GlslExprType::BOOL, GlslExprType::INT, GlslExprType::UINT, GlslExprType::FLOAT, GlslExprType::DOUBLE, // BOOL, INT, UINT, FLOAT, DOUBLE
			// Vectors.
			GlslExprType::BOOL, GlslExprType::INT, GlslExprType::UINT, GlslExprType::FLOAT, GlslExprType::DOUBLE, // BVEC2, IVEC2, UVEC2, VEC2, DVEC2
			GlslExprType::BOOL, GlslExprType::INT, GlslExprType::UINT, GlslExprType::FLOAT, GlslExprType::DOUBLE, // BVEC3, IVEC3, UVEC3, VEC3, DVEC3
			GlslExprType::BOOL, GlslExprType::INT, GlslExprType::UINT, GlslExprType::FLOAT, GlslExprType::DOUBLE, // BVEC4, IVEC4, UVEC4, VEC4, DVEC4
			// Matrices.
			GlslExprType::FLOAT, GlslExprType::DOUBLE, GlslExprType::FLOAT, GlslExprType::DOUBLE, GlslExprType::FLOAT, GlslExprType::DOUBLE, // MAT2X2, DMAT2X2, MAT2X3, DMAT2X3, MAT2X4, DMAT2X4
			GlslExprType::FLOAT, GlslExprType::DOUBLE, GlslExprType::FLOAT, GlslExprType::DOUBLE, GlslExprType::FLOAT, GlslExprType::DOUBLE, // MAT3X2, DMAT3X2, MAT3X3, DMAT3X3, MAT3X4, DMAT3X4
			GlslExprType::FLOAT, GlslExprType::DOUBLE, GlslExprType::FLOAT, GlslExprType::DOUBLE, GlslExprType::FLOAT, GlslExprType::DOUBLE, // MAT4X2, DMAT4X2, MAT4X3, DMAT4X3, MAT4X4, DMAT4X4
		};
		// FLOAT fundamental type is used because every matrix-vector or vector-matrix multiplication
		// produces the result that is at least FLOAT.
		static constexpr GlslExprType rowsColsTypeMap[][4] = {
			{GlslExprType::FLOAT, GlslExprType::VEC2,   GlslExprType::VEC3,   GlslExprType::VEC4  },
			{GlslExprType::VEC2,  GlslExprType::MAT2X2, GlslExprType::MAT2X3, GlslExprType::MAT2X4},
			{GlslExprType::VEC3,  GlslExprType::MAT3X2, GlslExprType::MAT3X3, GlslExprType::MAT3X4},
			{GlslExprType::VEC4,  GlslExprType::MAT4X2, GlslExprType::MAT4X3, GlslExprType::MAT4X4},
		};

		// 'matRowsColsOffset' integer is used to simplify indexing the two arrays below.
		// Using it we "shift" the type ids of the matrix types in the 'GlslExprType' enumeration to 0.
		static constexpr int matRowsColsOffset = static_cast<int>(GlslExprType::MAT2X2);
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

		// Row vectors.
		/*
		static constexpr size_t vecRowsTypeMap[] = {
			1, 1, 1, 1, 1, // BVEC2, IVEC2, UVEC2, VEC2, DVEC2
			1, 1, 1, 1, 1, // BVEC3, IVEC3, UVEC3, VEC3, DVEC3
			1, 1, 1, 1, 1, // BVEC4, IVEC4, UVEC4, VEC4, DVEC4
		};
		static constexpr size_t vecColsTypeMap[] = {
			2, 2, 2, 2, 2, // BVEC2, IVEC2, UVEC2, VEC2, DVEC2
			3, 3, 3, 3, 3, // BVEC3, IVEC3, UVEC3, VEC3, DVEC3
			4, 4, 4, 4, 4, // BVEC4, IVEC4, UVEC4, VEC4, DVEC4
		};
		*/

		// Column vectors by default.
		// If you want to retrieve the number of rows or columns
		// of a row vector,
		// use 'vecRowsTypeMap' for columns and 'vecColsTypeMap' for rows.
		// 'vecRowsColsOffset' integer is used to simplify indexing the two arrays below.
		// Using it we "shift" the type ids of the vector types in the 'GlslExprType' enumeration to 0.
		static constexpr int vecRowsColsOffset = static_cast<int>(GlslExprType::BVEC2);
		static constexpr size_t vecRowsTypeMap[] = {
			2, 2, 2, 2, 2, // BVEC2, IVEC2, UVEC2, VEC2, DVEC2
			3, 3, 3, 3, 3, // BVEC3, IVEC3, UVEC3, VEC3, DVEC3
			4, 4, 4, 4, 4, // BVEC4, IVEC4, UVEC4, VEC4, DVEC4
		};
		static constexpr size_t vecColsTypeMap[] = {
			1, 1, 1, 1, 1, // BVEC2, IVEC2, UVEC2, VEC2, DVEC2
			1, 1, 1, 1, 1, // BVEC3, IVEC3, UVEC3, VEC3, DVEC3
			1, 1, 1, 1, 1, // BVEC4, IVEC4, UVEC4, VEC4, DVEC4
		};

		GlslExprType GetGlslExprType(TokenType tokenType) {
			return tokenTypeToGlslExprTypeMap[static_cast<int>(tokenType) - tokenTypeOffset];
		}

		GlslExprType GetAliasType(std::string_view alias) {
			auto searchRes = aliasTypeMap.find(alias);
			if (searchRes == aliasTypeMap.end()) {
				return GlslExprType::UNDEFINED;
			}
			return searchRes->second;
		}

		int GetFundamentalTypeRank(GlslExprType type) {
			return static_cast<int>(GetFundamentalType(type));
		}
		GlslExprType GetFundamentalType(GlslExprType type) {
			return static_cast<GlslExprType>(fundamentalTypeMap[static_cast<size_t>(type)]);
		}

		GlslExprType GetTypeRowsCols(size_t rows, size_t cols) {
			assert(!(rows == cols == 1) && "Scalar can't be represented as a 1x1 matrix!");
			return rowsColsTypeMap[rows - 1][cols - 1];
		}
		size_t GetNumberOfRows(GlslExprType type, OperandPos pos) {
			if (MatrixType(type)) {
				return GetMatNumberOfRows(type);
			} else if (VectorType(type)) {
				return GetVecNumberOfRows(type, pos);
			} else /*if (ScalarType(type))*/ {
				return 0;
			}
		}
		size_t GetNumberOfCols(GlslExprType type, OperandPos pos) {
			if (MatrixType(type)) {
				return GetMatNumberOfCols(type);
			} else if (VectorType(type)) {
				return GetVecNumberOfCols(type, pos);
			} else /*if (ScalarType(type))*/ {
				return 0;
			}
		}

		size_t GetVecNumberOfRows(GlslExprType type, OperandPos pos) {
			// LHS vectors are treated as row vectors and
			// RHS vectors as column vectors.
			if (pos == OperandPos::LHS) {
				return GetRowVecNumberOfRows(type);
			} else /*(pos == OperandPos::RHS)*/ {
				return GetColVecNumberOfRows(type);
			}
		}
		size_t GetVecNumberOfCols(GlslExprType type, OperandPos pos) {
			// LHS vectors are treated as row vectors and
			// RHS vectors as column vectors.
			if (pos == OperandPos::LHS) {
				return GetRowVecNumberOfCols(type);
			} else /*(pos == OperandPos::RHS)*/ {
				return GetColVecNumberOfCols(type);
			}
		}

		size_t GetColVecNumberOfRows(GlslExprType type) {
			assert(VectorType(type) && "Not a vector type provided!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return vecRowsTypeMap[idx];
		}
		size_t GetColVecNumberOfCols(GlslExprType type) {
			assert(VectorType(type) && "Not a vector type provided!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return vecColsTypeMap[idx];
		}
		size_t GetRowVecNumberOfRows(GlslExprType type) {
			assert(VectorType(type) && "Not a vector type provided!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return vecColsTypeMap[idx];
		}
		size_t GetRowVecNumberOfCols(GlslExprType type) {
			assert(VectorType(type) && "Not a vector type provided!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return vecRowsTypeMap[idx];
		}

		size_t GetMatNumberOfRows(GlslExprType type) {
			assert(MatrixType(type) && "Not a matrix type provided!");
			int idx = static_cast<int>(type) - matRowsColsOffset;
			return matRowsTypeMap[idx];
		}
		size_t GetMatNumberOfCols(GlslExprType type) {
			assert(MatrixType(type) && "Not a matrix type provided!");
			int idx = static_cast<int>(type) - matRowsColsOffset;
			return matColsTypeMap[idx];
		}

		GlslExprType PromoteType(GlslExprType type, int rankDiff) {
#if defined (_DEBUG) || defined(DEBUG)
			if (ScalarType(type)) return PromoteScalarType(type, rankDiff);
			if (VectorType(type)) return PromoteVectorType(type, rankDiff);
			if (MatrixType(type)) return PromoteMatrixType(type, rankDiff);
#endif
			return static_cast<GlslExprType>(static_cast<int>(type) + rankDiff);
		}
		GlslExprType PromoteFundamentalType(GlslExprType type, int rankDiff) {
			assert((rankDiff >= -3 && rankDiff <= 3) && "Rank promotion number must be within the [-3, 3] range!");
			assert(FundamentalType(type) && "Type must be fundamental!");
			GlslExprType promotedType = static_cast<GlslExprType>(static_cast<int>(type) + rankDiff);
			assert(FundamentalType(promotedType) && "Promoted type must stay fundamental!");
			return promotedType;
		}
		GlslExprType PromoteScalarType(GlslExprType type, int rankDiff) {
			return PromoteFundamentalType(type, rankDiff);
		}
		GlslExprType PromoteVectorType(GlslExprType type, int rankDiff) {
			assert((rankDiff >= -3 && rankDiff <= 3) && "Rank promotion number must be within the [-3, 3] range!");
			assert(VectorType(type) && "Type must be one of the vector types!");
			GlslExprType promotedType = static_cast<GlslExprType>(static_cast<int>(type) + rankDiff);
			assert(VectorType(promotedType) && "Promoted type must stay one of the vector types!");
			return promotedType;
		}
		GlslExprType PromoteMatrixType(GlslExprType type, int rankDiff) {
			assert((rankDiff >= -1 && rankDiff <= 1) && "Rank promotion number must be within the [-1, 1] range!");
			assert(MatrixType(type) && "Type must be one of the matrix types!");
			GlslExprType promotedType = static_cast<GlslExprType>(static_cast<int>(type) + rankDiff);
			assert(MatrixType(promotedType) && "Promoted type must stay one of the matrix types!");
			return promotedType;
		}

		bool FundamentalTypeBool(GlslExprType type) {
			return GetFundamentalType(type) == GlslExprType::BOOL;
		}
		bool FundamentalTypeInt(GlslExprType type) {
			return GetFundamentalType(type) == GlslExprType::INT;
		}
		bool FundamentalTypeUint(GlslExprType type) {
			return GetFundamentalType(type) == GlslExprType::UINT;
		}
		bool FundamentalTypeFloat(GlslExprType type) {
			return GetFundamentalType(type) == GlslExprType::FLOAT;
		}
		bool FundamentalTypeDouble(GlslExprType type) {
			return GetFundamentalType(type) == GlslExprType::DOUBLE;
		}

		bool FundamentalType(GlslExprType type) {
			return type >= GlslExprType::BOOL && type <= GlslExprType::DOUBLE;
		}
		bool ScalarType(GlslExprType type) {
			return FundamentalType(type);
		}
		bool IntegralType(GlslExprType type) {
			return type >= GlslExprType::INT && type <= GlslExprType::UINT;
		}
		bool FloatingType(GlslExprType type) {
			return type >= GlslExprType::FLOAT && type <= GlslExprType::DOUBLE;
		}
		bool VectorType(GlslExprType type) {
			return type >= GlslExprType::BVEC2 && type <= GlslExprType::DVEC4;
		}
		bool MatrixType(GlslExprType type) {
			return type >= GlslExprType::MAT2X2 && type <= GlslExprType::DMAT4X4;
		}

		bool AddSubDivAllowed(GlslExprType lhs, GlslExprType rhs) {
			if (ScalarType(lhs) || ScalarType(rhs)) {
				// 1) Both operands are scalars, or
				// 2) at least one of them is.
				// In both cases all arithmetic binary operations are allowed.
				return true;
			} else {
				// Both operands are non-scalar objects.
				// The +, -, / arithmetic binary operations are allowed if
				// 1) Both operands are vectors of the same size.
				// 2) Both operands are matrices of the same size.
				// When one of the operands is a vector and the other one is a matrix, the operations are not allowed.
				if (VectorType(lhs) && VectorType(rhs)) {
					// 1)
					size_t lhsVecSize = GetColVecNumberOfRows(lhs);
					size_t rhsVecSize = GetColVecNumberOfRows(rhs);
					return lhsVecSize == rhsVecSize;
				} else if (MatrixType(lhs) && MatrixType(rhs)) {
					// 2)
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
		bool AdditionAllowed(GlslExprType lhs, GlslExprType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}
		bool SubtractionAllowed(GlslExprType lhs, GlslExprType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}
		bool MultiplicationAllowed(GlslExprType lhs, GlslExprType rhs) {
			if (ScalarType(lhs) || ScalarType(rhs)) {
				// 1) Both operands are scalars, or
				// 2) at least one of them is.
				// In both cases all arithmetic binary operations are allowed.
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
		bool DivisionAllowed(GlslExprType lhs, GlslExprType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}

		GlslExprType LookupExprType(GlslExprType lhs, GlslExprType rhs, TokenType op) {
			// Use the lookup table to figure out what type the result of the expression should have.
			// GlslExprType inferredType = binaryOpType[static_cast<size_t>(lhs)][static_cast<size_t>(rhs)];
			// The 'binaryOpType' array stores allowed types for multiplication as well, so we can't
			// immediately return what we got from it.
			// 1. Multiplication?
			// if (op == TokenType::STAR) {
				// We can return right away, because multiplication is covered fully by the array.
				// return inferredType;
			// }
			// 2. Other operators.
			// If the operations are allowed, return the inferred type we got at the top.
			// If not, then we might have extracted an inferred type for multiplication which is accidentally valid,
			// when addition, subtraction, and division are not. In that case we must return UNDEFINED.
			// if (!AddSubDivAllowed(lhs, rhs)) {
				// return GlslExprType::UNDEFINED;
			// }
			// return inferredType;
			return GlslExprType::UNDEFINED;
		}
		GlslExprType InferExprType(GlslExprType lhs, GlslExprType rhs, TokenType op) {
			// First of all, we need to figure out what type of operation is applied.
			if (op == TokenType::PLUS || op == TokenType::DASH ||
				op == TokenType::STAR || op == TokenType::SLASH) {
				// 1. Arithmetic binary operation.
				return InferArithmeticBinaryExprType(lhs, rhs, op);
			}
			// TODO: implement other types of operations.
			return GlslExprType::UNDEFINED;
		}
		GlslExprType InferArithmeticBinaryExprType(GlslExprType lhs, GlslExprType rhs, TokenType op) {
			// Following the specification's explanation on p.123
			// Link: https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.pdf
			// If any of the types' fundamental type is BOOL, the result is UNDEFINED, because according to the specification
			// only integer and floating-point scalars, vectors, and matrices are allowed with the arithmetic binary operators.
			if (FundamentalTypeBool(lhs) || FundamentalTypeBool(rhs)) {
				return GlslExprType::UNDEFINED;
			}
			if (ScalarType(lhs) && ScalarType(rhs)) {
				// 1. Both operands are scalars.
				// The operation is applied resulting in a scalar of the "biggest" type.
				// If both types are the same, the result will be of that type.
				// The "biggest" type means the type that goes later in the 'GlslExprType' enumeration.
				// The way the types are listed there follows the implicit conversion rules outlined in the specification.
				// INT -> UINT -> FLOAT -> DOUBLE, where UINT is "bigger" than INT, for example,
				// and DOUBLE is the "biggest" type of all the fundamental types.
				size_t typeId = std::max(static_cast<size_t>(lhs), static_cast<size_t>(rhs));
				return static_cast<GlslExprType>(typeId);
			}
			if (ScalarType(lhs) || ScalarType(rhs)) {
				// 2. One of the types is a scalar while the other one is either a vector or a matrix.
				// According to the specification the operation is applied to each component
				// of the non-scalar operand resulting in the same size vector or matrix.
				// First we need to figure out which operand is a scalar and which one is a vector or matrix.
				if (ScalarType(lhs)) {
					// Left-hand operand is a scalar.
					// The specification states that we must first match the fundamental types of both operands.
					// This means that we need to figure out the fundamental type of the right operand and then
					// either promote the right operand's type or return it as is.
					int rankDiff = GetFundamentalTypeRank(lhs) - GetFundamentalTypeRank(rhs);
					if (rankDiff > 0) {
						// The type of the non-scalar operand (rhs) must be promoted,
						// because its fundamental type is "smaller" than the fundamental type of the scalar operand (lhs).
						// i.e., FLOAT * IVEC3 -> VEC3, because
						// the fundamental type of the vector is smaller than the fundamental type of the scalar.
						return PromoteType(rhs, rankDiff);
					}
					// rankDiff == 0 means the types were the same.
					// Otherwise, either the two types are the same or the non-scalar operand's type doesn't need promotion.
					return rhs;
				} else {
					// Right-hand side operand is a scalar.
					// Same algorithm as above but for the left-hand side operand.
					int rankDiff = GetFundamentalTypeRank(rhs) - GetFundamentalTypeRank(lhs);
					if (rankDiff > 0) {
						return PromoteType(lhs, rankDiff);
					}
					return lhs;
				}
			}
			// At this pointe we can be sure that none of the operands is a scalar.
			if (VectorType(lhs) && VectorType(rhs)) {
				// The two operands are vectors of the same size.
				// The operation is done component-wise resulting in a vector of the same size.
				// Also, don't forget to promote the type of vector if necessary.
				// i.e., IVEC2 + VEC2 -> VEC2
				// We check this in two ways:
				// 1) Check the number of rows of both operands as column vectors, or
				// 2) Check the number of columns of both operands as row vectors.
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
					// Types are set up in such a way so as to allow to perform simple comparisons
					// to figure out whether a type promotion is needed or not.
					if (lhs > rhs) return lhs;
					else return rhs;
				} else {
					return GlslExprType::UNDEFINED;
				}
			}
			// Both operands are matrices, or one of them is a vector.
			if (op == TokenType::STAR) {
				// 1) Multiplication operation.
				// Multiplications is handled according to the rules of linear algebra
				// when matrix-vector or vector-matrix multiplications is performed.
				// Multiplication is allowed if the number of columns of the 'lhs' operand is
				// equal to the number of rows of the 'rhs' operand.
				// The problem is that the number of rows and columns of a vector operand
				// depends on where it's placed with respect to the binary arithmetic operator.
				// - A right vector is treated as a column vector.
				// - A left vector is treated as a rows vector.
				size_t rowsLhs{0};
				size_t colsLhs{0};
				size_t rowsRhs{0};
				size_t colsRhs{0};
				if (VectorType(lhs)) {
					// Left-hand side operand is a vector, it is treated as a row vector.
					rowsLhs = GetRowVecNumberOfRows(lhs);
					colsLhs = GetRowVecNumberOfCols(lhs);
					rowsRhs = GetMatNumberOfRows(rhs);
					colsRhs = GetMatNumberOfCols(rhs);
				} else if (VectorType(rhs)) {
					// Right-hand side operand is a vector, it is treated as a column vector.
					rowsLhs = GetMatNumberOfRows(lhs);
					colsLhs = GetMatNumberOfCols(lhs);
					rowsRhs = GetColVecNumberOfRows(rhs);
					colsRhs = GetColVecNumberOfCols(rhs);
				} else {
					rowsLhs = GetMatNumberOfRows(lhs);
					colsLhs = GetMatNumberOfCols(lhs);
					rowsRhs = GetMatNumberOfRows(rhs);
					colsRhs = GetMatNumberOfCols(rhs);
				}
				if (colsLhs == rowsRhs) {
					// Multiplication is allowed.
					// Now we need to figure out what type should be used for the result of the multiplication.
					// Since at least one of the operands is a matrix, the fundamental type of the result
					// will be at least FLOAT.
					GlslExprType fundamentalTypeLhs = GetFundamentalType(lhs);
					GlslExprType fundamentalTypeRhs = GetFundamentalType(rhs);
					// Should be either FLOAT or DOUBLE.
					GlslExprType biggestFundamentalType = std::max(fundamentalTypeLhs, fundamentalTypeRhs);
					GlslExprType inferredType = GetTypeRowsCols(rowsLhs, colsRhs);

					constexpr GlslExprType inferredFundamentalType = GlslExprType::FLOAT;
					// GlslExprType inferredFundamentalType = GetFundamentalType(inferredType);
					int rankDiff = static_cast<int>(biggestFundamentalType) - static_cast<int>(inferredFundamentalType);
					return PromoteType(inferredType, rankDiff);
				} else {
					return GlslExprType::UNDEFINED;
				}
			} else {
				// 2) Otherwise we're dealing with either addition, subtraction, or devision operation.
				// In any case we need both operands' dimensions to match.
				// Since at this point we're sure that both operands are not both vectors,
				// the only valid case for the +, -, and / arithmetic binary operators is when
				// both operands are matrices of the same size.
				// In other words, if one of the operands is a vector, the operation is not defined.
				if (VectorType(lhs) || VectorType(rhs)) {
					return GlslExprType::UNDEFINED;
				}
				// Both operands are matrices then. What's left is to check their size.
				size_t rowsLhs = GetMatNumberOfRows(lhs);
				size_t colsLhs = GetMatNumberOfCols(lhs);
				size_t rowsRhs = GetMatNumberOfRows(rhs);
				size_t colsRhs = GetMatNumberOfCols(rhs);
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
					// Types are set up in such a way so as to allow to perform simple comparisons
					// to figure out whether a type promotion is needed or not.
					if (lhs > rhs) return lhs;
					else return rhs;
				} else {
					return GlslExprType::UNDEFINED;
				}
			}
			return GlslExprType::UNDEFINED;
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
			return !IsAggregate();
		}
		bool TypeSpec::IsAggregate() const {
			return type.tokenType == TokenType::IDENTIFIER && !typeDecl;
		}
		bool TypeSpec::IsArray() const {
			return !dimensions.empty();
		}

		size_t TypeSpec::ArrayDimensionCount() const {
			return dimensions.size();
		}
		std::shared_ptr<Expr> TypeSpec::ArrayDimensionSizeExpr(size_t dimension) const {
			assert(dimension < dimensions.size() && "Invalid dimension provided!");
			return dimensions[dimension];
		}

	}
}