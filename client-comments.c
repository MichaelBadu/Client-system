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

//Sends user inputs to the server, when an option is choosen.
void send_user_input(int socket, char *in[INPUTSIZE])
{
	writen(socket, (unsigned char *) in, sizeof(size_t));
}


void get_student_info(int socket){
//Declaring the string
	char info_string [100];
	size_t n;

	readn(socket, (unsigned char *) &n, sizeof(size_t));
	readn(socket, (unsigned char *) info_string, n);
//Prints the server IP Address, name and ID 
	printf("\nServer IP Address, Student Name, Student ID: \n%s\n\n", info_string);
}
//Receiving and displaying a list of random numbers.
void get_random_numbers(int socket){
//Variable that contain the numbers from the server
	char random_list [10000];
	size_t n;
	readn(socket, (unsigned char *) &n, sizeof(size_t));
	readn(socket, (unsigned char *) random_list, n);
	printf("%s\n", random_list);
}
//Receiving and displaying uname information
void get_uname_info(int socket){

	struct utsname uname_info;
	size_t payload_length;

	readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
	readn(socket, (unsigned char *) &uname_info, payload_length);
// Displays the attributes of the struct
	printf("System Name = %s\n", uname_info.sysname);
    printf("Host Name   = %s\n", uname_info.nodename);
    printf("Release     = %s\n", uname_info.release);
    printf("Version     = %s\n", uname_info.version);
    printf("Machine     = %s\n", uname_info.machine);
}
//Displaying the files in the server
void get_filenames(int socket){
//Allocating the memory for the file string
	char *files = malloc(1000 * sizeof(char));
	size_t n;
//Gets the size of the file
	readn(socket, (unsigned char *) &n, sizeof(size_t));
	readn(socket, (unsigned char *) files, n);
	printf("List of files in upload directory:\n %s\n", files);
//free cleans up the memory after the memory has been allocated
        free(files);
}

void get_file(int socket){
//Gets the files from the server
	FILE *filePointer ;
	char filename [50];
//Allocating the memory of the content of the files
        char *file_content = malloc(10000 * sizeof(char));
	size_t n;
//This tells the user to input the file names
	printf("Enter file name:\n");
//Then the fget - gets the user inputs
	fgets(filename, 50, stdin);
	filename[strcspn(filename, "\n")] = 0;
//Send the name of the file that one want to the server
	writen(socket, (unsigned char *) filename, sizeof(strlen(filename)));
//This receives the contents of the file
	readn(socket, (unsigned char *) &n, sizeof(size_t));
	readn(socket, (unsigned char *) file_content, n);
//Open the new files that the user just inputs in
	filePointer = fopen(filename, "wb");
//checks for error or name of the file
	if ( filePointer == NULL ){
		printf( "%s file failed to open.\n",filename ) ;
	}
	else
	{
		if ( strlen ( file_content ) > 0 ){
// Contents is assigned into the file
			fputs(file_content, filePointer) ;
		}
		fclose(filePointer) ;
	}
//Empty's the file content
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
//Displays the menu
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
//fgets scans the user inputs 
			fgets(input, INPUTSIZE, stdin);
			input[strcspn(input, "\n")] = 0;
//Tranforms the user input from string to character.
			option = input[0];
//Re - display the user input or option
			printf("User input: %s\n",input);
 			if (strlen(input) > 1)
    			option = 'x';
//Sends the user inputs to the server
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
