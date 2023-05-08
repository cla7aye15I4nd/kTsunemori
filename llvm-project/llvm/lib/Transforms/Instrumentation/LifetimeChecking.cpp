//===- LifetimeChecking.cpp - Instrumentation for run-time bounds checking
//--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Instrumentation/LifetimeChecking.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TargetFolder.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Value.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdint>
#include <utility>

using namespace llvm;

#define DEBUG_TYPE "lifetime"

using BuilderTy = IRBuilder<TargetFolder>;

bool addLifetimeChecks(Function &F, TargetLibraryInfo &TLI) {
  if (F.getName().startswith("__lifetime_"))
    return false;

  Module *M = F.getParent();
  LLVMContext &C = M->getContext();
  const DataLayout &DL = M->getDataLayout();
  M->getOrInsertFunction("__lifetime_escape", Type::getVoidTy(C),
                         Type::getInt8PtrTy(C), Type::getInt8PtrTy(C));
  M->getOrInsertFunction("__lifetime_start", Type::getVoidTy(C),
                         Type::getInt8PtrTy(C), Type::getInt64Ty(C));
  M->getOrInsertFunction("__lifetime_end", Type::getVoidTy(C),
                         Type::getInt8PtrTy(C));

  Function *EscapeFn = M->getFunction("__lifetime_escape");
  Function *StartFn = M->getFunction("__lifetime_start");
  Function *EndFn = M->getFunction("__lifetime_end");

  SmallVector<CallInst *, 16> AllocCalls;
  SmallVector<CallInst *, 16> FreeCalls;
  SmallVector<StoreInst *, 16> StoreInsts;

  for (auto &BB : F) {
    for (auto &I : BB) {
      if (auto *CI = dyn_cast<CallInst>(&I)) {
        if (Function *Callee = CI->getCalledFunction()) {
          if (Callee->getName() == "kmalloc_wrapper") {
            AllocCalls.push_back(CI);
          } else if (Callee->getName() == "kfree") {
            FreeCalls.push_back(CI);
          }
        }
      } else if (auto *SI = dyn_cast<StoreInst>(&I)) {
        if (SI->getValueOperand()->getType()->isPointerTy()) {
          StoreInsts.push_back(SI);
        }
      }
    }
  }

  if (AllocCalls.empty() && FreeCalls.empty() && StoreInsts.empty())
    return false;

  for (auto *CI : AllocCalls) {
    Instruction *InsertPt = CI->getInsertionPointAfterDef();
    BuilderTy IRB(CI->getParent(), BasicBlock::iterator(InsertPt),
                  TargetFolder(DL));
    Value *Ptr = CI;
    Value *Size = CI->getArgOperand(0);
    IRB.CreateCall(StartFn, {Ptr, Size});
  }

  for (auto *CI : FreeCalls) {
    BuilderTy IRB(CI->getParent(), BasicBlock::iterator(CI), TargetFolder(DL));
    Value *Ptr = CI->getArgOperand(0);
    IRB.CreateCall(EndFn, {Ptr});
  }

  for (auto *SI : StoreInsts) {
    BuilderTy IRB(SI->getParent(), BasicBlock::iterator(SI), TargetFolder(DL));
    Value *Ptr = SI->getPointerOperand();
    Value *Val = SI->getValueOperand();

    IRB.CreateCall(EscapeFn, {Val, Ptr});
  }

  return true;
}

PreservedAnalyses LifetimeCheckingPass::run(Function &F,
                                            FunctionAnalysisManager &AM) {
  auto &TLI = AM.getResult<TargetLibraryAnalysis>(F);

  if (!addLifetimeChecks(F, TLI))
    return PreservedAnalyses::all();

  return PreservedAnalyses::none();
}

namespace {
struct LifetimeCheckingLegacyPass : public FunctionPass {
  static char ID;

  LifetimeCheckingLegacyPass() : FunctionPass(ID) {
    initializeLifetimeCheckingLegacyPassPass(*PassRegistry::getPassRegistry());
  }

  bool runOnFunction(Function &F) override {
    auto &TLI = getAnalysis<TargetLibraryInfoWrapperPass>().getTLI(F);
    return addLifetimeChecks(F, TLI);
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<TargetLibraryInfoWrapperPass>();
    AU.addRequired<ScalarEvolutionWrapperPass>();
  }
};
} // namespace

char LifetimeCheckingLegacyPass::ID = 0;
INITIALIZE_PASS_BEGIN(LifetimeCheckingLegacyPass, "lifetime-checking",
                      "Run-time lifetime checking", false, false)
INITIALIZE_PASS_DEPENDENCY(TargetLibraryInfoWrapperPass)
INITIALIZE_PASS_END(LifetimeCheckingLegacyPass, "lifetime-checking",
                    "Run-time lifetime checking", false, false)

FunctionPass *llvm::createLifetimeCheckingLegacyPass() {
  return new LifetimeCheckingLegacyPass();
}
