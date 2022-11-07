/*
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <libfdt.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_system.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/reset/fdt_reset.h>

static void *reset_reg;
static unsigned long preserve_mask;
static unsigned long reset_val;
static uint64_t reg_size;

static int mmio_system_reset_check(u32 type, u32 reason)
{
	return 1;
}

static unsigned long reset_reg_read(void)
{
	switch (reg_size) {
	case 4:
		return readl(reset_reg);
	case 8:
#if __riscv_xlen != 32
		return readq(reset_reg);
#endif
	default:
		__builtin_unreachable();
	}
}

static void reset_reg_write(unsigned long val)
{
	switch (reg_size) {
	case 4:
		writel(val, reset_reg);
		return;
	case 8:
#if __riscv_xlen != 32
		writeq(val, reset_reg);
		return;
#endif
	default:
		__builtin_unreachable();
	}
}

static void mmio_system_reset(u32 type, u32 reason)
{
	unsigned long val;

	val = reset_reg_read() & preserve_mask;
	val |= reset_val;
	reset_reg_write(val);
}

static struct sbi_system_reset_device mmio_reset = {
	.name = "mmio_reset",
	.system_reset_check = mmio_system_reset_check,
	.system_reset = mmio_system_reset
};

static int mmio_reset_init(void *fdt, int nodeoff,
			   const struct fdt_match *match)
{
	int rc;
	uint64_t reg_addr;
	int len;
	const fdt64_t *val;

	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, &reg_size);
	if (rc < 0 || !reg_size)
		return SBI_ENODEV;
	reset_reg = (void *)(unsigned long)reg_addr;

	val = fdt_getprop(fdt, nodeoff, "preserve-mask", &len);
	if (!val)
		return SBI_ENOENT;
	preserve_mask = fdt64_to_cpu(*val);

	val = fdt_getprop(fdt, nodeoff, "reset-val", &len);
	if (!val)
		return SBI_ENOENT;
	if (len != 4)
		reset_val = fdt64_to_cpu(*val);
	else
		reset_val = fdt32_to_cpu(*val);

	if (reg_size != 4
#if __riscv_xlen != 32
	    && reg_size != 8
#endif
	    )
		return SBI_EINVAL;

	sbi_system_reset_add_device(&mmio_reset);

	return 0;
}

static const struct fdt_match mmio_reset_match[] = {
	{ .compatible = "rxv,reset-mmio" },
	{ },
};

struct fdt_reset fdt_reset_mmio = {
	.match_table = mmio_reset_match,
	.init = mmio_reset_init
};
