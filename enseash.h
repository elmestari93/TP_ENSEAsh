#ifndef ENSEASH_H
#define ENSEASH_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h> 
#include <sys/wait.h>


#define BUFFER_SIZE 1024 // Define constant for buffer size to avoid magic numbers
#define MAX_PROMPT_SIZE 128 

// Prototypes
void print(const char* str);
int process_entry(char* buffer);

#endif