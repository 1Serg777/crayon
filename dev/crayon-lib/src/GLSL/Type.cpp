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