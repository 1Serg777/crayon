#include "GLSL/Analyzer/SemanticAnalyzer.h"

#include "GLSL/Error.h"

namespace crayon {
	namespace glsl {

		void SemanticAnalyzer::SetEnvironmentContext(const EnvironmentContext& envCtx) {
			this->envCtx = envCtx;
			exprTypeInferenceVisitor.SetEnvironmentContext(this->envCtx);
		}
		void SemanticAnalyzer::ResetEnvironmentContext() {
			this->envCtx = EnvironmentContext();
			exprTypeInferenceVisitor.ResetEnvironmentContext();
		}

		bool SemanticAnalyzer::CheckVertexAttribDecl(std::shared_ptr<VertexAttribDecl> vertexAttribDecl) {
			return CheckVertexAttribType(vertexAttribDecl) && CheckVertexAttribChannel(vertexAttribDecl);
		}
		bool SemanticAnalyzer::CheckVertexAttribType(std::shared_ptr<VertexAttribDecl> vertexAttribDecl) {
			// Check the attribute's type specifier. The type must not be:
			// 1. A boolean type
			// 2. An opaque type
			// 3. An array type, or
			// 4. A structure
			const TypeSpec& typeSpec = vertexAttribDecl->GetTypeSpec();
			TokenType fundType = GetFundamentalType(typeSpec.type.tokenType);
			// Check #1.
			if (fundType == TokenType::BOOL) {
				return false;
			}
			// Check #2.
			if (IsTypeOpaque(typeSpec.type.tokenType)) {
				return false;
			}
			// Check #3.
			if (typeSpec.IsArray()) {
				return false;
			}
			// Check #4.
			// if (IsTypeAggregate(typeSpec.type, currentScope)) {
			if (typeSpec.IsStructure()) {
				return false;
			}
			return true;
		}
		bool SemanticAnalyzer::CheckVertexAttribChannel(std::shared_ptr<VertexAttribDecl> vertexAttribDecl) {
			// The channel identifier must map to one of the values in the 'VertexAttribChannel' enumeration.
			return IdentifierTokenToVertexAttribChannel(vertexAttribDecl->GetChannel()) != VertexAttribChannel::UNDEFINED;
		}

		bool SemanticAnalyzer::CheckColorAttachmentDecl(std::shared_ptr<ColorAttachmentDecl> colorAttachmentDecl) {
			return CheckColorAttachmentType(colorAttachmentDecl) && CheckColorAttachmentChannel(colorAttachmentDecl);
		}
		bool SemanticAnalyzer::CheckColorAttachmentType(std::shared_ptr<ColorAttachmentDecl> colorAttachmentDecl) {
			// Check the attachment's type specifier. The type must not be:
			// 1. A boolean type
			// 2. A double-precision scalar or vector type
			// 3. An opaque type
			// 4. A matrix type
			// 5. A structure
			// 6. (Custom, not in the specification) An array type
			const TypeSpec& typeSpec = colorAttachmentDecl->GetTypeSpec();
			TokenType fundType = GetFundamentalType(typeSpec.type.tokenType);
			// Check #1.
			if (fundType == TokenType::BOOL) {
				return false;
			}
			// Check #2.
			if (fundType == TokenType::DOUBLE) {
				return false;
			}
			// Check #3.
			if (IsTypeOpaque(typeSpec.type.tokenType)) {
				return false;
			}
			// Check #4.
			if (IsTypeMatrix(typeSpec.type.tokenType)) {
				return false;
			}
			// Check #5.
			// if (IsTypeAggregate(typeSpec.type, currentScope)) {
			if (typeSpec.IsStructure()) {
				return false;
			}
			// Check #6.
			if (typeSpec.IsArray()) {
				return false;
			}
			return true;
		}
		bool SemanticAnalyzer::CheckColorAttachmentChannel(std::shared_ptr<ColorAttachmentDecl> colorAttachmentDecl) {
			// The channel identifier must map to one of the values in the 'ColorAttachmentChannel' enumeration.
			return IdentifierTokenToColorAttachmentChannel(colorAttachmentDecl->GetChannel()) != ColorAttachmentChannel::UNDEFINED;
		}

		bool SemanticAnalyzer::CheckVarDecl(std::shared_ptr<VarDecl> varDecl,
			                                DeclContext declContext,
										    ShaderType shaderType) {
			bool valid{true};
			FullSpecType& varType = varDecl->GetVarType();
			// 1. The "buffer", "in", "out", or "uniform" qualifiers are not allowed in contexts
			//    other than the external (global) context.
			// 2. Note, the "const" qualifier is allowed in any context.
			// 3. The "shared" qualifier is only allowed in compute shaders.
			// 4. The "attribute", and "varying" qualifiers are compatibility profile only,
			//    which we don't support.
			if (varType.qualifier.storage.has_value()) {
				const Token& storageQual = varType.qualifier.storage.value();
				// Again, the "const" qualifier is allowed everywhere.
				if (storageQual.tokenType != TokenType::CONST) {
					if (declContext != DeclContext::EXTERNAL) {
						if (storageQual.tokenType == TokenType::BUFFER ||
					        storageQual.tokenType == TokenType::IN ||
					        storageQual.tokenType == TokenType::OUT ||
					        storageQual.tokenType == TokenType::UNIFORM) {
					        valid = false;
							// Report storage qualifier-declaration context mismatch.
						}
					}
					// Should I even support these keywords
					// if I don't plan on supporting the compatibility profile?
					/*
					if (storageQual.tokenType == TokenType::ATTRIBUTE ||
				    	storageQual.tokenType == TokenType::VARYING) {
						// TODO: report to the user that these attributes are compatibility profile only,
						// which is not supported.
						valid = false;
					}
					*/
				}
			}
			// Next, we check the variable's type specifier. One of the checks we're going to do,
			// is to see whether the type is an array type and if so, whether the dimensions
			// are specified using constant expressions. There's a problem though.
			// Since we distringuish between type array specifiers and variable array specifiers,
			// we either have to check both, or, better yet, we can retrieve the combined type
			// of the declaration and check that instead.
			TypeSpec combinedVarDeclType = varDecl->GetVarTypeSpec();
			if (CheckTypeSpec(combinedVarDeclType)) {
				valid = false;
				// Report ill-formed type.
			}
			// The initializer expression check is the next step.
			if (varDecl->HasInitializerExpr()) {
				std::shared_ptr<Expr> initializer = varDecl->GetInitializerExpr();
				initializer->Accept(&exprTypeInferenceVisitor);
				size_t initExprTypeId = initializer->GetExprTypeId();
				const TypeSpec& initExprType = envCtx.typeTable->GetType(initExprTypeId);
				// 1. Strong type comparison. Types must be the same.
				/*
				if (initExprType != combinedVarDeclType) {
					valid = false;
					// Report a type mismatch between the variable declaration's type and its initializer.
				}
				*/
				// 2. We accept when the initializer expression's type is promotable
				//    to the type of the variable declaration.
				if (!IsTypePromotable(initExprType, combinedVarDeclType)) {
					valid = false;
					// Report a type mismatch between the variable declaration's type and its initializer.
				}
				return true;
			}
			return valid;
		}

		bool SemanticAnalyzer::CheckTypeSpec(TypeSpec& typeSpec) {
			bool valid{true};
			if (typeSpec.IsArray()) {
				// For now, we don't support implicit or variable dimension specializations,
				// meaning we must use a constant expression to specify the array dimension.
				for (ArrayDim& arrayDim : typeSpec.dimensions) {
					if (!arrayDim.dimExpr) {
						// Report that implicitly defined array dimension size is not supported!
						valid = false;
					} else {
						arrayDim.dimExpr->Accept(&exprTypeInferenceVisitor);
						if (!arrayDim.dimExpr->IsConstExpr()) {
							valid = false;
							// Report a semantic error, since a non-const expression was used
							// to declare the size of an array dimension!
						} else {
							size_t arrayDimExprTypeId = arrayDim.dimExpr->GetExprTypeId();
							const TypeSpec& arrayDimType = envCtx.typeTable->GetType(arrayDimExprTypeId);
							if (arrayDimType.IsScalar() &&
						        (arrayDimType.type.tokenType == TokenType::INT ||
								 arrayDimType.type.tokenType == TokenType::UINT)) {
								// Scalar integer array dimension expression type.
								arrayDim.dimExpr->Accept(&exprEvalVisitor);
								if (exprEvalVisitor.ResultInt()) {
									arrayDim.dimSize = static_cast<size_t>(exprEvalVisitor.GetIntResult());
								} else if (exprEvalVisitor.ResultUint()) {
									arrayDim.dimSize = static_cast<size_t>(exprEvalVisitor.GetUintResult());
								} else {
									// Report a semantic error, because the expression wasn't
									// an integer (int or uint) scalar expression.
									// But we shouldn't really reach here,
									// because of the type check above.
								}
							} else {
								valid = false;
								// Report an incorrect type of an expression defining
								// the size of an array dimension.
							}
						}
					}
				}
			}
			return valid;
		}

	}
}