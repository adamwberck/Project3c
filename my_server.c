#include <stdio.h>
#include "helper.h"
const char*  DEFAULT_DICTIONARY = "words.txt";
const int BUFSIZE = 512;
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

    //test array
    for(int i=0;i<line_count;i++){
        printf("%s\n",words[i]);
    }
}