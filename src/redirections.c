#include "jsh.h"

static int get_flags_redir_out(char *flags)
{
    if (!strcmp(flags, ">") || !strcmp(flags, "2>"))
        return (O_CREAT | O_WRONLY | O_EXCL);
    else if (!strcmp(flags, ">|") || !strcmp(flags, "2>|"))
        return (O_CREAT | O_WRONLY | O_TRUNC);
    else if (!strcmp(flags, ">>") || !strcmp(flags, "2>>"))
        return (O_CREAT | O_WRONLY | O_APPEND);
    return (-1);
}

static int is_stderror(char *flags)
{
    return flags && flags[0] == '2';
}

void redir_in(char *filename)
{
    int fd = open(filename, O_RDONLY);

    if (fd == -1)
    {
        if (errno == EEXIST)
            dprintf(2, "Erreur : le fichier '%s' existe déjà.\n", filename);
        else
            perror("open in redir_in");
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, STDIN_FILENO) == -1)
    {
        perror("dup2 in redir_in");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

void redir_out(char *flags, char *filename)
{
    int fd_flags = get_flags_redir_out(flags);

    int dup_out = STDOUT_FILENO;
    if (is_stderror(flags))
        dup_out = STDERR_FILENO;
    
    int fd = open(filename, fd_flags, 0644);
    
    if (fd == -1)
    {
        if (errno == EEXIST)
            dprintf(2, "Erreur : le fichier '%s' existe déjà.\n", filename);
        else
            perror("open in redir_out");
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, dup_out) == -1)
    {
        perror("dup2 in redir_out");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

// Fonction pour compter le nombre de segments dans la commande (basee sur les pipes)
int count_segments(char **cmds) 
{
    int count = 0;
    for (int i = 0; cmds[i] != NULL; i++) 
    {
        if (!strcmp(cmds[i], "|")) 
            count++;
    }
    return count + 1; // Nombre de segments = Nombre de pipes + 1
}

// Fonction pour diviser les commandes par les pipes
// Cette fonction renvoie un tableau de pointeurs `NULL` termine indiquant les positions des segments
int *parse_pipes(char **cmds, int *num_segments) 
{
    *num_segments = count_segments(cmds);
    int *segment_indices = malloc((*num_segments + 1) * sizeof(int));
    if (!segment_indices) 
    {
        perror("malloc parse_pipes");
        exit(EXIT_FAILURE);
    }

    int segment_index = 0;
    segment_indices[segment_index++] = 0; // Premier segment commence à l'index 0

    for (int i = 0; cmds[i] != NULL; i++) 
    {
        if (!strcmp(cmds[i], "|")) 
        {
            // cmds[i] = NULL; // Remplace '|' par NULL
			freed(&cmds[i]);
            segment_indices[segment_index++] = i + 1; // Stocke le debut du prochain segment
        }
    }
    segment_indices[segment_index] = -1; // Marque la fin des indices de segments

    return segment_indices;
}

void handle_redirections(char **cmds)
{
    int i = 0;
    while (cmds[i] != NULL)
    {
        if (strcmp(cmds[i], ">") == 0 || strcmp(cmds[i], ">>") == 0 || strcmp(cmds[i], "2>") == 0 || strcmp(cmds[i], "2>>") == 0 || strcmp(cmds[i], "<" ) == 0 || strcmp(cmds[i], ">|" ) == 0 || strcmp(cmds[i], "2>|" ) == 0)
        {
            if (cmds[i + 1] == NULL)
            {
                dprintf(2, "jsh: syntax error near unexpected token `newline'\n");
				free_tab(cmds);
                exit(EXIT_FAILURE);  // Sortie immédiate en cas d'erreur de syntaxe
            }
            if (!strcmp(cmds[i], "<"))
                redir_in(cmds[i + 1]);
            else
                redir_out(cmds[i], cmds[i + 1]);

            // Suppression des tokens de redirection du tableau de commandes
            int j = i;
            while (cmds[j + 2] != NULL)
            {
                cmds[j] = cmds[j + 2];
                j++;
            }
            cmds[j] = NULL;
        }
        else
            i++;
    }
}

int	have_pipe_and_do_pipe(char **cmds)
{
	for (int i = 0; cmds[i]; i++)
	{
		if (!strcmp(cmds[i], "|"))
		{
			handle_pipes(cmds);
			return 1;
		}
	}
	return (0);
}

void handle_pipes(char **cmds)
{
	int	tab_size = get_tab_size(cmds);
    int num_segments;
    int *segment_indices = parse_pipes(cmds, &num_segments);

    int pipefds[2 * (num_segments - 1)];
    
    for (int i = 0; i < num_segments - 1; i++) 
    {
        if (pipe(pipefds + i * 2) < 0) {
            perror("Couldn't create pipe");
			goto error;
        }
    }

    for (int i = 0; i < num_segments; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        { // Child process
            // Redirection des descripteurs de fichier pour les pipes
            if (i != 0) {
                if (dup2(pipefds[2 * (i - 1)], STDIN_FILENO) == -1) {
					perror("dup2");
                    goto error;
                }
            }
            if (i != num_segments - 1) {
				if (dup2(pipefds[2 * i + 1], STDOUT_FILENO) == -1) {
					perror("dup2");
					goto error;
				}
            }

            // Fermeture de tous les descripteurs de fichier du pipe
            for (int j = 0; j < 2 * (num_segments - 1); j++) {
                close(pipefds[j]);
            }

            // Execution de la commande du segment
            char **segment_cmd = cmds + segment_indices[i];
            handle_redirections(segment_cmd);
            execvp(segment_cmd[0], segment_cmd);
            perror("execvp");
			free_tab(segment_cmd);
			goto error;
        }
        else if (pid < 0)
        { // En cas d'erreur de fork
            perror("fork");
			goto error;
        }
    }
	free_tab_with_size(&cmds[0], tab_size);
    // Fermeture des descripteurs de fichier du pipe dans le processus parent
    for (int i = 0; i < 2 * (num_segments - 1); i++) {
        close(pipefds[i]);
    }

    // Attente de la fin des processus enfants
    for (int i = 0; i < num_segments; i++) {
        wait(NULL);
    }
    // Liberation des indices de segments
    free(segment_indices);
	return ;

	error :
		free_tab_with_size(cmds, tab_size);
		exit(EXIT_FAILURE);
}
