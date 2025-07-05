#pragma once

#include "GLSL/Reflect/ReflectCommon.h"

#include <string>
#include <string_view>

namespace crayon {
	namespace glsl {

		class ShaderProgram {
		public:
			ShaderProgram(std::string_view name);

			void SetMaterialProps(const MaterialPropsDesc& matProps);
			const MaterialPropsDesc& GetMaterialProps() const;

			void SetVertexInputLayout(const VertexInputLayoutDesc& vertexInputLayout);
			const VertexInputLayoutDesc& GetVertexInputLayout() const;

			bool HasVertexShaderSrc() const;
			void SetVertexShaderSrc(const std::string& src);
			const std::string& GetVertexShaderSrc() const;

			bool HasFragmentShaderSrc() const;
			void SetFragmentShaderSrc(const std::string& src);
			const std::string& GetFragmentShaderSrc() const;

			const std::string& GetShaderProgramName() const;

		private:
			MaterialPropsDesc materialProps;
			VertexInputLayoutDesc vertexInputLayout;
			
			std::optional<std::string> vs;
			std::optional<std::string> fs;

			std::string name;
		};

	}
}