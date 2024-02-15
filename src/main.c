#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "utils.h"
#include "network_utils.h"


typedef struct {
	int buffer_size;
	int connect_fd;
	pthread_t receiveThreadID;
	pthread_t sendThreadID;
} thread_arg_t;

void* receiveThread(void* arg){
	thread_arg_t *thread_arg = (thread_arg_t *)arg;
	char buffer[thread_arg->buffer_size];

	while(1){
		if(recv(thread_arg->connect_fd, buffer, sizeof(buffer), 0) != 0){
			printf("message from server : %s", buffer);
		}
		else{
			pthread_cancel(thread_arg->sendThreadID);
			break;
		}
	}
	return NULL;
}



void* sendThread(void* arg){
	thread_arg_t *thread_arg = (thread_arg_t *)arg;
	char buffer[thread_arg->buffer_size];

	while(1){
		fgets(buffer, sizeof(buffer), stdin);
		send(thread_arg->connect_fd, buffer, sizeof(buffer), 0);
	}
	return NULL;
}


int main(int argc, char *argv[]){
	//初始化log資訊
	log_data_t log_data;
	log_data.on = 0;
	log_data.process_name = "test_client";
	log_data.pid = getpid();

	//檢查參數完整性
	if(argc < 4){
		log_head(&log_data);
		printf("缺少參數\n");
		exit(1);
	}
	else{
		//是否開啟狀態回報----------------------------------------------------------------
		log_data.on = atoi(argv[1]);
	}


	//註冊信號事件----------------------------------------------------------------------------

	//取得用來連線的socket
	int client_fd = get_socket(&log_data);
	//設定 server 地址
	sockaddr_in_t server_addr = set_server_addr(&log_data, argv[2], argv[3]);
	//連接伺服器
	connect_server(&log_data, client_fd, &server_addr);

	thread_arg_t thread_arg;
	thread_arg.buffer_size = 1024;
	thread_arg.connect_fd = client_fd;

	pthread_create(&(thread_arg.receiveThreadID), NULL, receiveThread, (void *)&thread_arg);
	pthread_create(&(thread_arg.sendThreadID), NULL, sendThread, (void *)&thread_arg);

	pthread_join(thread_arg.receiveThreadID, NULL);
	pthread_join(thread_arg.sendThreadID, NULL);
	log_head(&log_data);
	printf("connect close\n");

	close(client_fd);

	return 0;
}
