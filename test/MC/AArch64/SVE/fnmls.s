// RUN: llvm-mc -triple=aarch64 -show-encoding -mattr=+sve < %s \
// RUN:        | FileCheck %s --check-prefixes=CHECK-ENCODING,CHECK-INST
// RUN: not llvm-mc -triple=aarch64 -show-encoding < %s 2>&1 \
// RUN:        | FileCheck %s --check-prefix=CHECK-ERROR
// RUN: llvm-mc -triple=aarch64 -filetype=obj -mattr=+sve < %s \
// RUN:        | llvm-objdump -d -mattr=+sve - | FileCheck %s --check-prefix=CHECK-INST
// RUN: llvm-mc -triple=aarch64 -filetype=obj -mattr=+sve < %s \
// RUN:        | llvm-objdump -d - | FileCheck %s --check-prefix=CHECK-UNKNOWN

fnmls z0.h, p7/m, z1.h, z31.h
// CHECK-INST: fnmls	z0.h, p7/m, z1.h, z31.h
// CHECK-ENCODING: [0x20,0x7c,0x7f,0x65]
// CHECK-ERROR: instruction requires: sve
// CHECK-UNKNOWN: 20 7c 7f 65 <unknown>

fnmls z0.s, p7/m, z1.s, z31.s
// CHECK-INST: fnmls	z0.s, p7/m, z1.s, z31.s
// CHECK-ENCODING: [0x20,0x7c,0xbf,0x65]
// CHECK-ERROR: instruction requires: sve
// CHECK-UNKNOWN: 20 7c bf 65 <unknown>

fnmls z0.d, p7/m, z1.d, z31.d
// CHECK-INST: fnmls	z0.d, p7/m, z1.d, z31.d
// CHECK-ENCODING: [0x20,0x7c,0xff,0x65]
// CHECK-ERROR: instruction requires: sve
// CHECK-UNKNOWN: 20 7c ff 65 <unknown>


// --------------------------------------------------------------------------//
// Test compatibility with MOVPRFX instruction.

movprfx z0.d, p7/z, z7.d
// CHECK-INST: movprfx	z0.d, p7/z, z7.d
// CHECK-ENCODING: [0xe0,0x3c,0xd0,0x04]
// CHECK-ERROR: instruction requires: sve
// CHECK-UNKNOWN: e0 3c d0 04 <unknown>

fnmls z0.d, p7/m, z1.d, z31.d
// CHECK-INST: fnmls	z0.d, p7/m, z1.d, z31.d
// CHECK-ENCODING: [0x20,0x7c,0xff,0x65]
// CHECK-ERROR: instruction requires: sve
// CHECK-UNKNOWN: 20 7c ff 65 <unknown>

movprfx z0, z7
// CHECK-INST: movprfx	z0, z7
// CHECK-ENCODING: [0xe0,0xbc,0x20,0x04]
// CHECK-ERROR: instruction requires: sve
// CHECK-UNKNOWN: e0 bc 20 04 <unknown>

fnmls z0.d, p7/m, z1.d, z31.d
// CHECK-INST: fnmls	z0.d, p7/m, z1.d, z31.d
// CHECK-ENCODING: [0x20,0x7c,0xff,0x65]
// CHECK-ERROR: instruction requires: sve
// CHECK-UNKNOWN: 20 7c ff 65 <unknown>
