#include "jobs.h"

// Cette fonction reprend l'execution d'un job specifie en arriere-plan
int my_bg(char **cmds, t_array_list *arrayl) 
{
    // Verifier si la commande est fournie correctement
    if (!cmds || !cmds[1])
    {
        dprintf(2, "Usage: bg %%job_number\n");
        return 1;
    }

    // Extrait le numero du job a partir de la commande
    int job_number = atoi(cmds[1] + 1); // "+1" pour sauter le caractere '%'
	if (job_number < 0)
	{
		dprintf(2, "job number is too low\n");
		return (1);
	}
    // Trouver le job correspondant dans la liste des jobs
    t_jobs *job = arrayl->array[job_number - 1];

    if (job == NULL)
    {
        dprintf(2, "bg: job %d not found\n", job_number);
        return 1;
    }
    
    if (strcmp(job->status, "Running") == 0)
    {
        dprintf(2, "bg: job %d is already running\n", job_number);
        return 1;
    }

    // Envoie le signal SIGCONT au processus pour le reprendre
    if (kill(-job->pid, SIGCONT) < 0)
    {
        perror("bg: kill(SIGCONT)");
        return 1;
    }

    // Mettre a jour le statut du job
    job->status = "Running";

    return 0;
}