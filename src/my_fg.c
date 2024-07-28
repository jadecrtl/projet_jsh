#include "jobs.h"

int my_fg(char **cmds, t_array_list *arrayl)
{
	int pos = 0;

	if (cmds[1] != NULL)
        pos = atoi(&cmds[1][1]);
    else 
	{
        dprintf(2, "job number erreur\n");
		return (1);
	}
	if (pos < 1 || pos > arrayl->size || arrayl->array[pos - 1] == NULL) 
	{
        dprintf(2, "Invalid job number\n");
        return (1);
    }
	pid_t job_pid = getpgid(arrayl->array[pos - 1]->pid);
    pid_t shell_pgid = getpgrp();

	if (tcsetpgrp(STDIN_FILENO, job_pid) == -1) 
	{
        perror("tcsetpgrp with job_pgid");
        return (1);
    }
	if (kill(-job_pid, SIGCONT) < 0) 
	{
        perror("erreur kill");
        return (1);
    }

	int val;
	while (1)
	{
		int code = waitpid(-job_pid, &val, WUNTRACED | WCONTINUED);
		if (code == -1) 
		{
			if (errno == ECHILD)
				return (1);
			else
				perror("waitpid failed"); 
		}
		else if (code > 0)
		{
			if (WIFEXITED(val) || WIFSIGNALED(val)) 
			{
				int exit = WEXITSTATUS(val);
				t_jobs *job = arrayl->array[pos - 1];
				free(job->cmd);
				free(job);
				for (int i = pos - 1; i < arrayl->size - 1; i++) 
				{
					arrayl->array[i] = arrayl->array[i + 1];
				}
				arrayl->array[arrayl->size - 1] = NULL;
				arrayl->size--;

				return exit;
				
            } 
			else if (WIFSTOPPED(val)) 
			{
                // Le job est stoppé à nouveau
                dprintf(2, "[%d] %d Stopped %s\n", pos, arrayl->array[pos - 1]->pid, arrayl->array[pos - 1]->cmd);
                break;
            }
		}
		if (tcsetpgrp(STDIN_FILENO, shell_pgid) == -1)
		{
			perror("tcsetpgrp with shell_pgid");
			return (1);
   		}
	}
	return (0);
}