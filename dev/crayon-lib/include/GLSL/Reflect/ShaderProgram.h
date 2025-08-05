#pragma once

#include "GLSL/Reflect/ReflectCommon.h"

#include <string>
#include <string_view>

namespace crayon {
	namespace glsl {

		class ShaderProgram {
		public:
			ShaderProgram(std::string_view name);

			void SetMaterialProps(const MaterialProps& matProps);
			const MaterialProps& GetMaterialProps() const;

			void SetVertexInputLayout(const VertexInputLayoutDesc& vertexInputLayout);
			const VertexInputLayoutDesc& GetVertexInputLayout() const;

			void AddColorAttachmentDesc(const ColorAttachmentDesc& colorAttachment);
			const ColorAttachments& GetColorAttachments() const;

			bool HasVertexShaderSrc() const;
			void SetVertexShaderSrc(const std::string& src);
			const std::string& GetVertexShaderSrc() const;

			bool HasFragmentShaderSrc() const;
			void SetFragmentShaderSrc(const std::string& src);
			const std::string& GetFragmentShaderSrc() const;

			const std::string& GetShaderProgramName() const;

		private:
			MaterialProps materialProps;
			VertexInputLayoutDesc vertexInputLayout;
			ColorAttachments colorAttachments;

			std::optional<std::string> vs;
			std::optional<std::string> fs;

			std::string name;
		};

	}
}