#include "jsh.h"
#include "jobs.h"

int status;
int saved_stdout;
int saved_stderr;

int is_built_in(char *cmd)
{
	return (!strcmp(cmd, "cd") || !strcmp(cmd, "pwd") || !strcmp(cmd, "exit") || !strcmp(cmd, "jobs") || !strcmp(cmd, "bg") || !strcmp(cmd, "fg") || !strcmp(cmd, "kill") || !strcmp(cmd, "?"));
}

int return_val_status(void)
{
	dprintf(1, "%d\n", status);
	return 0;
}

void restore_file_descriptors()
{
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stderr, STDERR_FILENO);
	close(saved_stdout);
	close(saved_stderr);
}

int launch_built_in(char **cmds, t_array_list *arrayl)
{
	char **tmp = setup_cmds_for_substitution(cmds);
	if (tmp)
	{
		free_tab(cmds);
		cmds = tmp;
	}

	if (have_pipe_and_do_pipe(cmds))
		return status;

	saved_stdout = dup(STDOUT_FILENO);
	saved_stderr = dup(STDERR_FILENO);
	handle_redirections(cmds);

	if (!strcmp(cmds[0], "cd"))
		status = commande_cd(cmds);
	else if (!strcmp(cmds[0], "pwd"))
		status = my_pwd();
	else if (!strcmp(cmds[0], "exit"))
		status = my_exit(cmds, arrayl);
	else if (!strcmp(cmds[0], "jobs"))
	{
		update_jobs(arrayl, 1);
		status = my_jobs(arrayl);
	}
	else if (!strcmp(cmds[0], "bg"))
		status = my_bg(cmds, arrayl);
	else if (!strcmp(cmds[0], "fg"))
		status = my_fg(cmds, arrayl);
	else if (!strcmp(cmds[0], "kill"))
		status = my_kill(cmds,arrayl);
	else if (!strcmp(cmds[0], "?"))
		status = return_val_status();
	restore_file_descriptors();
	return (free_tab(cmds), status);
}

int launch_exec(char **cmds, t_array_list *arrayl)
{
	char **tmp = setup_cmds_for_substitution(cmds);
	if (tmp)
	{
		free_tab(cmds);
		cmds = tmp;
	}
	int size = get_tab_size(cmds);
	pid_t shell_pgid = getpgrp();

	if (have_pipe_and_do_pipe(cmds))
		return status;

	if (!strcmp(cmds[size - 1], "&"))
	{
		pid_t pid = fork();
		if (!pid)
		{
			setpgid(0, 0);
			restore_signal();
			char **cmd_for_exec = create_cmd_for_background(cmds);
			if (!cmd_for_exec)
			return (1);
			execvp(cmd_for_exec[0], cmd_for_exec);
			perror("execvp");
			free_tab(cmds);
			exit(127);
		}
		else
		{
			setpgid(pid, pid);
			init_and_add_jobs(cmds, pid, arrayl, 0);
			free_tab(cmds);
			return (status);
		}
	}
	else
	{
		pid_t pid = fork();
		if (pid == 0)
		{
			setpgid(0, 0);
			restore_signal();

			handle_redirections(cmds);
			execvp(cmds[0], cmds);
			dprintf(2, "jsh: no such file or directory: %s\n", cmds[0]);
			free_tab(cmds);
			free_all_arrayl(arrayl);
			exit(127);
		}
		else
		{
			setpgid(pid, pid);
			tcsetpgrp(STDIN_FILENO, pid);

			int val;
			int code = waitpid(-pid, &val, WUNTRACED | WCONTINUED);
			tcsetpgrp(STDIN_FILENO, shell_pgid);
			if (code == -1) 
			{
				if (errno == ECHILD)
					return (1);
				else
					perror("waitpid failed");
			}
			else if (code > 0)
			{
				if (WIFEXITED(val))
					status = WEXITSTATUS(val);
				else if (WIFSIGNALED(val) || WIFSTOPPED(val))
				{
					val = WIFSIGNALED(val) ? WTERMSIG(val) : WSTOPSIG(val);
					if (val == 19 || val == 20)
					{
						int length = 1;
                        for(int i = 0; cmds[i] != NULL; i++){
                            length += strlen(cmds[i])+1;
                        }
                        char *joined = malloc(length);
                        if (joined == NULL)  perror("erreur malloc ");

                        joined[0] = '\0'; // Initialise la chaîne vide
                        for (int i = 0; cmds[i] != NULL; i++) {
                            strcat(joined, cmds[i]);
                            if (cmds[i + 1] != NULL) {
                                strcat(joined, " "); // Ajoute un espace entre les mots
                            }
                        }
                        init_and_add_jobs(cmds, pid, arrayl, 1);
					}
				}
			}
		}
	}
	free_tab(cmds);
	return (status);
}

int exec(char **cmds, t_array_list *arrayl)
{
	if (is_built_in(cmds[0]))
		return (launch_built_in(cmds, arrayl));
	else
		return (launch_exec(cmds, arrayl));
	return (0);
}

int main(void)
{
	char *prompt;
	char *cmd;
	char **cmds;
	status = 0;
	init_signal();

	t_array_list *arrayl = init_array_list();
	if (!arrayl)
		return (1);
	while (1)
	{
		prompt = my_prompt(arrayl);
		rl_outstream = stderr;
		cmd = readline(prompt);
		free(prompt);
		if (!cmd)
			break ;
		if (!(*cmd) || is_only_spaces(cmd))
		{
			freed(&cmd);
			continue ;
		}
		add_history(cmd);
		cmds = ft_split(cmd, ' ');
		freed(&cmd);
		if (!cmds)
			break ;
		status = exec(cmds, arrayl);
		update_jobs(arrayl, 0);
	}
	free_all_arrayl(arrayl);
	rl_clear_history();
	return (status);
}
