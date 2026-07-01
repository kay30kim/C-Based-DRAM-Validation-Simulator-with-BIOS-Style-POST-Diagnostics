#include "error_injection.h"

#include <stdio.h>

int inject_bit_flip32(Dram *dram,
                      uint32_t address,
                      uint32_t bit_mask,
                      FaultInjectionResult *result)
{
    uint32_t before_value = 0;
    uint32_t after_value = 0;

    if (dram == NULL || bit_mask == 0)
    {
        return -1;
    }

    if (dram_read32(dram, address, &before_value) != 0)
    {
        printf("[INJECT][FAIL] read failed at addr=0x%08X\n", address);
        return -1;
    }

    after_value = before_value ^ bit_mask;

    if (dram_write32(dram, address, after_value) != 0)
    {
        printf("[INJECT][FAIL] write failed at addr=0x%08X\n", address);
        return -1;
    }

    if (result != NULL)
    {
        result->address = address;
        result->bit_mask = bit_mask;
        result->before_value = before_value;
        result->after_value = after_value;
    }

    printf("[INJECT] bit flip addr=0x%08X mask=0x%08X before=0x%08X after=0x%08X\n",
           address,
           bit_mask,
           before_value,
           after_value);

    return 0;
}
