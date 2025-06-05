#pragma once

namespace crayon {
	namespace glsl {

		enum class IntConstType {
            OCT = 8, // 00005, 01732, etc.
            DEC = 10, // 1, 12, 35, etc.
            HEX = 16, // 0x1A, 0X0, etc.
        };

        enum class FloatConstType {
            NORM, // 1.0, 0.53, .99, 35., etc.
            EXP, // 1e5, 0e-3, 0.15e2, etc.
        };

	}
}