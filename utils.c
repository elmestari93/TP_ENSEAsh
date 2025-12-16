#include "enseash.h"

// Création d'une fonction print (Q1)
void print(const char* str) {
    if (write(STDOUT_FILENO, str, strlen(str)) == -1) {
        perror("write error");
        exit(EXIT_FAILURE);
    }
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

    // Remplacement du saut de ligne par \0 car si on a un \n à la fin d'une commande, on peut avoir des erreurs (Q2)
    if (ret > 0 && buffer[ret - 1] == '\n') {
        buffer[ret - 1] = '\0';
    } else {
        buffer[ret] = '\0';
    }

    return ret;
}

// Gère l'affichage du prompt complexe (Q4 & Q5)
void display_prompt(int has_run, int is_signal, int last_code, long elapsed_ms) {
    if (has_run == 0) {
        // Premier tour : prompt simple (Q1)
        print("enseash % ");
    } else {
        // Tours suivants : prompt avec infos (Q4)
        print("enseash [");
        if (is_signal) {
            print("sign:");
        } else {
            print("exit:");
        }
        print_int(last_code);
        // Affichage du temps (Q5)
        print("|");
        print_int((int)elapsed_ms);
        print("ms] % ");
    }
}

// Découpe la commande en arguments (Q6)
void parse_command(char* buffer, char** argv) {
    argv[0] = strtok(buffer, " "); // strtok coupe la chaine aux espaces
    int i = 0;
    // On boucle pour récupérer les morceaux suivants
    while (argv[i] != NULL && i < MAX_ARGS - 1) {
        i++;
        argv[i] = strtok(NULL, " "); // NULL dit à strtok de continuer sur la même chaine
    }
    argv[i] = NULL; // Le tableau d'arguments doit finir par NULL pour execvp
}

// Calcul du temps écoulé en millisecondes (Q5)
long calculate_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
}

// Gestion des redirections < et > (Q7)
void handle_redirections(char** argv) {
    // On parcourt les arguments pour trouver '<' ou '>'
    for (int j = 0; argv[j] != NULL; j++) {
        
        // Redirection de sortie (>)
        if (strcmp(argv[j], ">") == 0) {
            char *filename = argv[j+1]; // Le fichier est juste après le symbole
            if (filename == NULL) {
                perror("No file for redirection");
                exit(EXIT_FAILURE);
            }

            // Ouvrir le fichier : Écriture seule | Créer | Tronquer
            // Permissions : RW pour l'utilisateur
            int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (fd == -1) {
                perror("Open error");
                exit(EXIT_FAILURE);
            }

            // Rediriger STDOUT vers ce fichier
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("Dup2 error");
                exit(EXIT_FAILURE);
            }
            close(fd); 

            // On coupe la commande ici pour que execvp ignore le '> fichier'
            argv[j] = NULL; 
        }
        
        // Redirection d'entrée (<)
        else if (strcmp(argv[j], "<") == 0) {
            char *filename = argv[j+1];
            if (filename == NULL) {
                perror("No file for redirection");
                exit(EXIT_FAILURE);
            }

            // Ouvrir en lecture seule
            int fd = open(filename, O_RDONLY);
            if (fd == -1) {
                perror("Open error");
                exit(EXIT_FAILURE);
            }

            // Rediriger STDIN vers ce fichier
            if (dup2(fd, STDIN_FILENO) == -1) {
                perror("Dup2 error");
                exit(EXIT_FAILURE);
            }
            close(fd);
            argv[j] = NULL; // On coupe la commande ici
        }
    }
}