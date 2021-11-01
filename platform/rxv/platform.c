/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 * Copyright (c) 2021 Jamie Iles.
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_console.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_platform.h>

#include <sbi_utils/timer/aclint_mtimer.h>

#define UART_ADDRESS 0xffff1000

static struct aclint_mtimer_data mtimer = {
	.mtime_freq = 8000000,
	.mtime_addr = 0xf0000008,
	.mtime_size = 8,
	.mtimecmp_addr = 0xf0000000,
	.mtimecmp_size = 8,
	.first_hartid = 0,
	.hart_count = 1,
	.has_64bit_mmio = FALSE,
};

static void rxv_putc(char ch)
{
	writeb(ch, (volatile void *)UART_ADDRESS);
}

static int rxv_getc(void)
{
	uint32_t data = readl((volatile void *)UART_ADDRESS);

	return data & 0x100 ? data & 0xff : -1;
}

static struct sbi_console_device rxv_console = {
	.name = "rxv",
	.console_putc = rxv_putc,
	.console_getc = rxv_getc
};

static int platform_console_init(void)
{
	sbi_console_set_device(&rxv_console);

	return 0;
}

static int platform_timer_init(bool cold_boot)
{
	return cold_boot ? aclint_mtimer_cold_init(&mtimer, NULL) :
		aclint_mtimer_warm_init();
}

const struct sbi_platform_operations platform_ops = {
	.console_init		= platform_console_init,
	.timer_init		= platform_timer_init
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x00),
	.name			= "rxv",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.hart_count		= 1,
	.hart_stack_size	= SBI_PLATFORM_DEFAULT_HART_STACK_SIZE,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
