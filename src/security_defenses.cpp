#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>

int main() {
    /* 5‑byte “return 0” shellcode (x86‑64, Linux):
         xor edi,edi ; xor eax,eax ; mov al,60 ; syscall
       change if you’re on a different ISA.
    */
    const unsigned char code[] = {
        0x48,0x31,0xFF, 0x48,0x31,0xC0, 0xB0,0x3C, 0x0F,0x05
    };

    size_t pagesz = sysconf(_SC_PAGESIZE);
    void *buf = mmap(nullptr, pagesz, PROT_READ|PROT_WRITE,
                     MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (buf == MAP_FAILED) { perror("mmap"); return 1; }

    memcpy(buf, code, sizeof(code));
    printf("Heap buffer  @ %p\nmain()        @ %p\n", buf, (void*)&main);

    using fun = void(*)();
    fun f = (fun)buf;

    puts("Calling shellcode with heap **non‑executable** …");
    f();                             // likely SIGSEGV when NX/DEP active

    puts("\nRe‑mprotect → +PROT_EXEC, calling again …");
    if (mprotect(buf, pagesz, PROT_READ|PROT_EXEC) != 0) {
        perror("mprotect"); return 1;
    }
    f();                             // succeeds

    return 0;
}

/*
Build:
$ g++ -std=c++11 -Wall -O2 -o security_defenses security_defenses.cpp

Play:
  – Default run: first call aborts (NX), second succeeds.
  – Disable DEP/NX (if you can) e.g. `execstack -s security_defenses`
    or booting with “noexec=off”, then both calls succeed.

ASLR check: run multiple times; addresses printed change each run unless you
`echo 0 | sudo tee /proc/sys/kernel/randomize_va_space`.
*/
