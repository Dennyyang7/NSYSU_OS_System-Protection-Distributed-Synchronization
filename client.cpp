#include <iostream>
#include <sys/types.h>  // socket()
#include <sys/socket.h> // socket()
#include <netinet/in.h> // bind()
#include <arpa/inet.h>  //inet_addr 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>

#pragma comment(lib, "ws2_32.lib")
#define PORT 6666
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 128

using namespace std;
void wait_rw(void);
int main(){
	int Client = 0, ret; 
	struct sockaddr_in server; 
	char str[] = "Hello from client"; 
	char *hello = str;
	char buffer[BUFFER_SIZE]; 
	char message[1000] , server_reply[2000], buf1[100], name[10];
	
	if ((Client = socket(AF_INET, SOCK_STREAM, 0)) == -1){ // -1 if error 
		printf("\n Socket creation error \n"); 
		return -1; 
	}
	
	server.sin_family = AF_INET;				//IPv4
	server.sin_addr.s_addr = inet_addr("127.0.0.1");	
	server.sin_port = htons(PORT);				//specific port 6666
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) == 0){ // Âà´«ºôµ¸¥D¾÷¦a§}ip(127.0.0.1)¬°¤G¶i¨î¼Æ­È  = 0 ªí¥Ü¦a§}µL®Ä
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	}
	puts("Connected\n");
	if (connect(Client, (struct sockaddr *)&server, sizeof(server)) == -1){
		printf("\nConnection Failed \n"); 
		return -1; 
    	}
	printf("input ID(user1~6) :\n");
	scanf("%s", name);	
	while(strcmp(name,"user1") != 0 && strcmp(name, "user2") != 0 &&
		strcmp(name,"user3") != 0 && strcmp(name, "user4") != 0 &&
		strcmp(name,"user5") != 0 && strcmp(name, "user6") != 0){
		printf("Input error, please try again : \n");
		printf("input ID :\n");
		bzero(name,10);
		scanf("%s", name);
	}
	send(Client, name, strlen(name), 0);//return name 
	int read_size = recv(Client , buf1 , 40 , 0); //"user1 is in AOS-students group
	printf("\n%s",buf1);	
	bzero(buf1, strlen(buf1)); // buf1 æ¸… zero

	char comm[100] = "";
	char file_name[100], file_right[100], buf[1000], buf4[100];
	while(1){
		bzero(comm,100);
		printf("\nInput command:\n");
		scanf("%s", comm);
		send(Client, comm, strlen(comm), 0);//command line	
		if(strcmp(comm,"exit") == 0){
			break;
		}
		else if (strcmp(comm, "help") == 0){printf("exit/create/read/write/changemode");}
		else{
			if(strcmp(comm,"create") == 0){
				bzero(file_name, 100);
				bzero(file_right, 100);
				printf("file_name :\n");
				scanf("%s", file_name);
				send(Client, file_name, 40, 0);			
				printf("file_right :\n");
				scanf("%s", file_right);
				send(Client, file_right, 40, 0);
				FILE *fp = fopen(file_name, "w");
				printf("write something (input 0 to exit)... \n");	
				char c;
				while(1){
					scanf("%c", &c);	
					if(c == '0') break;			
					fprintf(fp, "%c", c);
				}
				fclose(fp);
			}
			else if(strcmp(comm, "read") == 0){
				printf("file read, please input file name : \n");
				bzero(file_name,100);
				bzero(buf4,1000);
				scanf("%s", file_name);
				send(Client, file_name, strlen(file_name), 0); // send file_name
				recv(Client, buf4, 100, 0); // recv file exist/permission
				printf("%s", buf4);
				if(strcmp(buf4, "you have permission") == 0){
					bzero(buf,1000);
					recv(Client, buf, 1000, 0); // read file
					printf("%s", buf);
				}
			}
			else if(strcmp(comm, "write") == 0){				
				bzero(file_name, 100);
				printf("file write, please input file name : \n");
				scanf("%s", file_name);
				send(Client, file_name, strlen(file_name), 0);
				bzero(buffer,1024);
				recv(Client, buffer, 1000, 0); // recv file exist/permission
				printf("%s\n", buffer);
				if(strcmp(buffer, "you have permission") == 0){
					char a_o[10]; 
					bzero(a_o,10);				
					printf("a_write / o_write ? \n");
					scanf("%s", a_o);
					send(Client, a_o, 10, 0); //a_write / o_write
					printf("(%s)write something (input 0 to exit) :\n",a_o);
					int length = 0;				
					char number[BUFFER_SIZE];
					bzero(number, 1024);
					char c;
					while(1){  // write content
						scanf("%c", &c);
						if(c == '0') break;			
						number[length++] = c;
					}
					send(Client, number, strlen(number), 0); //send content
					void wait_rw();
					bzero(buffer,1024);
					recv(Client, buffer,100,0); // Already written
					printf("%s\n", buffer);
				}
			}
			else if(strcmp(comm, "changemode") == 0){
				bzero(file_name, 100);
				printf("file changemode, please input file name : \n");
				scanf("%s", file_name);
				send(Client, file_name, strlen(file_name), 0);
				bzero(buffer,1024);
				recv(Client, buffer, 1000, 0); // recv file exist/permission
				printf("%s\n", buffer);
				if(strcmp(buffer, "you have permission") == 0){
					bzero(file_right, 100);
					printf("file_right :\n");
					scanf("%s", file_right);
					send(Client, file_right, 40, 0);
				}
			}
		}
	}
	shutdown(Client, SHUT_RDWR);
	close(Client);
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
	
