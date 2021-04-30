#include <nautilus/aspace.h>
#include <nautilus/nautilus.h>
#include <nautilus/process.h>

#define SYSCALL_NAME "sys_brk"
#include "impl_preamble.h"

#define HEAP_BOT \
  (void*)0x10000000000UL /* Lowest virtual address for the process heap */
#define HEAP_SIZE_INCREMENT \
  0x2000000UL /* Heap is increased by a multiple of this amount */

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

/// @param brk The process's requested new max data segment address, or zero.
/// @return If param brk is 0, returns the beginning of the data segment,
/// creating one if it doesn't exist. If param brk is non-zero, return the
/// requested segment end if possible, or otherwise return the current program
/// segment end.
uint64_t sys_brk(const uint64_t brk) {

  nk_process_t* current_process = syscall_get_proc();

  DEBUG("Called with brk=%p\n", brk);

  spin_lock(&current_process->lock);

  if (current_process->heap_end == 0) {
    // This is the first call to brk.
    void* new_heap = malloc(HEAP_SIZE_INCREMENT);
    if (!new_heap) {
      // Something terrible has happened. This may not be the correct response,
      // but the program will fail anyway.
      goto out;
    }
    nk_aspace_region_t heap_expand;
#ifdef NAUT_CONFIG_CARAT_PROCESS
    heap_expand.va_start = new_heap;
#else
    heap_expand.va_start = (void*)HEAP_BOT;
#endif
    heap_expand.pa_start = new_heap;
    heap_expand.len_bytes = HEAP_SIZE_INCREMENT;
    heap_expand.protect.flags = NK_ASPACE_READ | NK_ASPACE_WRITE |
                                NK_ASPACE_EXEC | NK_ASPACE_PIN |
                                NK_ASPACE_EAGER;
    if (nk_aspace_add_region(syscall_get_proc()->aspace, &heap_expand)) {
      nk_vc_printf("Fail to allocate initial heap to aspace\n");
      free(new_heap);
      goto out;
    }
    current_process->heap_begin = heap_expand.va_start;
    current_process->heap_end =
        current_process->heap_begin + HEAP_SIZE_INCREMENT;
    current_process->heap_region = heap_expand;
  } else {
    // Some memory has already been allocated
#ifdef NAUT_CONFIG_CARAT_PROCESS
    if ((void*)brk > current_process->heap_region.va_start +
                         current_process->heap_region.len_bytes) {
      nk_aspace_region_t heap_expand = current_process->heap_region;
      uint64_t actual_size = 0;
      uint64_t new_size =
          current_process->heap_region.len_bytes + HEAP_SIZE_INCREMENT;
      if (nk_aspace_resize_region(current_process->aspace, &heap_expand,
                                  new_size, 1, &actual_size)) {
        ERROR("Could not expand region\n");
        goto out;
      }
      heap_expand.len_bytes = actual_size;
      current_process->heap_region = heap_expand;
#else
    if ((void*)brk > current_process->heap_end) {
      void* new_heap = malloc(HEAP_SIZE_INCREMENT);
      if (!new_heap) {
        goto out;
      }
      nk_aspace_region_t heap_expand;
      heap_expand.va_start = current_process->heap_end;
      heap_expand.pa_start = new_heap;
      heap_expand.len_bytes = HEAP_SIZE_INCREMENT;
      heap_expand.protect.flags = NK_ASPACE_READ | NK_ASPACE_WRITE |
                                  NK_ASPACE_EXEC | NK_ASPACE_PIN |
                                  NK_ASPACE_EAGER;
      if (nk_aspace_add_region(syscall_get_proc()->aspace, &heap_expand)) {
        ERROR("Fail to allocate more heap to aspace\n");
        free(new_heap);
        goto out;
      }
      current_process->heap_end =
          current_process->heap_end + HEAP_SIZE_INCREMENT;
#endif
    }
  }

out:; // empty statement required to allow following declaration
  uint64_t retval;
  if (brk) {
    retval = MIN(brk, (uint64_t)current_process->heap_end);
  } else {
    retval = (uint64_t)current_process->heap_begin;
  }
  spin_unlock(&current_process->lock);
  DEBUG("Going to return %p\n", retval);
  return retval;
}
