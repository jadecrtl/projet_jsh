#include "jsh.h"

char	**setup_cmds_for_substitution(char **cmds)
{
	for (int i = 0; cmds[i]; i++)
		if (!strncmp(cmds[i], "<(", 2))
			return new_array_of_sub(cmds);
	return (NULL);
}

static char	**get_substitution_cmd(char **cmds, int index)
{
	int		len = 0;
	int		start_sub_cmd_index = 0;

	// get the size of the substitution cmd;
	for (int i = index; cmds[i]; i++)
	{
		if (!strncmp(cmds[i], "<(", 2))
		{
			start_sub_cmd_index = ++i;
			while (cmds[start_sub_cmd_index + len] && strcmp(cmds[start_sub_cmd_index + len], ")"))
				len++;
			break ;
		}
	}
	if (len == 0)
		return (NULL);
	// allocation and duplicate the substution cmd;
	char	**res = calloc(len + 1, sizeof(char *));
	if (!res)
		return (NULL);
	for (int i = start_sub_cmd_index, j = 0; j < len; i++, j++)
	{
		res[j] = strdup(cmds[i]);
		if (!res[j])
			return (free_tab(res), NULL);
	}
	res[len] = NULL;
	return (res);
}

static int	new_size_of_sub(char **cmds)
{
	int	res = 0;
	for (int i = 0; cmds[i]; i++)
	{
		if (!strcmp(cmds[i], "<("))
		{
			while (cmds[i] && strcmp(cmds[i], ")"))
				i++;
			if (!cmds[i])
				break ;
		}
		res++;
	}
	return (res);
}

char	**new_array_of_sub(char **cmds)
{
	int i = 0;
	int	j = 0;
	int	size = new_size_of_sub(cmds);
	char	**res = calloc(size + 1, sizeof(char *));
	if (!res)
		return (NULL);
	while (cmds[i] && j < size)
	{
		if (!strcmp(cmds[i], "<("))
		{
			res[j] = handle_substitution(cmds, i);
			j++;
			while (cmds[i] && strcmp(cmds[i], ")"))
				i++;
			if (cmds[i] && !strcmp(cmds[i], ")"))
				i++;
		}
		else
		{
			res[j] = strdup(cmds[i]);
			if (!res[j])
				return (free_tab(res), NULL);
			j++;
			i++;
		}
	}
	res[j] = NULL;
	return (res);
}


char	*handle_substitution(char **cmds, int index)
{
	char	**substitution_cmd = get_substitution_cmd(cmds, index);
	if (!substitution_cmd)
	{
		dprintf(2, "error calloc\n");
		return NULL;
	}
	int	pipe_pere[2];
	if (pipe(pipe_pere) < 0)
	{
		perror("pipe");
		free_tab(substitution_cmd);
		return NULL;
	}
	pid_t pid = fork();
	if (!pid)
	{
		for (int i = 0; substitution_cmd[i]; i++)
		{
			if (!strcmp(substitution_cmd[i], "|"))
			{
				handle_pipes(substitution_cmd);
				exit(EXIT_SUCCESS);
			}
		}
		dup2(pipe_pere[1], 1);
		close(pipe_pere[0]);
		execvp(substitution_cmd[0], substitution_cmd);
		free_tab(substitution_cmd);
		exit(EXIT_FAILURE);
	}
	else
	{
		free_tab(substitution_cmd);
		close(pipe_pere[1]);

		char fd_path[20] = {0};
        sprintf(fd_path, "/dev/fd/%d", pipe_pere[0]);
       	return (strdup(fd_path));
	}
}
