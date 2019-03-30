#include "my_server.h"
#define BUF_SIZE 512
#define THREADS 2
#define DEFAULT_DICTIONARY "words.txt"
#define msg_prompt ">>>"
#define msg_error "Error message not received\n"
#define msg_close "End Connection!\n"


bool run_threads =true;

struct my_int_sync_queue socket_queue;
struct my_str_sync_queue logger_queue;
char **words;
int word_count;
bool words_in_order;
void *work();
void* my_log();

int main(int argc, char* argv[]) {
    //load dictionary as 2nd argument or use default dictionary
    const char* dictionary = argc <= 2 ? DEFAULT_DICTIONARY : argv[2];
    //check if dictionary exists
    if(access(dictionary,F_OK) == -1){
        printf("Could not find %s \n",dictionary);
        return 1;
    }
    //open dictionary file and count lines
    FILE *dictionary_file ;
    dictionary_file = fopen(dictionary, "r");
    word_count = count_file_lines(dictionary_file);

    //allocate space for the char* pointers equal to the number of words plus the NULL terminator
    words = malloc(sizeof(char**) * (word_count + 1));

    //read lines into word array
    fseek(dictionary_file,0,SEEK_SET);//fseek() file back to beginning
    read_file_as_array(&words,dictionary_file);
    fclose(dictionary_file);

    //check if words are in order for binary search later
    words_in_order = array_in_order(words);

    //Pthread
    //create the logger thread and queue
    logger_queue = create_str_sync_queue();
    pthread_t logger;
    pthread_create(&logger, NULL, my_log, NULL);

    //create the worker threads and socket queue
    socket_queue = create_int_sync_queue();
    pthread_t workers[THREADS];
    for(int i=0;i<THREADS;i++){
        pthread_create(&workers[i], NULL, work, NULL);
    }

    //Setup connection
    struct sockaddr_in client;
    int connection_port = (int) (argc < 2 ? 12313 : strtol(argv[1],NULL,10));
    unsigned int client_len = sizeof(client);
    int connection_socket = open_listen_fd(connection_port);

    //main thread is now in charge of placing threads in queue
    printf("Server Started; Now Accepting Connections.\n");
    while(run_threads) {
        //accept gets the socket
        int client_socket = accept(connection_socket, (struct sockaddr *) &client, &client_len);
        if (client_socket == -1) {
            printf("Error connecting to client.\n");
            run_threads = false;
        }
        //add client to the queue
        add_isq(&socket_queue, client_socket);
    }
    return 0;
}

void *work() {
    while (run_threads) {
        //get client socket from queue
        int client_socket = remove_isq(&socket_queue);
        bool connected=true;
        while (connected) {
            //get bytes from client
            ssize_t bytes_returned;
            char rec_buffer[BUF_SIZE];
            rec_buffer[0] = '\0';
            send(client_socket, msg_prompt, strlen(msg_prompt), 0);
            bytes_returned = recv(client_socket, rec_buffer, BUFSIZ, 0);
            //if error send error
            if (bytes_returned == -1) {
                send(client_socket, msg_error, strlen(msg_error), 0);
            //if escape key send close connection and get a new client from queue
            } else if (rec_buffer[0] == 27) {
                send(client_socket, msg_close, strlen(msg_close), 0);
                close(client_socket);
                connected=false;
            } else {
                //PROCESS INPUT
                char *input = strdup(rec_buffer);//duplicate buffer into input
                remove_newline_char(&input);//remove the new line character

                //check spelling
                bool correct = find_in_array(words, input,words_in_order,word_count);
                const char *correct_str = correct ? " OK\n" : " MISSPELLED\n";

                //send input and spelling result back to client and into log queue
                send(client_socket, input, (size_t) strlen(input), 0);
                add_ssq(&logger_queue,input);
                send(client_socket, correct_str, strlen(correct_str), 0);
                add_ssq(&logger_queue, (char *) correct_str);
            }
        }
    }
    return NULL;
}

void* my_log(){
    //create log.txt file
    FILE* log_file = fopen("log.txt","w+");
    fclose(log_file);
    while(run_threads) {
        log_file = fopen("log.txt","a"); //open file in append mode
        char *string = remove_ssq(&logger_queue); //get string from log queue
        fprintf(log_file, "%s", string);//print into log file
        fclose(log_file);//close and update
    }
    return NULL;
}