#include "jsh.h"

int	ft_cd_home(void)
{
	char	*tmp;
	char	*pwd;

	tmp = getenv("HOME");
	if (tmp)
	{
		if (!chdir(tmp))
		{
			pwd = getenv("PWD");
			setenv("OLDPWD", pwd, 1);
			setenv("PWD", tmp, 1);
			return (0);
		}
		else
			return (dprintf(2, "cd: %s : No such file or directory\n", tmp), 1);
	}
	else
		return (dprintf(2, "bash: cd: HOME not set!\n"), 1);
}

int	ft_cd_absolute_path(char **cmds)
{
	char	*get_old_pwd_env;
	char	*tmp_pwd;

	tmp_pwd = clean_pwd_str(cmds[1]);
	if (!tmp_pwd)
		return (1);
	if (!chdir(tmp_pwd))
	{
		get_old_pwd_env = getenv("PWD");
		if (get_old_pwd_env)
			setenv("OLDPWD", get_old_pwd_env, 1);
		setenv("PWD", tmp_pwd, 1);
		free(tmp_pwd);
		return (0);
	}
	freed(&tmp_pwd);
	dprintf(2, "bash: cd: %s: No such file or directory\n", cmds[1]);
	return (1);
}

int	ft_cd_with_pwd(char **cmds)
{
	char	*pwd;
	char	*get_old_pwd_env;
	char	*tmp;

	pwd = getenv("PWD");
	if (!pwd)
		pwd = getcwd(NULL, 0);
	if (pwd[strlen(pwd) - 1] != '/')
		pwd = ft_strjoin_gnl(pwd, "/", 0);
	pwd = ft_strjoin_gnl(pwd, cmds[1], 1);
	tmp = pwd;
	pwd = clean_pwd_str(pwd);
	free(tmp);
	if (!chdir(pwd))
	{
		get_old_pwd_env = getenv("PWD");
		if (get_old_pwd_env)
			setenv("OLPWD", get_old_pwd_env, 1);
		setenv("PWD", pwd, 1);
		free(pwd);
		return (0);
	}
	free(pwd);
	dprintf(2, "bash: cd: %s: No such file or directory\n", cmds[1]);
	return (1);
}

int	ft_cd_with_old_pwd(char **cmds)
{
	char *old_pwd;
	char *pwd;
	(void)cmds;
	old_pwd = getenv("OLDPWD");
	pwd = getenv("PWD");
	if (!chdir(old_pwd))
	{
		setenv("OLDPWD", pwd, 1);
		setenv("PWD", old_pwd, 1);
		return (0);
	}
	return (1);
}

int	commande_cd(char **cmds)
{
	int	size;

	size = get_tab_size(cmds);
	if (size == 1)
		return (ft_cd_home());
	else if (size == 2)
	{
		if (cmds[1][0] == '/')
			return (ft_cd_absolute_path(cmds));
		else if (cmds[1][0] == '-')
			return (ft_cd_with_old_pwd(cmds));
		else
			return (ft_cd_with_pwd(cmds));
	}
	else
		return (dprintf(2,  "bash: cd: too many arguments\n") ,1);
}
