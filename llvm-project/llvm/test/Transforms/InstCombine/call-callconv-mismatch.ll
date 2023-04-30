; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -instcombine -S | FileCheck %s
; Verify that a cdecl-compatible calling convention does not trigger emitting
; unreachable idom `store i1 true, i1* undef`.

define arm_aapcs_vfpcc i8 @bar(i8* %0) {
; CHECK-LABEL: @bar(
; CHECK-NEXT:    [[TMP2:%.*]] = load i8, i8* [[TMP0:%.*]], align 1
; CHECK-NEXT:    ret i8 [[TMP2]]
;
  %2 = load i8, i8* %0, align 1
  ret i8 %2
}

define dso_local arm_aapcs_vfpcc i8 @foo(i8* %0) {
; CHECK-LABEL: @foo(
; CHECK-NEXT:    [[TMP2:%.*]] = call i8 @bar(i8* [[TMP0:%.*]])
; CHECK-NEXT:    ret i8 [[TMP2]]
;
  %2 = call i8 @bar(i8* %0)
  ret i8 %2
}