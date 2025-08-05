#include "GLSL/Analyzer/Environment.h"

#include <cassert>

namespace crayon {
	namespace glsl {

		NestedScopeEnvironment::NestedScopeEnvironment(std::shared_ptr<NestedScopeEnvironment> enclosingScope)
			: enclosingScope(enclosingScope) {
		}

		bool NestedScopeEnvironment::SymbolDeclared(std::string_view symbolName) const {
			if (VarDeclExists(symbolName)) {
				return true;
			}
			if (enclosingScope) {
				return enclosingScope->SymbolDeclared(symbolName);
			}
			return false;
		}
		bool NestedScopeEnvironment::HasEnclosingScope() const {
			if (enclosingScope) return true;
			return false;
		}
		bool NestedScopeEnvironment::IsExternalScope() const {
			if (!HasEnclosingScope()) return true;
			return false;
		}
		std::shared_ptr<NestedScopeEnvironment> NestedScopeEnvironment::GetEnclosingScope() const {
			assert(!IsExternalScope() && "Check if the scope is external first!");
			return enclosingScope;
		}
		
		void NestedScopeEnvironment::AddVarDecl(std::shared_ptr<VarDecl> varDecl) {
			variables.insert({varDecl->GetVarName().lexeme, varDecl});
		}
		void NestedScopeEnvironment::RemoveVarDecl(std::string_view varDeclName) {
			// TODO
		}
		bool NestedScopeEnvironment::VarDeclExists(std::string_view varName) const {
			auto searchRes = variables.find(varName);
			if (searchRes == variables.end()) {
				// Check "direct" variable declarations.
				if (enclosingScope) {
					return enclosingScope->VarDeclExists(varName);
				}
				// Give up.
				return false;
			}
			return true;
		}
		std::shared_ptr<VarDecl> NestedScopeEnvironment::GetVarDecl(std::string_view varName) const {
			std::shared_ptr<VarDecl> varDecl;
			auto searchRes = variables.find(varName);
			if (searchRes == variables.end()) {
				if (enclosingScope) {
					return enclosingScope->GetVarDecl(varName);
				}
			}
			assert(searchRes != variables.end() && "Check the existence of the variable declaration first!");
			varDecl = searchRes->second;
			return varDecl;
		}

		ExternalScopeEnvironment::ExternalScopeEnvironment()
			: NestedScopeEnvironment(std::shared_ptr<NestedScopeEnvironment>()) {
		}

		bool ExternalScopeEnvironment::SymbolDeclared(std::string_view symbolName) const {
			// 1. First, we check global variables.
			if (VarDeclExists(symbolName)) {
				return true;
			}
			// 2. Then, we check interface blocks.
			if (IntBlockFieldExists(symbolName)) {
				return true;
			}
			// 3. If the previous two failed, we check either of the following two:
			//    - Vertex input layoubt block, if we're parsing the vertex shader (only for vertex shaders), or
			//    - Color attachments bloc, we're parsing the fragment shader.
			if (VertexInputLayoutFieldExists(symbolName)) {
				return true;
			}
			// 4. If all failed, then perhaps the declaration is in the material properties block.
			if (MatPropsFieldExists(symbolName)) {
				return true;
			}
			// 5. Color attachments block. (Only for fragment shaders)
			if (ColorAttachmentFieldExists(symbolName)) {
				return true;
			}
			// Give up.
			return false;
		}

		void ExternalScopeEnvironment::SetVertexInputLayoutBlock(std::shared_ptr<VertexInputLayoutBlock> vertexInputLayout) {
			this->vertexInputLayout = vertexInputLayout;
			//for (const std::shared_ptr<VarDecl>& varDecl : CreateVertexAttribVarDecls(vertexInputLayout)) {
			//	AddVarDecl(varDecl);
			//}
		}
		void ExternalScopeEnvironment::SetMaterialPropertiesBlock(std::shared_ptr<MaterialPropertiesBlock> materialProperties) {
			this->materialProperties = materialProperties;
			// std::shared_ptr<InterfaceBlockDecl> uniformInterfaceBlock = CreateInterfaceBlockDecl(materialProperties);
			// AddInterfaceBlockDecl(uniformInterfaceBlock);
		}
		void ExternalScopeEnvironment::SetColorAttachmentsBlock(std::shared_ptr<ColorAttachmentsBlock> colorAttachments) {
			this->colorAttachments = colorAttachments;
		}

		bool ExternalScopeEnvironment::VertexInputLayoutFieldExists(std::string_view vertexAttribName) const {
			if (!vertexInputLayout)
				return false;
			return vertexInputLayout->HasVertexAttribDecl(vertexAttribName);
		}
		bool ExternalScopeEnvironment::MatPropsFieldExists(std::string_view matPropName) const {
			if (!materialProperties)
				return false;
			return materialProperties->HasMatPropDecl(matPropName);
		}
		bool ExternalScopeEnvironment::ColorAttachmentFieldExists(std::string_view colorAttachmentName) const {
			if (!colorAttachments)
				return false;
			return colorAttachments->HasColorAttachmentDecl(colorAttachmentName);
		}

		std::shared_ptr<VertexAttribDecl> ExternalScopeEnvironment::GetVertexAttribDecl(std::string_view vertexAttribName) const {
			assert(vertexInputLayout && "Vertex Input Layout block doesn't exist!");
			return vertexInputLayout->GetVertexAttribDecl(vertexAttribName);
		}
		std::shared_ptr<MatPropDecl> ExternalScopeEnvironment::GetMatPropDecl(std::string_view matPropName) const {
			assert(materialProperties && "Material Properties block doesn't exist!");
			return materialProperties->GetMatPropDecl(matPropName);
		}
		std::shared_ptr<ColorAttachmentDecl> ExternalScopeEnvironment::GetColorAttachmentDecl(std::string_view colorAttachmentName) const {
			assert(colorAttachments && "Color Attachments block doesn't exist!");
			return colorAttachments->GetColorAttachmentDecl(colorAttachmentName);
		}

		void ExternalScopeEnvironment::AddStructDecl(std::shared_ptr<StructDecl> structDecl) {
			structs.insert({structDecl->GetName().lexeme, structDecl});
		}
		void ExternalScopeEnvironment::AddInterfaceBlockDecl(std::shared_ptr<InterfaceBlockDecl> intBlockDecl) {
			interfaceBlocks.insert({ intBlockDecl->GetName().lexeme, intBlockDecl });
		}
		void ExternalScopeEnvironment::AddFunDecl(std::shared_ptr<FunDecl> funDecl) {
			functions.insert({funDecl->GetFunProto()->GetFunctionName().lexeme, funDecl});
		}

		void ExternalScopeEnvironment::RemoveStructDecl(std::string_view structDeclName) {
			// TODO
		}
		void ExternalScopeEnvironment::RemoveInterfaceBlockDecl(std::string_view interfaceBlockName) {
			// TODO
		}
		void ExternalScopeEnvironment::RemoveFunDecl(std::string_view funDeclName) {
			// TODO
		}

		bool ExternalScopeEnvironment::StructDeclExists(std::string_view structName) const {
			auto searchRes = structs.find(structName);
			return searchRes != structs.end();
		}
		bool ExternalScopeEnvironment::StructFieldExists(std::string_view structName, std::string_view fieldName) const {
			auto searchRes = structs.find(structName);
			if (searchRes == structs.end()) return false;
			return searchRes->second->HasField(fieldName);
		}
		bool ExternalScopeEnvironment::IntBlockDeclExists(std::string_view intBlockName) const {
			auto searchRes = interfaceBlocks.find(intBlockName);
			return searchRes != interfaceBlocks.end();
		}
		bool ExternalScopeEnvironment::IntBlockFieldExists(std::string_view intBlockName, std::string_view fieldName) const {
			auto searchRes = interfaceBlocks.find(intBlockName);
			if (searchRes == interfaceBlocks.end()) return false;
			return searchRes->second->HasField(fieldName);
		}
		bool ExternalScopeEnvironment::IntBlockFieldExists(std::string_view fieldName) const {
			for (const auto& intBlock : interfaceBlocks) {
				if (intBlock.second->HasField(fieldName))
					return true;
			}
			return false;
		}
		bool ExternalScopeEnvironment::FunDeclExists(std::string_view funName) const {
			auto searchRes = functions.find(funName);
			return searchRes != functions.end();
		}

		std::shared_ptr<StructDecl> ExternalScopeEnvironment::GetStructDecl(std::string_view structName) const {
			std::shared_ptr<StructDecl> structDecl;
			auto searchRes = structs.find(structName);
			if (searchRes != structs.end()) {
				structDecl = searchRes->second;
			}
			assert(searchRes != structs.end() && "Check the existence of the struct declaration first!");
			return structDecl;
		}
		std::shared_ptr<VarDecl> ExternalScopeEnvironment::GetStructField(std::string_view structName, std::string_view fieldName) const {
			std::shared_ptr<VarDecl> field;
			std::shared_ptr<StructDecl> structDecl = GetStructDecl(structName);
			if (structDecl) {
				field = structDecl->GetField(fieldName);
			}
			return field;
		}
		std::shared_ptr<InterfaceBlockDecl> ExternalScopeEnvironment::GetIntBlockDecl(std::string_view intBlockName) const {
			std::shared_ptr<InterfaceBlockDecl> intBlockDecl;
			auto searchRes = interfaceBlocks.find(intBlockName);
			if (searchRes != interfaceBlocks.end()) {
				intBlockDecl = searchRes->second;
			}
			assert(searchRes != interfaceBlocks.end() && "Check the existence of the interface block declaration first!");
			return intBlockDecl;
		}
		std::shared_ptr<VarDecl> ExternalScopeEnvironment::GetIntBlockField(std::string_view intBlockName, std::string_view fieldName) const {
			std::shared_ptr<VarDecl> field;
			std::shared_ptr<InterfaceBlockDecl> intBlockDecl = GetIntBlockDecl(intBlockName);
			if (intBlockDecl) {
				field = intBlockDecl->GetField(fieldName);
			}
			return field;
		}
		std::shared_ptr<FunDecl> ExternalScopeEnvironment::GetFunDecl(std::string_view funName) const {
			std::shared_ptr<FunDecl> funDecl;
			auto searchRes = functions.find(funName);
			if (searchRes != functions.end()) {
				funDecl = searchRes->second;
			}
			assert(searchRes != functions.end() && "Check the existence of the function declaration first!");
			return funDecl;
		}

	}
}