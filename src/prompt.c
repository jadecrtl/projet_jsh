#include "jsh.h"
#include "jobs.h"

/*
Bascules de couleur + balises : 15 (couleur) + 6 (balises) = 21 caractères
Nombre de jobs : 3 caractères 
Référence du répertoire : 30 caractères 
Dollar et espace : 2 caractères
+ \0

= 52 caractères
*/

int ColorChange(char *s, char color) {
    char *c;
    int size = 5;
    switch (color) {
        case 'v':   c = "\033[32m"; //Vert
            break;
        case 'j': c = "\033[33m";   //Jaune
            break;
        case 'b': c = "\033[34m";   //Bleu
            break;
        case 'c':   c = "\033[36m"; // Cyan
            break;
        case 'r':   c = "\033[91m"; //Rouge
            break;
        default:    c = "\033[00m"; //Normal
    }
    memcpy(s, c, size);
    return size;
}

int addPrompt(char *p, char *s) {
    memcpy(p, s, strlen(s));
    return strlen(s);
}

char *cut(char *path, size_t max) {
    char *p = calloc(max + 1, 1);
    if (p == NULL) perror("erreur calloc cut");
    if (strlen(path) <= max) {
        memcpy(p, path, strlen(path));
    } else {
        memcpy(p, "...", 3);
        memcpy(p + 3, path + strlen(path) - max + 3, max - 3);
    }
    return p;
}

char *my_prompt(struct s_arraylist *arrayl) {
	//taille total = 30 caractère + 15 couleurs + 6 balises + '\0'
    int size = 52;

    char *prompt = calloc(size, sizeof(char));
    if (prompt == NULL) perror("erreur calloc prompt");

    int i = 0;
    // Chemin du répertoire
    prompt[i++] = '\001';
    i += ColorChange(prompt + i, 'j');
    prompt[i++] = '\002';

    //jobs
    int nombre_de_jobs_actifs = arrayl->size; 
    char jobs[14] = {0};
    sprintf(jobs, "[%d]", nombre_de_jobs_actifs);
    i += addPrompt(prompt + i, jobs);

    //path
    int visibleElement= strlen(jobs) + 2; //dollar+ espace
    int max = 30 - visibleElement; //max path 
    char *wDir = getenv("PWD");
    char *path = cut(wDir, max);
    i += addPrompt(prompt + i, path);
    free(path);

    //deuxième couleur + $ 
    prompt[i++] = '\001';
    i += ColorChange(prompt + i, 'b');
    prompt[i++] = '\002';
    i += addPrompt(prompt + i, "$ ");

    //couleur retour a la normale
    prompt[i++] = '\001';
    i += ColorChange(prompt + i, 'd');
    prompt[i++] = '\002';
    
    return prompt;
}
