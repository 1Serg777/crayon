#pragma once

#include "GLSL/Type.h"

#include "GLSL/AST/Block.h"
#include "GLSL/AST/Decl.h"

#include "GLSL/Reflect/ReflectCommon.h"

#include <memory>
#include <string_view>
#include <unordered_map>

namespace crayon {
	namespace glsl {

		class NestedScopeEnvironment {
		public:
			NestedScopeEnvironment(std::shared_ptr<NestedScopeEnvironment> enclosingScope);

			virtual bool SymbolDeclared(std::string_view symbolName) const;

			bool HasEnclosingScope() const;
			bool IsExternalScope() const;
			std::shared_ptr<NestedScopeEnvironment> GetEnclosingScope() const;

			void AddVarDecl(std::shared_ptr<VarDecl> varDecl);
			void RemoveVarDecl(std::string_view varDeclName);
			bool VarDeclExists(std::string_view varName) const;
			std::shared_ptr<VarDecl> GetVarDecl(std::string_view varName) const;

		private:
			std::unordered_map<std::string_view, std::shared_ptr<VarDecl>> variables;
			std::shared_ptr<NestedScopeEnvironment> enclosingScope;
		};

		// Reflect the idea of the External Scope through classes.
		// The idea is that a nested environment will not have function declarations or
		// any of the language extension blocks that we introduced. As a result, this will just
		// only take space and potentially give the programmer the wrong idea about the interface of the class.
		class ExternalScopeEnvironment : public NestedScopeEnvironment {
		public:
			ExternalScopeEnvironment();

			virtual bool SymbolDeclared(std::string_view symbolName) const;

			// Extended GLSL block declarations.

			void SetVertexInputLayoutBlock(std::shared_ptr<VertexInputLayoutBlock> vertexInputLayout);
			void SetMaterialPropertiesBlock(std::shared_ptr<MaterialPropertiesBlock> materialProperties);
			void SetColorAttachmentsBlock(std::shared_ptr<ColorAttachmentsBlock> colorAttachments);

			bool VertexInputLayoutFieldExists(std::string_view vertexAttribName) const;
			bool MatPropsFieldExists(std::string_view matPropName) const;
			bool ColorAttachmentFieldExists(std::string_view colorAttachmentName) const;

			std::shared_ptr<VertexAttribDecl> GetVertexAttribDecl(std::string_view vertexAttribName) const;
			std::shared_ptr<MatPropDecl> GetMatPropDecl(std::string_view matPropName) const;
			std::shared_ptr<ColorAttachmentDecl> GetColorAttachmentDecl(std::string_view colorAttachmentName) const;

			// Core GLSL declarations.

			void AddStructDecl(std::shared_ptr<StructDecl> structDecl);
			void AddInterfaceBlockDecl(std::shared_ptr<InterfaceBlockDecl> intBlockDecl);
			void AddFunDecl(std::shared_ptr<FunDecl> funDecl);

			void RemoveStructDecl(std::string_view structDeclName);
			void RemoveInterfaceBlockDecl(std::string_view intBlockName);
			void RemoveFunDecl(std::string_view funDeclName);

			bool StructDeclExists(std::string_view structName) const;
			bool StructFieldExists(std::string_view structName, std::string_view fieldName) const;
			bool IntBlockDeclExists(std::string_view intBlockName) const;
			bool IntBlockFieldExists(std::string_view intBlockName, std::string_view fieldName) const;
			bool IntBlockFieldExists(std::string_view fieldName) const;
			bool FunDeclExists(std::string_view funName) const;

			std::shared_ptr<StructDecl> GetStructDecl(std::string_view structName) const;
			std::shared_ptr<VarDecl> GetStructField(std::string_view structName, std::string_view fieldName) const;
			std::shared_ptr<InterfaceBlockDecl> GetIntBlockDecl(std::string_view intBlockName) const;
			std::shared_ptr<VarDecl> GetIntBlockField(std::string_view intBlockName, std::string_view fieldName) const;
			std::shared_ptr<FunDecl> GetFunDecl(std::string_view funName) const;

		private:
			std::shared_ptr<VertexInputLayoutBlock> vertexInputLayout;
			std::shared_ptr<MaterialPropertiesBlock> materialProperties;
			std::shared_ptr<ColorAttachmentsBlock> colorAttachments;

			std::unordered_map<std::string_view, std::shared_ptr<StructDecl>> structs;
			std::unordered_map<std::string_view, std::shared_ptr<InterfaceBlockDecl>> interfaceBlocks;
			std::unordered_map<std::string_view, std::shared_ptr<FunDecl>> functions;
		};

		struct EnvironmentContext {
			ExternalScopeEnvironment* externalScope{nullptr};
			NestedScopeEnvironment* currentScope{nullptr};
			TypeTable* typeTable{nullptr};
		};

	}
}