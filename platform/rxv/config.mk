#
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (c) 2019 Western Digital Corporation or its affiliates.
# Copyright (c) 2021 Jamie Iles.

platform-cppflags-y =
platform-cflags-y =
platform-asflags-y =
platform-ldflags-y =

PLATFORM_RISCV_XLEN = 32
PLATFORM_RISCV_ABI = ilp32
PLATFORM_RISCV_ISA = rv32ima

FW_TEXT_START=0x80000000
FW_DYNAMIC=n
FW_PAYLOAD=y
FW_PAYLOAD_OFFSET=0x400000
