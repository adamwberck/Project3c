
#include <unistd.h>
#include "helper.h"
#include "my_server.h"

const char*  DEFAULT_DICTIONARY = "words.txt";
int main(int argc, char* argv[]) {
    printf("Hello, World!\n");
    const char* dictionary = argc == 1 ? DEFAULT_DICTIONARY : argv[1];
    FILE *file ;
    file = fopen(dictionary, "r");
    //count lines
    int line_count = count_file_lines(file);
    fclose(file);

    //read lines into word array
    char* words[line_count];
    file = fopen(dictionary, "r");
    read_file_as_array(words,file);
    fclose(file);

    //
    //sockaddr_in holds information about the user connection.
    //We don't need it, but it needs to be passed into accept().
    struct sockaddr_in client;
    int clientLen = sizeof(client);
    int connection_socket, client_socket;
    ssize_t bytes_returned;
    char recv_buffer[BUF_SIZE];
    recv_buffer[0] = '\0';

    int connection_port = 123313;

    connection_socket = open_listen_fd(connection_port);
    if((client_socket = accept(connection_socket, (struct sockaddr*)&client, &clientLen)) == -1){
        printf("Error connecting to client.\n");
        return -1;
    }
    printf("Connection success!\n");
    char* clientMessage = "Hello! I hope you can see this.\n";
    char* msgRequest = "Send me some text and I'll respond with something interesting!\nSend the escape key to close the connection.\n";
    char* msgResponse = "I actually don't have anything interesting to say...but I know you sent ";
    char* msgPrompt = ">>>";
    char* msgError = "I didn't get your message. ):\n";
    char* msgClose = "Goodbye!\n";

    send(client_socket, clientMessage,strlen(clientMessage),0);
    send(client_socket, msgRequest,strlen(msgRequest),0);

    while(1){
        send(client_socket, msgPrompt,strlen(msgPrompt),0);
        bytes_returned = recv(client_socket,recv_buffer,BUFSIZ,0);

        if(bytes_returned == -1){
            send(client_socket,msgError,strlen(msgError),0);
        } else if(recv_buffer[0]==27){
            send(client_socket, msgClose, strlen(msgClose), 0);
            close(client_socket);
            break;
        }
        else{
            send(client_socket, msgResponse, strlen(msgResponse), 0);
            send(client_socket, recv_buffer, (size_t) bytes_returned, 0);
        }
    }
    return 0;
}