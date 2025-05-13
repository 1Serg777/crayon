#include "GLSL/Type.h"

namespace crayon {
	namespace glsl {

		bool TypeQual::Empty() const {
			return layout.empty() &&
				!storage.has_value() &&
				!precision.has_value() &&
				!interpolation.has_value() &&
				!invariant.has_value() &&
				!precise.has_value();
		}
	
	}
}