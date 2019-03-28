//
// Created by mandr on 2019-03-28.
//

#ifndef PROJECT3C_HELPER_H
#define PROJECT3C_HELPER_H

#include <stdio.h>

void remove_newline_char(char **str);
void read_file_as_array(char **array, FILE *file);
int count_file_lines(FILE *file);
int open_listen_fd(int port);
#endif //PROJECT3C_HELPER_H
