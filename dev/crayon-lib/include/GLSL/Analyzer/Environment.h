#pragma once

#include "GLSL/AST/Decl.h"

#include <memory>
#include <string_view>
#include <unordered_map>

namespace crayon {
	namespace glsl {

		class Environment {
		public:
			Environment() = default;
			Environment(std::shared_ptr<Environment> enclosingScope);

			void AddStructDecl(std::shared_ptr<StructDecl> structDecl);
			void AddInterfaceBlockDecl(std::shared_ptr<InterfaceBlockDecl> interfaceBlockDecl);
			void AddFunDecl(std::shared_ptr<FunDecl> funDecl);
			void AddVarDecl(std::shared_ptr<VarDecl> varDecl);

			bool StructDeclExist(std::string_view structName) const;

			bool IntBlockDeclExist(std::string_view intBlockName) const;
			bool IntBlocksVarDeclExist(std::string_view varName) const;

			bool FunDeclExist(std::string_view funName) const;
			bool VarDeclExist(std::string_view varName) const;

			std::shared_ptr<StructDecl> GetStructDecl(std::string_view structName) const;
			std::shared_ptr<InterfaceBlockDecl> GetIntBlockDecl(std::string_view intBlockName) const;
			std::shared_ptr<FunDecl> GetFunDecl(std::string_view funName) const;
			std::shared_ptr<VarDecl> GetVarDecl(std::string_view varName) const;

			bool IsExternalScope() const;
			std::shared_ptr<Environment> GetEnclosingScope() const;

		private:
			std::unordered_map<std::string_view, std::shared_ptr<StructDecl>> aggregates;
			std::unordered_map<std::string_view, std::shared_ptr<InterfaceBlockDecl>> interfaceBlocks;
			std::unordered_map<std::string_view, std::shared_ptr<FunDecl>> functions;
			std::unordered_map<std::string_view, std::shared_ptr<VarDecl>> variables;
			std::shared_ptr<Environment> enclosingScope;
		};

	}
}