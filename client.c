// Cwk2: client.c - message length headers with variable sized payloads
//  also use of readn() and writen() implemented in separate code module

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#include "rdwrn.h"

#define INPUTSIZE 10


void send_user_input(int socket, char *in[INPUTSIZE])
{
	writen(socket, (unsigned char *) in, sizeof(size_t));
}


void get_student_info(int socket){
	char info_string [100];
	size_t n;

	readn(socket, (unsigned char *) &n, sizeof(size_t));
	readn(socket, (unsigned char *) info_string, n);
	printf("\nServer IP Address, Student Name, Student ID: \n%s\n\n", info_string);
}

void get_random_numbers(int socket){
	char random_list [10000];
	size_t n;
	readn(socket, (unsigned char *) &n, sizeof(size_t));
	readn(socket, (unsigned char *) random_list, n);
	printf("%s\n", random_list);
}

void get_uname_info(int socket){
	struct utsname uname_info;
	size_t payload_length;

	readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
	readn(socket, (unsigned char *) &uname_info, payload_length);
	printf("System Name = %s\n", uname_info.sysname);
    printf("Host Name   = %s\n", uname_info.nodename);
    printf("Release     = %s\n", uname_info.release);
    printf("Version     = %s\n", uname_info.version);
    printf("Machine     = %s\n", uname_info.machine);
}

void get_filenames(int socket){
	char *files = malloc(1000 * sizeof(char));
	size_t n;
	readn(socket, (unsigned char *) &n, sizeof(size_t));
	readn(socket, (unsigned char *) files, n);
	printf("List of files in upload directory:\n %s\n", files);
        free(files);
}

void get_file(int socket){
	FILE *filePointer ;
	char filename [50];
        char *file_content = malloc(10000 * sizeof(char));
	size_t n;

	printf("Enter file name:\n");
	fgets(filename, 50, stdin);
	filename[strcspn(filename, "\n")] = 0;
	writen(socket, (unsigned char *) filename, sizeof(strlen(filename)));

	readn(socket, (unsigned char *) &n, sizeof(size_t));
	readn(socket, (unsigned char *) file_content, n);
	filePointer = fopen(filename, "wb");

	if ( filePointer == NULL ){
		printf( "%s file failed to open.\n",filename ) ;
	}
	else
	{
		if ( strlen ( file_content ) > 0 ){
			fputs(file_content, filePointer) ;
		}
		fclose(filePointer) ;
	}
        free(file_content);
}


int main(void)
{
    // *** this code down to the next "// ***" does not need to be changed except the port number
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Error - could not create socket");
		exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;

    serv_addr.sin_port = htons(50031);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
		perror("Error - connect failed");
		exit(1);
    }
    else{
    	printf("Connected to server...\n");

    	char option;
    	char input[INPUTSIZE];

		do
		{
			printf("*****************************************************************\n");
			printf("*                    Select an Option:                          *\n");
			printf("*****************************************************************\n");
			printf("*    1. Display the Server IP Address & Student Details         *\n");
			printf("*    2. Display the Array of numbers                            *\n");
			printf("*    3. Display the uname information                           *\n");
			printf("*    4. Display the list of file from server                    *\n");
			printf("*    5. Copy file in server upload directory                    *\n");
			printf("*    6. Exit                                                    *\n");
			printf("*****************************************************************\n\n");

			fgets(input, INPUTSIZE, stdin);
			input[strcspn(input, "\n")] = 0;
			option = input[0];

			printf("User input: %s\n",input);
 			if (strlen(input) > 1)
    			option = 'x';

			send_user_input(sockfd, &input);
			printf("\n");
 			switch (option) {
				case '1':
					get_student_info(sockfd);
					break;
				case '2':
					get_random_numbers(sockfd);
					break;
				case '3':
					get_uname_info(sockfd);
					break;
				case '4':
					get_filenames(sockfd);
					break;
				case '5':
					get_file(sockfd);
					break;
				case '6':
					printf("Goodbye!\n");
					break;
				default:
					printf("Invalid choice - 0 displays options...!\n");
				break;
			}
		}
		while (option != '6');
	}
	close(sockfd);

    exit(EXIT_SUCCESS);
}
