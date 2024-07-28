#ifndef JOBS_H
#define JOBS_H

#include "jsh.h"

typedef struct s_jobs
{
    pid_t   pid;
    char    *cmd;
    char    *status;
    int     index;
}   t_jobs;

typedef struct s_arraylist
{
    t_jobs  **array;
    int     size;
    int     max;
}   t_array_list;

t_array_list    *init_array_list();
t_jobs          *create_jobs(pid_t pid, char **cmds, t_array_list *arrayl);
int             get_index_in_arrayl(t_array_list *arrayl);
int             add_in_jobs_list(t_array_list *arrayl, t_jobs *to_add);
int             my_jobs(t_array_list *arrayl);
void            update_jobs(t_array_list *arrayl, int sortie);
int				init_and_add_jobs(char **cmds, pid_t pid, t_array_list *arrayl,
						int is_stop);

char    **create_cmd_for_jobs(char **cmds);
char    **create_cmd_for_background(char **cmds);
void    free_jobs_cmds(t_array_list *arrayl);
void    free_all_arrayl(t_array_list *arrayl);
int     my_bg(char **cmds, t_array_list *arrayl);
int     my_fg(char **cmds, t_array_list *arrayl);

#endif