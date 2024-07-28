#include "jsh.h"
#include "jobs.h"

extern int status;

int	my_exit(char **cmds, t_array_list *arrayl)
{
	if (arrayl && arrayl->size > 0)
	{
		dprintf(2,"There is %d jobs", arrayl->size);
		return 1;
	}
	free_all_arrayl(arrayl);
	if (cmds && cmds[0] && cmds[1])
	{
		int ret = atoi(cmds[1]);
		free_tab(cmds);
		exit(ret);
	}
	free_tab(cmds);
	exit(status);
	return 0;
}
