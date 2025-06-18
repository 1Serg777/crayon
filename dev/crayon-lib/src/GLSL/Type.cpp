#include "GLSL/Type.h"

#include <cassert>

namespace crayon {
	namespace glsl {

		// "asmd" are the first letters of the names of the following binary operators:
		// a = add (+), s = subtract (-), m = multiply (*), d = divide (/).
		// "asmdBinaryOpType"
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

		bool ScalarType(GlslExprType type) {
			return type >= GlslExprType::INT && type <= GlslExprType::DOUBLE;
		}
		bool VectorType(GlslExprType type) {
			return type >= GlslExprType::BVEC2 && type <= GlslExprType::VEC4;
		}
		bool MatrixType(GlslExprType type) {
			return type >= GlslExprType::MAT2 && type <= GlslExprType::DMAT4X4;
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
			GlslExprType inferredType = binaryOpType[static_cast<size_t>(lhs)][static_cast<size_t>(rhs)];
			if (!ScalarType(lhs) && !ScalarType(rhs)) {
				// If the inferred type for non-scalar objects is NOT undefined, then multiplication is allowed.
				// Addition, subtraction, and division are only restricted to the same size objects.
				// Multiplication, on the other hand, not only allows what the aforementioned operators do,
				// but it can also accept operands of different sizes as defined in the specification.
				// Specification link: https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.pdf
				// 5.9 Expressions, p.122 - 123.
				return inferredType != GlslExprType::UNDEFINED;
			}
			// If one of the types is a scalar, then multiplication is allowed regardless of what the second type is.
			return true;
		}
		bool DivisionAllowed(GlslExprType lhs, GlslExprType rhs) {
			return AddSubDivAllowed(lhs, rhs);
		}

		GlslExprType InferExprType(GlslExprType lhs, GlslExprType rhs, TokenType op) {
			GlslExprType inferredType = binaryOpType[static_cast<size_t>(lhs)][static_cast<size_t>(rhs)];
			// The 'binaryOpType' array stores allowed types for multiplication as well, so we can't
			// immediately return what we got from it.
			// 1. Multiplication?
			if (op == TokenType::STAR) {
				// We can return right away, because multiplication is covered fully by the array.
				return inferredType;
			}
			// 2. Other operators.
			// If the operations are allowed, return the inferred type we got at the top.
			// If not, then we might have extracted an inferred type for multiplication which is accidentally valid,
			// when addition, subtraction, and division are not. In that case we must return UNDEFINED.
			if (!AddSubDivAllowed(lhs, rhs)) {
				return GlslExprType::UNDEFINED;
			}
			return inferredType;
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