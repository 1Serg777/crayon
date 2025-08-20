#pragma once

#include "GLSL/Reflect/ReflectCommon.h"

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace crayon {

	struct ShaderModule {
		bool IsValid() const;
		bool HasGlsl() const;
		bool HasSpvAsm() const;
		bool HasSpvBinary() const;

		std::string glsl;
		std::string spvAsm;
		std::vector<uint32_t> spvBinary;
	};

	class ShaderProgram {
	public:
		ShaderProgram() = default;
		ShaderProgram(std::string_view name);

		void SetMaterialProps(const MaterialProps& matProps);
		const MaterialProps& GetMaterialProps() const;

		void SetVertexInputLayout(const VertexInputLayoutDesc& vertexInputLayout);
		const VertexInputLayoutDesc& GetVertexInputLayout() const;

		void AddColorAttachmentDesc(const ColorAttachmentDesc& colorAttachment);
		const ColorAttachments& GetColorAttachments() const;

		bool HasShaderModule(ShaderType type) const;
		void SetShaderModuleGlsl(ShaderType type, const std::string& glsl);
		void SetShaderModuleSpvAsm(ShaderType type, const std::string& spvAsm);
		void SetShaderModuleSpvBinary(ShaderType type, const std::vector<uint32_t>& spvBinary);
		const ShaderModule& GetShaderModule(ShaderType type) const;

		void SetName(std::string_view name);
		const std::string& GetName() const;

	private:
		MaterialProps materialProps;
		VertexInputLayoutDesc vertexInputLayout;
		ColorAttachments colorAttachments;

		std::array<ShaderModule, static_cast<size_t>(ShaderType::COUNT)> shaders;

		std::string name;
	};

}