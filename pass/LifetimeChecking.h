#ifndef LLVM_TRANSFORMS_INSTRUMENTATION_LIFETIMECHECKING_H
#define LLVM_TRANSFORMS_INSTRUMENTATION_LIFETIMECHECKING_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

namespace llvm {

struct LifetimeCheckingPass : PassInfoMixin<LifetimeCheckingPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
  static bool isRequired() { return true; }
};

/// Legacy pass creation function for the above pass.
FunctionPass *createLifetimeCheckingLegacyPass();

} // end namespace llvm

#endif // LLVM_TRANSFORMS_INSTRUMENTATION_LIFETIMECHECKING_H
