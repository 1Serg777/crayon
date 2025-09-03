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

		bool SemanticAnalyzer::CheckVarDecl(VarDecl* varDecl,
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

		bool SemanticAnalyzer::CheckCtorCallExpr(CtorCallExpr* ctorCallExpr) {
			// We need to understand what category the type we're constructing belongs to.
			// This defines how the check will be performed.
			// Moreover, this separation will allow us to check constructors recursively (such
			// as the case when we're dealing with Array or structure types).
			const TypeSpec& ctorType = ctorCallExpr->GetType();
			if (ctorType.IsTransparent() && !ctorType.IsArray()) {
				// 1. If the type is a Transparent Non-Array type.
				// If the fundamental types of the arguments don't match, we assume
				// that the explicit conversion must take place.
				// For example, if we have a vec4 constructor call such as "vec4(int, devc2, float)",
				// the first and second arguments must be converted to "float" and "vec2", respectively.
				size_t compsToInit = GetComponentCount(ctorType.type.tokenType);
				size_t compsAvailable{0};
				for (const std::shared_ptr<Expr>& arg : ctorCallExpr->GetArgs()) {
					if (compsAvailable >= compsToInit) {
						// We're about to process another argument, but we already
						// have enough components to initalize the object with.
						// This is the case with an unused argument, which should be a compile-time error.
						// TODO: report this case.
						return false;
					}
					const TypeSpec& argType = envCtx.typeTable->GetType(arg->GetExprTypeId());
					// This is a pretty crude calculation, but since we're not generating actual
					// SPIR-V instructions, this approach should do.
					// However, when generating SPIR-V instructions we'll have to know precisely
					// which components from which arguments should be extracted, in which case
					// the calculation algorithm will have to be modified a bit.
					compsAvailable += GetComponentCount(argType.type.tokenType);
				}
			} else if (ctorType.IsStructure() && !ctorType.IsArray()) {
				// 2. If the type is a Non-Array Structure type.
				assert(false && "Structure constructors are not supported (implemented) yet!");
				return false;
			} else if (ctorType.IsArray()) {
				// 3. If the type is an Array type (Transparent or Structure).
				// In the case of an array type every argument must initialize exactly one element.
				// So if we have an array of vec2 elements, then each argument must initializy its
				// corresponding vec2 element in the array. If the types don't match, the
				// Implicit Conversion rules from GLSL spec. Chapter 4.1.10 are applied.
				// Note that the rules are different from the explicit conversions permitted
				// in non-array transparent type constructors.
				const std::vector<std::shared_ptr<Expr>>& ctorArgs = ctorCallExpr->GetArgs();
				if (ctorType.dimensions.size() != ctorArgs.size()) {
					// The number of arguments differs from the dimension of the array we're initializing.
					// TODO: report this
					return false;
				}
				// Now that we're here, we need to understand the type of each parameter we're expecting.
				// Essentially, it's the type minus the first array dimension.
				// For example, if we have a "vec2[3]"" array,
				// then each parameter (out of 3 needed) must be a "vec2".
				// If we have a "vec2[4][5]" array,
				// then each parameter (out of 4 needed) must be a "vec2[5]".
				// And this goes on for bigger arrays.
				TypeSpec paramType{};
				paramType.type = ctorType.type;
				paramType.typeDecl = ctorType.typeDecl;
				paramType.dimensions.resize(ctorType.dimensions.size() - 1);
				for (size_t i = 1; i < ctorType.dimensions.size(); i++) {
					paramType.dimensions[i - 1] = ctorType.dimensions[i];
				}
				// Now we check each argument's type if it's the same or at least promotable to
				// the parameter type we've just inferred.
				for (size_t i = 0; i < ctorArgs.size(); i++) {
					const std::shared_ptr<Expr>& arg = ctorArgs[i];
					const TypeSpec& argType = envCtx.typeTable->GetType(arg->GetExprTypeId());
					if (!IsTypePromotable(argType, paramType)) {
						// Argument type cannot be promoted to the parameter type.
						// This should include arrays, structures, etc.
						// I thing the "IsTypePromotable" function takes it all into account.
						// TODO: report this
						return false;
					}
				}
			} else {
				// Shouldn't be reachable.
				assert(false && "Shouldn't be reachable! Check the type of the constructor call!");
				return false;
			}
			// If we survived all the checks, then all's good, the constructor call if well-formed!
			return true;
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