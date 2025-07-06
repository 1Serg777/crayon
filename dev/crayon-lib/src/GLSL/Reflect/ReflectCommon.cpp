#include "GLSL/Reflect/ReflectCommon.h"

#include <cassert>
#include <string_view>
#include <unordered_map>

namespace crayon {
	namespace glsl {

		static std::unordered_map<std::string_view, VertexAttribChannel> idToVertexAttribChannelMap = {
			{std::string_view("POSITION"), VertexAttribChannel::POSITION},
			{std::string_view("NORMAL"),   VertexAttribChannel::NORMAL  },
			{std::string_view("TANGENT"),  VertexAttribChannel::TANGENT },
			{std::string_view("COLOR"),    VertexAttribChannel::COLOR   },
			{std::string_view("UV0"),      VertexAttribChannel::UV0     },
			{std::string_view("UV1"),      VertexAttribChannel::UV1     },
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

		bool MaterialPropsDesc::MatPropExists(std::string_view matPropName) const {
			auto pred = [=](const MaterialPropDesc& matPropDesc) {
				return matPropName == matPropDesc.name;
			};
			auto searchRes = std::find_if(matProps.begin(), matProps.end(), pred);
			return searchRes != matProps.end();
		}
		void MaterialPropsDesc::AddMatProp(const MaterialPropDesc& matPropDesc) {
			matProps.push_back(matPropDesc);
		}
		const MaterialPropDesc& MaterialPropsDesc::GetMatProp(std::string_view matPropName) const {
			auto pred = [=](const MaterialPropDesc& matPropDesc) {
				return matPropName == matPropDesc.name;
				};
			auto searchRes = std::find_if(matProps.begin(), matProps.end(), pred);
			assert(searchRes != matProps.end() && "Check if the material property exists first!");
			return *searchRes;
		}
		size_t MaterialPropsDesc::GetMatPropCount() const {
			return matProps.size();
		}

	}
}