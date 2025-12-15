#include "enseash.h"

// Creating a print function
void print(const char* str) {
    if (write(STDOUT_FILENO, str, strlen(str)) == -1) {
        perror("write error");
        exit(EXIT_FAILURE);
    }
}

int process_entry(char* buffer) {
    ssize_t ret = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);

    if (ret == 0) { // Gestion du Ctrl+D ---> si la valeur du read vaut 0, on sort de la fonction
        print("\nBye bye...\n");
        exit(EXIT_SUCCESS);
    }
    
    else if (ret < 0) {
        perror("Read error");
        exit(EXIT_FAILURE);
    }

    // Remplacement du saut de ligne par \0 car si on a un \n à la fin d'une commande, on peut avoir des erreurs dues au fait que l'utilisateur appuye sur entrée à la fin de sa commande
    if (ret > 0 && buffer[ret - 1] == '\n') {
        buffer[ret - 1] = '\0';
    }

    else {
        buffer[ret] = '\0';
    }

    return ret;
}