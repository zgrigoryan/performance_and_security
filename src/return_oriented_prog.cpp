#include <cstdio>
#include <cstring>
#include <unistd.h>

void foo() { write(STDOUT_FILENO, "foo\n", 4); }
void bar() { write(STDOUT_FILENO, "bar\n", 4); }
void baz() { write(STDOUT_FILENO, "baz\n", 4); }

int main() {
    puts("=== friendly program ===");
    foo(); bar(); baz();
    puts("========================");
    return 0;
}

/*
The binary isn’t vulnerable by itself; it merely gives us three functions that
compile into nice little “gadgets”.  A real exploit would:

 1. Find a separate **overflow** (e.g., in another function) that lets the
    attacker overwrite the saved RIP.

 2. Point RIP at a gadget chain:
        pop rdi ; ret        → arg1 = "/bin/sh"
        pop rsi ; ret        → arg2 = 0
        pop rdx ; ret        → arg3 = 0
        system@libc          → spawns shell

    Each `pop … ; ret` often lives inside existing code (e.g., within `foo`,
    `bar`, or glibc).  No new bytes are injected → DEP/NX bypassed.

Real‑world sightings
--------------------
* **Stuxnet (2010)** used ROP to bypass DEP on Windows XP/Vista.
* **CVE‑2016‑4657** WebKit 0‑day (iOS 9.3) chained >60 gadgets to get
  arbitrary kernel r/w.  Modern mitigations include CFI, CET shadow stacks,
  and ARMv8.3 PAC.

To explore hands‑on, compile with:
$ g++ -O0 -no-pie -fno-stack-protector -o rop return_oriented_prog.cpp
Then inspect gadgets with `ropper --file rop | less`.
*/
