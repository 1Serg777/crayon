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
		class InterfaceBlockDecl;
		class DeclList;
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
			virtual void VisitInterfaceBlockDecl(InterfaceBlockDecl* interfaceBlockDecl) = 0;
			virtual void VisitDeclList(DeclList* declList) = 0;
			virtual void VisitStructDecl(StructDecl* structDecl) = 0;
			virtual void VisitVarDecl(VarDecl* varDecl) = 0;
            virtual void VisitFunDecl(FunDecl* funDecl) = 0;
            virtual void VisitQualDecl(QualDecl* qualDecl) = 0;
        };

        class Decl {
        public:
			virtual ~Decl() = default;
            virtual void Accept(DeclVisitor* declVisitor) = 0;
        };

		class NamedEntity {
		public:
			NamedEntity() = default;
			NamedEntity(const Token& name);
			bool HasName() const;
			const Token& GetName() const;

		private:
			Token name;
		};

		class AggregateEntity {
		public:
			void AddField(std::shared_ptr<VarDecl> fieldDecl);
			bool HasField(std::string_view fieldName) const;
			std::shared_ptr<VarDecl> GetField(std::string_view fieldName);
			const std::vector<std::shared_ptr<VarDecl>>& GetFields() const;

		private:
			std::vector<std::shared_ptr<VarDecl>> fields;
		};

		class ArrayEntity {
		public:
			void AddDimension(std::shared_ptr<Expr> dimSizeExpr);
			size_t GetDimensionCount() const;
			bool IsArray() const;
			const std::vector<std::shared_ptr<Expr>>& GetDimensions() const;
		private:
			std::vector<std::shared_ptr<Expr>> dimensions;
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

		class InterfaceBlockDecl : public Decl,
								   public NamedEntity,
								   public AggregateEntity,
								   public ArrayEntity {
		public:
			InterfaceBlockDecl() = default;
			InterfaceBlockDecl(const Token& name);
			InterfaceBlockDecl(const Token& name, const TypeQual& typeQual);

			void Accept(DeclVisitor* declVisitor) override;

			const TypeQual& GetTypeQualifier() const;

		private:
			TypeQual typeQual;
		};

		class DeclList : public Decl {
		public:
			DeclList(const FullSpecType& fullSpecType);

			void Accept(DeclVisitor* declVisitor) override;

			void AddDecl(std::shared_ptr<VarDecl> decl);
			const std::vector<std::shared_ptr<VarDecl>>& GetDecls() const;

			const FullSpecType& GetFullSpecType() const;

		private:
			FullSpecType fullSpecType;
			std::vector<std::shared_ptr<VarDecl>> decls;
		};

		class StructDecl : public Decl,
						   public NamedEntity,
						   public AggregateEntity {
		public:
			StructDecl() = default;
			StructDecl(const Token& structName);

			void Accept(DeclVisitor* declVisitor) override;

			bool AnonymousStructDecl() const;
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

			void AddDimension(std::shared_ptr<Expr> dimSizeExpr);
			size_t GetDimensionCount() const;
			const std::vector<std::shared_ptr<Expr>>& GetDimensions() const;

			bool HasInitializerExpr() const;
			void SetInitializerExpr(std::shared_ptr<Expr> initExpr);
			std::shared_ptr<Expr> GetInitializerExpr() const;

			// Returns the type of an expression where the variable is used directly.
			// i.e., if we have a variable declared as "int[3] a[2]", then if it's used
			// in an expression by specifying its name directly as "a", its type will be "int[2][3]".
			GlslExprType GetExprType() const;

			const FullSpecType& GetVarType() const;
			const Token& GetVarName() const;

		private:
			FullSpecType varType;
			Token varName;
			std::vector<std::shared_ptr<Expr>> dimensions;
			std::shared_ptr<Expr> initExpr;
		};

        class FunParam : public VarDecl {
		public:
			FunParam(const FullSpecType& paramType);
			FunParam(const FullSpecType& paramType, const Token& paramName);
			virtual ~FunParam() = default;

			bool HasName() const;
		};

		class FunProto {
		public:
			FunProto(const FullSpecType& retType, const Token& funName);

			const FullSpecType& GetReturnType() const;
			const Token& GetFunctionName() const;

			void AddFunParam(std::shared_ptr<FunParam> funParam);
			bool FunParamListEmpty() const;
			const std::vector<std::shared_ptr<FunParam>>& GetFunParamList() const;

		private:
			FullSpecType retType;
			Token funName;
			std::vector<std::shared_ptr<FunParam>> params;
		};

        class FunDecl : public Decl {
		public:
			FunDecl(std::shared_ptr<FunProto> funProto);
			FunDecl(std::shared_ptr<FunProto> funProto, std::shared_ptr<BlockStmt> stmts);
			virtual ~FunDecl() = default;

			void Accept(DeclVisitor* declVisitor) override;

            bool IsFunDecl() const;
            bool IsFunDef() const;

			std::shared_ptr<FunProto> GetFunProto() const;
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