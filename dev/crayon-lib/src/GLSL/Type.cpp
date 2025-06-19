#include "GLSL/Type.h"

#include <algorithm>
#include <cassert>

namespace crayon {
	namespace glsl {

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

		constexpr GlslExprType rowsColsTypeMap[][4] = {
			{GlslExprType::FLOAT, GlslExprType::VEC2,   GlslExprType::VEC3,   GlslExprType::VEC4  },
			{GlslExprType::VEC2,  GlslExprType::MAT2X2, GlslExprType::MAT2X3, GlslExprType::MAT2X4},
			{GlslExprType::VEC3,  GlslExprType::MAT3X2, GlslExprType::MAT3X3, GlslExprType::MAT3X4},
			{GlslExprType::VEC4,  GlslExprType::MAT4X2, GlslExprType::MAT4X3, GlslExprType::MAT4X4},
		};

		GlslExprType GetFundamentalType(GlslExprType type) {
			if (FundamentalTypeBool(type)) return GlslExprType::BOOL;
			if (FundamentalTypeInt(type)) return GlslExprType::INT;
			if (FundamentalTypeUint(type)) return GlslExprType::UINT;
			if (FundamentalTypeFloat(type)) return GlslExprType::FLOAT;
			if (FundamentalTypeDouble(type)) return GlslExprType::DOUBLE;
			return GlslExprType::UNDEFINED;
		}
		int GetFundamentalTypeRank(GlslExprType type) {
			return static_cast<int>(GetFundamentalType(type));
		}

		int GetNumberOfRows(GlslExprType type) {
			// TODO
			return 0;
		}
		int GetNumberOfCols(GlslExprType type) {
			if (VectorType(type)) {
				// TODO
			}
			if (MatrixType(type)) {
				// TODO
			}
			if (ScalarType(type)) {
				// TODO
			}
			return 0;
		}

		GlslExprType PromoteType(GlslExprType type, int rank) {
			if (ScalarType(type)) return PromoteScalarType(type, rank);
			if (VectorType(type)) return PromoteVectorType(type, rank);
			if (MatrixType(type)) return PromoteMatrixType(type, rank);
			return GlslExprType::UNDEFINED;
		}
		GlslExprType PromoteFundamentalType(GlslExprType type, int rank) {
			assert((rank >= -3 && rank <= 3) && "Rank promotion number must be within the [-3, 3] range!");
			assert(FundamentalType(type) && "Type must be fundamental!");
			GlslExprType promotedType = static_cast<GlslExprType>(static_cast<int>(type) + rank);
			assert(FundamentalType(promotedType) && "Promoted type must stay fundamental!");
			return promotedType;
		}
		GlslExprType PromoteScalarType(GlslExprType type, int rank) {
			return PromoteFundamentalType(type, rank);
		}
		GlslExprType PromoteVectorType(GlslExprType type, int rank) {
			assert((rank >= -3 && rank <= 3) && "Rank promotion number must be within the [-3, 3] range!");
			assert(VectorType(type) && "Type must be one of the vector types!");
			GlslExprType promotedType = static_cast<GlslExprType>(static_cast<int>(type) + rank * 3);
			assert(VectorType(promotedType) && "Promoted type must stay one of the vector types!");
			return promotedType;
		}
		GlslExprType PromoteMatrixType(GlslExprType type, int rank) {
			assert((rank >= -1 && rank <= 1) && "Rank promotion number must be within the [-1, 1] range!");
			assert(MatrixType(type) && "Type must be one of the matrix types!");
			GlslExprType promotedType = static_cast<GlslExprType>(static_cast<int>(type) + rank * 12);
			assert(MatrixType(promotedType) && "Promoted type must stay one of the matrix types!");
			return promotedType;
		}

		bool FundamentalTypeBool(GlslExprType type) {
			return type == GlslExprType::BOOL ||
				   (type >= GlslExprType::BVEC2 && type <= GlslExprType::BVEC4);
		}

		bool FundamentalTypeInt(GlslExprType type) {
			return type == GlslExprType::INT ||
				   (type >= GlslExprType::IVEC2 && type <= GlslExprType::IVEC4);
		}
		bool FundamentalTypeUint(GlslExprType type) {
			return type == GlslExprType::UINT ||
				   (type >= GlslExprType::UVEC2 && type <= GlslExprType::UVEC4);
		}

		bool FundamentalTypeFloat(GlslExprType type) {
			return type == GlslExprType::FLOAT ||
				   (type >= GlslExprType::VEC2 && type <= GlslExprType::VEC4) ||
				   (type >= GlslExprType::MAT2 && type <= GlslExprType::MAT4X4);
		}
		bool FundamentalTypeDouble(GlslExprType type) {
			return type == GlslExprType::DOUBLE ||
				   (type >= GlslExprType::DVEC2 && type <= GlslExprType::DVEC4) ||
				   (type >= GlslExprType::DMAT2 && type <= GlslExprType::DMAT4X4);
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
			return type >= GlslExprType::BVEC2 && type <= GlslExprType::VEC4;
		}
		bool MatrixType(GlslExprType type) {
			return type >= GlslExprType::MAT2 && type <= GlslExprType::DMAT4X4;
		}

		bool TypesEqual(GlslExprType type1, GlslExprType type2) {
			bool typesEqual = type1 == type2;
			if (!typesEqual) {
				// Check aliases too.
				if (type1 == GlslExprType::MAT2 && type2 == GlslExprType::MAT2X2 ||
					type1 == GlslExprType::MAT2X2 && type2 == GlslExprType::MAT2 ||
					type1 == GlslExprType::MAT3 && type2 == GlslExprType::MAT3X3 ||
					type1 == GlslExprType::MAT3X3 && type2 == GlslExprType::MAT3 ||
					type1 == GlslExprType::MAT4 && type2 == GlslExprType::MAT4X4 ||
					type1 == GlslExprType::MAT4X4 && type2 == GlslExprType::MAT4) {
					return true;
				}
			}
			return typesEqual;
		}

		bool AddSubDivAllowed(GlslExprType lhs, GlslExprType rhs) {
			if (!ScalarType(lhs) && !ScalarType(rhs)) {
				if (lhs == rhs) return true;
				// Check aliases:
				// 2x2 matrix?
				if (lhs == GlslExprType::MAT2 && rhs == GlslExprType::MAT2X2 ||
					lhs == GlslExprType::MAT2X2 && rhs == GlslExprType::MAT2) {
					return true;
				}
				// 3x3 matrix?
				if (lhs == GlslExprType::MAT3 && rhs == GlslExprType::MAT3X3 ||
					lhs == GlslExprType::MAT3X3 && rhs == GlslExprType::MAT3) {
					return true;
				}
				// 4x4 matrix?
				if (lhs == GlslExprType::MAT4 && rhs == GlslExprType::MAT4X4 ||
					lhs == GlslExprType::MAT4X4 && rhs == GlslExprType::MAT4) {
					return true;
				}
				// We've exhausted all possible cases, the operation is not allowed.
				return false;
			}
			return true;
		}
		bool AdditionAllowed(GlslExprType lhs, GlslExprType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}
		bool SubtractionAllowed(GlslExprType lhs, GlslExprType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}
		bool MultiplicationAllowed(GlslExprType lhs, GlslExprType rhs) {
			// GlslExprType inferredType = binaryOpType[static_cast<size_t>(lhs)][static_cast<size_t>(rhs)];
			// if (!ScalarType(lhs) && !ScalarType(rhs)) {
				// If the inferred type for non-scalar objects is NOT undefined, then multiplication is allowed.
				// Addition, subtraction, and division are only restricted to the same size objects.
				// Multiplication, on the other hand, not only allows what the aforementioned operators do,
				// but it can also accept operands of different sizes as defined in the specification.
				// Specification link: https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.pdf
				// 5.9 Expressions, p.122 - 123.
				// return inferredType != GlslExprType::UNDEFINED;
			// }
			// If one of the types is a scalar, then multiplication is allowed regardless of what the second type is.
			// return true;

			if (!ScalarType(lhs) && !ScalarType(rhs)) {
				int cols = GetNumberOfRows(lhs);
				int rows = GetNumberOfCols(rhs);
				return cols == rows;
			}
			return true;
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
			// If any of the types is BOOL, the result is UNDEFINED, because according to the specification,
			// only integer and floating-point scalars, vectors, and matrices are allowed with the arithmetic binary operators.
			if (FundamentalTypeBool(lhs) || FundamentalTypeBool(rhs))
				return GlslExprType::UNDEFINED;

			if (ScalarType(lhs) && ScalarType(rhs)) {
				// 1. Both operands are scalars.
				// The operation is applied resulting in a scalar of the biggest "type".
				// If both types are the same, the result will be of that type.
				size_t typeId = std::max(static_cast<size_t>(lhs), static_cast<size_t>(rhs));
				return static_cast<GlslExprType>(typeId);
			}
			if (ScalarType(lhs) || ScalarType(rhs)) {
				// 2. One of the types is a scalar while the other one is either a vector or a matrix.
				// According to the specification the operation is applied to each component
				// of the non-scalar operand resulting in the same size vector or matrix.
				// 
				// a) First we need to figure out which operand is a scalar and which one is a vector or matrix.
				if (!ScalarType(lhs)) {
					// Left-hand operand is a vector or matrix.
					// The specification states that we must first match the fundamental types of both operands.
					// This means we need to figure out the fundamental type of the left operand and then
					// either promote the lhs operand's type or the rhs operand's one.
					GlslExprType lhsFundamentalType = GetFundamentalType(lhs);
					GlslExprType rhsFundamentalType = rhs;
					if (lhsFundamentalType < rhsFundamentalType) {
						// The type of the non-scalar operand (lhs) must be promoted.
						int rankDiff = static_cast<int>(rhsFundamentalType) - static_cast<int>(lhsFundamentalType);
						return PromoteType(lhs, rankDiff);
					}
					// Otherwise, either the two types are the same or the non-scalar operand's type doesn't need promotion.
					return lhs;
				} else {
					// Right-hand operand is a vector or matrix.
					// Same algorithm as above but for the right-hand side operand.
					GlslExprType rhsFundamentalType = GetFundamentalType(rhs);
					GlslExprType lhsFundamentalType = lhs;
					if (rhsFundamentalType < lhsFundamentalType) {
						// The type of the non-scalar operand (rhs) must be promoted.
						int rankDiff = static_cast<int>(lhsFundamentalType) - static_cast<int>(rhsFundamentalType);
						return PromoteType(rhs, rankDiff);
					}
					return rhs;
				}
			}
			if (!ScalarType(lhs) && !ScalarType(rhs)) {
				// (The if clause is not necessary)
				// 3. Both types are non-scalar types (vector or matrix).
				// 
				// At this point we have two potential situations.
				if (op == TokenType::STAR) {
					// Multiplications is handled according to the rules of linear algebra
					// when matrix-vector or vector-matrix multiplications is performed.
					// Multiplication is allowed if the number of columns of the 'lhs' operand is
					// equal to the number of rows of the 'rhs' operand.
					// TODO
				}
				// Otherwise we're dealing with either addition, subtraction, or devision operation.
				// In any case we need both operands' dimensions to match!
				// TODO
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