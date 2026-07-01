#include "dram_model.h"
#include "logger.h"
#include "memory_test.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_DRAM_MB 64U
#define BYTES_PER_MB (1024U * 1024U)
#define TEST_LOG_PATH "dram_test_results.csv"

static int parse_dram_size_mb(int argc, char **argv, size_t *out_mb)
{
    char *endptr = NULL;
    unsigned long value = 0;

    if (out_mb == NULL)
    {
        return -1;
    }

    if (argc < 2)
    {
        *out_mb = DEFAULT_DRAM_MB;
        return 0;
    }

    errno = 0;
    value = strtoul(argv[1], &endptr, 10);
    if (errno != 0 || endptr == argv[1] || *endptr != '\0' || value == 0)
    {
        return -1;
    }

    *out_mb = (size_t)value;
    return 0;
}

static int run_basic_rw_smoke_test(Dram *dram,
                                   uint32_t address,
                                   uint32_t expected,
                                   uint32_t *out_actual)
{
    uint32_t actual = 0;

    printf("[TEST] Basic 32-bit read/write smoke test\n");

    if (out_actual != NULL)
    {
        *out_actual = 0;
    }

    if (dram_write32(dram, address, expected) != 0)
    {
        printf("[FAIL] write failed at addr=0x%08X\n", address);
        return -1;
    }

    if (dram_read32(dram, address, &actual) != 0)
    {
        printf("[FAIL] read failed at addr=0x%08X\n", address);
        return -1;
    }

    if (out_actual != NULL)
    {
        *out_actual = actual;
    }

    if (actual != expected)
    {
        printf("[FAIL] addr=0x%08X expected=0x%08X actual=0x%08X\n",
               address, expected, actual);
        return -1;
    }

    printf("[PASS] addr=0x%08X expected=0x%08X actual=0x%08X\n",
           address, expected, actual);
    return 0;
}

int main(int argc, char **argv)
{
    Dram dram;
    Logger logger;
    MemoryTestResult pattern_result;
    size_t dram_mb = 0;
    size_t dram_bytes = 0;
    uint32_t smoke_address = 0x1000U;
    uint32_t smoke_expected = 0xA5A5A5A5U;
    uint32_t smoke_actual = 0;
    int smoke_pass = 0;
    int pattern_pass = 0;

    if (parse_dram_size_mb(argc, argv, &dram_mb) != 0)
    {
        printf("[ERROR] Invalid DRAM size argument\n");
        return 1;
    }

    dram_bytes = dram_mb * BYTES_PER_MB;

    printf("[BOOT] C-Based DRAM Validation Simulator\n");
    printf("[DRAM] Initializing virtual DRAM: %zu MB\n", dram_mb);

    if (logger_open(&logger, TEST_LOG_PATH) != 0)
    {
        printf("[ERROR] Failed to open test log: %s\n", TEST_LOG_PATH);
        return 1;
    }

    printf("[LOG] Writing test log to %s\n", TEST_LOG_PATH);

    if (dram_init(&dram, dram_bytes) != 0)
    {
        printf("[ERROR] Failed to initialize virtual DRAM\n");
        logger_close(&logger);
        return 1;
    }

    printf("[DRAM] Virtual DRAM initialized: %zu bytes\n", dram_size_bytes(&dram));

    smoke_pass = run_basic_rw_smoke_test(&dram,
                                         smoke_address,
                                         smoke_expected,
                                         &smoke_actual) == 0;
    logger_log_smoke_test(&logger,
                          "basic_rw_smoke",
                          smoke_pass,
                          smoke_address,
                          smoke_expected,
                          smoke_actual);

    if (!smoke_pass)
    {
        dram_free(&dram);
        logger_close(&logger);
        return 1;
    }

    pattern_pass = memory_test_constant_pattern(&dram,
                                                0x2000U,
                                                64U * 1024U,
                                                0xAAAAAAAAU,
                                                &pattern_result) == 0;
    logger_log_memory_test(&logger,
                           "constant_pattern",
                           pattern_pass,
                           0x2000U,
                           64U * 1024U,
                           0xAAAAAAAAU,
                           &pattern_result);

    if (!pattern_pass)
    {
        dram_free(&dram);
        logger_close(&logger);
        return 1;
    }

    printf("[RESULT] PASS\n");
    dram_free(&dram);
    logger_close(&logger);
    printf("[DRAM] Virtual DRAM released\n");

    return 0;
}
