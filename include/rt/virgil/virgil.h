#ifndef __NK_VIRGIL

// NK's implementation of a VIRGIL-like interface
// is built on the following elements:
//
// NK's task framework is used to implement the worker pool.  The
// kernel is configured with the task framework included and with each
// CPU running a task thread (NAUT_CONFIG_TASK_THREAD). The task
// thread is the worker from the VIRGIL perspective.  The CPU's task
// thread does not migrate.  The VIRGIL interface can create tasks for
// specific CPUs, or for any CPU.  In the later case, tasks are
// currently assigned randomly to CPUs.  Tasks do not currently
// migrate.  A CPU's task thread (worker) consumes tasks in fifo order.
// The task framework assumes tasks do not block, or, if they do, that
// it is acceptable that the task thread running the task will block.
//
// VIRGIL's queues, specifically MARC::ThreadSafeNautilusQueue<T>,
// are used for intertask communication.
//

// C++ interfaces for queues are in
// #include <rt/virgil/ThreadSafeNautilusQueue.hpp>

// C interfaces are here

// a task is opaque from the user's perspective
typedef void *nk_virgil_task_t;

// a task runs a callback function pointer of the following form.
// the input pointer points to an opaque quantity, while the return
// value is a pointer to an opaque quantity
typedef void * (*nk_virgil_func_t)(void *input);

// number of workers / number of cpus in system
int nk_virgil_get_num_cpus(void);

// submit a task to any cpu
nk_virgil_task_t nk_virgil_submit_task_to_any_cpu(nk_virgil_func_t func,
						  void *input);

// submit a task to any cpu and detach
nk_virgil_task_t nk_virgil_submit_task_to_any_cpu_and_detach(nk_virgil_func_t func,
							     void *input);

// submit a task to a specific cpu
nk_virgil_task_t nk_virgil_submit_task_to_specific_cpu(nk_virgil_func_t func,
						       void *input,
						       int cpu);

// submit a task to any one of a set of cpus
nk_virgil_task_t nk_virgil_submit_task_to_cpu_set(nk_virgil_func_t func,
						  void *input,
						  cpu_set_t *cpuset);


// immediately returns
//    <0 => error
//    =0 => task is not yet done
//    >0 => task is done, *output contains its output pointer
//          [in this case, do not check on the task again]
int nk_virgil_check_for_task_completion(nk_virgil_task_t task, void **output);

// waits for task completion or error
//    <0 => error
//     0 => should not happen
//    >0 => task is done, *output contains its output pointer
//          [in this case, do not check on the task again]
int nk_virgil_wait_for_task_completion(nk_virgil_task_t task, void **output);

// get the number of tasks that are queued on a given cpu
// negative return => error
int nk_virgil_waiting_tasks(int cpu);

// get number of idle cpus (ones without tasks)
// note that there is no way to make this scalable...
// negative return => error
int nk_virgil_idle_cpus(void);

// get the number of tasks that are queued throughout the system
// note that there is no way to make this scalable...
// negative return => error
int nk_virgil_waiting_tasks(void);

// for internal use
int nk_virgil_init();
int nk_virgil_init_ap();
void nk_virgil_deinit();


#endif

















#endif














