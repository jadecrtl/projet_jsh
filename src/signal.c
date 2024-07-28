#include "jsh.h"
#include "jobs.h"

void signal_handler(int sig) {
    switch(sig) {
        case SIGINT:
            // Action pour SIGINT
            break;
        case SIGTERM:
            break;
        case SIGQUIT:
            // Action pour SIGQUIT
            break;
        case SIGTSTP:
            // Action pour SIGTSTP
            break;
        // Ajoutez d'autres cas si nécessaire
        default:
            break;
    }
}
void    init_signal(void)
{
    struct sigaction action = {0};
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    // Définit le gestionnaire de signaux pour les signaux spécifiés
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGTSTP, &action, NULL);

    // Conservez l'ignorance de SIGTTIN et SIGTTOU
    action.sa_handler = SIG_IGN;
    sigaction(SIGTTIN, &action, NULL);
    sigaction(SIGTTOU, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

void	restore_signal(void)
{
	struct sigaction action = {0};
	action.sa_handler = SIG_DFL;
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
}
