#include <winsock2.h>
#include <stdio.h>

#define MAX_SIZE 2048
#define MY_ERROR(s) printf(s); system("PAUSE"); exit(1);
#define MAX_BUFF_MSG 128
#define MSG_PER_PAGE 32

int main(int argc, char const *argv[]) {
	SOCKET serverSocket, clientSocket; // create a socket
	struct sockaddr_in serverAddress, clientAddress; // sockaddr_in：IP4 格式使用 ,  sockaddr_in6：IP6 格式使用 , sockaddr：通用格式
	int clientAddressLen;
	int bytesRead;
	char buf[MAX_SIZE];
	char resp[MAX_SIZE];
	int serverPort;

	if (argc == 2) {
		serverPort = atoi(argv[1]);
	}
	else {
		serverPort = 80;
	}

	// call WSAStartup first for Winsock
    WSADATA wsadata;
    if( WSAStartup(MAKEWORD(2,2),(LPWSADATA)&wsadata) != 0) { // ( version of winsock )
        MY_ERROR("Winsock Error\n");
	}

	// Create socket
	serverSocket = socket(PF_INET, SOCK_STREAM, 0); // (address , type , protocal(0表示不強制) )

	// Set the server information
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(serverPort); //converts a u_short from host to TCP/IP network byte order

	// Bind the socket
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
	MY_ERROR("Bind Error\n");
	}

	// Prepare to listen to the incoming clients
	if (listen(serverSocket, 3) < 0) {
		MY_ERROR("Listen Error\n");
	}

	int state = 1;
	/*
		1: Menu
		2: New msg
		3: Read msg
	*/
	char msg_history[MAX_BUFF_MSG][MAX_SIZE];
	int msg_num = 0;
	int page_num = 0;
	int page_id = 0;

	while (1) {
		printf("Waiting...\n");

		// Accept a client
		clientAddressLen = sizeof(clientAddress);
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLen);

		// Receive the data from the client, and send it back
		bytesRead = recv(clientSocket, buf, MAX_SIZE, 0);
		buf[bytesRead] = '\0';
		// printf("Client IP is : %s \n", inet_ntoa(clientAddress.sin_addr));
		printf("Receive %d byte(s): %s\n", bytesRead, buf);


		switch(state){
			case 1:
				if(strcmp(buf, "2\0") == 0) {
					// printf("* Menu -> New msg\n");
					strcpy(resp, "Please enter the message: \0");
					state = 2;
				}
				else if(strcmp(buf, "3\0") == 0) {
					if (page_num == 0){
						// printf("No message.\n");
						// printf("* Menu -> Menu\n");
						strcpy(resp, "No message to display.\n1: Menu, 2: New message, 3: Message history\0");
						state = 1;
						page_id = 0;
					}
					else{
						// printf("* Menu -> Read msg\n");
						strcpy(resp, "1: Menu, 2: Page up, 3: Page down");
						int max_disp_id = msg_num < (page_id+1)*MSG_PER_PAGE? msg_num:(page_id+1)*MSG_PER_PAGE;
						for(int i=0; i<max_disp_id; i++){
							printf("%d. %s\n", i, msg_history[i]);

							strcat(resp, "\n");
							char tmp[10];
							sprintf(tmp, "%d", i+1);
							strcat(resp, tmp);
							strcat(resp, ". ");
							strcat(resp, msg_history[i]);
						}
						state = 3;
						page_id = 0;
					}
				}
				else{
					// printf("* Menu -> Menu\n");
					strcpy(resp, "1: Menu, 2: New message, 3: Message history\0");
					state = 1;
				}
				break;
			case 2:
				strcpy(msg_history[msg_num], buf);
				msg_num++;
				page_num = (msg_num - 1) / MSG_PER_PAGE + 1;
				// printf("* New msg -> Menu\n");
				state = 1;
				strcpy(resp, "Done.\n1: Menu, 2: New message, 3: Message history\0");
				break;
			case 3:
				if(strcmp(buf, "1\0") == 0) {
					// printf("* Read msg -> Menu\n");
					strcpy(resp, "Back to menu...\n1: Menu, 2: New message, 3: Message history\0");
					state = 1;
					page_id = 0;
				}
				else if(strcmp(buf, "2\0") == 0) {
					if (page_id > 0){
						page_id--;
						// printf("* Read msg -> Read msg\n");
						strcpy(resp, "1: Menu, 2: Page up, 3: Page down");
						int max_disp_id = msg_num < (page_id+1)*MSG_PER_PAGE? msg_num:(page_id+1)*MSG_PER_PAGE;
						for(int i=page_id*MSG_PER_PAGE; i<max_disp_id; i++){
							printf("%d. %s\n", i, msg_history[i]);

							strcat(resp, "\n");
							char tmp[10];
							sprintf(tmp, "%d", i+1);
							strcat(resp, tmp);
							strcat(resp, ". ");
							strcat(resp, msg_history[i]);
						}
					}
					else{
						// printf("* Read msg -> Read msg\n");
						strcpy(resp, "1: Menu, 2: Page up, 3: Page down");
						int max_disp_id = msg_num < (page_id+1)*MSG_PER_PAGE? msg_num:(page_id+1)*MSG_PER_PAGE;
						for(int i=page_id*MSG_PER_PAGE; i<max_disp_id; i++){
							printf("%d. %s\n", i, msg_history[i]);

							strcat(resp, "\n");
							char tmp[10];
							sprintf(tmp, "%d", i+1);
							strcat(resp, tmp);
							strcat(resp, ". ");
							strcat(resp, msg_history[i]);
						}
					}
				}
				else if(strcmp(buf, "3\0") == 0) {
					if (page_num > page_id+1){
						// printf("* Read msg -> Read msg\n");
						strcpy(resp, "1: Menu, 2: Page up, 3: Page down");
						page_id++;
						int max_disp_id = msg_num < (page_id+1)*MSG_PER_PAGE? msg_num:(page_id+1)*MSG_PER_PAGE;
						for(int i=page_id*MSG_PER_PAGE; i<max_disp_id; i++){
							printf("%d. %s\n", i, msg_history[i]);

							strcat(resp, "\n");
							char tmp[10];
							sprintf(tmp, "%d", i+1);
							strcat(resp, tmp);
							strcat(resp, ". ");
							strcat(resp, msg_history[i]);
						}
					}
					else{
						// printf("* Read msg -> Read msg\n");
						strcpy(resp, "1: Menu, 2: Page up, 3: Page down");
						int max_disp_id = msg_num < (page_id+1)*MSG_PER_PAGE? msg_num:(page_id+1)*MSG_PER_PAGE;
						for(int i=page_id*MSG_PER_PAGE; i<max_disp_id; i++){
							printf("%d. %s\n", i, msg_history[i]);

							strcat(resp, "\n");
							char tmp[10];
							sprintf(tmp, "%d", i+1);
							strcat(resp, tmp);
							strcat(resp, ". ");
							strcat(resp, msg_history[i]);
						}
					}
				}
				else{
					;
				}
				break;
			default:
				// printf("* default -> Menu\n");
				strcpy(resp, "1: Menu, 2: New message, 3: Message history\0");
				state = 1;
		}


		send(clientSocket, resp, strlen(resp), 0);
		closesocket(clientSocket);
	}

	WSACleanup();
	return 0;
}
