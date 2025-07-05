#pragma once

#include "GLSL/Token.h"
#include "GLSL/Type.h"

#include <cstdint>
#include <string>
#include <vector>

namespace crayon {
	namespace glsl {

		enum class ShaderType {
			UNDEFIND = -1,
			// Graphics pipeline shaders:
			VS,
			TCS, TES,
			GS,
			FS,
			// Compute pipeline shaders:
			CS,
			// Ray Tracing pipeline shaders:
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
			// Don't forget to change the places that use this enumeration.
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

		VertexAttribChannel IdentifierTokenToVertexAttribChannel(const Token& token);

		VertexAttribType TokenTypeToVertexAttribType(TokenType tokenType);
		TokenType VertexAttribTypeToTokenType(VertexAttribType type);

		MatPropType TokenTypeToMaterialPropertyType(TokenType tokenType);
		TokenType MaterialPropertyTypeToTokenType(MatPropType type);

		struct VertexAttribDesc {
			uint32_t GetVertexAttributeSize() const;

			std::string name;

			uint32_t dimension{0}; // Attribute's dimensionality. 3 if vec3, 2 if vec2 and so on
			uint32_t offset{0}; // Attribute's offset within the vertex.

			VertexAttribChannel channel{};
			VertexAttribType type{};
		};
		struct VertexInputLayoutDesc {
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
		struct MaterialPropsDesc {
			bool MatPropExists(std::string_view matPropName) const;
			void AddMatProp(const MaterialPropDesc& matPropDesc);
			const MaterialPropDesc& GetMatProp(std::string_view matPropName) const;
			size_t GetMatPropCount() const;

			std::string name;
			std::vector<MaterialPropDesc> matProps;
		};

		// class RenderPass;
		// class RenderPassAttachments;
		// class RenderPassAttachmentDesc;
		class AttachmentDesc {
			// TODO
		};

	}
}