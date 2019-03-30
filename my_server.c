
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include "helper.h"
#include "my_server.h"
#include "my_int_sync_queue.h"
#include "my_str_sync_queue.h"
#define THREADS 3

const char* client_message = "Hello! I hope you can see this.\n";
const char* msg_request = "I'll spell check entered words\n";
const char* msg_response = "Word: ";
const char* msg_prompt = ">>>";
const char* msg_error = "I didn't get your message. ):\n";
const char* msg_close = "Goodbye!\n";


bool run_threads =true;

struct my_int_sync_queue socket_queue;
struct my_str_sync_queue logger_queue;
char **words;
int line_count;
bool words_in_order;
const char*  DEFAULT_DICTIONARY = "words.txt";
void *work();
void* my_log();

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

    //socket_queue = create_int_sync_queue();

    //Pthread
    logger_queue = create_str_sync_queue();
    pthread_t logger;
    pthread_create(&logger, NULL, my_log, NULL);
    pthread_t workers[THREADS];
    for(int i=0;i<THREADS;i++){
        pthread_create(&workers[i], NULL, work, NULL);
    }

    //sockaddr_in holds information about the user connection.
    //We don't need it, but it needs to be passed into accept().
    struct sockaddr_in client;


    int connection_port = (int) (argc < 2 ? 12313 : strtol(argv[1],NULL,10));
    int clientLen = sizeof(client);
    int connection_socket = open_listen_fd(connection_port);
    while(run_threads) {
        int client_socket = accept(connection_socket, (struct sockaddr *) &client, &clientLen);
        if (client_socket == -1) {
            printf("Error connecting to client.\n");
            run_threads=false;
        }
        add_isq(&socket_queue, client_socket);
    }
    return 0;
}

void *work() {
    int m = 0;
    while (run_threads) {
        int client_socket = remove_isq(&socket_queue);
        bool connected=true;
        while (connected) {
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
                connected=false;
            } else {
                //PROCESS INPUT
                char *input = strdup(rec_buffer);
                remove_newline_char(&input);
                bool correct = find_in_array(words, input,words_in_order,line_count);
                const char *correct_str = correct ? " OK\n" : " MISSPELLED\n";

                send(client_socket, input, (size_t) strlen(input), 0);
                add_ssq(&logger_queue,input);
                send(client_socket, correct_str, strlen(correct_str), 0);
                add_ssq(&logger_queue, (char *) correct_str);
            }
        }
        m++;
    }
    return NULL;
}

void* my_log(){
    FILE* log_file = fopen("log.txt","w+");
    while(run_threads) {
        char *string = remove_ssq(&logger_queue);
        printf("about to log %s\n",string);
        fprintf(log_file, "%s", string);
    }
    fclose(log_file);
    return NULL;
}