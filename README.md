# ida-decompiler-crash

## What it does
A single `_bextr_u64` with **length ≥ 64** disables decompilation in **Hex-Rays IDA**:

```

Internal error 51666 has occurred and further decompilation is impossible

```

After that every **F5** shows:

```

Cannot continue after an internal error, sorry

```

*Tested on IDA 9.1 (current public build when this PoC was written).*

---

## Why it works

| Layer | Behaviour |
|-------|-----------|
| **CPU** | If the *length* exceeds the operand width, the instruction returns `0` and sets **ZF = 1**. See the [Intel® SDM — `BEXTR`](https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/current/bextr-u32-64.html). |
| **IDA / Hex-Rays** | The decompiler assumes `0 ≤ length < 64`. A larger value trips an internal assertion and raises **Internal Error 51666**, blocking any further decompilation until the database is reloaded. |

---

## Reproducing the issue

1. Build the program in `bextr_poc.c` with any compiler that supports BMI1.  
2. Load the resulting binary into IDA 9.1 and let auto-analysis finish.  
3. Press **F5** in the function that contains the oversized-length `BEXTR`.  
   The first attempt pops up *Internal Error 51666*; every subsequent attempt shows *Cannot continue after an internal error*.

> **Tip:** Keep the oversized-length `BEXTR` **inside the very function you want to sabotage**. Place the intrinsic directly in that function’s body so decompiling that function reliably triggers Internal Error 51666.

---

## Technical notes

* Intel defines the *length* field as the “number of bits to extract,” with a valid range of **1–64** for 64-bit operands. Values ≥ 64 are architecturally valid but produce an all-zero result.  
* IDA’s micro-code verifier was not written to cope with this edge case and aborts when it encounters it, resulting in **Internal Error 51666**.

---

## AI-generated content

This README and the accompanying PoC source code were authored with the assistance of an AI language model.  
Generation date: **13 July 2025**.

---

## License

Released under the **MIT License** – see `LICENSE` for details.  
For research and teaching use only.
