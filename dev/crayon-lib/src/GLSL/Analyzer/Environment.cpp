#include "GLSL/Analyzer/Environment.h"

#include <cassert>

namespace crayon {
	namespace glsl {

		Environment::Environment(std::shared_ptr<Environment> enclosingScope)
			: enclosingScope(enclosingScope) {
		}

		void Environment::AddVertexInputLayoutBlock(std::shared_ptr<VertexInputLayoutBlock> vertexInputLayout) {
			this->vertexInputLayout = vertexInputLayout;
		}
		void Environment::AddMaterialPropertiesBlock(std::shared_ptr<MaterialPropertiesBlock> materialProperties) {
			this->materialProperties = materialProperties;
		}

		bool Environment::VertexAttribDeclExists(std::string_view vertexAttribName) const {
			if (!vertexInputLayout)
				return false;
			return vertexInputLayout->HasVertexAttribDecl(vertexAttribName);
		}

		bool Environment::MatPropDeclExists(std::string_view matPropName) const {
			if (!materialProperties)
				return false;
			return materialProperties->HasMatPropDecl(matPropName);
		}

		std::shared_ptr<VertexAttribDecl> Environment::GetVertexAttribDecl(std::string_view vertexAttribName) const {
			assert(vertexInputLayout && "Vertex Input Layout block doesn't exist!");
			return vertexInputLayout->GetVertexAttribDecl(vertexAttribName);
		}
		std::shared_ptr<MatPropDecl> Environment::GetMatPropDecl(std::string_view matPropName) const {
			assert(materialProperties && "Material Properties block doesn't exist!");
			return materialProperties->GetMatPropDecl(matPropName);
		}

		void Environment::AddStructDecl(std::shared_ptr<StructDecl> structDecl) {
			aggregates.insert({structDecl->GetName().lexeme, structDecl});
		}
		void Environment::AddInterfaceBlockDecl(std::shared_ptr<InterfaceBlockDecl> interfaceBlockDecl) {
			interfaceBlocks.insert({ interfaceBlockDecl->GetName().lexeme, interfaceBlockDecl });
		}
		void Environment::AddFunDecl(std::shared_ptr<FunDecl> funDecl) {
			functions.insert({funDecl->GetFunProto()->GetFunctionName().lexeme, funDecl });
		}
		void Environment::AddVarDecl(std::shared_ptr<VarDecl> varDecl) {
			variables.insert({ varDecl->GetVarName().lexeme, varDecl });
		}

		bool Environment::StructDeclExist(std::string_view structName) const {
			auto searchRes = aggregates.find(structName);
			if (searchRes == aggregates.end()) {
				if (enclosingScope) {
					return enclosingScope->StructDeclExist(structName);
				}
				return false;
			}
			return true;
		}

		bool Environment::IntBlockDeclExist(std::string_view interfaceBlockName) const {
			auto searchRes = interfaceBlocks.find(interfaceBlockName);
			if (searchRes == interfaceBlocks.end()) {
				if (enclosingScope) {
					return enclosingScope->IntBlockDeclExist(interfaceBlockName);
				}
				return false;
			}
			return true;
		}
		bool Environment::IntBlocksVarDeclExist(std::string_view varName) const {
			for (const auto& intBlockDecl : interfaceBlocks) {
				if (intBlockDecl.second->HasField(varName)) {
					return true;
				}
			}
			return false;
		}

		bool Environment::FunDeclExist(std::string_view funName) const {
			auto searchRes = functions.find(funName);
			if (searchRes == functions.end()) {
				if (enclosingScope) {
					return enclosingScope->FunDeclExist(funName);
				}
				return false;
			}
			return true;
		}
		bool Environment::VarDeclExist(std::string_view varName) const {
			auto searchRes = variables.find(varName);
			if (searchRes == variables.end()) {
				// Check "direct" variable declarations.
				if (enclosingScope) {
					if (enclosingScope->VarDeclExist(varName)) {
						return true;
					}
				}
				// If we reached here, the enclosing scope doesn't exist, which in turn means
				// we're now in the global (external) scope.
				// 
				// Since we couldn't find "direct" variable declarations,
				// we're going to check all interface blocks in the scope.
				if (IntBlocksVarDeclExist(varName)) {
					return true;
				}
				// Still haven't found the declaration name?
				// Well, our last hope is the extension GLSL blocks:
				// 1. Vertex Input Layout block.
				// 2. Material Properties block.
				// Check them starting with the vertex input layout block.
				if (VertexAttribDeclExists(varName)) {
					return true;
				}
				if (MatPropDeclExists(varName)) {
					return true;
				}
				// Give up.
				return false;
			}
			return true;
		}

		std::shared_ptr<StructDecl> Environment::GetStructDecl(std::string_view structName) const {
			auto searchRes = aggregates.find(structName);
			if (searchRes == aggregates.end()) {
				if (enclosingScope) {
					return enclosingScope->GetStructDecl(structName);
				}
				assert(searchRes != aggregates.end() && "Check the existence of a struct declaration first!");
				return std::shared_ptr<StructDecl>{};
			}
			return searchRes->second;
		}
		std::shared_ptr<InterfaceBlockDecl> Environment::GetIntBlockDecl(std::string_view interfaceBlockName) const {
			auto searchRes = interfaceBlocks.find(interfaceBlockName);
			if (searchRes == interfaceBlocks.end()) {
				if (enclosingScope) {
					return enclosingScope->GetIntBlockDecl(interfaceBlockName);
				}
				assert(searchRes != interfaceBlocks.end() && "Check the existence of an interface block declaration first!");
				return std::shared_ptr<InterfaceBlockDecl>{};
			}
			return searchRes->second;
		}
		std::shared_ptr<FunDecl> Environment::GetFunDecl(std::string_view funName) const {
			auto searchRes = functions.find(funName);
			if (searchRes == functions.end()) {
				if (enclosingScope) {
					return enclosingScope->GetFunDecl(funName);
				}
				assert(searchRes != functions.end() && "Check the existence of a function declaration first!");
				return std::shared_ptr<FunDecl>{};
			}
			return searchRes->second;
		}
		std::shared_ptr<VarDecl> Environment::GetVarDecl(std::string_view varName) const {
			auto searchRes = variables.find(varName);
			if (searchRes == variables.end()) {
				if (enclosingScope) {
					return enclosingScope->GetVarDecl(varName);
				}
				assert(searchRes != variables.end() && "Check the existence of a variable declaration first!");
				return std::shared_ptr<VarDecl>{};
			}
			return searchRes->second;
		}

		bool Environment::IsExternalScope() const {
			if (!enclosingScope)
				return true;
			return false;
		}
		std::shared_ptr<Environment> Environment::GetEnclosingScope() const {
			return enclosingScope;
		}

	}
}