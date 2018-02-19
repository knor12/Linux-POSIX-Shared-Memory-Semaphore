/*
 * common.h
 *
 *  Created on: Feb 19, 2018
 *      Author: home
 */

#ifndef COMMON_H_
#define COMMON_H_

//shared memory file name in the file system
#define SHARED_MEMORY_NAME "shm_file"

//2K should be enough for waht we are doing
#define SHARED_MEMORY_SIZE 0x7FF

//open shared memory for read and write
//create a name in the file system if it doesn't exist
#define SHARED_MEMORY_SERVER_FLAGS (O_CREAT|O_RDWR)

#define SHARED_MEMORY_CLIENT_FLAGS (O_RDWR)

//shared file descriptor mode
mode_t shm_mode = 0x666;

//named semaphore
#define SEMAPHORE_NAME "shm_semaphore"
#define SEMAPHORE_SERVER_FLAGS (O_CREAT)
#define SEMAPHORE_CLIENT_FLAGS (O_CREAT)

//message offsets
#define OFFSET_MESSAGE_FLAG 0 //any aditional meta data between the server and client
#define OFFSET_MESSAGE_PALYLOAD 4

//message flags
#define FLAG_NEW_MESSAGE 1


//delays
#define SERVER_DLY_SEC 3
#define CLIENT_DLY_SEC 1

#endif /* COMMON_H_ */
