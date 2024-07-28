# Architecture du Projet

## Aperçu général
Ce document fourni un aperçu de l'architecture général, de la structure de données et les algorithmes utilisés dans notre projet.
Ce fichier est conçu pour aider à comprendre rapidement la structure du code ainsi que les choix de conception.

## Architecture Logicielle

### Main Module
Le module principal (main.c) est le cœur de l'application. 
On récupère l'input via la fonction readline que l'on transforme en tableau de chaine de caractere.
Puis lors de l'exécution ce tableau peut être modifier s'il y a des substitution et exécuter les pipes, un processus en avant ou arriere plan en fonction de celle-ci.

### Prompt Module
Le module 'prompt.c' est responsable de l'interface utilisateur, fournissant une boucle interactive pour la saisie des commandes.

### Split Module
Le module 'split.c' permet de traiter les entrées de l'utilisateur en divisant en commandes et arguments.

### Jobs Module
Le module 'jobs.c' gère les tâches en arrière-plan, permettant un contrôle multitâche au sein de l'application

### Free All Module
Le module 'free_all.c' assure une gestion efficace de la mémoire en libérant les ressources dont on a plus besoin.

### Redirections Module
Le module 'redirections.c' gère les redirections d'entrée/sortie, permettant une manipulation flexible des flux de données.

## Commandes et Utilitaires
Les fichiers comme 'signal.c', 'substitution.c', 'my_pwd.c', 'my_kill.c', 'my_exit.c', 'my_bg.c', 'my_fg.c', 'cd_utils.c' et 'cd.c' implémentent diverses commandes et utilitaires tels que gérer des fonctionnalitées spécifiques telles que la gestion des signaux, la navigation et la manipulation de l'environnement de l'application.


## Structures de Données
Les fichiers 'jobs.h' et 'jsh.h' contiennent des définitions essentielles de structures de données, qui sont cruciales pour organiser et gérer les données au sein de l'application.
Les jobs ont été implementé avec une structure qui contient un identifiant, une commande, un etat et sa position la structure arraylist.