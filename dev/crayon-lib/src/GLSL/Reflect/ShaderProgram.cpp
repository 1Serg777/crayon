#include "GLSL/Reflect/ShaderProgram.h"

namespace crayon {
	namespace glsl {

		ShaderProgram::ShaderProgram(std::string_view name)
			: name(name) {
		}

		void ShaderProgram::SetMaterialProps(const MaterialPropsDesc& matProps) {
			this->materialProps = matProps;
		}
		const MaterialPropsDesc& ShaderProgram::GetMaterialProps() const {
			return materialProps;
		}

		void ShaderProgram::SetVertexInputLayout(const VertexInputLayoutDesc& vertexInputLayout) {
			this->vertexInputLayout = vertexInputLayout;
		}
		const VertexInputLayoutDesc& ShaderProgram::GetVertexInputLayout() const {
			return vertexInputLayout;
		}

		bool ShaderProgram::HasVertexShaderSrc() const {
			return vs.has_value();
		}
		void ShaderProgram::SetVertexShaderSrc(const std::string& src) {
			vs = src;
		}
		const std::string& ShaderProgram::GetVertexShaderSrc() const {
			return vs.value();
		}

		bool ShaderProgram::HasFragmentShaderSrc() const {
			return fs.has_value();
		}
		void ShaderProgram::SetFragmentShaderSrc(const std::string& src) {
			fs = src;
		}
		const std::string& ShaderProgram::GetFragmentShaderSrc() const {
			return fs.value();
		}

		const std::string& ShaderProgram::GetShaderProgramName() const {
			return name;
		}

	}
}