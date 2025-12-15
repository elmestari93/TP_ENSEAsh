#include "enseash.h"

int main(void) {
    char buffer[BUFFER_SIZE];
    int status;
    pid_t pid;

    print("Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n");

    while (1) {
        print("enseash % ");

        process_entry(buffer);

        pid = fork(); // Création d'un processus fils car la fonction exec remplace le programme actuel
        
        if (pid == -1) {
            perror("Fork error");
        }
        
        else if (pid == 0) {
            execlp(buffer, buffer, (char *)NULL);
            perror("Command execution error"); // Erreur si la fonction exec ne s'est pas exécutée
            exit(EXIT_FAILURE); 
        }
        else {
            wait(&status);
        }

        if (strcmp(buffer, "exit") == 0) { // Commande exit ---> si la valeur du read vaut 0, on sort de la fonction
        print("Bye bye...\n");
        exit(EXIT_SUCCESS); 
        }
    }
    return 0;
}