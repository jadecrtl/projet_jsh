#include "jsh.h"

static char	*malloc_s1(char *s1)
{
	s1 = (char *)malloc(1 * sizeof(char));
	if (!s1)
		return (NULL);
	s1[0] = '\0';
	return (s1);
}

char	*ft_strjoin_gnl(char *s1, char *s2, int n)
{
	size_t	i;
	size_t	c;
	char	*str;

	if (!s1)
		s1 = malloc_s1(s1);
	if (!s1 || !s2)
		return (freed(&s1), NULL);
	str = calloc((strlen(s1) + strlen(s2) + 1), sizeof(char));
	if (!str)
		return (NULL);
	i = -1;
	c = 0;
	if (s1)
		while (s1[++i] != '\0')
			str[i] = s1[i];
	while (s2[c] != '\0')
		str[i++] = s2[c++];
	str[strlen(s1) + strlen(s2)] = '\0';
	if (n == 1)
		freed(&s1);
	return (str);
}

int	get_tab_size(char **tab)
{
	int	size = 0;
	while (tab[size])
		size++;
	return (size);
}

static void	free_previous_entry(char **res, int *j)
{
	(*j)--;
	if (*j >= 0 && res[*j])
	{
		free(res[*j]);
		res[*j] = NULL;
	}
}

static int	clean_pwd_len(char **tab, int size)
{
	int	i;
	int	len;

	i = 0;
	len = 0;
	while (i < size)
	{
		if (!strcmp(tab[i], ".."))
		{
			len--;
			if (len - 1 != 0)
				len--;
			else
				break ;
		}
		else if (!strcmp(tab[i], "."))
		{
			i++;
			continue ;
		}
		len++;
		i++;
	}
	return (len);
}

char	**clean_pwd(char **tab, int size)
{
	int		i;
	int		j;
	char	**res;

	res = calloc(clean_pwd_len(tab, size) + 1, sizeof(char *));
	if (!res)
		return (free_tab(tab), NULL);
	i = 0;
	j = 0;
	while (i < size)
	{
		if (!strcmp(tab[i], ".."))
		{
			free_previous_entry(res, &j);
			i++;
			continue ;
		}
		else if (!strcmp(tab[i], "."))
		{
			i++;
			continue ;
		}
		res[j++] = strdup(tab[i++]);
	}
	return (free_tab(tab), res);
}

char	*clean_pwd_str(char *pwd)
{
	char	**split_on_slash;
	char	**clean;
	char	*clean_str;
	int		i;
	int		new_size;

	if ((pwd[0] == 47 && strlen(pwd) == 1) || !strcmp(pwd, "/.."))
		return (strdup("/"));
	split_on_slash = ft_split(pwd, '/');
	if (!split_on_slash)
		return (NULL);
	new_size = get_tab_size(split_on_slash);
	clean = clean_pwd(split_on_slash, new_size);
	clean_str = ft_strjoin_gnl(NULL, "/", 1);
	if (!clean)
		return (clean_str);
	i = -1;
	new_size = get_tab_size(clean);
	while (++i < new_size)
	{
		clean_str = ft_strjoin_gnl(clean_str, clean[i], 1);
		if (i != new_size - 1)
			clean_str = ft_strjoin_gnl(clean_str, "/", 1);
	}
	return (free_tab(clean), clean_str);
}
