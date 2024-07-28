#include "jsh.h"

static int count_word(char const *s, char c)
{
	int i;
	int res;

	i = 0;
	res = 0;
	while (s[i])
	{
		while (s[i] == c && s[i])
			i++;
		if (s[i] != c && s[i])
		{
			res++;
			while (s[i] != c && s[i])
			i++;
		}
	}
	return (res);
}

static char *get_word(char const *s, char c, int *index)
{
	int i;
	int len;
	char *res;

	i = 0;
	len = 0;
	while (s[(*index)] == c && s[(*index)])
		(*index)++;
	while (s[(*index) + len] != c && s[(*index) + len])
		len++;
	res = calloc(len +1, sizeof(char));
	if (!res)
	{
		return (NULL);
	}
	while (i < len)
	{
		res[i] = s[(*index)];
		i++;
		(*index)++;
	}
	res[i] = '\0';
	return (res);
}

char **ft_split(char const *s, char c)
{
	int i;
	int j;
	char **res;
	int word_count = count_word(s, c);

	i = 0;
	j = 0;
	res = calloc(word_count + 1, sizeof(char *));
	if (!res)
		return (NULL);

	while (i < word_count)
	{
		res[i] = get_word(s, c, &j);
		if (!res[i])
		{
			while (i > 0)
			{
				free(res[--i]); // Libere chaque chaîne allouee avant l'erreur
			}
			free(res); // Libere le tableau de chaines
			return (NULL);
		}
		i++;
	}
	res[i] = NULL;
	return (res);
}

int is_only_spaces(char *str)
{
	for (int i = 0; str[i]; i++)
	if (str[i] != ' ' && str[i] != '\t')
		return (0);
	return (1);
}
