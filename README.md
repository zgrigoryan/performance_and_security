# Performance and Security

This repository examines the critical interplay between performance and security in software systems. It highlights how optimizing performance can sometimes introduce vulnerabilities, and explores common security flaws such as buffer overflows, return-oriented programming, and memory execution protections.

All code was written and tested on **macOS (Apple M2)**. Notes on protection mechanisms and observable behavior are provided for each scenario.

---

## 🔧 Repository Structure

```

performance\_and\_security/
├── src/
│   ├── stack\_smashing.cpp
│   ├── security\_defenses.cpp
│   ├── security\_measure\_overhead.cpp
│   └── return\_oriented\_prog.cpp

````

---

## 🚨 1. Stack Smashing

**File**: `stack_smashing.cpp`  
**Goal**: Demonstrate how a buffer overflow can overwrite the return address on the stack and how stack canaries protect against this.

### ✅ Observed Results on macOS

| Protection | Behavior          |
|------------|-------------------|
| Off (`-fno-stack-protector`) | `bus error` (stack corrupted) |
| On (default)                 | `Abort trap: 6` (canary triggered) |

### Summary

- A small buffer is overflowed using `strcpy`.
- When compiled **without protection**, the program crashes with a `bus error`.
- When compiled **with stack protection**, macOS detects the corruption and safely aborts execution.

---

## 🛡️ 2. Security Defenses: DEP & ASLR

**File**: `security_defenses.cpp`  
**Goal**: Show how **Data Execution Prevention (DEP)** blocks execution from heap memory and how **ASLR** randomizes memory layout.

### ✅ Observed Results

- Attempting to execute code from a heap buffer results in a `bus error` due to **DEP/NX**.
- Memory addresses printed for the heap buffer and `main()` **change on every run**, confirming **ASLR** is active.

### Summary

- macOS prevents heap memory from being executed unless explicitly marked (which is heavily restricted).
- Executable memory protection and address layout randomization are enforced by default.

---

## 📉 3. Measuring Security Overhead

**File**: `security_measure_overhead.cpp`  
**Goal**: Measure performance overhead from compiler hardening features.

### How It Was Measured

Compiled two versions:

```bash
g++ -O2 -o overhead_no_prot security_measure_overhead.cpp
g++ -O2 -fstack-protector-all -D_FORTIFY_SOURCE=2 -o overhead_hardened security_measure_overhead.cpp
````

### ✅ Observed Results

| Binary              | loop + ret | malloc/free | Real Time                                        |
| ------------------- | ---------- | ----------- | ------------------------------------------------ |
| `overhead_no_prot`  | \~118 ms   | \~0.88 ms   | \~0.54 s                                         |
| `overhead_hardened` | (missing)  | (missing)   | N/A (compile failed due to Linux-specific flags) |

### Summary

* On macOS, `-z relro`, `-pie`, and other linker flags are unsupported.
* We still observed timing using `/usr/bin/time` and confirmed the loop behavior.

---

## 🧩 4. Return-Oriented Programming (ROP)

**File**: `return_oriented_prog.cpp`
**Goal**: Demonstrate conceptually how an attacker could hijack control flow using instruction "gadgets" without injecting code.

### ✅ Observed Results

```
=== friendly program ===
foo
bar
baz
========================
```

### Summary

* This program calls simple functions that could be used as "gadgets" in a real ROP attack.
* On **macOS with ARM64**, real ROP is extremely difficult due to **Pointer Authentication Codes (PAC)** and **DEP**.
* This file serves as a conceptual illustration, not an actual exploit.

---

## 🗂️ Notes for macOS Users

* Many Linux-specific exploit flags (`-m32`, `-z`, `-no-pie`, `execstack`) are **unsupported** on macOS ARM.
* Stack protection and DEP are enabled by default and cannot be fully disabled.
* This repository focuses on **observing crash behavior and defense mechanisms**, rather than successful code hijacking.

---

## ✅ Summary

| Demo                            | Protection Demonstrated | Behavior on macOS (M2)                 |
| ------------------------------- | ----------------------- | -------------------------------------- |
| `stack_smashing.cpp`            | Stack canaries          | `bus error` (off), `abort trap` (on)   |
| `security_defenses.cpp`         | DEP + ASLR              | Crashes on execution from heap         |
| `security_measure_overhead.cpp` | Hardened vs baseline    | Timing results collected               |
| `return_oriented_prog.cpp`      | ROP concept             | No real attack, but shows gadget setup |

---

## 📎 Future Work

To explore full exploits:

* Run on **Linux x86-64** or use a **Docker sandbox**.
* Use `gdb`, `objdump`, or `ropper` to analyze memory and gadgets.
* Explore **real-world exploits** using ROP to bypass DEP and ASLR.

---

## 📜 License

MIT License.