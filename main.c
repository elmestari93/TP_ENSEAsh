#include "enseash.h"


int main(void) {
    char buffer[BUFFER_SIZE];
    int status;
    pid_t pid;
    
    // Variables pour l'état (Q4)
    int has_run_command = 0; 
    int last_code = 0;
    int is_signal = 0;

    // Variables pour le temps (Q5)
    struct timespec start, end;
    long elapsed_ms = 0;

    // Tableau de pointeurs pour stocker les arguments (Q6)
    char *argv[MAX_ARGS];

    print("Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n");

    while (1) {
        // Affichage du prompt
        if (has_run_command == 0) {
            // Premier tour : prompt simple (Q1)
            print("enseash % ");
        } else {
            // Tours suivants : prompt avec infos (Q4)
            print("enseash [");
            if (is_signal) {
                print("sign:");
            } 
            
            else {
                print("exit:");
            }
            print_int(last_code);
            // Affichage du temps (Q5)
            print("|");
            print_int((int)elapsed_ms);
            print("ms] % ");
        }

        process_entry(buffer);

        if (strcmp(buffer, "exit") == 0) { // Commande exit ---> si la valeur du read vaut 0, on sort de la fonction (Q3)
            print("Bye bye...\n");
            exit(EXIT_SUCCESS); 
        }

        // On sépare les commandes et les arguments (Q6)
        argv[0] = strtok(buffer, " "); // strtok coupe la chaine aux espaces (" ") et retourne le premier morceau
        int i = 0;
        // On boucle pour récupérer les morceaux suivants
        while (argv[i] != NULL && i < MAX_ARGS - 1) {
            i++;
            argv[i] = strtok(NULL, " "); // NULL dit à strtok de continuer sur la même chaine
        }
        argv[i] = NULL; // Le tableau d'arguments doit finir par NULL pour execvp
        // Sécurité : Si l'utilisateur a juste fait "Entrée" (argv[0] est vide)
        if (argv[0] == NULL) {
            continue; // On retourne au début de la boucle while
        }

        clock_gettime(CLOCK_MONOTONIC, &start); // On mesure le temps juste avant le fork pour inclure la création du processus (Q5)

        pid = fork(); // Création d'un processus fils car la fonction exec remplace le programme actuel (Q2)
        if (pid == -1) {    
            perror("Fork error");
        }
        else if (pid == 0) {
            // === QUESTION 7 : GESTION DES REDIRECTIONS ===
            // On parcourt les arguments pour trouver '<' ou '>'
            for (int j = 0; argv[j] != NULL; j++) {
                
                // Cas 1 : Redirection de sortie (>)
                if (strcmp(argv[j], ">") == 0) {
                    char *filename = argv[j+1]; // Le fichier est juste après le symbole
                    if (filename == NULL) {
                        perror("No file for redirection");
                        exit(EXIT_FAILURE);
                    }

                    // Ouvrir le fichier : Écriture seule | Créer si existe pas | Tronquer (écraser)
                    // Permissions : RW pour l'utilisateur (S_IRUSR | S_IWUSR)
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
                    close(fd); // On ferme le descripteur original (plus besoin)

                    // On coupe la commande ici pour que execvp ignore le '> fichier'
                    argv[j] = NULL; 
                }
                
                // Cas 2 : Redirection d'entrée (<)
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

                    // On coupe la commande ici
                    argv[j] = NULL;
                }
            }
            // === FIN QUESTION 7 ===
            execvp(argv[0], argv);
            perror("Command execution error"); // Erreur si la fonction exec ne s'est pas exécutée
            exit(EXIT_FAILURE); 
        }
        else {
            wait(&status);
            clock_gettime(CLOCK_MONOTONIC, &end);

            // Calcul du temps écoulé en millisecondes (Q5)
            elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000; // (Différence des secondes * 1000) + (Différence des nanosecondes / 1 000 000)
            
            has_run_command = 1; // On utilisera le prompt avec infos désormais en stockant les valeurs dans des variables reconstruire le prompt au tour suivant (Q4)
            
            if (WIFEXITED(status)) {
                is_signal = 0;
                last_code = WEXITSTATUS(status);
            }
            else if (WIFSIGNALED(status)) {
                is_signal = 1;
                last_code = WTERMSIG(status);
            }
        }
    }
    return 0;
}