/* On RISC-V, word = 32-bit (x86 Long); double = 64-bit (x86 Quad-word).
 * We make use of pseudo-instructions to make this clearer. */

#define WRITEL(p,v) __asm__ __volatile__ ("sw %0, (%1)" : : "r"(v), "r"(p) : "memory")
#define READL(p,v) __asm__ __volatile__ ("lw %0, %1" : "=r"(v) :"r"(p) : "memory")
#define WRITEQ(p,v) __asm__ __volatile__ ("sd %0, (%1)" : : "r"(v), "r"(p) : "memory")
#define READQ(p,v) __asm__ __volatile__ ("ld %0, %1" : "=r"(v) :"r"(p) : "memory")
