#pragma once

#if defined(_MSC_VER)
  #define NOINLINE  __declspec(noinline)
  #include <intrin.h>
  inline void cpu_relax() { _ReadWriteBarrier(); }
#else
  #define NOINLINE  __attribute__((noinline))
  inline void cpu_relax() { asm volatile("" ::: "memory"); }
#endif
