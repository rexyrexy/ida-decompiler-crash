/*
 * bextr_poc.c — one-instruction PoC that bricks IDA’s decompiler
 *
 * Requires BMI1 for the _bextr_u64 intrinsic.
 * A single BEXTR with length ≥ 64 triggers IDA Internal Error 51666.
 */

#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER)
#  define NOINLINE __declspec(noinline)
#else
#  define NOINLINE __attribute__((noinline))
#endif

 /* Volatile default keeps the compiler from treating the value as a build-time constant. */
    static volatile uint64_t default_val = 0x1122334455667788ULL;

/* Opaque predicate:
   · Always true for normal inputs, so _bextr_u64 is not executed.
   · Compiler cannot prove the result at compile time, so the branch remains. */
static NOINLINE int opaque_predicate(uint64_t x)
{
#if defined(__clang__) || defined(__GNUC__)
    /* Prevent constant-propagation of ‘x’. */
    asm volatile("" : "+r"(x));
#endif
    return x != 0xBADF00DCAFEBABEULL;      /* False only for a rare, specific value */
}

#define BEXTR_LEN_TOO_BIG  80U             /* Any value ≥ 64 works */

int main(int argc, char** argv)
{
    uint64_t v = (argc > 1) ? strtoull(argv[1], NULL, 0)
        : default_val;

    /* Critical instruction that breaks IDA’s decompiler.
       Executed only if opaque_predicate returns 0 (practically never). */
    if (!opaque_predicate(v))
        v = _bextr_u64(v, 0x00, BEXTR_LEN_TOO_BIG);

    printf("dummy result: 0x%016llx\n", (unsigned long long)v);
    return 0;
}