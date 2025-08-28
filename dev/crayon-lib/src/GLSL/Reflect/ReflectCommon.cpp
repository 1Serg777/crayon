#include "GLSL/Reflect/ReflectCommon.h"

#include <algorithm>
#include <cassert>
#include <string_view>
#include <unordered_map>

namespace crayon {

	using namespace glsl;

	static std::unordered_map<std::string_view, VertexAttribChannel> idToVertexAttribChannelMap = {
		{std::string_view("POSITION"), VertexAttribChannel::POSITION},
		{std::string_view("NORMAL"),   VertexAttribChannel::NORMAL  },
		{std::string_view("TANGENT"),  VertexAttribChannel::TANGENT },
		{std::string_view("COLOR"),    VertexAttribChannel::COLOR   },
		{std::string_view("UV0"),      VertexAttribChannel::UV0     },
		{std::string_view("UV1"),      VertexAttribChannel::UV1     },
	};
	static std::unordered_map<std::string_view, ColorAttachmentChannel> idToColorAttachmentChannelMap = {
		{std::string_view("COLOR0"), ColorAttachmentChannel::COLOR0},
		{std::string_view("COLOR1"), ColorAttachmentChannel::COLOR1},
		{std::string_view("COLOR2"), ColorAttachmentChannel::COLOR2},
		{std::string_view("COLOR3"), ColorAttachmentChannel::COLOR3},
		{std::string_view("COLOR4"), ColorAttachmentChannel::COLOR4},
		{std::string_view("COLOR5"), ColorAttachmentChannel::COLOR5},
		{std::string_view("COLOR6"), ColorAttachmentChannel::COLOR6},
		{std::string_view("COLOR7"), ColorAttachmentChannel::COLOR7},
		{std::string_view("COLOR8"), ColorAttachmentChannel::COLOR8},
		{std::string_view("COLOR9"), ColorAttachmentChannel::COLOR9},
	};

	static std::unordered_map<GlslBasicType, VertexAttribType> tokenTypeToVertexAttribTypeMap = {
		{GlslBasicType::INT,   VertexAttribType::INT32  },
		{GlslBasicType::UINT,  VertexAttribType::UINT32 },
		{GlslBasicType::FLOAT, VertexAttribType::FLOAT32},
	};
	static std::unordered_map<VertexAttribType, GlslBasicType> vertexAttribTypeToTokenTypeMap = {
		{VertexAttribType::INT32  , GlslBasicType::INT  },
		{VertexAttribType::UINT32 , GlslBasicType::UINT },
		{VertexAttribType::FLOAT32, GlslBasicType::FLOAT},
	};

	static std::unordered_map<TokenType, MatPropType> tokenTypeToMatPropTypeMap = {
		{TokenType::INT,       MatPropType::INTEGER  },
		{TokenType::FLOAT,     MatPropType::FLOAT    },
		{TokenType::VEC2,      MatPropType::VECTOR2  },
		{TokenType::VEC3,      MatPropType::VECTOR3  },
		{TokenType::VEC4,      MatPropType::VECTOR4  },
		{TokenType::VEC4,      MatPropType::COLOR    },
		{TokenType::SAMPLER2D, MatPropType::TEXTURE2D},
	};
	static std::unordered_map<MatPropType, TokenType> matPropTypeToTokenType = {
		{MatPropType::INTEGER,   TokenType::INT      },
		{MatPropType::FLOAT,     TokenType::FLOAT    },
		{MatPropType::VECTOR2,   TokenType::VEC2     },
		{MatPropType::VECTOR3,   TokenType::VEC3     },
		{MatPropType::VECTOR4,   TokenType::VEC4     },
		{MatPropType::COLOR,     TokenType::VEC4     },
		{MatPropType::TEXTURE2D, TokenType::SAMPLER2D},
	};

	static std::unordered_map<GlslBasicType, ColorAttachmentType> tokenTypeToColorAttachmentTypeMap = {
		{GlslBasicType::INT,   ColorAttachmentType::INT  },
		{GlslBasicType::UINT,  ColorAttachmentType::UINT },
		{GlslBasicType::FLOAT, ColorAttachmentType::FLOAT},
		{GlslBasicType::IVEC2, ColorAttachmentType::IVEC2},
		{GlslBasicType::IVEC3, ColorAttachmentType::IVEC3},
		{GlslBasicType::IVEC4, ColorAttachmentType::IVEC4},
		{GlslBasicType::UVEC2, ColorAttachmentType::UVEC2},
		{GlslBasicType::UVEC3, ColorAttachmentType::UVEC3},
		{GlslBasicType::UVEC4, ColorAttachmentType::UVEC4},
		{GlslBasicType::VEC2,  ColorAttachmentType::VEC2 },
		{GlslBasicType::VEC3,  ColorAttachmentType::VEC3 },
		{GlslBasicType::VEC4,  ColorAttachmentType::VEC4 },
	};
	static std::unordered_map<ColorAttachmentType, GlslBasicType> colorAttachmentTypeToTokenTypeMap = {
		{ColorAttachmentType::INT,   GlslBasicType::INT  },
		{ColorAttachmentType::UINT,  GlslBasicType::UINT },
		{ColorAttachmentType::FLOAT, GlslBasicType::FLOAT},
		{ColorAttachmentType::IVEC2, GlslBasicType::IVEC2},
		{ColorAttachmentType::IVEC3, GlslBasicType::IVEC3},
		{ColorAttachmentType::IVEC4, GlslBasicType::IVEC4},
		{ColorAttachmentType::UVEC2, GlslBasicType::UVEC2},
		{ColorAttachmentType::UVEC3, GlslBasicType::UVEC3},
		{ColorAttachmentType::UVEC4, GlslBasicType::UVEC4},
		{ColorAttachmentType::VEC2,  GlslBasicType::VEC2 },
		{ColorAttachmentType::VEC3,  GlslBasicType::VEC3 },
		{ColorAttachmentType::VEC4,  GlslBasicType::VEC4 },
	};

	static uint32_t GetVertexAttributeFormatSizeInBytes(VertexAttribType vertexAttribType) {
		switch (vertexAttribType) {
			case VertexAttribType::FLOAT32:
			case VertexAttribType::UINT32:
			case VertexAttribType::INT32:
				return 4;
			default:
				assert(true && "Unknown vertex attribute type provided!");
				return 0;
		}
	}

	VertexAttribChannel IdentifierTokenToVertexAttribChannel(const Token& token) {
			auto searchRes = idToVertexAttribChannelMap.find(token.lexeme);
			if (searchRes == idToVertexAttribChannelMap.end()) {
				return VertexAttribChannel::UNDEFINED;
			}
			return searchRes->second;
		}
	ColorAttachmentChannel IdentifierTokenToColorAttachmentChannel(const Token& token) {
			auto searchRes = idToColorAttachmentChannelMap.find(token.lexeme);
			if (searchRes == idToColorAttachmentChannelMap.end()) {
				return ColorAttachmentChannel::UNDEFINED;
			}
			return searchRes->second;
		}

	VertexAttribType TokenTypeToVertexAttribType(TokenType tokenType) {
			GlslBasicType glslBasicType = TokenTypeToGlslBasicType(tokenType);
			GlslBasicType glslfundamentalType = GetFundamentalType(glslBasicType);
			auto searchRes = tokenTypeToVertexAttribTypeMap.find(glslfundamentalType);
			if (searchRes == tokenTypeToVertexAttribTypeMap.end()) {
				return VertexAttribType::UNDEFINED;
			}
			return searchRes->second;
		}
	TokenType VertexAttribTypeToTokenType(VertexAttribType type) {
			auto searchRes = vertexAttribTypeToTokenTypeMap.find(type);
			if (searchRes == vertexAttribTypeToTokenTypeMap.end()) {
				return TokenType::UNDEFINED;
			}
			GlslBasicType glslBasicType = searchRes->second;
			TokenType tokenType = GlslBasicTypeToTokenType(glslBasicType);
			return tokenType;
		}

	MatPropType TokenTypeToMaterialPropertyType(TokenType tokenType) {;
			auto searchRes = tokenTypeToMatPropTypeMap.find(tokenType);
			if (searchRes == tokenTypeToMatPropTypeMap.end()) {
				return MatPropType::UNDEFINED;
			}
			return searchRes->second;
		}
	TokenType MaterialPropertyTypeToTokenType(MatPropType type) {
			auto searchRes = matPropTypeToTokenType.find(type);
			if (searchRes == matPropTypeToTokenType.end()) {
				return TokenType::UNDEFINED;
			}
			return searchRes->second;
		}

	ColorAttachmentType TokenTypeToColorAttachmentType(TokenType tokenType) {
			GlslBasicType glslBasicType = TokenTypeToGlslBasicType(tokenType);
			auto searchRes = tokenTypeToColorAttachmentTypeMap.find(glslBasicType);
			if (searchRes == tokenTypeToColorAttachmentTypeMap.end()) {
				return ColorAttachmentType::UNDEFINED;
			}
			return searchRes->second;
		}
	TokenType ColorAttachmentTypeToTokenType(ColorAttachmentType type) {
			auto searchRes = colorAttachmentTypeToTokenTypeMap.find(type);
			if (searchRes == colorAttachmentTypeToTokenTypeMap.end()) {
				return TokenType::UNDEFINED;
			}
			GlslBasicType glslBasicType = searchRes->second;
			TokenType tokenType = GlslBasicTypeToTokenType(glslBasicType);
			return tokenType;
		}

	int GetVertexAttribChannelNum(VertexAttribChannel vertexAttribChannel) {
			// 1. We could use a map or a lookup table and index them
			//    using the numerical value of each enumeration value,
			//    but simply casting an enumeration value to an integer is easier.
			assert(vertexAttribChannel != VertexAttribChannel::UNDEFINED &&
				   vertexAttribChannel != VertexAttribChannel::COUNT &&
				   "Invalid vertex attribute channel provided!");
			return static_cast<int>(vertexAttribChannel);
		}
	int GetColorAttachmentChannelNum(ColorAttachmentChannel colorAttachmentChannel) {
			// 1. We could use a map or a lookup table and index them
			//    using the numerical value of each enumeration value,
			//    but simply casting an enumeration value to an integer is easier.
			assert(colorAttachmentChannel != ColorAttachmentChannel::UNDEFINED &&
				   colorAttachmentChannel != ColorAttachmentChannel::COUNT &&
				   "Invalid color attachment channel provided!");
			return static_cast<int>(colorAttachmentChannel);
		}

	uint32_t VertexAttribDesc::GetVertexAttributeSize() const {
			return dimension * GetVertexAttributeFormatSizeInBytes(type);
		}

	bool VertexInputLayoutDesc::IsEmpty() const {
			return attributes.size() == 0;
		}
	void VertexInputLayoutDesc::AddVertexAttrib(const VertexAttribDesc& vertexAttribDesc) {
			attributes.push_back(vertexAttribDesc);
		}
	size_t VertexInputLayoutDesc::GetVertexAttribCount() const {
			return attributes.size();
		}
	uint32_t VertexInputLayoutDesc::GetStride() const {
			uint32_t stride{0};
			for (const VertexAttribDesc& vertexAttribDesc : attributes) {
				stride += vertexAttribDesc.GetVertexAttributeSize();
			}
			return stride;
		}

	bool MaterialProps::IsEmpty() const {
			return matProps.size() == 0;
		}
	bool MaterialProps::MatPropExists(std::string_view matPropName) const {
			auto pred = [=](const MaterialPropDesc& matPropDesc) {
				return matPropName == matPropDesc.name;
			};
			auto searchRes = std::find_if(matProps.begin(), matProps.end(), pred);
			return searchRes != matProps.end();
		}
	void MaterialProps::AddMatProp(const MaterialPropDesc& matPropDesc) {
			matProps.push_back(matPropDesc);
		}
	const MaterialPropDesc& MaterialProps::GetMatProp(std::string_view matPropName) const {
			auto pred = [=](const MaterialPropDesc& matPropDesc) {
				return matPropName == matPropDesc.name;
				};
			auto searchRes = std::find_if(matProps.begin(), matProps.end(), pred);
			assert(searchRes != matProps.end() && "Check if the material property exists first!");
			return *searchRes;
		}
	size_t MaterialProps::GetMatPropCount() const {
			return matProps.size();
		}

	bool ColorAttachments::IsEmpty() const {
			return GetColorAttachmentCount() == 0;
		}

	void ColorAttachments::AddColorAttachment(const ColorAttachmentDesc& colorAttachment) {
			attachments[static_cast<size_t>(colorAttachment.channel)] = colorAttachment;
		}
	void ColorAttachments::AddColorAttachment(ColorAttachmentChannel channel, ColorAttachmentType type) {
			attachments[static_cast<size_t>(channel)].channel = channel;
			attachments[static_cast<size_t>(channel)].type = type;
		}

	bool ColorAttachments::ColorAttachmentChannelExists(ColorAttachmentChannel channel) const {
			return attachments[static_cast<size_t>(channel)].channel != ColorAttachmentChannel::UNDEFINED;
		}
	ColorAttachmentType ColorAttachments::GetColorAttachmentType(ColorAttachmentChannel channel) const {
			assert(ColorAttachmentChannelExists(channel) && "Check if the color channel exists first!");
			return attachments[static_cast<size_t>(channel)].type;
		}
	size_t ColorAttachments::GetColorAttachmentCount() const {
			size_t count{0};
			for (const ColorAttachmentDesc& attachment : attachments) {
				if (attachment.channel != ColorAttachmentChannel::UNDEFINED)
					count++;
			}
			return count;
		}

	std::vector<ColorAttachmentDesc> ColorAttachments::GetColorAttachments() const {
			size_t colorAttachmentCount = GetColorAttachmentCount();
			std::vector<ColorAttachmentDesc> colorAttachments(colorAttachmentCount);
			size_t i{ 0 };
			for (const ColorAttachmentDesc& colorAttachment : attachments) {
				if (colorAttachment.channel != ColorAttachmentChannel::UNDEFINED) {
					colorAttachments[i++] = colorAttachment;
				}
			}
			return colorAttachments;
		}

}