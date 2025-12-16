#ifndef ENSEASH_H
#define ENSEASH_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>


#define BUFFER_SIZE 1024 // Define constant for buffer size to avoid magic numbers
#define MAX_ARGS 64 // Define constant for argv max size

void print(const char* str);
void print_int(int n);
int process_entry(char* buffer);

#endif