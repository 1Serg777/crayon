#pragma once

#include "GLSL/Token.h"
#include "GLSL/Type.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace crayon {

	enum class ShaderType {
		UNDEFINED = -1,
		// Graphics pipeline shaders:
		VS,
		TCS, TES,
		GS,
		FS,
		// Compute pipeline shaders:
		CS,
		// Ray Tracing pipeline shaders:
		COUNT,
	};

	enum class VertexAttribChannel {
		UNDEFINED = -1,
		POSITION,
		NORMAL,
		TANGENT,
		COLOR,
		UV0,
		UV1,
		// TODO: add more later if necessary.
		// Don't forget to change all the places that use this enumeration.
		COUNT,
	};

	enum class VertexAttribType {
		UNDEFINED = -1,
		INT32,
		UINT32,
		FLOAT32,
		// TODO: add more types if needed.
		// Don't forget to change the places that use this enumeration.
	};

	enum class MatPropType {
		UNDEFINED = -1,
		INTEGER,
		FLOAT,
		VECTOR2,
		VECTOR3,
		VECTOR4,
		COLOR,
		TEXTURE2D,
	};

	enum class ColorAttachmentChannel {
		UNDEFINED = -1,
		COLOR0,
		COLOR1,
		COLOR2,
		COLOR3,
		COLOR4,
		COLOR5,
		COLOR6,
		COLOR7,
		COLOR8,
		COLOR9,
		COUNT,
	};

	enum class ColorAttachmentType {
		UNDEFINED = -1,
		INT,
		UINT,
		FLOAT,
		IVEC2,
		IVEC3,
		IVEC4,
		UVEC2,
		UVEC3,
		UVEC4,
		VEC2,
		VEC3,
		VEC4,
	};

	VertexAttribChannel IdentifierTokenToVertexAttribChannel(const glsl::Token& token);
	ColorAttachmentChannel IdentifierTokenToColorAttachmentChannel(const glsl::Token& token);

	VertexAttribType TokenTypeToVertexAttribType(glsl::TokenType tokenType);
	glsl::TokenType VertexAttribTypeToTokenType(VertexAttribType type);

	MatPropType TokenTypeToMaterialPropertyType(glsl::TokenType tokenType);
	glsl::TokenType MaterialPropertyTypeToTokenType(MatPropType type);

	ColorAttachmentType TokenTypeToColorAttachmentType(glsl::TokenType tokenType);
	glsl::TokenType ColorAttachmentTypeToTokenType(ColorAttachmentType type);

	int GetVertexAttribChannelNum(VertexAttribChannel vertexAttribChannel);
	int GetColorAttachmentChannelNum(ColorAttachmentChannel colorAttachmentChannel);

	struct VertexAttribDesc {
		uint32_t GetVertexAttributeSize() const;

		std::string name;
		glsl::GlslBasicType glslType{glsl::GlslBasicType::UNDEFINED};

		uint32_t dimension{0}; // Attribute's dimensionality. 3 if vec3, 2 if vec2 and so on
		uint32_t offset{0}; // Attribute's offset within the vertex.

		VertexAttribChannel channel{};
		VertexAttribType type{};
	};
	struct VertexInputLayoutDesc {
		bool IsEmpty() const;
		void AddVertexAttrib(const VertexAttribDesc& vertexAttribDesc);
		size_t GetVertexAttribCount() const;
		uint32_t GetStride() const;

		std::vector<VertexAttribDesc> attributes;
	};

	struct MaterialPropDesc {
		std::string name;
		std::string visibleName;
		MatPropType type{MatPropType::UNDEFINED};
		bool showInEditor{true};
	};
	struct MaterialProps {
		bool IsEmpty() const;
		bool MatPropExists(std::string_view matPropName) const;
		void AddMatProp(const MaterialPropDesc& matPropDesc);
		const MaterialPropDesc& GetMatProp(std::string_view matPropName) const;
		size_t GetMatPropCount() const;

		std::string name;
		std::vector<MaterialPropDesc> matProps;
	};

	struct ColorAttachmentDesc {
		std::string_view name;
		ColorAttachmentChannel channel{ColorAttachmentChannel::UNDEFINED};
		ColorAttachmentType type{ColorAttachmentType::UNDEFINED};
	};
	struct ColorAttachments {
		bool IsEmpty() const;

		void AddColorAttachment(const ColorAttachmentDesc& colorAttachment);
		void AddColorAttachment(ColorAttachmentChannel channel, ColorAttachmentType type);

		bool ColorAttachmentChannelExists(ColorAttachmentChannel channel) const;
		ColorAttachmentType GetColorAttachmentType(ColorAttachmentChannel channel) const;
		size_t GetColorAttachmentCount() const;

		// Sorted by channel index.
		std::vector<ColorAttachmentDesc> GetColorAttachments() const;

		std::array<ColorAttachmentDesc, static_cast<size_t>(ColorAttachmentChannel::COUNT)> attachments;
	};

}