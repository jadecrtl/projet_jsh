#ifndef JSH_H
#define JSH_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

struct s_jobs;
struct s_arraylist;

// === split.c ===
char	**ft_split(char const *s, char c);
int		is_only_spaces(char *str);

// === my_exit.c ===
int		my_exit(char **cmds, struct s_arraylist *arrayl);

// === my_pwd.c ===
int 	is_redir (char *cmd);
int 	set_redir (char *operator, char *fname);
int     my_pwd();

// === signal.c ===
void	init_signal(void);
void	restore_signal(void);

// === free_all.c ===
void	free_tab(char **cmds);
void	free_tab_with_size(char **cmds, int size);
void	freed(char **ptr);
void	print_tab(char **tab);


// === cd.c ===
int		commande_cd(char **cmds);

// === cd_utils.c ===
char	*ft_strjoin_gnl(char *s1, char *s2, int n);
int		get_tab_size(char **tab);
char	**clean_pwd(char **tab, int size);
char	*clean_pwd_str(char *pwd);

// === prompt.c ===
char    *my_prompt(struct s_arraylist *arrayl);
int     ColorChange(char *s, char color);
int     addPrompt(char *p, char *s);
char    *cut(char *path, size_t max);

// === redirections.c ===
void	redir_out(char *flags, char *filename);
void	redir_in(char *filename);
int		count_active_jobs(struct s_arraylist *arrayl);
int 	count_segments(char **cmds);
int 	*parse_pipes(char **cmds, int *num_segments);
void	handle_redirections(char **cmds);
int		have_pipe_and_do_pipe(char **cmds);
void	handle_pipes(char **cmds);
void	execute_command(const char *cmd);

// === substitution.c ===
char	*handle_substitution(char **cmds, int index);
char	**new_array_of_sub(char **cmds);
char	**setup_cmds_for_substitution(char **cmds);

// === my_kill.c ====
int 	my_kill(char **cmds, struct s_arraylist *arrayl);

#endif
