/*
 * MicroBlaze gdb server stub
 *
 * Copyright (c) 2003-2005 Fabrice Bellard
 * Copyright (c) 2013 SUSE LINUX Products GmbH
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#include "qemu/osdep.h"
#include "cpu.h"
#include "exec/gdbstub.h"

/*
 * GDB expects SREGs in the following order:
 * PC, MSR, EAR, ESR, FSR, BTR, EDR, PID, ZPR, TLBX, TLBSX, TLBLO, TLBHI.
 *
 * PID, ZPR, TLBx, TLBsx, TLBLO, and TLBHI aren't modeled, so we don't
 * map them to anything and return a value of 0 instead.
 */

enum {
    GDB_PC    = 32 + 0,
    GDB_MSR   = 32 + 1,
    GDB_EAR   = 32 + 2,
    GDB_ESR   = 32 + 3,
    GDB_FSR   = 32 + 4,
    GDB_BTR   = 32 + 5,
    GDB_PVR0  = 32 + 6,
    GDB_PVR11 = 32 + 17,
    GDB_EDR   = 32 + 18,
    GDB_SLR   = 32 + 25,
    GDB_SHR   = 32 + 26,
};

int mb_cpu_gdb_read_register(CPUState *cs, GByteArray *mem_buf, int n)
{
    MicroBlazeCPU *cpu = MICROBLAZE_CPU(cs);
    CPUClass *cc = CPU_GET_CLASS(cs);
    CPUMBState *env = &cpu->env;
    uint32_t val;

    if (n > cc->gdb_num_core_regs) {
        return 0;
    }

    switch (n) {
    case 1 ... 31:
        val = env->regs[n];
        break;
    case GDB_PC:
        val = env->pc;
        break;
    case GDB_MSR:
        val = env->sregs[SR_MSR];
        break;
    case GDB_EAR:
        val = env->sregs[SR_EAR];
        break;
    case GDB_ESR:
        val = env->sregs[SR_ESR];
        break;
    case GDB_FSR:
        val = env->sregs[SR_FSR];
        break;
    case GDB_BTR:
        val = env->sregs[SR_BTR];
        break;
    case GDB_PVR0 ... GDB_PVR11:
        /* PVR12 is intentionally skipped */
        val = env->pvr.regs[n - GDB_PVR0];
        break;
    case GDB_EDR:
        val = env->sregs[SR_EDR];
        break;
    case GDB_SLR:
        val = env->slr;
        break;
    case GDB_SHR:
        val = env->shr;
        break;
    default:
        /* Other SRegs aren't modeled, so report a value of 0 */
        val = 0;
        break;
    }
    return gdb_get_reg32(mem_buf, val);
}

int mb_cpu_gdb_write_register(CPUState *cs, uint8_t *mem_buf, int n)
{
    MicroBlazeCPU *cpu = MICROBLAZE_CPU(cs);
    CPUClass *cc = CPU_GET_CLASS(cs);
    CPUMBState *env = &cpu->env;
    uint32_t tmp;

    if (n > cc->gdb_num_core_regs) {
        return 0;
    }

    tmp = ldl_p(mem_buf);

    switch (n) {
    case 1 ... 31:
        env->regs[n] = tmp;
        break;
    case GDB_PC:
        env->pc = tmp;
        break;
    case GDB_MSR:
        env->sregs[SR_MSR] = tmp;
        break;
    case GDB_EAR:
        env->sregs[SR_EAR] = tmp;
        break;
    case GDB_ESR:
        env->sregs[SR_ESR] = tmp;
        break;
    case GDB_FSR:
        env->sregs[SR_FSR] = tmp;
        break;
    case GDB_BTR:
        env->sregs[SR_BTR] = tmp;
        break;
    case GDB_PVR0 ... GDB_PVR11:
        /* PVR12 is intentionally skipped */
        env->pvr.regs[n - GDB_PVR0] = tmp;
        break;
    case GDB_EDR:
        env->sregs[SR_EDR] = tmp;
        break;
    case GDB_SLR:
        env->slr = tmp;
        break;
    case GDB_SHR:
        env->shr = tmp;
        break;
    }
    return 4;
}
