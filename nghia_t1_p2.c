/** @file task1.c
* Demonstrates semaphores to contol the synchronization of threads.
*
* This program starts a number of processes that are synchronized with
* semaphores to control the text output.
*
* @author Robert van Engelen
* Modified 3/1/21 with permission by CAC
* Modified 4/11/21 by Nghia Nguyen for CSCE311
**/

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#ifdef _POSIX_THREADS
# include <pthread.h>
#endif

#include <semaphore.h>

sem_t sema0;
sem_t sema1;
sem_t sema2;
sem_t sema3;
sem_t sema4;
sem_t sema5;
sem_t sema6;
int counter = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

void *text(void *arg);

int code[] = { 0,1,2,3,4,5,6 };

int main()
{
  sem_init(&sema0, 0, 1);
  sem_init(&sema1, 0, 1);
  sem_init(&sema2, 0, 1);
  sem_init(&sema3, 0, 1);
  sem_init(&sema4, 0, 1);
  sem_init(&sema5, 0, 1);
  sem_init(&sema6, 0, 1);

  pthread_cond_init(&cond, 0);

  int i;
  pthread_t tid[7];

  for (i = 0; i < 7; i++)
    pthread_create(&tid[i], NULL, text, (void*)&code[i]);

  for (i = 0; i < 7; i++)
    pthread_join(tid[i], NULL);

  sem_destroy(&sema0);
  sem_destroy(&sema1);
  sem_destroy(&sema2);
  sem_destroy(&sema3);
  sem_destroy(&sema4);
  sem_destroy(&sema5);
  sem_destroy(&sema6);
  pthread_cond_destroy(&cond);
  return 0;
}

void *text(void *arg)
{
  int n = *(int*)arg;
  if (n==0)
    sem_wait(&sema0);
  else if (n==1)
    sem_wait(&sema1);
  else if (n==2)
    sem_wait(&sema2);
  else if (n==3)
    sem_wait(&sema3);
  else if (n==4)
    sem_wait(&sema4);
  else if (n==5)
    sem_wait(&sema5);
  else if (n==6)
    sem_wait(&sema6);

  pthread_mutex_lock(&mutex);
  while (counter != n)
    pthread_cond_wait(&cond, &mutex);


  switch (n)
  {
    case 0:
      printf("A semaphore S is an integer-valued variable which can take only non-negative\n");
      printf("values. Exactly two operations are defined on a semaphore:\n\n");
      sem_post(&sema1);
      counter++;
      break;

    case 1:
      printf("Signal(S): If there are processes that have been suspended on this semaphore,\n");
      printf(" wake one of them, else S := S+1.\n\n");
      sem_post(&sema2);
      counter++;
      break;

    case 2:
      printf("Wait(S): If S>0 then S:=S-1, else suspend the execution of this process.\n");
      printf(" The process is said to be suspended on the semaphore S.\n\n");
      sem_post(&sema3);
      counter++;
      break;

    case 3:
      printf("The semaphore has the following properties:\n\n");
      sem_post(&sema4);
      counter++;
      break;

    case 4:
      printf("1. Signal(S) and Wait(S) are atomic instructions. In particular, no\n");
      printf(" instructions can be interleaved between the test that S>0 and the\n");
      printf(" decrement of S or the suspension of the calling process.\n\n");
      sem_post(&sema5);
      counter++;
      break;

    case 5:
      printf("2. A semaphore must be given an non-negative initial value.\n\n");
      sem_post(&sema6);
      counter++;
      break;

    case 6:
      printf("3. The Signal(S) operation must waken one of the suspended processes. The\n");
      printf(" definition does not specify which process will be awakened.\n\n");
      break;
  }
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
  pthread_exit(0);
}

