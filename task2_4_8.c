#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

typedef struct {
	int id_num; //id
	char direct; // direction they are coming from 
	char right; // which is to the right
} bat;

//One mutex to get into ALL functions 
pthread_mutex_t mutex;
pthread_cond_t northFirst;
pthread_cond_t eastFirst;
pthread_cond_t southFirst;
pthread_cond_t westFirst;
pthread_cond_t northQueue;
pthread_cond_t eastQueue;
pthread_cond_t southQueue;
pthread_cond_t westQueue;

//Some counters: 
int total = 0; //total at intersections
int northBats = 0;
int eastBats = 0;
int southBats = 0;
int westBats = 0;
//...

void *goBatGo(void *arg);
void *doTheRest(void *arg);
void *arrive(void *arg);
void *cross(void *arg);
void *leave(void *arg);
void *check(void *arg);


int main(int argc, char **argv){
  pthread_cond_init(&northFirst, 0);
  pthread_cond_init(&eastFirst, 0);
  pthread_cond_init(&southFirst, 0);
  pthread_cond_init(&westFirst, 0);
  pthread_cond_init(&northQueue, 0);
  pthread_cond_init(&eastQueue, 0);
  pthread_cond_init(&southQueue, 0);
  pthread_cond_init(&westQueue, 0);

	//Let's safely assume that we will at most have 
	//100 BATs
  bat bats[100];
	char directions[100]; 
	strcpy(directions, argv[1]);
	printf("Argc: %i and directions %s\n", argc, directions);
	//1) loop through input (strlen(directions)) and 
	//   create array of bat structs
	size_t i;
  size_t n = strlen(directions);
  char direction;
  char right;
  for (i = 0; i < n; ++i) {
    char direction = directions[i];
    char right; 
    if (direction == 'n')
      right = 'w';
    else if (direction == 'e')
      right = 'n';
    else if (direction == 's')
      right = 'e';
    else if (direction == 'w')
      right = 's'; 

    bat a = { .id_num = i+1, .direct = direction, .right = right };
    bats[i] = a;
  }
  
	
	//2) loop through bats now and dispatch all of
	//   the threads 
	// maybe send to a function that takes in a 
	// bat struct... 
  pthread_t tid[n];
	for (i = 0; i < n; ++i)
    pthread_create(&tid[i], NULL, goBatGo, (void*)&bats[i]);

  // clean up
  for (i = 0; i < 7; ++i)
    pthread_join(tid[i], NULL);

  pthread_cond_destroy(&northFirst);
  pthread_cond_destroy(&eastFirst);
  pthread_cond_destroy(&southFirst);
  pthread_cond_destroy(&westFirst);
  pthread_cond_destroy(&northQueue);
  pthread_cond_destroy(&eastQueue);
  pthread_cond_destroy(&southQueue);
  pthread_cond_destroy(&westQueue);
	return 0; 
}

void *goBatGo(void *arg) {
	bat *b = (bat *) arg; 
	check(b);
	doTheRest(b);
}

void *doTheRest(void *arg) {
	bat *b = (bat *) arg;
  arrive(b);
	cross(b);
	leave(b);
}

void *arrive(void *arg) {
	bat *b = (bat *) arg; 
	//lock
	//if b->direct == 'n' 
	//    northBats++
	//    if northBats > 1
	//       pthread_cond_wait(&northQueue, &mutex)
	//...
	//total++
	//unlock
  pthread_mutex_lock(&mutex);
  printf("BAT %d from %c arrives at crossing\n", b->id_num, b->direct);
  if (b->direct == 'n') {
    northBats++;
    if (northBats > 1)
      pthread_cond_wait(&northQueue, &mutex);
  }
  if (b->direct == 'e') {
    eastBats++;
    if (eastBats > 1)
      pthread_cond_wait(&eastQueue, &mutex);
  }
  if (b->direct == 's') {
    southBats++;
    if (southBats > 1)
      pthread_cond_wait(&southQueue, &mutex);
  }
  if (b->direct == 'w') {
    westBats++;
    if (westBats > 1)
      pthread_cond_wait(&westQueue, &mutex);
  }
  total++;
  pthread_mutex_unlock(&mutex);
}

void *cross(void *arg) {
	bat *b = (bat *) arg; 
	//lock
	//if b->right == 'n' && northBats > 0 
	      //pthread_cond_wait(&northFirst, &mutex); 
	//...
	//if b->direction == 'n'
	//   northBats--
	//   pthread_cond_signal(&northQueue);
	//...
	//total-- 
	//unlock
  pthread_mutex_lock(&mutex);
  if (b->right == 'n' && northBats > 0)
    pthread_cond_wait(&northFirst, &mutex);
  if (b->right == 'e' && eastBats > 0)
    pthread_cond_wait(&eastFirst, &mutex);
  if (b->right == 's' && southBats > 0)
    pthread_cond_wait(&southFirst, &mutex);
  if (b->right == 'w' && westBats > 0)
    pthread_cond_wait(&westFirst, &mutex);

  if (b->direct == 'n') {
    northBats--;
    pthread_cond_signal(&northQueue);
  }
  if (b->direct == 'e') {
    eastBats--;
    pthread_cond_signal(&eastQueue);
  }
  if (b->direct == 's') {
    southBats--;
    pthread_cond_signal(&southQueue);
  }
  if (b->direct == 'w') {
    westBats--;
    pthread_cond_signal(&westQueue);
  }
  sleep(1);
  total--;
  pthread_mutex_unlock(&mutex);
}

void *leave(void *arg) {
	bat *b = (bat *) arg;
	//lock 
	//if b->direct == 'n'
	//    pthread_cond_signal(&northFirst)
	//...
	
	//unlock
  pthread_mutex_lock(&mutex);
  printf("BAT %d from %c leaving crossing\n", b->id_num, b->direct);
  if (b->direct == 'n')
    pthread_cond_signal(&northFirst);
  if (b->direct == 'e')
    pthread_cond_signal(&eastFirst);
  if (b->direct == 's')
    pthread_cond_signal(&southFirst);
  if (b->direct == 'w')
    pthread_cond_signal(&westFirst);
  pthread_mutex_unlock(&mutex);
}

void *check(void *arg) {
	bat *b = (bat *) arg;
	//if total == 4
	//   if b->direct == 'n' 
	//       pthread_cond_signal(&northFirst)
  if (northBats > 0 && southBats > 0 && eastBats > 0 && westBats>0) {
    printf("BAT jam detected, signaling North to go\n");
    pthread_cond_signal(&northFirst);
  }
}

