//===-- PPC64LE_ehframe_Registers.h -----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef utility_PPC64LE_ehframe_Registers_h_
#define utility_PPC64LE_ehframe_Registers_h_

// The register numbers used in the eh_frame unwind information.
// Should be the same as DWARF register numbers.

namespace ppc64le_ehframe {

enum {
  r0 = 0,
  r1,
  r2,
  r3,
  r4,
  r5,
  r6,
  r7,
  r8,
  r9,
  r10,
  r11,
  r12,
  r13,
  r14,
  r15,
  r16,
  r17,
  r18,
  r19,
  r20,
  r21,
  r22,
  r23,
  r24,
  r25,
  r26,
  r27,
  r28,
  r29,
  r30,
  r31,
  f0,
  f1,
  f2,
  f3,
  f4,
  f5,
  f6,
  f7,
  f8,
  f9,
  f10,
  f11,
  f12,
  f13,
  f14,
  f15,
  f16,
  f17,
  f18,
  f19,
  f20,
  f21,
  f22,
  f23,
  f24,
  f25,
  f26,
  f27,
  f28,
  f29,
  f30,
  f31,
  lr = 65,
  ctr,
  cr = 68,
  xer = 76,
  vr0,
  vr1,
  vr2,
  vr3,
  vr4,
  vr5,
  vr6,
  vr7,
  vr8,
  vr9,
  vr10,
  vr11,
  vr12,
  vr13,
  vr14,
  vr15,
  vr16,
  vr17,
  vr18,
  vr19,
  vr20,
  vr21,
  vr22,
  vr23,
  vr24,
  vr25,
  vr26,
  vr27,
  vr28,
  vr29,
  vr30,
  vr31,
  vscr = 110,
  vrsave = 117,
  pc,
  softe,
  trap,
  origr3,
  fpscr,
  msr,
  vs0,
  vs1,
  vs2,
  vs3,
  vs4,
  vs5,
  vs6,
  vs7,
  vs8,
  vs9,
  vs10,
  vs11,
  vs12,
  vs13,
  vs14,
  vs15,
  vs16,
  vs17,
  vs18,
  vs19,
  vs20,
  vs21,
  vs22,
  vs23,
  vs24,
  vs25,
  vs26,
  vs27,
  vs28,
  vs29,
  vs30,
  vs31,
  vs32,
  vs33,
  vs34,
  vs35,
  vs36,
  vs37,
  vs38,
  vs39,
  vs40,
  vs41,
  vs42,
  vs43,
  vs44,
  vs45,
  vs46,
  vs47,
  vs48,
  vs49,
  vs50,
  vs51,
  vs52,
  vs53,
  vs54,
  vs55,
  vs56,
  vs57,
  vs58,
  vs59,
  vs60,
  vs61,
  vs62,
  vs63,
};
}

#endif // utility_PPC64LE_ehframe_Registers_h_
