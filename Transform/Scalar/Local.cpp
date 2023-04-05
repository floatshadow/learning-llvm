#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h" 
#include <llvm/ADT/SmallVector.h>
#include <llvm/Config/llvm-config-64.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/Casting.h>

/* LLVM New Pass Manager: https://llvm.org/docs/WritingAnLLVMNewPMPass.html */

using namespace llvm;

namespace {

int algebraicIdentity(Function &F) {
    SmallVector<Instruction *, 10> WorkList;
    for (auto I = inst_begin(F), E = inst_end(F); I != E; ++I) {

        if (!I->isBinaryOp())
            continue;
        
        Value *Op0 = I->getOperand(0);
        Value *Op1 = I->getOperand(1);

        int64_t ConstVal0, ConstVal1;
        bool HasDeadInst = true;
        if (isa<ConstantInt>(Op0)) {
            ConstVal0 = dyn_cast<ConstantInt>(Op0)->getSExtValue();
        }
        if (isa<ConstantInt>(Op1)) {
            ConstVal1 = dyn_cast<ConstantInt>(Op1)->getSExtValue();
        }

        switch (I->getOpcode()) {
        case Instruction::Add:
            if (isa<ConstantInt>(Op0) && ConstVal0 == 0) {
                I->replaceAllUsesWith(Op1); // 0 + x = x
            } else if (isa<ConstantInt>(Op1) && ConstVal1 == 0) {
                I->replaceAllUsesWith(Op0); // x + 0 = x
            } else {
                HasDeadInst = false;
            }
            break;
        case Instruction::Sub:
            if (isa<ConstantInt>(Op1) && ConstVal1 == 0) {
                I->replaceAllUsesWith(Op0); // x - 0 = x
            } else if (Op0 == Op1) {
                I->replaceAllUsesWith(ConstantInt::getSigned(I->getType(), 0)); // x - x = 0
            } else {
                HasDeadInst = false;
            }
            break;
        case Instruction::Mul:
            if (isa<ConstantInt>(Op0) && ConstVal0 == 1) {
                I->replaceAllUsesWith(Op1); // 1 * x = x
            } else if (isa<ConstantInt>(Op1) && ConstVal1 == 1) {
                I->replaceAllUsesWith(Op0); // x * 1 = x
            } else {
                HasDeadInst = false;
            }
            break;
        case Instruction::SDiv:
            if (isa<ConstantInt>(Op1) && ConstVal1 == 1) {
                I->replaceAllUsesWith(Op0); // x / 1 = x
            } else if (Op0 == Op1) {
                I->replaceAllUsesWith(ConstantInt::getSigned(I->getType(), 1));
            } else {
                HasDeadInst = false;
            }
            break;
        default:
            HasDeadInst = false;
            break;
        }

        if (HasDeadInst) {
            WorkList.push_back(&*I);
            outs() << "[Algebraic identity] ";
            I->print(errs());
            outs() << "\n";
        }
    }

    for (auto I : WorkList) {
        I->eraseFromParent();
    }
    return WorkList.size();
}

int constantFolding(Function &F) {
    SmallVector<Instruction *, 10> WorkList;
    for (auto I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        if (!I->isBinaryOp())
            continue;
        
        Value *Op0 = I->getOperand(0);
        Value *Op1 = I->getOperand(1);

        int64_t ConstVal0, ConstVal1;
        bool HasDeadInst = true;
        if (isa<ConstantInt>(Op0)) {
            ConstVal0 = dyn_cast<ConstantInt>(Op0)->getSExtValue();
        }
        if (isa<ConstantInt>(Op1)) {
            ConstVal1 = dyn_cast<ConstantInt>(Op1)->getSExtValue();
        }

        if (!isa<ConstantInt>(Op0) && !isa<ConstantInt>(Op1))
            continue;
        
        switch (I->getOpcode()) {
        case Instruction::Add:
            I->replaceAllUsesWith(ConstantInt::getSigned(I->getType(), ConstVal0 + ConstVal1));
            break;
        case Instruction::Sub:
            I->replaceAllUsesWith(ConstantInt::getSigned(I->getType(), ConstVal0 - ConstVal1));
            break;
        case Instruction::Mul:
            I->replaceAllUsesWith(ConstantInt::getSigned(I->getType(), ConstVal0 * ConstVal1));
            break;
        case Instruction::SDiv:
            if (ConstVal1 != 0) {
                I->replaceAllUsesWith(ConstantInt::getSigned(I->getType(), ConstVal0 / ConstVal1));
            } else {
                HasDeadInst = false;
            }
            break;
        default:
            HasDeadInst = false;
            break;
        }

        if (HasDeadInst) {
            WorkList.push_back(&*I);
            outs() << "[Constant folding] ";
            I->print(errs());
            outs() << "\n";
        }
    }

    for (auto I : WorkList) {
        I->eraseFromParent();
    }
    return WorkList.size();
}


static int getPower2(int64_t x) {
    if ((x & (x-1)) != 0) 
        return -1;
    return 64 - __builtin_clzll(x) - 1;
} 

int strengthReduction(Function &F) {
    SmallVector<Instruction *, 10> WorkList;
    for (auto I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        if (!I->isBinaryOp())
            continue;
        
        Value *Op0 = I->getOperand(0);
        Value *Op1 = I->getOperand(1);

        int64_t ConstVal0, ConstVal1;
        bool HasDeadInst = true;
        if (isa<ConstantInt>(Op0)) {
            ConstVal0 = dyn_cast<ConstantInt>(Op0)->getSExtValue();
        }
        if (isa<ConstantInt>(Op1)) {
            ConstVal1 = dyn_cast<ConstantInt>(Op1)->getSExtValue();
        }
        
        switch (I->getOpcode()) {
        case Instruction::Mul:
            if (isa<ConstantInt>(Op0) && getPower2(ConstVal1) != -1) {
                Value *V = ConstantInt::getSigned(I->getType(), getPower2(ConstVal1));
                I->replaceAllUsesWith(BinaryOperator::Create(Instruction::Shl, Op0, V, "shl", &*I));
            } else {
                HasDeadInst = false;
            }
            break;
        case Instruction::SDiv:
            if (isa<ConstantInt>(Op0) && getPower2(ConstVal1) != -1) {
                Value *V = ConstantInt::getSigned(I->getType(), getPower2(ConstVal1));
                I->replaceAllUsesWith(BinaryOperator::Create(Instruction::LShr, Op0, V, "lshr", &*I));
            } else {
                HasDeadInst = false;
            }
            break;
        default:
            HasDeadInst = false;
            break;
        }

        if (HasDeadInst) {
            WorkList.push_back(&*I);
            outs() << "[Strength reduction] ";
            I->print(errs());
            outs() << "\n";
        }
    }

    for (auto I : WorkList) {
        I->eraseFromParent();
    }
    return WorkList.size();
}

void functionInfo(Function &F) {
    int InstCount = 0;
    // name
    outs() << F.getName() << ",\t";

    // args
    if (F.isVarArg()) {
        outs() << "*" << ",\t";
    } else {
        outs() << F.arg_size() << ",\t";
    }

    for (auto BB = F.begin(), BE = F.end(); BB != BE; ++BB) {
        InstCount += BB->size();
    }

    outs() << F.getNumUses() << ",\t" << F.size() << ",\t" << InstCount << "\n";
}

bool runLocalOpt(Function &F) {
    int Algebraic = algebraicIdentity(F);
    int ConstantFolding = constantFolding(F);
    int StrengthReduction = strengthReduction(F);

    outs() << "Transformations applied:" << "\n";
    outs() << "  Algebraic identities: " << Algebraic << "\n";
    outs() << "  Constant folding: " << ConstantFolding << "\n";
    outs() << "  Strength reduction: " << StrengthReduction << "\n";

    return false;
}

/// \brief: Local optimziation with algebraic identities, constant folding and strength reduction.
struct LocalOpt : public PassInfoMixin<LocalOpt> {


    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        if (!runLocalOpt(F)) {
            PreservedAnalyses PA;
            PA.preserveSet<CFGAnalyses>();
            return PA;
        }
        return PreservedAnalyses::all();
    }
};

}

/*
 To register and build with llvm: add FUNCTION_PASS("local", LocalOpt()) in llvm/lib/Passes/PassRegistery.def
 */

PassPluginLibraryInfo getLocalOptPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "LocalOpt", LLVM_VERSION_STRING, 
        [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                [](StringRef Name, llvm::FunctionPassManager &PM,
                   ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "local-opt") {
                    PM.addPass(LocalOpt());
                    return true;
                  }
                  return false;
                });
        }
    };
}

#ifndef LLVM_LOCAL_LINK_INTO_TOOLS
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getLocalOptPluginInfo();
}
#endif
