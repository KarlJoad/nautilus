#include <nautilus/aspace.h>
#include <nautilus/nautilus.h>
#include <nautilus/process.h>

#define HEAP_BOT 0x100000000 /* Lowest virtual address for the process heap */
#define HEAP_SIZE_INCREMENT \
  0x1400000 /* Heap is increased by a multiple of this amount */

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

#define DEBUG(fmt, args...) DEBUG_PRINT("syscall_brk: " fmt, ##args)

#define ret                                                 \
  uint64_t retval;                                          \
  if (brk) {                                                \
    retval = MIN(brk, (uint64_t)current_process->heap_end); \
  } else {                                                  \
    retval = current_process->heap_begin;                   \
  }                                                         \
  spin_unlock(&current_process->lock);                      \
  DEBUG("Going to return %p\n", retval);                    \
  return (uint64_t)retval;

/// @param brk The process's requested new max data segment address, or zero.
/// @return If param brk is 0, returns the beginning of the data segment,
/// creating one if it doesn't exist. If param brk is non-zero, return the
/// requested segment end if possible, or otherwise return the current program
/// segment end.
///
/// @todo this currently causes a memory leak and doesn't use the allocator
/// system, which is wrong
uint64_t sys_brk(const uint64_t brk) {

  nk_process_t* current_process = nk_process_current();
  if (!current_process) {
    panic("Call to sys_brk out of the context of a process.\n");
  }

  DEBUG("Called with brk=%p\n", brk);

  spin_lock(&current_process->lock);

  if (current_process->heap_end == 0) {
    // This is the first call to brk.
    void* new_heap = malloc(HEAP_SIZE_INCREMENT);
    if (!new_heap) {
      // Something terrible has happened. This may not be the correct response,
      // but the program will fail anyway.
      ret
    }
    nk_aspace_region_t heap_expand;
    heap_expand.va_start = (void*)HEAP_BOT;
    heap_expand.pa_start = new_heap;
    heap_expand.len_bytes = HEAP_SIZE_INCREMENT;
    heap_expand.protect.flags = NK_ASPACE_READ | NK_ASPACE_WRITE |
                                NK_ASPACE_EXEC | NK_ASPACE_PIN |
                                NK_ASPACE_EAGER;
    if (nk_aspace_add_region(nk_process_current()->aspace, &heap_expand)) {
      nk_vc_printf("Fail to allocate initial heap to aspace\n");
      free(new_heap);
      ret
    }
    current_process->heap_begin = HEAP_BOT;
    current_process->heap_end =
        current_process->heap_begin + HEAP_SIZE_INCREMENT;
    // goto ret;
  } else {
    // Some memory has already been allocated
    if ((void*)brk > current_process->heap_end) {
      void* new_heap = malloc(HEAP_SIZE_INCREMENT);
      if (!new_heap) {
        ret
      }
      nk_aspace_region_t heap_expand;
      heap_expand.va_start = current_process->heap_end;
      heap_expand.pa_start = new_heap;
      heap_expand.len_bytes = HEAP_SIZE_INCREMENT;
      heap_expand.protect.flags = NK_ASPACE_READ | NK_ASPACE_WRITE |
                                  NK_ASPACE_EXEC | NK_ASPACE_PIN |
                                  NK_ASPACE_EAGER;
      if (nk_aspace_add_region(nk_process_current()->aspace, &heap_expand)) {
        nk_vc_printf("Fail to allocate more heap to aspace\n");
        free(new_heap);
        ret
      }
      current_process->heap_end += HEAP_SIZE_INCREMENT;
    }
    // goto ret;
  }

  ret
}