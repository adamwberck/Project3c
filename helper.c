//
// Created by mandr on 2019-03-28.
//

#include "helper.h"

//finds newline char and replaces it with null terminator
void remove_newline_char(char **str){
    char *s = *str;//get pointer to char
    int i=0;
    //loop through string
    while(s[i]!='\0'){//stop looping when you get to null terminator
        //replace newline with null terminator
        if(s[i]=='\n'||s[i]=='\r'){
            s[i]='\0';
        }
        i++;
    }
}

int count_file_lines(FILE *file) {
    char line[256];
    int i=0;
    while(fgets(line, sizeof(line), file)){//keep going unless end of file
        i++;
    }
    return i;
}

bool find_in_array(char **array,char *element){
    int i=0;
    while(array[i]!=NULL) {
        if (strcmp(array[i], element) == 0) {
            return true;
        }
        i++;
    }
    return false;
}

void read_file_as_array(char ***p_array, FILE *file) {
    char** array = *p_array;
    char line[256];
    int i=0;
    while(fgets(line, sizeof(line), file)){//keep going unless end of file
        char *input;
        input = malloc(sizeof(char) * (strlen(line)+1));
        sprintf(input,"%s", line);
        remove_newline_char(&input);
        array[i++] = strdup(input);
    }
    array[i]= NULL;
}

int open_listen_fd(int port) {
    int listenfd, optval=1;
    struct sockaddr_in server_address;

    if((listenfd= socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return -1;
    }
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)) < 0){
        return -1;
    }

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((unsigned short)port);
    if((bind(listenfd,(struct sockaddr*)&server_address,sizeof(server_address)) < 0)){
        return -1;
    }

    if(listen(listenfd, 20) < 0){
        return -1;
    }

    return listenfd;
}