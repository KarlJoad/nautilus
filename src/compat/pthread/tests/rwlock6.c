/*
 * rwlock6.c
 *
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-embedded (PTE) - POSIX Threads Library for embedded systems
 *      Copyright(C) 2008 Jason Schmidlapp
 *
 *      Contact Email: jschmidlapp@users.sourceforge.net
 *
 *
 *      Based upon Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2005 Pthreads-win32 contributors
 *
 *      Contact Email: rpj@callisto.canberra.edu.au
 *
 *      The original list of contributors to the Pthreads-win32 project
 *      is contained in the file CONTRIBUTORS.ptw32 included with the
 *      source code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 * --------------------------------------------------------------------------
 *
 * Check writer and reader locking
 *
 * Depends on API functions:
 *      pthread_rwlock_rdlock()
 *      pthread_rwlock_wrlock()
 *      pthread_rwlock_unlock()
 */

#include "test.h"

static pthread_rwlock_t rwlock1 = PTHREAD_RWLOCK_INITIALIZER;

static int bankAccount = 0;

static void * wrfunc(void * arg)
{
  int ba;

  assert(pthread_rwlock_wrlock(&rwlock1) == 0);
  pte_osThreadSleep(2000);
  bankAccount += 10;
  ba = bankAccount;
  assert(pthread_rwlock_unlock(&rwlock1) == 0);

  return ((void *)  (int64_t) ba);
}

static void * rdfunc(void * arg)
{
  int ba;

  assert(pthread_rwlock_rdlock(&rwlock1) == 0);
  ba = bankAccount;
  assert(pthread_rwlock_unlock(&rwlock1) == 0);

  return ((void *)  (int64_t) ba);
}

int pthread_test_rwlock6()
{
  pthread_t wrt1;
  pthread_t wrt2;
  pthread_t rdt;
  int wr1Result = 0;
  int wr2Result = 0;
  int rdResult = 0;

  rwlock1 = PTHREAD_RWLOCK_INITIALIZER;

  bankAccount = 0;

  assert(pthread_create(&wrt1, NULL, wrfunc, NULL) == 0);
  pte_osThreadSleep(500);
  assert(pthread_create(&rdt, NULL, rdfunc, NULL) == 0);
  pte_osThreadSleep(500);
  assert(pthread_create(&wrt2, NULL, wrfunc, NULL) == 0);

  assert(pthread_join(wrt1, (void **) &wr1Result) == 0);
  assert(pthread_join(rdt, (void **) &rdResult) == 0);
  assert(pthread_join(wrt2, (void **) &wr2Result) == 0);

  assert(wr1Result == 10);
  assert(rdResult == 10);
  assert(wr2Result == 20);

  assert(pthread_rwlock_destroy(&rwlock1) == 0);

  return 0;
}
