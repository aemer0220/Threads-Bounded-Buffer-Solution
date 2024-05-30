/*
* Alexandra Emerson
* CS 441/541: Threads (Bounded Buffer)
*
* Last Modified: 3/31/2024
*/
#include "bounded-buffer.h"

int main(int argc, char * argv[]) {
	int *options;
	int i, rc;
	
	if(argc > 5 || argc < 4){
		fprintf(stderr, "ERROR: Parameters incorrectly specified.\n");
		exit(-1);
	}
	
  options = process_args(argc, argv);
  sleep_time = options[0];
  num_producers = options[1];
  num_consumers = options[2];
  buffer_size = options[3];
  
  if (sleep_time < 0 || num_producers < 0 || num_consumers < 0 || buffer_size < 0){
		fprintf(stderr, "ERROR: Parameters incorrectly specified\n");
		exit(-1);
  }

  //allocate spaces for threads and buffer
  p_threads = malloc(sizeof(pthread_t) * num_producers);
  c_threads = malloc(sizeof(pthread_t) * num_consumers);
  buffer = (int *) malloc(sizeof(int) * buffer_size);

  //initialize each buffer item to -1 to indicate empty
  for (i = 0; i < buffer_size; i++){
	  buffer[i] = -1;
  }

  initialize();

  //print buffer
  printf("Buffer Size               :%4d\n", buffer_size);
  printf("Time To Live (seconds)    :%4d\n", sleep_time);
  printf("Number of Producer threads:%4d\n", num_producers);
  printf("Number of Consumer threads:%4d\n", num_consumers);
  printf("-------------------------------\n");
  printf("Initial Buffer:                 \t%s\n", buffer_to_string(buffer));

  //create producer thread(s)
  for (i = 0; i < num_producers; i++){
	  if ((rc = pthread_create(&p_threads[i], NULL, producer_handler, (void *) (intptr_t)i)) != 0){
		  fprintf(stderr, "ERROR: Return code from pthread_create() is %d\n", rc);
		  exit(-1);
	  }
  }

  //create consumer thread(s)
  for (i = 0; i < num_consumers; i++){
	  if ((rc = pthread_create(&c_threads[i], NULL, consumer_handler, (void *) (intptr_t)i)) != 0){
		  fprintf(stderr, "ERROR: Return code from pthread_create() is %d\n", rc);
		  exit(-1);
	  }
  }

  sleep(sleep_time);

  // thread cancel/joins - ISSUE HERE
  for (i = 0; i < num_producers; i++){
  	  //pthread_cancel(p_threads[i]); //for some reason - seg fault on cancel??
	  //pthread_join(p_threads[i], NULL); //deadlock on join
  }
  
  for (i = 0; i < num_consumers; i++){
	  //pthread_cancel(c_threads[i]); //for some reason - seg fault on cancel??
	  //pthread_join(c_threads[i], NULL); //deadlock on join
  }

  printf("-----------+---------\n");
  printf("Produced   | %4d\n", num_producers);
  printf("Consumed   | %4d\n", num_consumers);
  printf("-----------+---------\n");
  fflush(NULL);
  
  return 0;
}

int * process_args(int argc, char * argv[]){
	int i, *options;

	options = (int *) malloc(sizeof(int) * 4); // 4 = # of options
	options[3] = 10; //default
	
	for (i = 1; i < argc; i++){
		if (is_integer(argv[i]) == 0){ //if argument is not an integer
			fprintf(stderr, "ERROR: Parameters incorrectly specified.\n");
			exit(-1);
		}

		options[i-1] = strtol(argv[i], NULL, 10); //convert string to int and put in array, starting at 0.
	}

	buffer_size = options[3]; //default or user-specified
	return options;
}

int is_integer(char *input){
	size_t length = strlen(input);
	int i;
	
	for(i = 0; i < length; i++){
		if (!isdigit(input[i])){ //if a non-digit character is called, return 0 (false)
			return 0; 
		}
	}
	return 1;
}

char * buffer_to_string(int *buffer_arr){
	char *string;
	char build[10]; //temporary storage to make concatenation doable
	int r_pos = read_pos;
	int w_pos = write_pos;
	int i;
	
	string = (char *) malloc(sizeof(char) * buffer_size * 8); //8-bit word
	sprintf(string, "[");

	//repeatedly concatenate to build up the line
	for (i = 0; i < buffer_size; i++){
		sprintf(build, "%4d", buffer_arr[i]); //build needed here due to # of parameters
		strcat(string, build);
		
		if (i == w_pos){
			strcat(string, "^");
		}
		if (i == r_pos){
			strcat(string, "v");
		}
	}

	strcat(string, "]");
	return string;
}


//initializes mutex, items, and spaces semaphores 
void initialize(){
	int rc;
	pos = 0;
	
	/* allocate sempahores */
	mutex = malloc(sizeof(semaphore_t));
	items_sem = malloc(sizeof(semaphore_t));
	spaces_sem = malloc(sizeof(semaphore_t));
	
	/* initialize mutex */
	if ((rc = semaphore_create(mutex, 1)) != 0){
		fprintf(stderr, "ERROR: Failed to create Mutex.\n");
		exit(-1);
	}

	//create items semaphore
	if ((rc = semaphore_create(items_sem, 0)) != 0){ 
		fprintf(stderr, "ERROR: Failed to create Items Sempahore.\n");
		exit(-1);
	}

	//create spaces semaphore
	if ((rc = semaphore_create(spaces_sem, buffer_size)) != 0){
		fprintf(stderr, "ERROR: Failed to create Spaces Sempahore.\n");
		exit(-1);
	}
}
		  
void * producer_handler(void *threadid){
	int tid = (intptr_t)threadid;
	int rc, item;

	while(true){
		usleep(random() % 1000000); //sleep for random # of microseconds
		item = rand() % 10;
		
		semaphore_wait(spaces_sem);
		semaphore_wait(mutex);

   		if ((rc = buffer_add(item)) != 0){
			fprintf(stderr, "ERROR: Failed to add item to buffer.\n");
			exit(-1);
		}
		num_producers++;
		printf("Producer %2d: Total %3d, Value %3d\t%s\n", tid, num_producers, item, buffer_to_string(buffer));
		fflush(NULL);
		semaphore_post(mutex);
		semaphore_post(items_sem);
	}

	semaphore_post(mutex);
	semaphore_post(items_sem);
	pthread_exit(NULL);
}


void * consumer_handler(void *threadid){
	int tid = (intptr_t)threadid;
	int rc, item;

	while(true){
		usleep(random() % 1000000); //sleep for random # of microseconds
		item = rand() % 10;
		
		semaphore_wait(items_sem);
		semaphore_wait(mutex);
		
		item = buffer[pos % buffer_size];
		
		if ((rc = buffer_get(&item)) != 0){
			fprintf(stderr, "ERROR: Failed to remove item from buffer.\n");
			exit(-1);
		}
		
		num_consumers++;
		printf("Consumer %2d: Total %3d, Value %3d\t%s\n", tid, num_consumers, item, buffer_to_string(buffer));
		fflush(NULL);
		
		semaphore_post(mutex);
		semaphore_post(spaces_sem);
	}

	semaphore_post(mutex);
	semaphore_post(spaces_sem);

	pthread_exit(NULL);
}

int buffer_add(int item){
	if (buffer[write_pos] != -1){
  		return -1;
	}
	pos++;
	
	buffer[write_pos] = item; //insert item into buffer
	write_pos++; //advance position

	while (write_pos >= buffer_size){
		write_pos = write_pos - buffer_size; //reset write position
	}

	return 0;
}

int buffer_get(int *item){
	if(buffer[read_pos] == -1){ //if buffer position is empty
		return -1;
	}

	*item = buffer[read_pos]; //place object into item
	read_pos++;
	buffer[read_pos - 1] = - 1; //empty buffer position

	while (read_pos >= buffer_size){ //reset read position
		read_pos = read_pos - buffer_size;
	}
	
	return 0;
}
