#include "enseash.h"

// Creating a print function (Q1)
void print(const char* str) {
    if (write(STDOUT_FILENO, str, strlen(str)) == -1) {
        perror("write error");
        exit(EXIT_FAILURE);
    }
}

int process_entry(char* buffer) {
    ssize_t ret = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);

    if (ret == 0) { // Gestion du Ctrl+D ---> si la valeur du read vaut 0, on sort de la fonction (Q3)
        print("\nBye bye...\n");
        exit(EXIT_SUCCESS);
    }
    
    else if (ret < 0) {
        perror("Read error");
        exit(EXIT_FAILURE);
    }

    // Remplacement du saut de ligne par \0 car si on a un \n à la fin d'une commande, on peut avoir des erreurs dues au fait que l'utilisateur appuye sur entrée à la fin de sa commande (Q2)
    if (ret > 0 && buffer[ret - 1] == '\n') {
        buffer[ret - 1] = '\0';
    }

    else {
        buffer[ret] = '\0';
    }

    return ret;
}

// Affiche un entier en utilisant print (Q4)
void print_int(int n) {
    char buffer[12]; // Assez grand pour stocker un int max
    int i = 0;
    
    // Cas spécial pour 0
    if (n == 0) {
        print("0");
        return;
    }

    // Extraction des chiffres (à l'envers)
    int temp = n;
    while (temp > 0) {
        buffer[i++] = (temp % 10) + '0'; // Convertit chiffre en caractère ASCII
        temp /= 10;
    }
    buffer[i] = '\0'; // Fin de chaine

    // On doit inverser la chaine car on a extrait les unités en premier
    char reversed[12];
    int j;
    for (j = 0; j < i; j++) {
        reversed[j] = buffer[i - 1 - j];
    }
    reversed[i] = '\0';

    print(reversed);
}