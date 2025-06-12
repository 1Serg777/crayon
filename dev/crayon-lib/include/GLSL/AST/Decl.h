#pragma once

#include "GLSL/Token.h"
#include "GLSL/Type.h"
#include "GLSL/AST/Stmt.h"

#include <memory>
#include <vector>

namespace crayon {
	namespace glsl {

		class Expr;
        class TransUnit;
		class StructDecl;
		class VarDecl;
        class FunDecl;
        class QualDecl;

        enum class DeclContext {
            EXTERNAL,
            BLOCK,
        };

        class DeclVisitor {
        public:
            virtual void VisitTransUnit(TransUnit* transUnit) = 0;
			virtual void VisitStructDecl(StructDecl* structDecl) = 0;
            virtual void VisitFunDecl(FunDecl* funDecl) = 0;
            virtual void VisitQualDecl(QualDecl* qualDecl) = 0;
			virtual void VisitVarDecl(VarDecl* varDecl) = 0;
        };

        class Decl {
        public:
			virtual ~Decl() = default;
            virtual void Accept(DeclVisitor* declVisitor) = 0;
        };

        class TransUnit : public Decl {
		public:
			void Accept(DeclVisitor* declVisitor) override;

			// void AddFunDecl(std::shared_ptr<FunDecl> funDecl);
			// void AddQualDecl(std::shared_ptr<QualDecl> qualDecl);
			// void AddVarDecl(std::shared_ptr<VarDecl> varDecl);
			void AddDeclaration(std::shared_ptr<Decl> decl);

			const std::vector<std::shared_ptr<Decl>>& GetDeclarations();

		private:
			std::vector<std::shared_ptr<Decl>> decls;
		};

		class StructDecl : public Decl {
		public:
			StructDecl() = default;
			StructDecl(const Token& structName);

			void Accept(DeclVisitor* declVisitor) override;

			void AddField(std::shared_ptr<VarDecl> fieldDecl);

			const Token& GetStructName() const;
			const std::vector<std::shared_ptr<VarDecl>>& GetFields() const;

		private:
			Token structName;
			std::vector<std::shared_ptr<VarDecl>> fields;
		};

        class VarDecl : public Decl {
		public:
			VarDecl(const FullSpecType& varType, const Token& varName);
			virtual ~VarDecl() = default;

			void Accept(DeclVisitor* declVisitor) override;

			bool IsVarTypeBasic() const;
			bool IsVarTypeAggregate() const;
			// i.e., int[] a; float[][] b;
			bool IsVarTypeArray() const;
			// i.e., int a[]; float b[][];
			bool IsVarArray() const;
			// i.e., int[] a; float b[]; vec3[] c[];
			bool IsArray() const;

			// bool StructDeclPresent() const;
			// void SetStructDecl(std::shared_ptr<StructDecl> structDecl);
			// std::shared_ptr<StructDecl> GetStructDecl() const;

			void AddDimension(std::shared_ptr<Expr> dimSizeExpr);
			size_t GetDimensionCount() const;
			const std::vector<std::shared_ptr<Expr>>& GetDimensions() const;

			bool HasInitializerExpr() const;
			void SetInitializerExpr(std::shared_ptr<Expr> initExpr);
			std::shared_ptr<Expr> GetInitializerExpr() const;

			const FullSpecType& GetVarType() const;
			const Token& GetVarName() const;

		private:
			FullSpecType varType;
			Token varName;
			std::vector<std::shared_ptr<Expr>> dimensions;
			std::shared_ptr<StructDecl> structDecl;
			std::shared_ptr<Expr> initExpr;
		};

        class FunParam : public VarDecl {
		public:
			FunParam(const FullSpecType& paramType);
			FunParam(const FullSpecType& paramType, const Token& paramName);
			virtual ~FunParam() = default;

			bool HasName() const;
		};

		class FunParamList {
		public:
			void AddFunctionParameter(const FunParam& funParam);

			bool Empty() const;

			const std::list<FunParam>& GetParams() const;

		private:
			std::list<FunParam> funParams;
		};

		class FunProto {
		public:
			FunProto(const FullSpecType& retType, const Token& funName);
			FunProto(const FullSpecType& retType, const Token& funName, std::shared_ptr<FunParamList> params);

			const FullSpecType& GetReturnType() const;
			const Token& GetFunctionName() const;

			bool FunctionParameterListEmpty() const;
			const FunParamList& GetFunctionParameterList() const;

		private:
			FullSpecType retType;
			Token funName;

			std::shared_ptr<FunParamList> params;
		};

        class FunDecl : public Decl {
		public:
			FunDecl(std::shared_ptr<FunProto> funProto);
			FunDecl(std::shared_ptr<FunProto> funProto, std::shared_ptr<BlockStmt> stmts);
			virtual ~FunDecl() = default;

			void Accept(DeclVisitor* declVisitor) override;

            bool IsFunDecl() const;
            bool IsFunDef() const;
			bool IsFunBlockEmpty() const;

			const FunProto& GetFunProto() const;
            std::shared_ptr<BlockStmt> GetBlockStmt() const;

		private:
			std::shared_ptr<FunProto> funProto;
			std::shared_ptr<BlockStmt> stmts;
		};

        class QualDecl : public Decl {
		public:
			QualDecl(const TypeQual& qualifier);
			virtual ~QualDecl() = default;

			void Accept(DeclVisitor* declVisitor) override;

			const TypeQual& GetTypeQualifier() const;

		private:
			TypeQual qualifier;
		};
   
	}
}