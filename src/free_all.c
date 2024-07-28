#include "jsh.h"
#include "jobs.h"

void	free_tab(char **cmds)
{
	if (!cmds && cmds[0])
		return ;
	for (int i = 0; cmds[i]; i++)
		freed(&cmds[i]);
	free(cmds);
}

void	free_tab_with_size(char **cmds, int size)
{
	if (!cmds)
		return;
	for (int i = 0; i < size; i++)
		if (cmds[i])
			freed(&cmds[i]);
	free(cmds);
}

void	freed(char **ptr)
{
	if (!*ptr)
		return ;
	free(*ptr);
	*ptr = NULL;
}

void	print_tab(char **tab)
{
	for (int i = 0; tab[i]; i++)
		dprintf(1, "%s\n", tab[i]);
}

void	free_jobs_cmds(t_array_list *arrayl)
{
	if (!arrayl->array)
		return ;
	for (int i = 0; i < arrayl->max; i++)
	{
		if (!arrayl->array[i] || arrayl->array[i]->pid == -1)
			continue;
		freed(&arrayl->array[i]->cmd);
		free(arrayl->array[i]);
	}
	free(arrayl->array);
}

void	free_all_arrayl(t_array_list *arrayl)
{
	if (!arrayl)
		return ;
	free_jobs_cmds(arrayl);
	free(arrayl);
}
