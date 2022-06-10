#define WRITEL(p,v) __asm__ __volatile__ ("movl %0, (%1)" : : "r"(v), "r"(p) : "memory")
#define READL(p,v) __asm__ __volatile__ ("movl (%1), %0" : "=r"(v) :"r"(p) : "memory")
#define WRITEQ(p,v) __asm__ __volatile__ ("movq %0, (%1)" : : "r"(v), "r"(p) : "memory")
#define READQ(p,v) __asm__ __volatile__ ("movq (%1), %0" : "=r"(v) :"r"(p) : "memory")
