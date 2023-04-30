; NOTE: Assertions have been autogenerated by utils/update_mir_test_checks.py
; RUN: llc -global-isel -march=amdgcn -mcpu=gfx900 -stop-after=irtranslator -o - %s | FileCheck %s
; Make sure intrinsics with metadata arguments are translated

define i32 @reloc_constant() {
  ; CHECK-LABEL: name: reloc_constant
  ; CHECK: bb.1 (%ir-block.0):
  ; CHECK:   liveins: $sgpr30_sgpr31
  ; CHECK:   [[COPY:%[0-9]+]]:sgpr_64 = COPY $sgpr30_sgpr31
  ; CHECK:   [[INT0:%[0-9]+]]:_(s32) = G_INTRINSIC intrinsic(@llvm.amdgcn.reloc.constant), !0
  ; We cannot have any specific metadata check here as ConstantAsMetadata is printed as <raw_ptr_val>
  ; CHECK:   [[INT1:%[0-9]+]]:_(s32) = G_INTRINSIC intrinsic(@llvm.amdgcn.reloc.constant), <0x{{[0-9a-f]+}}>
  ; CHECK:   [[SUM:%[0-9]+]]:_(s32) = G_ADD [[INT0]], [[INT1]]
  ; CHECK:   $vgpr0 = COPY [[SUM]](s32)
  ; CHECK:   [[COPY1:%[0-9]+]]:ccr_sgpr_64 = COPY [[COPY]]
  ; CHECK:   S_SETPC_B64_return [[COPY1]], implicit $vgpr0
  %val0 = call i32 @llvm.amdgcn.reloc.constant(metadata !0)
  %val1 = call i32 @llvm.amdgcn.reloc.constant(metadata i32 4)
  %res = add i32 %val0, %val1
  ret i32 %res
}

declare i32 @llvm.amdgcn.reloc.constant(metadata) #0

attributes #0 = { nounwind readnone speculatable willreturn }

!0 = !{!"arst"}