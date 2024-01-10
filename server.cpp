#include <iostream>
#include <sys/types.h>  // socket()
#include <sys/socket.h> // socket()
#include <netinet/in.h> // bind()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> //for threading , link with lpthread
#include <vector>
#include <string>
#include <unistd.h>


#pragma comment(lib, "ws2_32.lib")
#define PORT 6666
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 128

using namespace std;

void *connection_handler(void *);

struct client 
{
	int fd;
	int leave;
	int name[100];
	int group;
	char accessRW[100];
};

struct finfo 
{
	char same[20];
	char other[20];
	char fname[20];
	char access[10];
	char date[30];
	char owner[20];
	int group;
	int fsize;
	bool flag; //r or w
};
struct client cli[100];
struct finfo ff[100];
int clientNum = 0;
int fileNum = 0;

void wait_rw(void);
void show_cap(void);

int main(){
	int socketfd, new_socket, c, *new_sock;
	struct sockaddr_in server, client;
	char *message;

	//Create TCP socket
	socketfd = socket( AF_INET, SOCK_STREAM, 0);  // -1 if error
	if (socketfd == -1){
		printf("Fail to create a socket");		
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;                 //IPv4
    	server.sin_addr.s_addr = INADDR_ANY;         //any address
    	server.sin_port = htons(PORT);               //specific port 6666 它將本機端的字節序(endian)轉換成了網路端的字節序
	
	// bind the socket to an ip address and port
	if (bind(socketfd, (struct sockaddr*)&server, sizeof(server)) < 0){  
		printf("Server Bind Port: %d Failed!\n", PORT);  
		exit(1);  
    	}
	puts("bind done");
	// Tell socket the socket is for listening
	if (listen(socketfd, LENGTH_OF_LISTEN_QUEUE)){   //backlog(LENGTH_OF_LISTEN_QUEUE) 指定監聽佇列大小
        	printf("Server Listen Failed!\n");  
        	exit(1);  
    	}  
	// Wait for a connection
	c = sizeof(struct sockaddr_in);
	while( (cli[clientNum].fd = accept(socketfd, (struct sockaddr *)&client, (socklen_t*)&c)) ){
		puts("Connection accepted");
		pthread_t sniffer_thread;
		new_sock = (int *)malloc(1);
		*new_sock = cli[clientNum].fd;
		cli[clientNum].leave = 0;//login
		clientNum++;
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0){
			perror("could not create thread");
			return 1;
		}

		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
		}

	if (new_socket < 0){
		perror("accept failed");
		return 1;
	}
}

void *connection_handler(void *socketfd){
	//Get the socket descriptor
	int sock = *(int*)socketfd;
	char name[10];
	char buf[BUFFER_SIZE], buffer[BUFFER_SIZE];
	char file_name[100], file_right[100];
	while(1){
		bzero(name,10);	
		recv(sock, name, 10, 0);//get name
		puts("............ \n");
		int group;
		int i; 
		if(strcmp(name,"user1") == 0) { send(sock,"user1 is in AOS_group\n", 40, 0); 
						cli[0].name[0] = 0; cli[0].group = 1; ff[0].group = 1; i= 0;}
		else if(strcmp(name,"user2") == 0) { send(sock,"user2 is in AOS_group\n", 40, 0); 
						cli[1].name[1] = 1; cli[1].group = 1; ff[1].group = 1; i= 1;}
		else if(strcmp(name,"user3") == 0) { send(sock,"user3 is in AOS_group\n", 40, 0);
						cli[2].name[2] = 2; cli[2].group = 1; ff[2].group = 1; i= 2;}
		else if(strcmp(name,"user4") == 0) { send(sock,"user4 is in CSE_group\n", 40, 0); 
						cli[3].name[3] = 3; cli[3].group = 2; ff[3].group = 2; i= 3;}
		else if(strcmp(name,"user5") == 0) { send(sock,"user5 is in CSE_group\n", 40, 0); 
						cli[4].name[4] = 4; cli[4].group = 2; ff[4].group = 2; i= 4;}
		else if(strcmp(name,"user6") == 0) { send(sock,"user6 is in CSE_group\n", 40, 0); 
						cli[5].name[5] = 5; cli[5].group = 2; ff[5].group = 2; i= 5;}

		//Free the socket pointer
		//void free(void *sock);
		//find numbers user
		while(1){
			bzero(buf,1000);//init
			bzero(buffer,1000);
			recv(sock, buf, 1000, 0);	//command line (exit/create/read/write/changemode)
			if(strcmp(buf,"exit") == 0){
				cli[i].leave = 1;
				printf("user : %s exit\n",name);
			}
			else{
				if(strcmp(buf, "create") == 0){
					int j;
					bzero(file_name,100);
					bzero(file_right,100);					
					recv(sock , file_name , 40 , 0); // file name
					recv(sock , file_right , 40 , 0); // file right
					for(j = 0; j < strlen(file_right); j++){
						ff[i].access[j] = file_right[j]; // file right access
					}					
					for(j = 0; j < strlen(file_name); j++){
						ff[i].fname[j] = file_name[j]; //  file name access
					}
					ff[i].owner[0] = i+1;
					show_cap();
				}
				else if(strcmp(buf, "read") == 0){
					char buf1[40];
					bzero(buf1,40);
					recv(sock, buf1, 40, 0); // recv file_name
					char r_buffer[BUFFER_SIZE], ch; // read file char
					bzero(buffer, BUFFER_SIZE);
					int file_block_length = 0;
					int j;	
					int which = -1;
					int owner_permi = 0, group_permi = 0, other_permi = 0; // read right
					for(j = 0; j < 6; j++) { // find same group
						if(strcmp(buf1,ff[j].fname) == 0) {
							which = j;
							ff[7].owner[0] = j;
							break;
						}
					}
					if(which == -1){
						send(sock, "no exist", 20, 0);
					}
					else{
						if(cli[i].group == ff[which].group){// using user's group = file[owner] group
							if(ff[which].access[0] == 'r') owner_permi = 1;
							else if(ff[which].access[0] == '-') owner_permi = 0;
							if(ff[which].access[2] == 'r') group_permi = 1;
							else if(ff[which].access[2] == '-') group_permi = 0;
						}
						else{
							if(ff[which].access[4] == 'r') other_permi = 1;
							else if(ff[which].access[4] == '-') other_permi = 0;
						}
						
						if((cli[i].group == ff[which].group) &&(group_permi == 0) && (ff[7].owner[0] != i)){
							send(sock, "you don't have permission to read the file", 100, 0);
						}
						else if((cli[i].group != ff[which].group) && other_permi == 0){
							send(sock, "you don't have permission to read the file", 100, 0);
						}
						if(owner_permi == 1 ||((cli[i].group != ff[which].group) && group_permi == 1)){
							ff[0].flag = 1;						
							if(ff[0].flag == 1){
								send(sock,"you have permission", 100, 0);
								FILE *fp = fopen(ff[which].fname,"r");
								int j = 0;
								while((ch=fgetc(fp))!= EOF){
									r_buffer[j++]  = ch;
								}
								fclose(fp);
								//wait
								puts("reading....");
								wait_rw();
								send(sock,r_buffer, 1000, 0); // file content
								ff[0].flag = 0;
								puts("done");
							}
							else if(ff[0].flag = 0){printf("Someone reading/writting..");};
						}
						else if((cli[i].group != ff[which].group) && other_permi == 1){
							ff[0].flag = 1;		
							if(ff[0].flag == 1){							
								send(sock,"you have permission", 100, 0);
								FILE *fp = fopen(ff[which].fname,"r");
								int j = 0;
								while((ch=fgetc(fp))!= EOF){
									r_buffer[j]  = ch;
									j++;
								}
								fclose(fp);
								//wait
								puts("reading....");
								wait_rw();
								send(sock,r_buffer, 1000, 0);
								ff[0].flag = 0;
								puts("done");
							}
							else if(ff[0].flag = 0){printf("Someone reading/writting..");};
						}
					}
				}
				else if(strcmp(buf, "write") == 0){
					char buf2[100];
					bzero(buf2,100);
					recv(sock, buf2, 100, 0); // recv file_name;
					char w_buffer[1024], ch, a_o[10]; // write file a/o
					int file_block_length = 0;
					int j;	
					int which = -1;
					int owner_permi = 0, group_permi = 0, other_permi = 0; // read right
					for(j = 0; j < 6; j++) { // find same group
						if(strcmp(buf2,ff[j].fname) == 0) {
							which = j;
							ff[7].owner[0] = j;
							break;
						}
					}

					if(which == -1){
						send(sock, "no exist", 20, 0);
					}
					else{
						if(cli[i].group == ff[which].group){// same group				
							if(ff[which].access[1] == 'w') owner_permi = 1;
							else if(ff[which].access[1] == '-') owner_permi = 0;
							if(ff[which].access[3] == 'w') group_permi = 1;
							else if(ff[which].access[3] == '-') group_permi = 0;
						}
						else{ //other group
							if(ff[which].access[5] == 'w') other_permi = 1;
							else if(ff[which].access[5] == '-') other_permi = 0;
						}
						wait_rw();
						if((cli[i].group == ff[which].group) && group_permi == 0 && (ff[7].owner[0] != i)){
							send(sock, "you don't have permission to write the file", 100, 0);
						}
						else if((cli[i].group != ff[which].group) && other_permi == 0){
							send(sock, "you don't have permission to write the file", 100, 0);
						}
						if(owner_permi == 1 || ((cli[i].group == ff[which].group) && group_permi == 1)){
							send(sock,"you have permission", 100, 0);
							bzero(a_o,10);
							recv(sock, a_o, 10, 0); // a/o
							ff[0].flag = 1;	
							if(ff[0].flag == 1){
								if(strcmp(a_o,"a_write") == 0){
									FILE *fp = fopen(ff[which].fname,"a");
									bzero(w_buffer, 1024);
									recv(sock, w_buffer, 1024, 0); // write content
									fprintf(fp, "%s", w_buffer);
									fclose(fp);
									//wait
									puts("writting....");
									wait_rw();
									send(sock,"Already written",100,0);
									ff[0].flag = 0;
									puts("done");
								}
								else if(strcmp(a_o,"o_write") == 0){
									FILE *fp = fopen(ff[which].fname,"w");
									bzero(w_buffer, 1024);
									recv(sock, w_buffer, 1024, 0);
									fprintf(fp, "%s", w_buffer);
									fclose(fp);
									//wait
									puts("writting....");
									wait_rw();
									send(sock,"Already written",100,0);
									ff[0].flag = 0;
									puts("done");
								}
							}
							else if(ff[0].flag = 0){printf("Someone reading/writting..");};
						}
						else if((cli[i].group != ff[which].group) && other_permi == 1){
							send(sock,"you have permission", 100, 0);
							bzero(a_o,10);		
							recv(sock, a_o, 10, 0); // a/o
							ff[0].flag = 1;	
							if(ff[0].flag == 1){
								if(strcmp(a_o,"a_write") == 0){
									FILE *fp = fopen(ff[which].fname,"a");
									bzero(w_buffer, 1024);
									recv(sock, w_buffer, 1024, 0); // write content
									fprintf(fp, "%s", w_buffer);
									fclose(fp);
									//wait
									puts("writting....");
									wait_rw();
									send(sock,"Already written",100,0);
									ff[0].flag = 0;
									puts("done");
								}
								else if(strcmp(a_o,"o_write") == 0){
									FILE *fp = fopen(ff[which].fname,"w");
									bzero(w_buffer, 1024);
									recv(sock, w_buffer, 1024, 0); // write content
									fprintf(fp, "%s", w_buffer);
									fclose(fp);
									//wait
									puts("writting....");
									wait_rw();
									send(sock,"Already written",100,0);
									ff[0].flag = 0;
									puts("done");		
								}
							}
							else if(ff[0].flag = 0){printf("Someone reading/writting..");}	
						}
					}
					show_cap();
				}
				else if(strcmp(buf, "changemode") == 0){
					char buf2[100];
					bzero(buf2,100);
					recv(sock, buf2, 100, 0); // recv file_name
					int j;	
					int which = -1;
					for(j = 0; j < 6; j++) { // find file owner
						if(strcmp(buf2,ff[j].fname) == 0) {
							which = j;
							ff[7].owner[0] = j;
							break;
						}
					}
					if(which == -1){
						send(sock, "no exist", 20, 0);
					}
					else{
						if(ff[7].owner[0] != i){ // only file onwer can changemode
							send(sock, "you don't have permission to changemode the file", 100, 0);
						}
						else{
							send(sock,"you have permission", 100, 0);
							bzero(file_right, 100);
							recv(sock , file_right , 40 , 0); // file right
							ff[0].flag = 1;	
							if(ff[0].flag == 1){
								for(j = 0; j < strlen(file_right); j++){
									ff[i].access[j] = file_right[j]; // file right access
								}
							}
							else if(ff[0].flag = 0){printf("Someone reading/writting..");};
						}
					}
					show_cap();					
				}							
			}
		}
	}
	return 0;
}
void wait_rw(){
	int delay=0;
	int z,y;
	for( z = 0 ; z < 10000000 ; z++){
		for(y = 0 ; y < 50 ; y++){
			delay+=z;
			if(delay > 100000) delay = 0;
		}
	}		
}
void show_cap(){
	int i,j;	
	printf("===========all_file=========== \n");
	for(i = 0; i < 6; i++){
		for(j = 0; j < 10; j++){
			printf("%c",ff[i].fname[j]);
		}
		printf(" | ");
		for(j = 0; j < 6; j++){
			printf("%c",ff[i].access[j]);		
		}
		printf(" | ");
		if(ff[i].owner[0] != 0){
			printf(" user%d",ff[i].owner[0]);
			printf(" | ");
			if((ff[i].owner[0]) < 3){
				printf("AOS");		
			}
			else{printf("CSE");};
		}		
		printf("\n");	
	}
}

