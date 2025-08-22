#include "GLSL/Type.h"
#include "GLSL/Analyzer/Environment.h"

#include <algorithm>
#include <cassert>
#include <string_view>
#include <unordered_map>

namespace crayon {
	namespace glsl {

		static constexpr int tokenTypeOffset = static_cast<int>(TokenType::VOID);
		static constexpr GlslBasicType tokenTypeToGlslExprTypeMap[] = {
			GlslBasicType::VOID,
			// Scalar basic types:
			GlslBasicType::BOOL, GlslBasicType::INT, GlslBasicType::UINT, GlslBasicType::FLOAT, GlslBasicType::DOUBLE,
			// Vector types:
			GlslBasicType::BVEC2, GlslBasicType::IVEC2, GlslBasicType::UVEC2, GlslBasicType::VEC2, GlslBasicType::DVEC2,
			GlslBasicType::BVEC3, GlslBasicType::IVEC3, GlslBasicType::UVEC3, GlslBasicType::VEC3, GlslBasicType::DVEC3,
			GlslBasicType::BVEC4, GlslBasicType::IVEC4, GlslBasicType::UVEC4, GlslBasicType::VEC4, GlslBasicType::DVEC4,
			// Matrix types:
			GlslBasicType::MAT2X2,  GlslBasicType::MAT3X3,  GlslBasicType::MAT4X4,
			GlslBasicType::DMAT2X2, GlslBasicType::DMAT3X3, GlslBasicType::DMAT4X4,
			GlslBasicType::MAT2X2,  GlslBasicType::DMAT2X2, GlslBasicType::MAT2X3, GlslBasicType::DMAT2X3, GlslBasicType::MAT2X4, GlslBasicType::DMAT2X4,
			GlslBasicType::MAT3X2,  GlslBasicType::DMAT3X2, GlslBasicType::MAT3X3, GlslBasicType::DMAT3X3, GlslBasicType::MAT3X4, GlslBasicType::DMAT3X4,
			GlslBasicType::MAT4X2,  GlslBasicType::DMAT4X2, GlslBasicType::MAT4X3, GlslBasicType::DMAT4X3, GlslBasicType::MAT4X4, GlslBasicType::DMAT4X4,
		};

		// "asmd" are the first letters of the names of the following binary operators:
		// a = add (+), s = subtract (-), m = multiply (*), d = divide (/).
		// "asmdBinaryOpType"
		// The table is formed for the alternative definition of the 'GlslBasicType' enumeration #2.
		/*
		constexpr GlslBasicType binaryOpType[][static_cast<size_t>(GlslBasicType::COUNT)] = {
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
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED,                          // 2 INT,   UINT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED,                          // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 2. INT
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::INT,       GlslBasicType::UINT,                               // 2 INT,   UINT
				GlslBasicType::FLOAT,     GlslBasicType::DOUBLE,                             // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::IVEC2,     GlslBasicType::IVEC3,     GlslBasicType::IVEC4,     // 5 IVEC
				GlslBasicType::UVEC2,     GlslBasicType::UVEC3,     GlslBasicType::UVEC4,     // 6 UVEC
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::MAT2,      GlslBasicType::MAT3,      GlslBasicType::MAT4,      // 9 MAT
				GlslBasicType::MAT2X2,    GlslBasicType::MAT2X3,    GlslBasicType::MAT2X4,    // 10 MAT2X
				GlslBasicType::MAT3X2,    GlslBasicType::MAT3X3,    GlslBasicType::MAT3X4,    // 11 MAT3X
				GlslBasicType::MAT4X2,    GlslBasicType::MAT4X3,    GlslBasicType::MAT4X4,    // 12 MAT4X
				GlslBasicType::DMAT2,     GlslBasicType::DMAT3,	  GlslBasicType::DMAT4,     // 13 DMAT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 14 DMAT2X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 15 DMAT3X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 16 DMAT4X
			},
			// 3. UINT
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::UINT,      GlslBasicType::UINT,                               // 2 INT,   UINT
				GlslBasicType::FLOAT,     GlslBasicType::DOUBLE,                             // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UVEC2,     GlslBasicType::UVEC3,     GlslBasicType::UVEC4,     // 5 IVEC
				GlslBasicType::UVEC2,     GlslBasicType::UVEC3,     GlslBasicType::UVEC4,     // 6 UVEC
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::MAT2,      GlslBasicType::MAT3,      GlslBasicType::MAT4,      // 9 MAT
				GlslBasicType::MAT2X2,    GlslBasicType::MAT2X3,    GlslBasicType::MAT2X4,    // 10 MAT2X
				GlslBasicType::MAT3X2,    GlslBasicType::MAT3X3,    GlslBasicType::MAT3X4,    // 11 MAT3X
				GlslBasicType::MAT4X2,    GlslBasicType::MAT4X3,    GlslBasicType::MAT4X4,    // 12 MAT4X
				GlslBasicType::DMAT2,     GlslBasicType::DMAT3,	  GlslBasicType::DMAT4,     // 13 DMAT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 14 DMAT2X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 15 DMAT3X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 16 DMAT4X
			},
			// 4. FLOAT
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::FLOAT,     GlslBasicType::FLOAT,                              // 2 INT,   UINT
				GlslBasicType::FLOAT,     GlslBasicType::DOUBLE,                             // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 5 IVEC
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 6 UVEC
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::MAT2,      GlslBasicType::MAT3,      GlslBasicType::MAT4,      // 9 MAT
				GlslBasicType::MAT2X2,    GlslBasicType::MAT2X3,    GlslBasicType::MAT2X4,    // 10 MAT2X
				GlslBasicType::MAT3X2,    GlslBasicType::MAT3X3,    GlslBasicType::MAT3X4,    // 11 MAT3X
				GlslBasicType::MAT4X2,    GlslBasicType::MAT4X3,    GlslBasicType::MAT4X4,    // 12 MAT4X
				GlslBasicType::DMAT2,     GlslBasicType::DMAT3,	  GlslBasicType::DMAT4,     // 13 DMAT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 14 DMAT2X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 15 DMAT3X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 16 DMAT4X
			},
			// 5. DOUBLE
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DOUBLE,    GlslBasicType::DOUBLE,                             // 2 INT,   UINT
				GlslBasicType::DOUBLE,    GlslBasicType::DOUBLE,                             // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 5 IVEC
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 6 UVEC
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::DMAT2,     GlslBasicType::DMAT3,     GlslBasicType::DMAT4,     // 9 MAT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 10 MAT2X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 11 MAT3X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 12 MAT4X
				GlslBasicType::DMAT2,     GlslBasicType::DMAT3,	  GlslBasicType::DMAT4,     // 13 DMAT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 14 DMAT2X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 15 DMAT3X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 16 DMAT4X
			},
			// 6. BVEC2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED,                          // 2 INT,   UINT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED,                          // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 7. BVEC3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED,                          // 2 INT,   UINT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED,                          // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 8. BVEC4
			{
			    GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
			    GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED,                          // 2 INT,   UINT
			    GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED,                          // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
		    },
			// 9. IVEC2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::IVEC2,     GlslBasicType::UVEC2,                              // 2 INT,   UINT
				GlslBasicType::VEC2,      GlslBasicType::DVEC2,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::IVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 10. IVEC3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::IVEC3,     GlslBasicType::UVEC3,                              // 2 INT,   UINT
				GlslBasicType::VEC3,      GlslBasicType::DVEC3,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::IVEC3,     GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UVEC3,     GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 11. IVEC4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::IVEC4,     GlslBasicType::UVEC4,                              // 2 INT,   UINT
				GlslBasicType::VEC4,      GlslBasicType::DVEC4,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::IVEC4,     // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UVEC4,     // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 16 DMAT4X
			},
			// 12. UVEC2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::UVEC2,     GlslBasicType::UVEC2,                              // 2 INT,   UINT
				GlslBasicType::VEC2,      GlslBasicType::DVEC2,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 13. UVEC3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::UVEC3,     GlslBasicType::UVEC3,                              // 2 INT,   UINT
				GlslBasicType::VEC3,      GlslBasicType::DVEC3,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UVEC3,     GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UVEC3,     GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 14. UVEC4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::UVEC4,     GlslBasicType::UVEC4,                              // 2 INT,   UINT
				GlslBasicType::VEC4,      GlslBasicType::DVEC4,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UVEC4,     // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UVEC4,     // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 16 DMAT4X
			},
			// 15. VEC2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::VEC2,      GlslBasicType::VEC2,                               // 2 INT,   UINT
				GlslBasicType::VEC2,      GlslBasicType::DVEC2,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 16. VEC3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::VEC3,      GlslBasicType::VEC3,                               // 2 INT,   UINT
				GlslBasicType::VEC3,      GlslBasicType::DVEC3,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 17. VEC4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::VEC4,      GlslBasicType::VEC4,                               // 2 INT,   UINT
				GlslBasicType::VEC4,      GlslBasicType::DVEC4,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::VEC2,      GlslBasicType::VEC3,      GlslBasicType::VEC4,      // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 16 DMAT4X
			},
			// 18. DVEC2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DVEC2,     GlslBasicType::DVEC2,                              // 2 INT,   UINT
				GlslBasicType::DVEC2,     GlslBasicType::DVEC2,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 19. DVEC3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DVEC3,     GlslBasicType::DVEC3,                              // 2 INT,   UINT
				GlslBasicType::DVEC3,     GlslBasicType::DVEC3,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 20. DVEC4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DVEC4,      GlslBasicType::DVEC4,                             // 2 INT,   UINT
				GlslBasicType::DVEC4,      GlslBasicType::DVEC4,                             // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DVEC2,     GlslBasicType::DVEC3,     GlslBasicType::DVEC4,     // 16 DMAT4X
			},
			// 21. MAT2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT2,      GlslBasicType::MAT2,                               // 2 INT,   UINT
				GlslBasicType::MAT2,      GlslBasicType::DMAT2,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::MAT2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::MAT2X2,    GlslBasicType::MAT2X3,    GlslBasicType::MAT2X4,    // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DMAT2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 22. MAT3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT3,      GlslBasicType::MAT3,                               // 2 INT,   UINT
				GlslBasicType::MAT3,      GlslBasicType::DMAT3,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::MAT3,      GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::MAT3X2,    GlslBasicType::MAT3X3,    GlslBasicType::MAT3X4,    // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT3,     GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 23. MAT4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT4,      GlslBasicType::MAT4,                               // 2 INT,   UINT
				GlslBasicType::MAT4,      GlslBasicType::DMAT4,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::MAT4,      // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::MAT4X2,    GlslBasicType::MAT4X3,    GlslBasicType::MAT4X4,    // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT4,     // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 16 DMAT4X
			},
			// 24. MAT2X2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT2X2,    GlslBasicType::MAT2X2,                             // 2 INT,   UINT
				GlslBasicType::MAT2X2,    GlslBasicType::DMAT2X2,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::MAT2X2,    GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::MAT2X2,    GlslBasicType::MAT2X3,    GlslBasicType::MAT2X4,    // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DMAT2X2,   GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 25. MAT2X3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT2X3,    GlslBasicType::MAT2X3,                             // 2 INT,   UINT
				GlslBasicType::MAT2X3,    GlslBasicType::DMAT2X3,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC2,      GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::MAT2X3,    GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::MAT2X2,    GlslBasicType::MAT2X3,    GlslBasicType::MAT2X4,    // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT2X3,   GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 25. MAT2X4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT2X4,    GlslBasicType::MAT2X4,                             // 2 INT,   UINT
				GlslBasicType::MAT2X4,    GlslBasicType::DMAT2X4,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC2,      // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC2,      // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC2,      // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC2,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::MAT2X4,    // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::MAT2X2,    GlslBasicType::MAT2X3,    GlslBasicType::MAT2X4,    // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT2X4,   // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 16 DMAT4X
			},
			// 26. MAT3X2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT3X2,    GlslBasicType::MAT3X2,                             // 2 INT,   UINT
				GlslBasicType::MAT3X2,    GlslBasicType::DMAT3X2,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::MAT3X2,    GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::MAT3X2,    GlslBasicType::MAT3X3,    GlslBasicType::MAT3X4,    // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DMAT3X2,   GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 27. MAT3X3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT3X3,    GlslBasicType::MAT3X3,                             // 2 INT,   UINT
				GlslBasicType::MAT3X3,    GlslBasicType::DMAT3X3,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::MAT3X3,    GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::MAT3X2,    GlslBasicType::MAT3X3,    GlslBasicType::MAT3X4,    // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT3X3,   GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 28. MAT3X4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT3X4,    GlslBasicType::MAT3X4,                             // 2 INT,   UINT
				GlslBasicType::MAT3X4,    GlslBasicType::DMAT3X4,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC3,      // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::MAT3X4,    // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::MAT3X2,    GlslBasicType::MAT3X3,    GlslBasicType::MAT3X4,    // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT3X4,   // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 16 DMAT4X
			},
			// 29. MAT4X2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT4X2,    GlslBasicType::MAT4X2,                             // 2 INT,   UINT
				GlslBasicType::MAT4X2,    GlslBasicType::DMAT4X2,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::VEC4,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::VEC4,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::VEC4,      GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC4,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::MAT4X2,    GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::MAT4X2,    GlslBasicType::MAT4X3,    GlslBasicType::MAT4X4,    // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DMAT4X2,   GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 30. MAT4X3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT4X3,    GlslBasicType::MAT4X3,                             // 2 INT,   UINT
				GlslBasicType::MAT4X3,    GlslBasicType::DMAT4X3,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::MAT4X3,    GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::MAT4X2,    GlslBasicType::MAT4X3,    GlslBasicType::MAT4X4,    // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT4X3,   GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT4X4,   // 14 DMAT2X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 31. MAT4X4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::MAT4X4,    GlslBasicType::MAT4X4,                             // 2 INT,   UINT
				GlslBasicType::MAT4X4,    GlslBasicType::DMAT4X4,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::VEC4,      // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::MAT4X4,    // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::MAT4X2,    GlslBasicType::MAT4X3,    GlslBasicType::MAT4X4,    // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT4X4,   // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 16 DMAT4X
			},
			// 32. DMAT2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT2,     GlslBasicType::DMAT2,                              // 2 INT,   UINT
				GlslBasicType::DMAT2,     GlslBasicType::DMAT2,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::DMAT2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DMAT2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 33. DMAT3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT3,     GlslBasicType::DMAT3,                              // 2 INT,   UINT
				GlslBasicType::DMAT3,     GlslBasicType::DMAT3,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT3,     GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT3,     GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 34. DMAT4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT4,     GlslBasicType::DMAT4,                              // 2 INT,   UINT
				GlslBasicType::DMAT4,     GlslBasicType::DMAT4,                              // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT4,     // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT4,     // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 16 DMAT4X
			},
			// 35. DMAT2X2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X2,                            // 2 INT,   UINT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X2,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::DMAT2X2,   GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DMAT2X2,   GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 36. DMAT2X3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X3,                            // 2 INT,   UINT
				GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X3,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC2,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT2X3,   GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT2X3,   GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 37. DMAT2X4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT2X4,   GlslBasicType::DMAT2X4,                            // 2 INT,   UINT
				GlslBasicType::DMAT2X4,   GlslBasicType::DMAT2X4,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC2,     // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC2,     // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC2,     // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC2,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT2X4,   // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT2X4,   // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DMAT2X2,   GlslBasicType::DMAT2X3,   GlslBasicType::DMAT2X4,   // 16 DMAT4X
			},
			// 38. DMAT3X2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X2,                            // 2 INT,   UINT
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X2,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::DMAT3X2,   GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DMAT3X2,   GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 39. DMAT3X3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X3,                            // 2 INT,   UINT
				GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X3,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT3X3,   GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT3X3,   GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 40. DMAT3X4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT3X4,   GlslBasicType::DMAT3X4,                            // 2 INT,   UINT
				GlslBasicType::DMAT3X4,   GlslBasicType::DMAT3X4,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC3,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT3X4,   // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT3X4,   // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DMAT3X2,   GlslBasicType::DMAT3X3,   GlslBasicType::DMAT3X4,   // 16 DMAT4X
			},
			// 41. DMAT4X2
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X2,                            // 2 INT,   UINT
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X2,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::DVEC4,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::DVEC4,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::DVEC4,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::DVEC4,     GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::DMAT4X2,   GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::DMAT4X2,   GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 42. DMAT4X3
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X3,                            // 2 INT,   UINT
				GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X3,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     GlslBasicType::UNDEFINED, // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     GlslBasicType::UNDEFINED, // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     GlslBasicType::UNDEFINED, // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     GlslBasicType::UNDEFINED, // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT4X3,   GlslBasicType::UNDEFINED, // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 11 MAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::DMAT4X3,   GlslBasicType::UNDEFINED, // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT4X4,   // 14 DMAT2X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 15 DMAT3X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 16 DMAT4X
			},
			// 43. DMAT4X4
			{
				GlslBasicType::UNDEFINED,                                                   // 1 BOOL,
				GlslBasicType::DMAT4X4,   GlslBasicType::DMAT4X4,                            // 2 INT,   UINT
				GlslBasicType::DMAT4X4,   GlslBasicType::DMAT4X4,                            // 3 FLOAT, DOUBLE
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 4 BVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 5 IVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 6 UVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 7 VEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DVEC4,     // 8 DVEC
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT4X4,   // 9 MAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 10 MAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 11 MAT3X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 12 MAT4X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::DMAT4X4,   // 13 DMAT
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 14 DMAT2X
				GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, GlslBasicType::UNDEFINED, // 15 DMAT3X
				GlslBasicType::DMAT4X2,   GlslBasicType::DMAT4X3,   GlslBasicType::DMAT4X4,   // 16 DMAT4X
			},
		};
		*/

		//constexpr GlslBasicType rowsColsFloatTypeMap[][] = {
		//	{MAT1X1, MAT1X2, MAT1X3, MAT1X4},
		//	{MAT2X1, MAT2X2, MAT2X3, MAT2X4},
		//	{MAT3X1, MAT3X2, MAT3X3, MAT3X4},
		//	{MAT4X1, MAT4X2, MAT4X3, MAT4X4},
		//};
		static std::unordered_map<std::string_view, GlslBasicType> aliasTypeMap{
			{"mat2",  GlslBasicType::MAT2X2 },
			{"mat3",  GlslBasicType::MAT3X3 },
			{"mat4",  GlslBasicType::MAT4X4 },
			{"dmat2", GlslBasicType::DMAT2X2},
			{"dmat3", GlslBasicType::DMAT3X3},
			{"dmat4", GlslBasicType::DMAT4X4},
		};

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

		static constexpr GlslBasicType fundamentalTypeMap[] = {
			GlslBasicType::VOID,
			// Scalars.
			GlslBasicType::BOOL, GlslBasicType::INT, GlslBasicType::UINT, GlslBasicType::FLOAT, GlslBasicType::DOUBLE, // BOOL, INT, UINT, FLOAT, DOUBLE
			// Vectors.
			GlslBasicType::BOOL, GlslBasicType::INT, GlslBasicType::UINT, GlslBasicType::FLOAT, GlslBasicType::DOUBLE, // BVEC2, IVEC2, UVEC2, VEC2, DVEC2
			GlslBasicType::BOOL, GlslBasicType::INT, GlslBasicType::UINT, GlslBasicType::FLOAT, GlslBasicType::DOUBLE, // BVEC3, IVEC3, UVEC3, VEC3, DVEC3
			GlslBasicType::BOOL, GlslBasicType::INT, GlslBasicType::UINT, GlslBasicType::FLOAT, GlslBasicType::DOUBLE, // BVEC4, IVEC4, UVEC4, VEC4, DVEC4
			// Matrices.
			GlslBasicType::FLOAT, GlslBasicType::DOUBLE, GlslBasicType::FLOAT, GlslBasicType::DOUBLE, GlslBasicType::FLOAT, GlslBasicType::DOUBLE, // MAT2X2, DMAT2X2, MAT2X3, DMAT2X3, MAT2X4, DMAT2X4
			GlslBasicType::FLOAT, GlslBasicType::DOUBLE, GlslBasicType::FLOAT, GlslBasicType::DOUBLE, GlslBasicType::FLOAT, GlslBasicType::DOUBLE, // MAT3X2, DMAT3X2, MAT3X3, DMAT3X3, MAT3X4, DMAT3X4
			GlslBasicType::FLOAT, GlslBasicType::DOUBLE, GlslBasicType::FLOAT, GlslBasicType::DOUBLE, GlslBasicType::FLOAT, GlslBasicType::DOUBLE, // MAT4X2, DMAT4X2, MAT4X3, DMAT4X3, MAT4X4, DMAT4X4
		};

		// 'matRowsColsOffset' integer is used to simplify indexing the two arrays below.
		// Using it we "shift" the type ids of the matrix types in the 'GlslBasicType' enumeration to 0.
		static constexpr int matRowsColsOffset = static_cast<int>(GlslBasicType::MAT2X2);
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
		// Using it we "shift" the type ids of the vector types in the 'GlslBasicType' enumeration to 0.
		static constexpr int vecRowsColsOffset = static_cast<int>(GlslBasicType::BVEC2);
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

		bool GlslExprType::BasicType() const {
			return !CustomType();
		}
		bool GlslExprType::CustomType() const {
			return type == GlslBasicType::CUSTOM;
		}
		bool GlslExprType::AnonymousCustomType() const {
			if (name.empty())
				return false;
			return name[0] == '$';
		}
		bool GlslExprType::Array() const {
			return !dimensions.empty();
		}

		bool operator==(const GlslExprType& type1, const GlslExprType& type2) {
			bool typesEqual{false};
			if (type1.type == GlslBasicType::CUSTOM && type2.type == GlslBasicType::CUSTOM) {
				// 1. Custom types. They are equal iff their type names are the same.
				// Even though anonymous structure don't have user-defined names,
				// the compiler still gives them unique internal names, so the check applies to them too.
				typesEqual = type1.name == type2.name;
			}
			// 2. One of the types is a custom type, or
			// 3. both types are basic types.
			// In any case, it's enough to check their base types.
			// (CUSTOM != to any of the basic types)
			typesEqual = type1.type == type2.type;
			
			// Check if the types are arrays.
			// If so, check the number of dimensions and their sizes.
			// Both must be equal.

			// It's enough to check whether one of the types is an array type.
			// If one of them is not an array type, its dimensions count will be 0, which is
			// guaranteed not to equal to the dimensions count of the array type.
			if (type1.Array() || type2.Array()) {
				if (type1.dimensions.size() == type2.dimensions.size()) {
					// Both types are arrays and their dimensions are > 0.
					// (We could've also chosen 'type2.dimensions.size()' in this case.
					for (size_t i = 0; i < type1.dimensions.size(); i++) {
						if (type1.dimensions[i] != type2.dimensions[i]) {
							// The sizes of the same dimension differ, the types are not equal.
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
		bool TypesEqual(const GlslExprType& type1, const GlslExprType& type2) {
			return type1 == type2;
		}
		bool TypePromotable(const GlslExprType& check, const GlslExprType& promoteTo) {
			//if (TypesEqual(check, promoteTo))
			//	return true;
			if (check.BasicType() && promoteTo.BasicType()) {
				// 1. Both types are basic. These are the types that have a keyword in the language.
				// First we check if the fundamental type is promotable or not.
				int checkRank = GetFundamentalTypeRank(check.type);
				int promoteToRank = GetFundamentalTypeRank(promoteTo.type);
				if (checkRank == 0 || promoteToRank == 0 || checkRank > promoteToRank)
					return false;
				// Then, based on what category the types belong to, we check the types' sizes.
				if (IsVectorType(check.type) && IsVectorType(promoteTo.type)) {
					// 1.1 Both types are vector types.
					// First we check the vectors' sizes, and then we do the same checks
					// as the ones for scalar types but using the vectors' fundamental types.
					if (GetColVecNumberOfRows(check.type) != GetColVecNumberOfRows(promoteTo.type))
						return false;
				} else if (IsMatrixType(check.type) && IsMatrixType(promoteTo.type)) {
					// 1.2 Both types are matrix types.
					if (GetMatNumberOfRows(check.type) != GetMatNumberOfRows(promoteTo.type) ||
						GetMatNumberOfCols(check.type) != GetMatNumberOfCols(promoteTo.type))
						return false;
				} else {
					// 1.3 If both types are scalars, then the 'check' type is promotable if:
					// - None of the types is a BOOL
					// - The 'check' operand type's rank is lower than the rank of the 'promoteTo' operand's type.
					// We've already done these checks, so if both types are scalars we simply move on.
					// If the check below is 'true', the the types' categories don't match, which is not promotable.
					if (!IsScalarType(check.type) && !IsScalarType(promoteTo.type))
						// 1.4 Types are mixed, which is not promotable.
						return false;
				}
			} else if (check.CustomType() && promoteTo.CustomType()) {
				// 2. Both types are user-defined structures.
				// User-defined types cannot be implicitly converted to any other type.
				// The only valid cases is when both user-defined types are the same, which is exactly what we check here.
				if (check.name != promoteTo.name)
					return false;
			} else {
				// 3. Types' categories don't match, can't promote a basic type to a custom one, or the other way around.
				return false;
			}
			// Types can be arrays, which must be considered as well.
			if (check.Array() || promoteTo.Array()) {
				// 4. Any of the types is an array. The reason we don't check if both types
				//    are arrays is because when one is an array and the other one is not,
				//    the non-array type's dimension of 0 is not going to be equal to the
				//    dimension of the array type be definition. This means the check that follows
				//    will be false, which is what we expect for a case like that.
				if (check.dimensions.size() != promoteTo.dimensions.size())
					return false;
				for (size_t i = 0; i < check.dimensions.size(); i++) {
					if (check.dimensions[i] != promoteTo.dimensions[i]) {
						return false;
					}
				}
			}
			// All checks were successful, the 'check' type can be promoted to the 'promoteTo' type.
			return true;
		}

		bool IsTypeBasic(TokenType tokenType) {
			if (tokenType >= TokenType::VOID &&
				tokenType <= TokenType::UIMAGE2DMSARRAY) {
				return true;
			}
			return false;
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
				tokenType <= TokenType::DOUBLE) {
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

		GlslBasicType TokenTypeToGlslBasicType(TokenType tokenType) {
			if (tokenType == TokenType::IDENTIFIER)
				return GlslBasicType::CUSTOM;
			int idx = static_cast<int>(tokenType) - tokenTypeOffset;
			assert(idx >= 0 && idx <= static_cast<int>(GlslBasicType::DMAT4X4) &&
				"TokenType is out of range of Basic Types!");
			return tokenTypeToGlslExprTypeMap[idx];
		}
		TokenType GlslBasicTypeToTokenType(GlslBasicType glslBasicType) {
			if (glslBasicType == GlslBasicType::CUSTOM)
				return TokenType::IDENTIFIER;
			int idx = static_cast<int>(glslBasicType) + tokenTypeOffset;
			assert(idx >= static_cast<int>(TokenType::BOOL) &&
				   idx <= static_cast<int>(TokenType::DMAT4X4) &&
				   "GlslBasicType is out of range of Token Types!");
			return static_cast<TokenType>(idx);
		}

		static std::unordered_map<GlslBasicType, std::string_view> glslBasicTypeToStrMap{
			{GlslBasicType::VOID,   "void"  },
			{GlslBasicType::BOOL,   "bool"  },
			{GlslBasicType::INT,    "int"   },
			{GlslBasicType::UINT,   "uint"  },
			{GlslBasicType::FLOAT,  "float" },
			{GlslBasicType::DOUBLE, "double"},

			{GlslBasicType::BVEC2,  "bvec2" },
			{GlslBasicType::IVEC2,  "ivec2" },
			{GlslBasicType::UVEC2,  "uvec2" },
			{GlslBasicType::VEC2,   "vec2"  },
			{GlslBasicType::DVEC2,  "dvec2" },

			{GlslBasicType::BVEC3,  "bvec3" },
			{GlslBasicType::IVEC3,  "ivec3" },
			{GlslBasicType::UVEC3,  "uvec3" },
			{GlslBasicType::VEC3,   "vec3"  },
			{GlslBasicType::DVEC3,  "dvec3" },

			{GlslBasicType::BVEC4,  "bvec4" },
			{GlslBasicType::IVEC4,  "ivec4" },
			{GlslBasicType::UVEC4,  "uvec4" },
			{GlslBasicType::VEC4,   "vec4"  },
			{GlslBasicType::DVEC4,  "dvec4" },

			{GlslBasicType::MAT2X2 , "mat2x2" },
			{GlslBasicType::DMAT2X2, "dmat2x2"},
			{GlslBasicType::MAT2X3 , "mat2x3" },
			{GlslBasicType::DMAT2X3, "dmat2x3"},
			{GlslBasicType::MAT2X4 , "mat2x4" },
			{GlslBasicType::DMAT2X4, "dmat2x4"},

			{GlslBasicType::MAT3X2 , "mat3x2" },
			{GlslBasicType::DMAT3X2, "dmat3x2"},
			{GlslBasicType::MAT3X3 , "mat3x3" },
			{GlslBasicType::DMAT3X3, "dmat3x3"},
			{GlslBasicType::MAT3X4 , "mat3x4" },
			{GlslBasicType::DMAT3X4, "dmat3x4"},

			{GlslBasicType::MAT4X2 , "mat4x2" },
			{GlslBasicType::DMAT4X2, "dmat4x2"},
			{GlslBasicType::MAT4X3 , "mat4x3" },
			{GlslBasicType::DMAT4X3, "dmat4x3"},
			{GlslBasicType::MAT4X4 , "mat4x4" },
			{GlslBasicType::DMAT4X4, "dmat4x4"},
		};
		std::string_view GetGlslBasicTypeName(GlslBasicType glslType) {
			if (glslType == GlslBasicType::UNDEFINED ||
				glslType == GlslBasicType::CUSTOM ||
				glslType == GlslBasicType::COUNT) {
				return "";
			}
			auto searchRes = glslBasicTypeToStrMap.find(glslType);
			assert(searchRes != glslBasicTypeToStrMap.end() && "Couldn't find the GLSL basic type provided!");
			if (searchRes == glslBasicTypeToStrMap.end()) {
				return "";
			}
			return searchRes->second;
		}

		std::array<std::string_view, 5> fundamentalTypeToStrMap = {
			std::string_view{"bool"  },
			std::string_view{"int"   },
			std::string_view{"uint"  },
			std::string_view{"float" },
			std::string_view{"double"},
		};
		std::string_view GetFundamentalTypeName(GlslBasicType glslType) {
			assert(IsFundamentalType(glslType) && "The type provided is not fundamental!");
			return fundamentalTypeToStrMap[static_cast<size_t>(glslType)];
		}

		GlslBasicType GetAliasType(std::string_view alias) {
			auto searchRes = aliasTypeMap.find(alias);
			if (searchRes == aliasTypeMap.end()) {
				return GlslBasicType::UNDEFINED;
			}
			return searchRes->second;
		}

		bool IsMaterialPropertyType(TokenType tokenType) {
			return tokenType >= TokenType::MAT_PROP_TYPE_INT &&
				   tokenType <= TokenType::MAT_PROP_TYPE_TEX2D;
		}
		TokenType MapMaterialPropertyType(TokenType tokenType) {
			assert(tokenType >= TokenType::MAT_PROP_TYPE_INT && tokenType <= TokenType::MAT_PROP_TYPE_TEX2D &&
				   "Not a material property type provided!");
			int idx = static_cast<int>(tokenType) - materialPropertyTypeOffset;
			return materialPropertyTypeToGlslType[idx];
		}

		int GetFundamentalTypeRank(GlslBasicType type) {
			return static_cast<int>(GetFundamentalType(type));
		}
		GlslBasicType GetFundamentalType(GlslBasicType type) {
			return static_cast<GlslBasicType>(fundamentalTypeMap[static_cast<size_t>(type)]);
		}

		size_t GetDimensionCountNonArray(GlslBasicType type) {
			assert(type != GlslBasicType::COUNT && "Not a type!");
			assert(type != GlslBasicType::UNDEFINED && "Undefined type is not allowed!");
			assert(type != GlslBasicType::CUSTOM && "Custom types can't have dimensions!");
			if (IsScalarType(type)) {
				return 1;
			}
			if (IsVectorType(type)) {
				return GetColVecNumberOfRows(type);
			}
			if (IsMatrixType(type)) {
				size_t rows = GetMatNumberOfRows(type);
				size_t cols = GetMatNumberOfCols(type);
				return rows * cols;
			}
			// To avoid a warning message.
			return 0;
		}

		static constexpr GlslBasicType rowsColsFloatTypeMap[4][4] = {
			{GlslBasicType::FLOAT, GlslBasicType::VEC2,   GlslBasicType::VEC3,   GlslBasicType::VEC4  },
			{GlslBasicType::VEC2,  GlslBasicType::MAT2X2, GlslBasicType::MAT2X3, GlslBasicType::MAT2X4},
			{GlslBasicType::VEC3,  GlslBasicType::MAT3X2, GlslBasicType::MAT3X3, GlslBasicType::MAT3X4},
			{GlslBasicType::VEC4,  GlslBasicType::MAT4X2, GlslBasicType::MAT4X3, GlslBasicType::MAT4X4},
		};
		static constexpr GlslBasicType rowsColsDoubleTypeMap[4][4] = {
			{GlslBasicType::DOUBLE, GlslBasicType::DVEC2,   GlslBasicType::DVEC3,   GlslBasicType::DVEC4  },
			{GlslBasicType::DVEC2,  GlslBasicType::DMAT2X2, GlslBasicType::DMAT2X3, GlslBasicType::DMAT2X4},
			{GlslBasicType::DVEC3,  GlslBasicType::DMAT3X2, GlslBasicType::DMAT3X3, GlslBasicType::DMAT3X4},
			{GlslBasicType::DVEC4,  GlslBasicType::DMAT4X2, GlslBasicType::DMAT4X3, GlslBasicType::DMAT4X4},
		};
		GlslBasicType GetGlslBasicTypeRowsCols(size_t rows, size_t cols) {
			assert(!(rows == cols == 1) && "Scalar can't be represented as a 1x1 matrix!");
			return rowsColsFloatTypeMap[rows - 1][cols - 1];
		}
		GlslBasicType GetGlslBasicTypeRowsCols(GlslBasicType fundamentalType, size_t rows, size_t cols) {
			assert(IsFundamentalType(fundamentalType) && "Not a fundamental type provided!");
			if (!IsFundamentalType(fundamentalType)) {
				return GlslBasicType::UNDEFINED;
			}
			assert(fundamentalType != GlslBasicType::FLOAT &&
				   fundamentalType != GlslBasicType::DOUBLE &&
				   "Only FLOAT or DOUBLE can be represented as a matrix!");
			if (fundamentalType != GlslBasicType::FLOAT &&
				fundamentalType != GlslBasicType::DOUBLE) {
				return GlslBasicType::UNDEFINED;
			}
			assert(!(rows == cols == 1) && "Scalar can't be represented as a 1x1 matrix!");
			if (fundamentalType == GlslBasicType::FLOAT) {
				return rowsColsFloatTypeMap[rows - 1][cols - 1];
			} else /*if (fundamentalType == GlslBasicType::DOUBLE)*/ {
				return rowsColsDoubleTypeMap[rows - 1][cols - 1];
			}
		}

		size_t GetNumberOfRows(GlslBasicType type, OperandPos pos) {
			if (IsMatrixType(type)) {
				return GetMatNumberOfRows(type);
			} else if (IsVectorType(type)) {
				return GetVecNumberOfRows(type, pos);
			} else /*if (IsScalarType(type))*/ {
				return 0;
			}
		}
		size_t GetNumberOfCols(GlslBasicType type, OperandPos pos) {
			if (IsMatrixType(type)) {
				return GetMatNumberOfCols(type);
			} else if (IsVectorType(type)) {
				return GetVecNumberOfCols(type, pos);
			} else /*if (IsScalarType(type))*/ {
				return 0;
			}
		}

		size_t GetVecNumberOfRows(GlslBasicType type, OperandPos pos) {
			// LHS vectors are treated as row vectors and
			// RHS vectors as column vectors.
			if (pos == OperandPos::LHS) {
				return GetRowVecNumberOfRows(type);
			} else /*(pos == OperandPos::RHS)*/ {
				return GetColVecNumberOfRows(type);
			}
		}
		size_t GetVecNumberOfCols(GlslBasicType type, OperandPos pos) {
			// LHS vectors are treated as row vectors and
			// RHS vectors as column vectors.
			if (pos == OperandPos::LHS) {
				return GetRowVecNumberOfCols(type);
			} else /*(pos == OperandPos::RHS)*/ {
				return GetColVecNumberOfCols(type);
			}
		}

		size_t GetColVecNumberOfRows(GlslBasicType type) {
			assert(IsVectorType(type) && "Not a vector type provided!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return vecRowsTypeMap[idx];
		}
		size_t GetColVecNumberOfCols(GlslBasicType type) {
			assert(IsVectorType(type) && "Not a vector type provided!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return vecColsTypeMap[idx];
		}
		size_t GetRowVecNumberOfRows(GlslBasicType type) {
			assert(IsVectorType(type) && "Not a vector type provided!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return vecColsTypeMap[idx];
		}
		size_t GetRowVecNumberOfCols(GlslBasicType type) {
			assert(IsVectorType(type) && "Not a vector type provided!");
			int idx = static_cast<int>(type) - vecRowsColsOffset;
			return vecRowsTypeMap[idx];
		}

		size_t GetMatNumberOfRows(GlslBasicType type) {
			assert(IsMatrixType(type) && "Not a matrix type provided!");
			int idx = static_cast<int>(type) - matRowsColsOffset;
			return matRowsTypeMap[idx];
		}
		size_t GetMatNumberOfCols(GlslBasicType type) {
			assert(IsMatrixType(type) && "Not a matrix type provided!");
			int idx = static_cast<int>(type) - matRowsColsOffset;
			return matColsTypeMap[idx];
		}

		static constexpr GlslBasicType fundamentalTypeToVectorType[][3] = {
			{GlslBasicType::BVEC2, GlslBasicType::BVEC3, GlslBasicType::BVEC4}, // BOOL
			{GlslBasicType::IVEC2, GlslBasicType::IVEC3, GlslBasicType::IVEC4}, // INT
			{GlslBasicType::UVEC2, GlslBasicType::UVEC3, GlslBasicType::UVEC4}, // UINT
			{GlslBasicType::VEC2,  GlslBasicType::VEC3,  GlslBasicType::VEC4 }, // FLOAT
			{GlslBasicType::DVEC2, GlslBasicType::DVEC3, GlslBasicType::DVEC4}, // DOUBLE
		};
		static constexpr int fundamentalTypeToVectorTypeOffset = 2;
		TokenType FundamentalTypeToVectorType(TokenType tokenType, size_t dimension) {
			GlslBasicType glslBasicVectorType = FundamentalTypeToVectorType(TokenTypeToGlslBasicType(tokenType), dimension);
			return GlslBasicTypeToTokenType(glslBasicVectorType);
		}
		TokenType FundamentalTypeToMatrixType(TokenType tokenType, size_t rows, size_t cols) {
			GlslBasicType glslBasicMatrixType = FundamentalTypeToMatrixType(TokenTypeToGlslBasicType(tokenType), rows, cols);
			return GlslBasicTypeToTokenType(glslBasicMatrixType);
		}
		GlslBasicType FundamentalTypeToVectorType(GlslBasicType glslBasicType, size_t dimension) {
			assert(IsFundamentalType(glslBasicType) && "Not a fundamental type provided!");
			if (!IsFundamentalType(glslBasicType)) {
				return GlslBasicType::UNDEFINED;
			}
			int idx = static_cast<int>(dimension) - fundamentalTypeToVectorTypeOffset;
			return fundamentalTypeToVectorType[static_cast<int>(glslBasicType)][idx];
		}
		GlslBasicType FundamentalTypeToMatrixType(GlslBasicType glslBasicType, size_t rows, size_t cols) {
			assert(IsFundamentalType(glslBasicType) && "Not a fundamental type provided!");
			if (!IsFundamentalType(glslBasicType)) {
				return GlslBasicType::UNDEFINED;
			}
			if (glslBasicType != GlslBasicType::FLOAT && glslBasicType != GlslBasicType::DOUBLE) {
				return GlslBasicType::UNDEFINED;
			}
			return GetGlslBasicTypeRowsCols(glslBasicType, rows, cols);
		}

		GlslBasicType PromoteType(GlslBasicType type, int rankDiff) {
#if defined (_DEBUG) || defined(DEBUG)
			if (IsScalarType(type)) return PromoteScalarType(type, rankDiff);
			if (IsVectorType(type)) return PromoteVectorType(type, rankDiff);
			if (IsMatrixType(type)) return PromoteMatrixType(type, rankDiff);
#endif
			return static_cast<GlslBasicType>(static_cast<int>(type) + rankDiff);
		}
		GlslBasicType PromoteFundamentalType(GlslBasicType type, int rankDiff) {
			assert((rankDiff >= -3 && rankDiff <= 3) && "Rank promotion number must be within the [-3, 3] range!");
			assert(IsFundamentalType(type) && "Type must be fundamental!");
			GlslBasicType promotedType = static_cast<GlslBasicType>(static_cast<int>(type) + rankDiff);
			assert(IsFundamentalType(promotedType) && "Promoted type must stay fundamental!");
			return promotedType;
		}
		GlslBasicType PromoteScalarType(GlslBasicType type, int rankDiff) {
			return PromoteFundamentalType(type, rankDiff);
		}
		GlslBasicType PromoteVectorType(GlslBasicType type, int rankDiff) {
			assert((rankDiff >= -3 && rankDiff <= 3) && "Rank promotion number must be within the [-3, 3] range!");
			assert(IsVectorType(type) && "Type must be one of the vector types!");
			GlslBasicType promotedType = static_cast<GlslBasicType>(static_cast<int>(type) + rankDiff);
			assert(IsVectorType(promotedType) && "Promoted type must stay one of the vector types!");
			return promotedType;
		}
		GlslBasicType PromoteMatrixType(GlslBasicType type, int rankDiff) {
			assert((rankDiff >= -1 && rankDiff <= 1) && "Rank promotion number must be within the [-1, 1] range!");
			assert(IsMatrixType(type) && "Type must be one of the matrix types!");
			GlslBasicType promotedType = static_cast<GlslBasicType>(static_cast<int>(type) + rankDiff);
			assert(IsMatrixType(promotedType) && "Promoted type must stay one of the matrix types!");
			return promotedType;
		}

		bool FundamentalTypeBool(GlslBasicType type) {
			return GetFundamentalType(type) == GlslBasicType::BOOL;
		}
		bool FundamentalTypeInt(GlslBasicType type) {
			return GetFundamentalType(type) == GlslBasicType::INT;
		}
		bool FundamentalTypeUint(GlslBasicType type) {
			return GetFundamentalType(type) == GlslBasicType::UINT;
		}
		bool FundamentalTypeFloat(GlslBasicType type) {
			return GetFundamentalType(type) == GlslBasicType::FLOAT;
		}
		bool FundamentalTypeDouble(GlslBasicType type) {
			return GetFundamentalType(type) == GlslBasicType::DOUBLE;
		}

		bool IsFundamentalType(GlslBasicType type) {
			return type >= GlslBasicType::BOOL && type <= GlslBasicType::DOUBLE;
		}
		bool IsScalarType(GlslBasicType type) {
			return IsFundamentalType(type);
		}
		bool IsIntegralType(GlslBasicType type) {
			return type >= GlslBasicType::INT && type <= GlslBasicType::UINT;
		}
		bool IsFloatingType(GlslBasicType type) {
			return type >= GlslBasicType::FLOAT && type <= GlslBasicType::DOUBLE;
		}
		bool IsVectorType(GlslBasicType type) {
			return type >= GlslBasicType::BVEC2 && type <= GlslBasicType::DVEC4;
		}
		bool IsMatrixType(GlslBasicType type) {
			return type >= GlslBasicType::MAT2X2 && type <= GlslBasicType::DMAT4X4;
		}

		bool AddSubDivAllowed(GlslBasicType lhs, GlslBasicType rhs) {
			if (IsScalarType(lhs) || IsScalarType(rhs)) {
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
				if (IsVectorType(lhs) && IsVectorType(rhs)) {
					// 1)
					size_t lhsVecSize = GetColVecNumberOfRows(lhs);
					size_t rhsVecSize = GetColVecNumberOfRows(rhs);
					return lhsVecSize == rhsVecSize;
				} else if (IsMatrixType(lhs) && IsMatrixType(rhs)) {
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
		bool AdditionAllowed(GlslBasicType lhs, GlslBasicType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}
		bool SubtractionAllowed(GlslBasicType lhs, GlslBasicType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}
		bool MultiplicationAllowed(GlslBasicType lhs, GlslBasicType rhs) {
			if (IsScalarType(lhs) || IsScalarType(rhs)) {
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
		bool DivisionAllowed(GlslBasicType lhs, GlslBasicType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}

		GlslBasicType LookupExprType(GlslBasicType lhs, GlslBasicType rhs, TokenType op) {
			// Use the lookup table to figure out what type the result of the expression should have.
			// GlslBasicType inferredType = binaryOpType[static_cast<size_t>(lhs)][static_cast<size_t>(rhs)];
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
				// return GlslBasicType::UNDEFINED;
			// }
			// return inferredType;
			return GlslBasicType::UNDEFINED;
		}
		GlslBasicType InferExprType(GlslBasicType lhs, GlslBasicType rhs, TokenType op) {
			// First of all, we need to figure out what type of operation is applied.
			if (op == TokenType::PLUS || op == TokenType::DASH ||
				op == TokenType::STAR || op == TokenType::SLASH) {
				// 1. Arithmetic binary operation.
				return InferArithmeticBinaryExprType(lhs, rhs, op);
			}
			// TODO: implement other types of operations.
			return GlslBasicType::UNDEFINED;
		}
		GlslBasicType InferArithmeticBinaryExprType(GlslBasicType lhs, GlslBasicType rhs, TokenType op) {
			// Following the specification's explanation on p.123
			// Link: https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.pdf
			// If any of the types' fundamental type is BOOL, the result is UNDEFINED, because according to the specification
			// only integer and floating-point scalars, vectors, and matrices are allowed with the arithmetic binary operators.
			if (FundamentalTypeBool(lhs) || FundamentalTypeBool(rhs)) {
				return GlslBasicType::UNDEFINED;
			}
			if (IsScalarType(lhs) && IsScalarType(rhs)) {
				// 1. Both operands are scalars.
				// The operation is applied resulting in a scalar of the "biggest" type.
				// If both types are the same, the result will be of that type.
				// The "biggest" type means the type that goes later in the 'GlslBasicType' enumeration.
				// The way the types are listed there follows the implicit conversion rules outlined in the specification.
				// INT -> UINT -> FLOAT -> DOUBLE, where UINT is "bigger" than INT, for example,
				// and DOUBLE is the "biggest" type of all the fundamental types.
				size_t typeId = std::max(static_cast<size_t>(lhs), static_cast<size_t>(rhs));
				return static_cast<GlslBasicType>(typeId);
			}
			if (IsScalarType(lhs) || IsScalarType(rhs)) {
				// 2. One of the types is a scalar while the other one is either a vector or a matrix.
				// According to the specification the operation is applied to each component
				// of the non-scalar operand resulting in the same size vector or matrix.
				// First we need to figure out which operand is a scalar and which one is a vector or matrix.
				if (IsScalarType(lhs)) {
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
			if (IsVectorType(lhs) && IsVectorType(rhs)) {
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
					return GlslBasicType::UNDEFINED;
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
				if (IsVectorType(lhs)) {
					// Left-hand side operand is a vector, it is treated as a row vector.
					rowsLhs = GetRowVecNumberOfRows(lhs);
					colsLhs = GetRowVecNumberOfCols(lhs);
					rowsRhs = GetMatNumberOfRows(rhs);
					colsRhs = GetMatNumberOfCols(rhs);
				} else if (IsVectorType(rhs)) {
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
					GlslBasicType fundamentalTypeLhs = GetFundamentalType(lhs);
					GlslBasicType fundamentalTypeRhs = GetFundamentalType(rhs);
					// Should be either FLOAT or DOUBLE.
					GlslBasicType biggestFundamentalType = std::max(fundamentalTypeLhs, fundamentalTypeRhs);
					GlslBasicType inferredType = GetGlslBasicTypeRowsCols(rowsLhs, colsRhs);

					constexpr GlslBasicType inferredFundamentalType = GlslBasicType::FLOAT;
					// GlslBasicType inferredFundamentalType = GetFundamentalType(inferredType);
					int rankDiff = static_cast<int>(biggestFundamentalType) - static_cast<int>(inferredFundamentalType);
					return PromoteType(inferredType, rankDiff);
				} else {
					return GlslBasicType::UNDEFINED;
				}
			} else {
				// 2) Otherwise we're dealing with either addition, subtraction, or devision operation.
				// In any case we need both operands' dimensions to match.
				// Since at this point we're sure that both operands are not both vectors,
				// the only valid case for the +, -, and / arithmetic binary operators is when
				// both operands are matrices of the same size.
				// In other words, if one of the operands is a vector, the operation is not defined.
				if (IsVectorType(lhs) || IsVectorType(rhs)) {
					return GlslBasicType::UNDEFINED;
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
					return GlslBasicType::UNDEFINED;
				}
			}
			return GlslBasicType::UNDEFINED;
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
		bool TypeSpec::IsStructure() const {
			return type.tokenType == TokenType::IDENTIFIER && !typeDecl;
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

	}
}