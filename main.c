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
        // Affichage du prompt (Q1, Q4, Q5)
        display_prompt(has_run_command, is_signal, last_code, elapsed_ms);

        process_entry(buffer);

        if (strcmp(buffer, "exit") == 0) { // Commande exit (Q3)
            print("Bye bye...\n");
            exit(EXIT_SUCCESS); 
        }

        // On sépare les commandes et les arguments (Q6)
        parse_command(buffer, argv);

        // Sécurité : Si l'utilisateur a juste fait "Entrée" (argv[0] est vide)
        if (argv[0] == NULL) {
            continue; // On retourne au début de la boucle while
        }

        clock_gettime(CLOCK_MONOTONIC, &start); // Mesure du temps avant fork (Q5)

        pid = fork(); // Création du processus fils (Q2)
        if (pid == -1) {    
            perror("Fork error");
        }
        
        else if (pid == 0) {
            // === QUESTION 7 : GESTION DES REDIRECTIONS ===
            handle_redirections(argv);
            
            // Exécution de la commande
            execvp(argv[0], argv);
            perror("Command execution error"); // Si exec échoue
            exit(EXIT_FAILURE); 
        }

        else {
            wait(&status);
            clock_gettime(CLOCK_MONOTONIC, &end);

            // Calcul du temps écoulé (Q5)
            elapsed_ms = calculate_time(start, end);
            
            has_run_command = 1; // Le prompt affichera les infos au prochain tour (Q4)
            
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