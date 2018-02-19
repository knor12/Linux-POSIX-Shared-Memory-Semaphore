/*
 ============================================================================
 Name        : shm_client.c
 Author      : Noreddine Kessa
 Version     :
 Description : Client/consumer side application for posix shared memory and named semaphore
 code example.
 Notes: 	 : To compile: gcc -o client shm_client.c -lrt -lpthread
 	 	 	   To run: sudo ./server
 	 	 	   The server side process needed to be running before running the client, because the
 	 	 	   server prepares shared memory.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "./common.h"
#include <semaphore.h>

int main(void) {

	int err;

	//file descriptor
	int shm_fd;

	//create shared memory file descriptor
	shm_fd = shm_open(SHARED_MEMORY_NAME, SHARED_MEMORY_CLIENT_FLAGS, shm_mode);

	//check for errors
	if (shm_fd < 0) {
		printf("Error opening shared memory, error:%d\n", errno);
		return EXIT_FAILURE;
	} else {
		printf("File descriptor %s created OK \n", SHARED_MEMORY_NAME);
	}

	// truncate the file descriptor to the shared memory size
	err = ftruncate(shm_fd, SHARED_MEMORY_SIZE);
	if (err == -1) {
		printf("Error trucating file %s.\n", SHARED_MEMORY_NAME);
		return EXIT_FAILURE;
	} else {
		printf("File %s truncated OK\n", SHARED_MEMORY_NAME);
	}

	//map shared memory to file descriptor
	void * ptr_shm = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE,
	MAP_SHARED, shm_fd, 0);
	if (ptr_shm == MAP_FAILED) {
		printf("Error mapping memory to file descriptor, error:%d\n", errno);
	} else {
		printf("Memory mapped to file descriptor OK \n");

	}

	//create a named semaphore for execusive access to shared memory
	sem_t * shm_sem = sem_open(SEMAPHORE_NAME, SEMAPHORE_CLIENT_FLAGS, 0644, 1);
	sem_unlink(SEMAPHORE_NAME);
	if (shm_sem == SEM_FAILED) {
		printf("Error opening semaphore error:%d\n", errno);
		return EXIT_FAILURE;
	} else {
		printf("Semaphore opened OK\n");
	}

	//get the pointers for share mempry fields
	char * prt_shm_message = (char*) (ptr_shm + OFFSET_MESSAGE_PALYLOAD);
	char * ptr_shm_flags = (char*) (ptr_shm + OFFSET_MESSAGE_FLAG);

	//forever loop
	while (1) {

		//get the semaphore
		sem_wait(shm_sem);

		//check if a new message is available
		if (*ptr_shm_flags & FLAG_NEW_MESSAGE) {

			//clear the flag
			*ptr_shm_flags &= ~(FLAG_NEW_MESSAGE);

			//get and print the message
			printf("Read from shared memory: %s\n", prt_shm_message);
		}

		//release the semaphore
		sem_post(shm_sem);

		//no need to wait, consume data as it is produced by the server
		//sleep(CLIENT_DLY_SEC);

	}

	//this point is never reached
	//exiting sequence
	err = shm_unlink(SHARED_MEMORY_NAME);
	if (err < 0) {
		printf("Error closing shm %s\n", SHARED_MEMORY_NAME);
		return EXIT_FAILURE;
	} else {
		printf("File descriptor %s closed OK\n", SHARED_MEMORY_NAME);
	}

	return EXIT_SUCCESS;
}
