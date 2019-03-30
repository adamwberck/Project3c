
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include "helper.h"
#include "my_server.h"
#include "my_sync_queue.h"

const char* client_message = "Hello! I hope you can see this.\n";
const char* msg_request = "I'll spell check entered words\n";
const char* msg_response = "Word: ";
const char* msg_prompt = ">>>";
const char* msg_error = "I didn't get your message. ):\n";
const char* msg_close = "Goodbye!\n";




struct my_sync_queue socket_queue;
char **words;
int line_count;
bool words_in_order;
const char*  DEFAULT_DICTIONARY = "words.txt";
void *work();

int main(int argc, char* argv[]) {

    const char* dictionary = argc <= 2 ? DEFAULT_DICTIONARY : argv[2];
    FILE *file ;
    file = fopen(dictionary, "r");
    line_count = count_file_lines(file);
    words = malloc(sizeof(char**) * (line_count + 1));
    fclose(file);

    //read lines into word array
    file = fopen(dictionary, "r");
    read_file_as_array(&words,file);
    fclose(file);
    words_in_order = array_in_order(words);

    socket_queue = create_sync_queue();

    //Pthread
    pthread_t worker1;
    pthread_create(&worker1,NULL,work,NULL);



    //sockaddr_in holds information about the user connection.
    //We don't need it, but it needs to be passed into accept().
    struct sockaddr_in client;


    int connection_port = (int) (argc < 2 ? 12313 : strtol(argv[1],NULL,10));
    int clientLen = sizeof(client);
    int connection_socket = open_listen_fd(connection_port);
    while(1) {
        int client_socket = accept(connection_socket, (struct sockaddr *) &client, &clientLen);
        if (client_socket == -1) {
            printf("Error connecting to client.\n");
            return -1;
        }
        add_sq(&socket_queue, client_socket);
    }

    return 0;
}

void *work() {
    printf("hi worker thread here\n");
    int m = 0;
    while (m < 100) {
        int client_socket = remove_sq(&socket_queue);
        while (1) {
            ssize_t bytes_returned;
            char rec_buffer[BUF_SIZE];
            rec_buffer[0] = '\0';
            send(client_socket, msg_prompt, strlen(msg_prompt), 0);
            bytes_returned = recv(client_socket, rec_buffer, BUFSIZ, 0);
            if (bytes_returned == -1) {
                send(client_socket, msg_error, strlen(msg_error), 0);
            } else if (rec_buffer[0] == 27) {
                send(client_socket, msg_close, strlen(msg_close), 0);
                close(client_socket);
                break;
            } else {
                //PROCESS INPUT
                char *input = strdup(rec_buffer);
                remove_newline_char(&input);
                send(client_socket, msg_response, strlen(msg_response), 0);
                send(client_socket, input, (size_t) strlen(input), 0);
                bool correct = find_in_array(words, input,words_in_order,line_count);
                const char *correct_str = correct ? " OK\n" : " MISSPELLED\n";
                send(client_socket, correct_str, strlen(correct_str), 0);
            }
        }
        m++;
    }
    return NULL;
}