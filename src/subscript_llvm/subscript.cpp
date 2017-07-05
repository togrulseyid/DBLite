//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This small program provides an example of how to build quickly a small module
// with function Fibonacci and execute it with the JIT.
//
// The goal of this snippet is to create in the memory the LLVM module
// consisting of one function as follow:
//
//   int f(int v1, int v2, int v3, int v4) {
//     return (v1 + v2) * (v3 - v4);
//   }
//
// Once we have this, we compile the module via JIT, then execute the `fib'
// function and return result to a driver, i.e. to a "host program".
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/APInt.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;

static Function *CreateFFunction(Module *M, LLVMContext &Context) {

  Function *FibF =
    cast<Function>(M->getOrInsertFunction("f", Type::getInt32Ty(Context),
                                          Type::getInt32Ty(Context),
                                          Type::getInt32Ty(Context),
                                          Type::getInt32Ty(Context),
                                          Type::getInt32Ty(Context),
                                          nullptr));

  Function::arg_iterator args = FibF->arg_begin();
  Argument* v1 = &*args;
  args++;
  v1->setName("v1");
  Argument* v2 = &*args;
  args++;
  v2->setName("v2");
  Argument* v3 = &*args;
  args++;
  v3->setName("v3");
  Argument* v4 = &*args;
  args++;
  v4->setName("v4");
  
  BasicBlock* RecurseBB = BasicBlock::Create(Context, "recurse", FibF);

  Value *add1 = BinaryOperator::CreateAdd(v1, v2, "arg", RecurseBB);

  Value *sub1 = BinaryOperator::CreateSub(v3, v4, "arg", RecurseBB);

  Value *Sum = BinaryOperator::CreateMul(add1, sub1, "addresult", RecurseBB);

  ReturnInst::Create(Context, Sum, RecurseBB);
  return FibF;
}

int main(int argc, char **argv) {
  int v1 = atol(argv[1]);
  int v2 = atol(argv[2]);
  int v3 = atol(argv[3]);
  int v4 = atol(argv[4]);
  bool useJIT = false;

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  LLVMContext Context;

  std::unique_ptr<Module> Owner(new Module("test", Context));
  Module *M = Owner.get();
  Function *FibF = CreateFFunction(M, Context);
  std::string errStr;
  ExecutionEngine *EE =
    EngineBuilder(std::move(Owner))
    .setEngineKind(useJIT?EngineKind::JIT:EngineKind::Interpreter)
    .setErrorStr(&errStr)
    .create();

  if (!EE) {
    errs() << argv[0] << ": Failed to construct ExecutionEngine: " << errStr
           << "\n";
    return 1;
  }

  errs() << "verifying... ";
  if (verifyModule(*M)) {
    errs() << argv[0] << ": Error constructing function!\n";
    return 1;
  }

  errs() << "OK\n";
  errs() << "We just constructed this LLVM module:\n\n---------\n" << *M;

  std::vector<GenericValue> Args(4);
  Args[0].IntVal = APInt(32, v1);
  Args[1].IntVal = APInt(32, v2);
  Args[2].IntVal = APInt(32, v3);
  Args[3].IntVal = APInt(32, v4);
  
  GenericValue GV = EE->runFunction(FibF, Args);

  // import result of execution
  outs() << "Result: " << GV.IntVal << "\n";

  return 0;
}
