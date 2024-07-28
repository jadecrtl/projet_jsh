#include "jobs.h"

char **create_cmd_for_jobs(char **cmds)
{
    int total_cmds = 0;
    while (cmds[total_cmds])
    {
        total_cmds++;
    }

    // Si le dernier argument est "&", réduire la taille du tableau de 1
    if (total_cmds > 0 && strcmp(cmds[total_cmds - 1], "&") == 0)
    {
        total_cmds--;
    }

    // Allouer de la mémoire pour le nouveau tableau de chaînes
    char **res = calloc(total_cmds + 1, sizeof(char *));
    if (!res)
    {
        perror("Erreur d'allocation mémoire");
        return NULL;
    }

    // Copier chaque commande dans le nouveau tableau
    for (int i = 0; i < total_cmds; i++)
    {
        res[i] = strdup(cmds[i]);
        if (!res[i])
        {
            // Nettoyer en cas d'échec d'allocation
            for (int j = 0; j < i; j++)
            {
                free(res[j]);
            }
            free(res);
            perror("Erreur d'allocation mémoire");
            return NULL;
        }
        
    }

    // Le dernier élément du tableau doit être NULL
    res[total_cmds] = NULL;
    return res;
}

char	**create_cmd_for_background(char **cmds)
{
	char **res;
	int	size;

	size = get_tab_size(cmds) - 1;
	res = calloc(size + 1, sizeof(char *));
	if (!res)
		return (free_tab(cmds), res);
	for (int i = 0; i < size; i++)
	{
		res[i] = strdup(cmds[i]);
		if (!res[i])
		{
			free_tab(cmds);
			free_tab(res);
			return (NULL);
		}
	}
	return (free_tab(cmds), res);
}

t_array_list	*init_array_list()
{
	t_array_list	*arrayl = calloc(1, sizeof(t_array_list));
	if (!arrayl)
		return (NULL);
	arrayl->max = 5;
	arrayl->size = 0;
	arrayl->array = calloc(arrayl->max, sizeof(t_jobs *));
	if (!arrayl->array)
		return (free(arrayl), NULL);
	return (arrayl);
}

int	get_index_in_arrayl(t_array_list *arrayl)
{	
	int	i = 0;

	for (; (i < arrayl->max) && arrayl->array[i] && arrayl->array[i]->pid != -1; i++);
	return (i);
}

t_jobs	**double_size_arrayl(t_array_list *arrayl)
{
	t_jobs **new_array = calloc(arrayl->max * 2, sizeof(t_jobs *));
	if (!new_array)
		return (free(arrayl->array), NULL);
	for (int i = 0; i < arrayl->max; i++)
		new_array[i] = arrayl->array[i];
	arrayl->max *= 2;
	return (free(arrayl->array), new_array);	
}

int	add_in_jobs_list(t_array_list *arrayl, t_jobs *to_add)
{
	int index = get_index_in_arrayl(arrayl);
	if ((index + 1) == arrayl->max)
	{
		arrayl->array = double_size_arrayl(arrayl);
		if (!arrayl->array)
			return (0);
		arrayl->array[index] = to_add;
		arrayl->size += 1;
		return (1);
	}
	arrayl->array[index] = to_add;
	arrayl->size += 1;
	return (1);
}

t_jobs    *create_jobs(pid_t pid, char **cmds, t_array_list *arrayl)
{
    t_jobs *job = calloc(1, sizeof(t_jobs));
    if (!job) return (NULL);

    int length = 1;
    for (int i = 0; cmds[i] != NULL; i++)
        length += strlen(cmds[i]) + 1;

    char *joinedCmd = malloc(length);
    if (!joinedCmd)
        return (free(job), perror("erreur malloc "), NULL);

    joinedCmd[0] = '\0'; // Initialise la chaîne vide
    for (int i = 0; cmds[i] != NULL; i++)
    {
        strcat(joinedCmd, cmds[i]);
        if (cmds[i + 1] != NULL)
            strcat(joinedCmd, " "); // Ajoute un espace entre les mots
    }
    job->pid = pid;
    job->cmd = joinedCmd;
    job->status = NULL;
    if (!job->cmd)
        return (free(job), NULL);
    job->index = get_index_in_arrayl(arrayl);
    return (job);
}

int	my_jobs(t_array_list *arrayl)
{
	if (!arrayl)
	{
		dprintf(2, "jobs Not existing\n");
		return (1);
	}
	for (int i = 0; i < arrayl->max; i++)
	{
		t_jobs *job = arrayl->array[i];
		if (job && job->pid != -1)
			dprintf(1, "[%d]	%d	%s %s\n", job->index, job->pid, job->status, job->cmd);
	}
	return (0);
}

void update_jobs(t_array_list *arrayl, int sortie)
{
	int status = 0;

	for (int i = 0; i < arrayl->max; i++)
	{
		if (!arrayl->array[i] || arrayl->array[i]->pid == -1)
			continue;
		int code = waitpid(arrayl->array[i]->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
		if (code == -1) {
			if (errno == ECHILD) {
				arrayl->array[i]->status = "Done";
			} else {
				perror("waitpid failed");
			}
		}
		else if (code > 0)
		{
			if (WIFSTOPPED(status)) {
				arrayl->array[i]->status = "Stopped";
				dprintf(2,"[%d]    %d    %s %s\n", arrayl->array[i]->index, arrayl->array[i]->pid, arrayl->array[i]->status, arrayl->array[i]->cmd);
			} else if (WIFSIGNALED(status)) {
				arrayl->array[i]->status = "Killed";
			} else if (WIFEXITED(status)) {
				arrayl->array[i]->status = "Done";
			} else if (WIFCONTINUED(status)) {
				arrayl->array[i]->status = "Running";
				dprintf(2,"[%d]	%d	%s %s\n", arrayl->array[i]->index, arrayl->array[i]->pid, arrayl->array[i]->status, arrayl->array[i]->cmd);
			}
		}
		else if (code == 0) {
			if (!arrayl->array[i]->status)
				arrayl->array[i]->status = "Running";
		}
		if (!strcmp(arrayl->array[i]->status, "Done")
			|| !strcmp(arrayl->array[i]->status, "Killed"))
		{
			t_jobs *job = arrayl->array[i];
			// si sortie = 1, jobs appelle donc std_out
			if (sortie)
				dprintf(1, "[%d]	%d	%s	%s\n", job->index, job->pid, job->status, job->cmd);
			else
				dprintf(2, "[%d]	%d	%s	%s\n", job->index, job->pid, job->status, job->cmd);
			freed(&arrayl->array[i]->cmd);
			free(arrayl->array[i]);
			arrayl->array[i] = NULL;
			arrayl->size -= 1;
		}
	}
}

int	init_and_add_jobs(char **cmds, pid_t pid, t_array_list *arrayl, int is_stop)
{
	char	**jobs_cmd = &cmds[0];
	t_jobs	*job;

	if (!is_stop)
	{
		jobs_cmd = create_cmd_for_jobs(cmds);
		if (!jobs_cmd)
		{
			dprintf(2, "error alloc jobs_cmd in init_and_add_jobs\n");
			return (0);
		}
	}
	job = create_jobs(pid, jobs_cmd, arrayl);
	if (!is_stop)
		free_tab(jobs_cmd);
	if (!job)
		return (write(2,"error alloc\n", 13), 0);		
	add_in_jobs_list(arrayl, job);
	if (!is_stop)
	{
		job->status = "Running";
		dprintf(2, "[%d] %d Running %s\n", job->index, job->pid, job->cmd);
	}
	else
	{
		job->status = "Stopped";
		dprintf(2, "[%d] %d Stopped %s\n", job->index, job->pid, job->cmd);
	}
	return (1);
}
