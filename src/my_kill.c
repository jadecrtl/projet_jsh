#include "jsh.h"
#include "jobs.h"

int my_kill(char **cmds, t_array_list *arrayl)
{
	int signal = SIGTERM; // Signal par défaut
	pid_t pid;
	int i = 1;

	// si signal spécifier
	if (cmds[1] && cmds[1][0] == '-')
	{
		signal = atoi(&cmds[1][1]); // recupère le numéro du signal
		i++;
	}

	if (!cmds[i])
	{
		dprintf(2, "kill: usage: kill [-sig] pid |  %%job\n'");
		return 1;
	}

	// Traiter les PID et les jobs
	for (; cmds[i]; i++)
	{
		if (cmds[i][0] == '%')
		{
			// Traiter un job
			int job_num = atoi(&cmds[i][1]);
			if (job_num < 0)
			{
				dprintf(2, "job number too low\n");
				return (1);
			}
			t_jobs *job = arrayl->array[job_num - 1];
			if (job && job->pid > 0)
			{
				if (kill(-job->pid, signal) < 0)
				{
					perror("kill");
					return 1;
				}
			}
			else
			{
				dprintf(2, "kill: %s: no such job\n", cmds[i]);
				return 1;
			}
		}
		else
		{
			// Traiter un PID
			pid = atoi(cmds[i]);
			if (kill(-pid, signal) < 0)
			{
				perror("kill");
				return 1;
			}
		}
	}

	return 0;
}
