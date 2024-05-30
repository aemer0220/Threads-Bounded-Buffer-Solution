/*
 * Alexandra Emerson
 * CS 441/541: Threads (Bounded Buffer)
 *
 * Last Modified: 3/31/2024
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include "semaphore_support.h"
/*****************************
 * Defines
 *****************************/

/*****************************
 * Structures
 *****************************/


/*****************************
 * Global Variables
 *****************************/

/* semaphores */
semaphore_t *mutex, *spaces_sem, *items_sem;

/* how long to sleep, and number of producers and consumers */
int sleep_time;
int num_producers = 0;
int num_consumers = 0;

/* size of buffer - default or user-specified*/
int buffer_size;

/* buffer to hold items */
int *buffer;

/* ptthreads */
pthread_t *p_threads, *c_threads;

/* current read and write positions, and general position for buffer index helper in consumer method */
int read_pos, write_pos, pos;


/*****************************
 * Function Declarations
 *****************************/
/*
 * Initializes mutex, items.
 * Spaces semaphores
 *
 * Parameters:
 *   None
 * Returns:
 *   None
 */
void initialize(void);

/*
 * Processes command line arguments
 *
 * Parameters:
 *   argc (from main)
 *   argv (from main)
 * Returns:
 *   integer array of arguments
 */
int * process_args(int argc, char **argv);

/* 
 * Determine whether input is an integer.
 *
 * Parameters:
 *   input: a string
 * Returns:
 *   0 if true
 *   -1 if false
 */
int is_integer(char *input);

/*
 * Method for handling Producer thread
 *
 * Parameters:
 *   options: array of command arguments
 * Returns:
 *   NULL
 */
void *producer_handler(void *options);

/*
 * Method for handling Consumer thread
 *
 * Parameters:
 *   options: array of command arguments
 * Returns:
 *   NULL
 */
void *consumer_handler(void *options);

/* 
 * Inserts item into buffer
 *
 * Returns:
 *   0 on success
 *   -1 on failure/error
 */
int buffer_add(int item);

/*
 * Removes object from buffer and
 * places it in item.
 *
 * Parameters:
 *   item: an integer
 * Returns:
 *   0 on success
 *   -1 on failure/error
 */
int buffer_get(int *item);

/*
 * Converts buffer array to a string to print
 *
 * Parameters:
 *   buf: the buffer array
 * Returns:
 *   the resulting string
 *
 */
char * buffer_to_string(int *buf);
