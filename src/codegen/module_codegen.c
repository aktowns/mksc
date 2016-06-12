#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/OrcBindings.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/lto.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mks_node.h>
#include <uthash.h>

typedef struct mks_named_value {
    const char *name;
    LLVMValueRef value;
    UT_hash_handle hh;
} mks_named_value;

LLVMValueRef mks_codegen(mks_node_t *node, LLVMModuleRef module, LLVMBuilderRef builder);

LLVMValueRef mks_codegen_type(mks_node_t *node, LLVMModuleRef module, LLVMBuilderRef builder);

LLVMValueRef mks_codegen_number(mks_node_t *node) {
    return LLVMConstReal(LLVMDoubleType(), node->number);
}

LLVMValueRef mks_codegen_string(mks_node_t *node) {
    return LLVMConstString(node->string, (unsigned int) strlen(node->string), false);
}

LLVMValueRef mks_codegen_bool(mks_node_t *node) {
    return LLVMConstInt(LLVMInt1Type(), (unsigned long long int) node->boolean, false);
}

LLVMValueRef mks_codegen_function_call(mks_node_t *node, LLVMModuleRef module, LLVMBuilderRef builder) {
    LLVMValueRef func = LLVMGetNamedFunction(module, node->function_call->name->identifier);
    if (func == NULL) {
        return NULL;
    }

    return LLVMBuildCall(builder, func, NULL, 0, "calltmp");
}

LLVMValueRef mks_codegen_prototype(mks_node_t *node, LLVMModuleRef module) {
    LLVMValueRef func = LLVMGetNamedFunction(module, node->assignment->name->identifier);
    if (func != NULL) {
        // Verify parameter count matches.
        if (LLVMCountParams(func) != 1) {
            fprintf(stderr, "Existing function exists with different parameter count");
            return NULL;
        }

        // Verify that the function is empty.
        if (LLVMCountBasicBlocks(func) != 0) {
            fprintf(stderr, "Existing function exists with a body");
            return NULL;
        }
    } else {
        LLVMTypeRef *param = malloc(sizeof(LLVMTypeRef));
        *param = LLVMDoubleType();

        LLVMTypeRef funcType = LLVMFunctionType(LLVMDoubleType(), param, 1, 0);
        func = LLVMAddFunction(module, node->assignment->name->identifier, funcType);
        LLVMSetLinkage(func, LLVMExternalLinkage);
    }

    return func;
}


LLVMValueRef mks_codegen_if_expr(mks_node_t *node, LLVMModuleRef module, LLVMBuilderRef builder) {
    return NULL;
}

LLVMValueRef mks_codegen_type(mks_node_t *node, LLVMModuleRef module, LLVMBuilderRef builder) {
    switch (node->type->resolved_type) {
        case TY_NUMBER: {
            return mks_codegen_number(node);
        }
        case TY_STRING:
            break;
        case TY_ARRAY:
            break;
        case TY_BOOL: {
            return mks_codegen_bool(node);
        }
        case TY_TUPLE:
            break;
        case TY_MODULE:
            break;
        case TY_FUNCTION:
            break;
        case TY_UNIT:
            break;
    }

    return NULL;
}

LLVMValueRef mks_codegen(mks_node_t *node, LLVMModuleRef module, LLVMBuilderRef builder) {
    switch (node->tag) {
        case NODE_MODULE:
            break;
        case NODE_IMPORT:
            break;
        case NODE_FUNCTION_CALL: {
            return mks_codegen_function_call(node, module, builder);
        }
        case NODE_FUNCTION:
            break;
        case NODE_IDENTIFIER:
            break;
        case NODE_NUMBER_LITERAL: {
            return mks_codegen_number(node);
        }
        case NODE_STRING_LITERAL: {
            return mks_codegen_string(node);
        }
        case NODE_ARRAY_LITERAL:
            break;
        case NODE_SEQUENCE:
            break;
        case NODE_TUPLE:
            break;
        case NODE_ASSIGNMENT:
            break;
        case NODE_IF: {
            return mks_codegen_if_expr(node, module, builder);
        }
        case NODE_EMPTY:
            break;
        case NODE_BOOL: {
            return mks_codegen_bool(node);
        }
        case NODE_OPERATOR:break;
    }

    return NULL;
}

uint64_t symbol_resolver(const char *name, void *lookupCtx) {
    printf("symbol resolver: %s\n", name);
    return 0;
}


void shim_main(LLVMModuleRef mod) {
    LLVMTypeRef param_types[] = {LLVMInt32Type(), LLVMPointerType(LLVMInt8Type(), 0)};
    LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 2, 0);
    LLVMValueRef main = LLVMAddFunction(mod, "main", ret_type);
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main, "entry");
    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entry);
    LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(main, 0), LLVMGetParam(main, 0), "tmp");
    LLVMBuildRet(builder, tmp);
}

void emit_module() {
    LLVMModuleRef mod = LLVMModuleCreateWithName("test_module");
    LLVMTypeRef param_types[] = {LLVMInt32Type(), LLVMInt32Type()};
    LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 2, 0);
    LLVMValueRef sum = LLVMAddFunction(mod, "sum", ret_type);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(sum, "entry");

    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entry);

    LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sum, 0), LLVMGetParam(sum, 1), "tmp");
    LLVMBuildRet(builder, tmp);

    shim_main(mod);

    char *error = NULL;
    LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    LLVMExecutionEngineRef engine;
    error = NULL;

    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    // LLVMLinkInMCJIT();

    if (LLVMCreateExecutionEngineForModule(&engine, mod, &error) != 0) {
        fprintf(stderr, "failed to create execution engine\n");
        abort();
    }
    if (error) {
        fprintf(stderr, "error: %s\n", error);
        LLVMDisposeMessage(error);
        exit(EXIT_FAILURE);
    }

    LLVMTargetMachineRef tm = LLVMGetExecutionEngineTargetMachine(engine);

    LLVMOrcJITStackRef jit = LLVMOrcCreateInstance(tm);
    LLVMOrcAddEagerlyCompiledIR(jit, mod, symbol_resolver, NULL);
    printf("error: %s", LLVMOrcGetErrorMsg(jit));
    LLVMOrcTargetAddress target = LLVMOrcGetSymbolAddress(jit, "sum");
    printf("error: %s", LLVMOrcGetErrorMsg(jit));
    LLVMOrcErrorCode ec = LLVMOrcCreateIndirectStub(jit, "woo", target);
    if (ec != 0) {
        printf("error: %s", LLVMOrcGetErrorMsg(jit));
    }

    int32_t res = ((int32_t (*)(int32_t, int32_t)) target)(10, 20);
    printf("res: %i\n", res);

    char *err = NULL;

    LLVMPrintModuleToFile(mod, "output.ir", &err);
    LLVMWriteBitcodeToFile(mod, strdup("output.bc"));
    LLVMTargetMachineEmitToFile(tm, mod, strdup("output.asm"), LLVMAssemblyFile, &err);
    LLVMTargetMachineEmitToFile(tm, mod, strdup("output.o"), LLVMObjectFile, &err);
    printf("error: %s\n", err);

//    LLVMGenericValueRef args[] = {
//            LLVMCreateGenericValueOfInt(LLVMInt32Type(), 10, 0),
//            LLVMCreateGenericValueOfInt(LLVMInt32Type(), 20, 0)
//    };

    //LLVMGenericValueRef res = LLVMRunFunction(engine, sum, 2, args);
    //printf("%d\n", (int)LLVMGenericValueToInt(res, 0));
}
