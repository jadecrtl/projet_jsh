#include "jsh.h"

/*
Renvoie le chemin absolu du répertoire courant.
*/
 int my_pwd()
{
    char *path = getenv("PWD");
    if (!path)
    {
        perror("getenv in my_pwd");
        return 1;
    }

    dprintf(1, "%s\n", path);
    return 0;
}
