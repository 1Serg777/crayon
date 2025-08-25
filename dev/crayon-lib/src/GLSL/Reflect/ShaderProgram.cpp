#include "GLSL/Reflect/ShaderProgram.h"

namespace crayon {

	bool ShaderModule::IsValid() const {
		return HasGlsl() || HasSpvAsm() || HasSpvBinary();
	}
	bool ShaderModule::HasGlsl() const {
		return !glsl.empty();
	}
	bool ShaderModule::HasSpvAsm() const {
		return !spvAsm.empty();
	}
	bool ShaderModule::HasSpvBinary() const {
		return spvBinary.size() != 0;
	}

	ShaderProgram::ShaderProgram(std::string_view name)
		: name(name) {
	}

	void ShaderProgram::SetMaterialProps(const MaterialProps& matProps) {
		this->materialProps = matProps;
	}
	const MaterialProps& ShaderProgram::GetMaterialProps() const {
		return materialProps;
	}

	void ShaderProgram::SetVertexInputLayout(const VertexInputLayoutDesc& vertexInputLayout) {
		this->vertexInputLayout = vertexInputLayout;
	}
	const VertexInputLayoutDesc& ShaderProgram::GetVertexInputLayout() const {
		return vertexInputLayout;
	}

	void ShaderProgram::SetColorAttachments(const ColorAttachments& colorAttachments) {
		this->colorAttachments = colorAttachments;
	}
	void ShaderProgram::AddColorAttachmentDesc(const ColorAttachmentDesc& colorAttachment) {
		colorAttachments.AddColorAttachment(colorAttachment);
	}
	const ColorAttachments& ShaderProgram::GetColorAttachments() const {
		return colorAttachments;
	}

	bool ShaderProgram::HasShaderModule(ShaderType type) const {
		return shaders[static_cast<size_t>(type)].IsValid();
	}
	void ShaderProgram::SetShaderModuleGlsl(ShaderType type, const std::string& glsl) {
		shaders[static_cast<size_t>(type)].glsl = glsl;
	}
	void ShaderProgram::SetShaderModuleSpvAsm(ShaderType type, const std::string& spvAsm) {
		shaders[static_cast<size_t>(type)].spvAsm = spvAsm;
	}
	void ShaderProgram::SetShaderModuleSpvBinary(ShaderType type, const std::vector<uint32_t>& spvBinary) {
		shaders[static_cast<size_t>(type)].spvBinary = spvBinary;
	}
	const ShaderModule& ShaderProgram::GetShaderModule(ShaderType type) const {
		return shaders[static_cast<size_t>(type)];
	}

	void ShaderProgram::SetName(std::string_view name) {
		this->name = name;
	}
	const std::string& ShaderProgram::GetName() const {
		return name;
	}

}