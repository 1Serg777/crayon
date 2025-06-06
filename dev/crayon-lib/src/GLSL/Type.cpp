#include "GLSL/Type.h"

#include <cassert>

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
	
		bool TypeSpec::ArrayType() const {
			// return x.has_value() || y.has_value();
			return !dimensions.empty();
		}
		size_t TypeSpec::ArrayDimensionSize() const {
			return dimensions.size();
		}
		size_t TypeSpec::ArraySize(size_t dimension) const {
			assert(dimension < dimensions.size() && "Invalid dimension provided!");
			// TODO: compute dimensions[dimension] expression,
			// assuming it's an integral constant expression.
			return 0;
		}
		std::shared_ptr<Expr> TypeSpec::ArraySizeExpr(size_t dimension) const {
			assert(dimension < dimensions.size() && "Invalid dimension provided!");
			return dimensions[dimension];
		}

	}
}