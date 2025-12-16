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
#define MAX_ARGS 64 // Define constant for argv max size to avoid magic numbers

// Fonctions d'affichage basiques
void print(const char* str);
void print_int(int n);

// Fonctions logique métier
int process_entry(char* buffer);
void display_prompt(int has_run, int is_signal, int last_code, long elapsed_ms);
void parse_command(char* buffer, char** argv);
long calculate_time(struct timespec start, struct timespec end);
void handle_redirections(char** argv);

#endif