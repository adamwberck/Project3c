
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include "helper.h"
#include "my_server.h"
#include "my_sync_queue.h"

struct args{
    char **words;
    struct my_sync_queue *queue;
    int test;
};
struct my_sync_queue socket_queue;
const char*  DEFAULT_DICTIONARY = "words.txt";
void *work(void* arguments);
int main(int argc, char* argv[]) {

    const char* dictionary = argc <= 2 ? DEFAULT_DICTIONARY : argv[2];
    FILE *file ;
    file = fopen(dictionary, "r");
    int line_count = count_file_lines(file);
    char **words = malloc(sizeof(char**)*(line_count+1));
    fclose(file);

    //read lines into word array
    file = fopen(dictionary, "r");
    read_file_as_array(&words,file);
    fclose(file);
    int i=0;
    while(words[i]!=NULL) {
        printf("1 %s\n", words[i++]);
    }
    socket_queue = create_sync_queue();

    //Pthread
    pthread_t worker1;
    struct args arguments;
    arguments.queue = &socket_queue;
    arguments.words = words;
    pthread_create(&worker1,NULL,work,&arguments);




    //sockaddr_in holds information about the user connection.
    //We don't need it, but it needs to be passed into accept().
    struct sockaddr_in client;
    int clientLen = sizeof(client);
    int connection_socket, client_socket;
    ssize_t bytes_returned;
    char recv_buffer[BUF_SIZE];
    recv_buffer[0] = '\0';

    int connection_port = (int) (argc < 2 ? 12313 : strtol(argv[1],NULL,10));
    connection_socket = open_listen_fd(connection_port);
    client_socket = accept(connection_socket, (struct sockaddr*)&client, &clientLen);
    if(client_socket == -1){
        printf("Error connecting to client.\n");
        return -1;
    }
    add_sq(&socket_queue,client_socket);


    printf("Connection success!\n");
    char* client_message = "Hello! I hope you can see this.\n";
    char* msg_request = "I'll spell check entered words\n";
    char* msg_response = "Word: ";
    char* msg_prompt = ">>>";
    char* msg_error = "I didn't get your message. ):\n";
    char* msg_close = "Goodbye!\n";

    send(client_socket, client_message,strlen(client_message),0);
    send(client_socket, msg_request,strlen(msg_request),0);


    while(1){
        send(client_socket, msg_prompt,strlen(msg_prompt),0);
        bytes_returned = recv(client_socket,recv_buffer,BUFSIZ,0);

        if(bytes_returned == -1){
            send(client_socket,msg_error,strlen(msg_error),0);
        } else if(recv_buffer[0]==27){
            send(client_socket, msg_close, strlen(msg_close), 0);
            close(client_socket);
            break;
        }
        else{
            //PROCESS INPUT
            char* input = strdup(recv_buffer);
            remove_newline_char(&input);
            send(client_socket, msg_response, strlen(msg_response), 0);
            send(client_socket, input, (size_t) strlen(input), 0);
            bool correct  =  find_in_array(words,input);
            const char* correct_str =  correct ? " OK\n" : " NOT OK\n";
            send(client_socket, correct_str, strlen(correct_str), 0);
        }
    }
    return 0;
}

void *work(void* arguments){

}