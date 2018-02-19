/*
 ============================================================================
 Name        : shm_server.c
 Author      : Noreddine Kessa
 Version     :
 Description : Server/producer side application for posix shared memory and named semaphore
 	 	 	   code example.
 Notes: 	 : To compile: gcc -o server shm_server.c -lrt -lpthread
 	 	 	   To run: sudo ./server
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
#include <fcntl.h>
#include <semaphore.h>
#include "./common.h"

int main(void) {

	int err;

	//file descriptor
	int shm_fd;

	//just in case
	shm_unlink(SHARED_MEMORY_NAME);

	//create shared memory file descriptor
	shm_fd = shm_open(SHARED_MEMORY_NAME, SHARED_MEMORY_SERVER_FLAGS, shm_mode);

	//check for errors
	if (shm_fd < 0) {
		printf("Error creating shared memory, error:%d\n", errno);
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
		return EXIT_FAILURE;
	} else {
		printf("Memory mapped to file descriptor OK \n");

	}



	//create a named semaphore for exclusive access to memory
	sem_unlink(SEMAPHORE_NAME);
	sem_t *	 shm_sem = sem_open(SEMAPHORE_NAME, SEMAPHORE_CLIENT_FLAGS, 0666, 1);
	sem_unlink(SEMAPHORE_NAME);
	if (shm_sem == SEM_FAILED) {
		printf("Error creating/opening semaphore error:%d\n", errno);
		return EXIT_FAILURE;
	} else {
		printf("Semaphore created OK\n");
	}

	//in case the semaphore is in a weird state
	sem_unlink(SEMAPHORE_NAME);

	//counter
	int i = 0;

	//get the pointers for share mempry fields
	char * prt_shm_message = (char*) (ptr_shm + OFFSET_MESSAGE_PALYLOAD);
	char * ptr_shm_flags = (char*) (ptr_shm + OFFSET_MESSAGE_FLAG);

	//message temp
	char message[100]={0};
	//forever loop
	while (1) {

		//prepare the message to be sent to the client
		message[0]=0;
		sprintf(message, "Message number %d", i);
		i++;

		//get the semaphore
		sem_wait(shm_sem);

		//write the message to shared memory
		prt_shm_message[0]=0;
		strcpy(prt_shm_message , message);
		printf("Wrote to shared memory: %s\n" , message);

		//set the flag to inform client od a new message
		*ptr_shm_flags = FLAG_NEW_MESSAGE;

		//release the semaphore
		sem_post(shm_sem);

		//delay
		sleep(SERVER_DLY_SEC);

	}

	//write a message to shared memory
	char msg[] = "message send to shared memory";
	strcpy((char*) (ptr_shm), msg);

	//exiting sequence
	char c = getchar();
	err = shm_unlink(SHARED_MEMORY_NAME);
	if (err < 0) {
		printf("Error closing shm %s\n", SHARED_MEMORY_NAME);
		return EXIT_FAILURE;
	} else {
		printf("File descriptor %s closed OK\n", SHARED_MEMORY_NAME);
	}

	return EXIT_SUCCESS;
}
