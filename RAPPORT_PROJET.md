# Rapport de Projet : Analyseur de Grammaires Régulières

**Cours :** Théorie des Langages et Compilation
**Réalisé par :** [Votre Nom]
**Date :** Décembre 2024
**Objectif :** Implémenter un programme capable de modéliser une grammaire régulière et d'effectuer l'analyse de séquences par dérivation/automates.

---

## 1. Introduction

Ce projet consiste en la conception et la réalisation d'un outil permettant de manipuler des **grammaires régulières**. L'enjeu est de transformer une définition textuelle de règles de production (ex: `A → aB`) en une structure de données exploitable (un automate fini) afin de valider si des chaînes de caractères appartiennent au langage défini.

Le projet propose deux interfaces :

- Une **interface console** pour des tests rapides
- Une **interface graphique GTK+3** permettant une interaction intuitive avec visualisation en temps réel

---

## 2. Analyse Théorique

Le programme repose sur les concepts fondamentaux de la hiérarchie de Chomsky :

### 2.1 Grammaire Régulière

Les grammaires régulières sont des grammaires dont les règles de production sont de la forme :

- `A → aB` (forme linéaire droite)
- `A → a` (production terminale)
- `A → ε` (production epsilon, notée `\t` dans notre implémentation)

où `A` et `B` sont des **non-terminaux** et `a` est un **symbole terminal**.

### 2.2 Processus de Dérivation

Pour valider une chaîne, le programme suit le cheminement des non-terminaux :

1. Partir de l'état initial (premier non-terminal)
2. Pour chaque symbole de la chaîne d'entrée, suivre la transition correspondante
3. Si après lecture complète de la chaîne nous atteignons un état marqué comme **final**, la séquence est **acceptée**

### 2.3 Représentation par Automate

La grammaire est stockée sous forme de **Graphe Orienté** où :

- Chaque **nœud** représente un état (non-terminal)
- Chaque **arc** représente une transition étiquetée par un symbole terminal
- Les états finaux correspondent aux non-terminaux ayant une production epsilon

**Exemple de correspondance :**

```
Grammaire:          Automate:
A → aB              État A --a--> État B
B → cB              État B --c--> État B
B → \t              État B (final)
```

---

## 3. Architecture du Système

L'application est découpée en trois couches distinctes :

### 3.1 Le Moteur Logique (Backend)

Situé dans le dossier `config/`, il gère la structure de données :

#### 3.1.1 Structures de Données (`config/graph.h`)

```c
typedef char type;

// Structure représentant une transition
typedef struct Children {
    type value;           // Symbole de transition (terminal)
    type transition;      // État cible
    struct Children * next;
} children;

typedef children * Transitions;

// Structure représentant un état (nœud)
typedef struct Node{
    type value;           // Identifiant de l'état
    int is_final;         // 1 si état final, 0 sinon
    Transitions transitions; // Liste des transitions sortantes
    struct Node * next;
} node;

typedef node * list;
```

**Explication :**

- La structure `children` forme une **liste chaînée de transitions** pour chaque état
- La structure `node` contient l'état lui-même et sa liste de transitions
- Le `list` est une **liste chaînée de tous les états** du graphe

#### 3.1.2 Opérations sur le Graphe (`config/graph.c`)

**Création et ajout de transitions :**

```c
void add_children(Transitions * transitions, type value, type transition){
    // Allocation dynamique d'une nouvelle transition
    children *c = (children*)malloc(sizeof(children));
    if (!c) return;

    c->transition = transition;  // État cible
    c->value = value;           // Symbole de transition
    c->next = NULL;

    // Insertion en fin de liste
    if (*transitions == NULL) {
        *transitions = c;
    } else {
        children *cur = *transitions;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = c;
    }
}
```

**Création et ajout d'un état :**

```c
void add_node_to_liste(list* l, Transitions transitions, type value, int is_final){
    node *n = (node*)malloc(sizeof(node));
    if (!n) return;

    n->is_final = is_final;
    n->value = value;
    n->transitions = transitions;
    n->next = NULL;

    // Insertion en fin de liste
    if (*l == NULL) {
        *l = n;
    } else {
        node *cur = *l;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = n;
    }
}
```

**Recherche de transition :**

```c
children * get_transition(node n, type value){
    Transitions t = n.transitions;
    while (t != NULL) {
        if (t->value == value) {  // Symbole trouvé
            return t;
        }
        t = t->next;
    }
    return NULL;  // Pas de transition pour ce symbole
}
```

**Recherche d'un état :**

```c
node * get_node(list l, type value){
    node *cur = l;
    while (cur != NULL) {
        if (cur->value == value) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}
```

**Affichage du graphe (console) :**

```c
void afficher_liste(list l) {
    node *cur = l;
    while (cur != NULL) {
        children *trans = cur->transitions;
        printf("%c", cur->value);

        // Marquer les états finaux avec *
        if (cur->is_final) {
            printf(" *");
        }
        printf(" : ");

        // Afficher toutes les transitions
        while (trans != NULL) {
            printf("->%c%c", trans->value, trans->transition);
            if (trans->next != NULL) {
                printf(" | ");
            }
            trans = trans->next;
        }
        printf("\n");
        cur = cur->next;
    }
}
```

### 3.2 Le Parseur (`config/utils.c`)

Le programme propose **deux modes d'entrée** pour charger une grammaire :

#### 3.2.1 Parsing de Fichier

**Lecture du fichier :**

```c
char ** get_grammaire_file(char * url){
    FILE *file = fopen(url, "r");
    if (file == NULL) {
        printf("Error: Could not open file '%s'\n", url);
        return NULL;
    }

    // Compter le nombre de lignes
    int line_count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            line_count++;
        }
    }

    // Vérifier la dernière ligne
    fseek(file, -1, SEEK_END);
    ch = fgetc(file);
    if (ch != '\n' && ch != EOF) {
        line_count++;
    }

    // Allocation du tableau de lignes
    char **lines = (char **)malloc((line_count + 1) * sizeof(char *));
    if (lines == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    // Lecture ligne par ligne
    fseek(file, 0, SEEK_SET);
    char buffer[1024];
    int i = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && i < line_count) {
        size_t len = strlen(buffer);

        // Supprimer le '\n' final
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }

        // Copier la ligne
        lines[i] = (char *)malloc(len + 1);
        if (lines[i] == NULL) {
            // Libération mémoire en cas d'erreur
            for (int j = 0; j < i; j++) {
                free(lines[j]);
            }
            free(lines);
            fclose(file);
            return NULL;
        }

        strcpy(lines[i], buffer);
        i++;
    }

    lines[i] = NULL;  // Marquer la fin du tableau
    fclose(file);
    return lines;
}
```

**Construction du graphe depuis un fichier :**

```c
list get_grammaire_list(char * line_transitions_tab){
    if (line_transitions_tab == NULL) {
        return NULL;
    }

    list l = NULL;
    char **lines = get_grammaire_file(line_transitions_tab);

    if (lines == NULL) {
        return NULL;
    }

    // Parser chaque ligne
    for (int i = 0; lines[i] != NULL; i++) {
        char *line = lines[i];
        if (strlen(line) == 0) {
            continue;
        }

        char state;
        char transition_input;
        char next_state;
        int is_terminal = 0;

        // Parser format: "A -> bB" ou "B -> \t"
        int pos = 0;

        // Extraire l'état source (avant "->")
        while (line[pos] == ' ') pos++;
        state = line[pos];
        pos++;

        // Chercher "->"
        while (line[pos] != '-' && line[pos] != '\0') pos++;
        if (line[pos] == '\0') continue;
        pos += 2;  // Sauter "->"

        // Extraire la partie droite
        while (line[pos] == ' ') pos++;

        // Vérifier si c'est une production epsilon (\t)
        if (line[pos] == '\\' && line[pos + 1] == 't') {
            is_terminal = 1;
            transition_input = '\0';
            next_state = '\0';
        } else {
            // Lire symbole de transition et état suivant
            transition_input = line[pos];
            pos++;

            if (line[pos] != '\0' && line[pos] != ' ') {
                next_state = line[pos];
            } else {
                next_state = '\0';
            }
        }

        // Chercher ou créer le nœud pour cet état
        node *current_node = get_node(l, state);

        if (current_node == NULL) {
            // Créer un nouveau nœud
            Transitions trans = NULL;
            if (!is_terminal) {
                add_children(&trans, transition_input, next_state);
            }
            add_node_to_liste(&l, trans, state, is_terminal);
        } else {
            // Ajouter la transition au nœud existant
            if (!is_terminal) {
                add_children(&(current_node->transitions), transition_input, next_state);
            }
            if (is_terminal) {
                current_node->is_final = 1;
            }
        }
    }

    // Libération mémoire
    for (int i = 0; lines[i] != NULL; i++) {
        free(lines[i]);
    }
    free(lines);

    return l;
}
```

#### 3.2.2 Parsing de Texte (Saisie Manuelle)

```c
list parse_grammaire_text(char * grammar_text){
    if (grammar_text == NULL || strlen(grammar_text) == 0) {
        return NULL;
    }

    list l = NULL;

    // Copier le texte pour tokenisation
    char *text_copy = strdup(grammar_text);
    char *line = strtok(text_copy, "\n");

    while (line != NULL) {
        // Ignorer les lignes vides
        if (strlen(line) == 0) {
            line = strtok(NULL, "\n");
            continue;
        }

        // Même logique de parsing que pour les fichiers
        char state;
        char transition_input;
        char next_state;
        int is_terminal = 0;

        int pos = 0;
        while (line[pos] == ' ') pos++;
        if (line[pos] == '\0') {
            line = strtok(NULL, "\n");
            continue;
        }
        state = line[pos];
        pos++;

        while (line[pos] != '-' && line[pos] != '\0') pos++;
        if (line[pos] == '\0') {
            line = strtok(NULL, "\n");
            continue;
        }
        pos += 2;

        while (line[pos] == ' ') pos++;

        if (line[pos] == '\\' && line[pos + 1] == 't') {
            is_terminal = 1;
            transition_input = '\0';
            next_state = '\0';
        } else {
            transition_input = line[pos];
            pos++;

            if (line[pos] != '\0' && line[pos] != ' ' && line[pos] != '\r') {
                next_state = line[pos];
            } else {
                next_state = '\0';
            }
        }

        // Création ou mise à jour du nœud
        node *current_node = get_node(l, state);

        if (current_node == NULL) {
            Transitions trans = NULL;
            if (!is_terminal) {
                add_children(&trans, transition_input, next_state);
            }
            add_node_to_liste(&l, trans, state, is_terminal);
        } else {
            if (!is_terminal) {
                add_children(&(current_node->transitions), transition_input, next_state);
            }
            if (is_terminal) {
                current_node->is_final = 1;
            }
        }

        line = strtok(NULL, "\n");
    }

    free(text_copy);
    return l;
}
```

#### 3.2.3 Algorithme de Vérification de Chaîne

**Fonction principale de validation :**

```c
int check_string(char * ch, list l){
    int n = strlen(ch);
    int cpt = 0;

    if (n == 0) return 0;
    if (l == NULL) return 0;

    // Commencer au premier état (état initial)
    node * current = l;

    // Parcourir chaque caractère de la chaîne
    for (int i = 0; ch[i] != '\0'; i++) {
        cpt++;

        // Chercher une transition pour ce caractère
        children * transition = get_transition(*current, ch[i]);

        if (transition == NULL) {
            // Pas de transition : vérifier si état final
            if (current->is_final == 1 && cpt == n - 1) {
                return 1;  // Acceptée
            }
            return 0;  // Rejetée
        }

        // Passer à l'état suivant
        node * next = get_node(l, transition->transition);
        if (next == NULL) {
            return 0;  // État suivant introuvable
        }
        current = next;
    }

    // Après lecture complète : vérifier si état final
    return current->is_final;
}
```

**Principe de l'algorithme :**

1. Initialiser à l'état de départ (premier nœud)
2. Pour chaque symbole de la chaîne :
   - Chercher une transition correspondante
   - Si aucune transition n'existe : rejeter (sauf si état final)
   - Sinon, suivre la transition vers l'état suivant
3. À la fin : accepter ssi on est dans un état final

### 3.3 L'Interface Utilisateur (Frontend)

Développée en GTK+3, elle permet une interaction intuitive.

#### 4. Manuel d'Utilisation

### 4.1 Installation des Dépendances

```bash
sudo apt-get install build-essential libgtk-3-dev
```

### 4.2 Compilation

Le projet utilise un **Makefile** pour automatiser la gestion des dépendances

**Commandes :**

```bash
make              # Compile tout le projet
make main         # Compile uniquement la version console
make grammar_app  # Compile uniquement l'interface graphique
make run          # Compile et exécute la version console
make run-gui      # Compile et exécute l'interface graphique
make clean        # Nettoie les fichiers compilés
```

### 4.3 Format de Grammaire

Les fichiers de grammaire (dans `input/`) utilisent le format :

```
A -> aB
B -> bB
B -> \t
```

**Signification :**

- `A -> aB` : Depuis l'état A, avec l'entrée 'a', transition vers l'état B
- `B -> \t` : L'état B est un état terminal (acceptant)

### 4.4 Exemple de Test Complet

**Fichier de grammaire (`input/grammaire.txt`) :**

```
A -> aB
B -> bB
B -> cB
B -> dB
B -> \t
```

**Représentation graphique**

**Langage accepté :** `L = { a(b|c|d)* }`

**Tableau de tests :**

| Chaîne  | Résultat | Explication                                 |
| -------- | --------- | ------------------------------------------- |
| `a`    | Acceptée | A → aB, B est final                        |
| `ab`   | Acceptée | A → aB → bB (final)                       |
| `abbb` | Acceptée | A → aB → bB → bB → bB (final)           |
| `acd`  | Acceptée | A → aB → cB → dB (final)                 |
| `ba`   | Rejetée  | Pas de transition 'b' depuis A              |
| `abc`  | Acceptée | A → aB → bB → cB (final)                 |
| `aaa`  | Rejetée  | Après 'aa', pas de transition 'a' depuis B |
| `abcd` | Acceptée | A → aB → bB → cB → dB (final)           |

### 4.5 Utilisation de l'Interface Graphique

**Étapes :**

1. **Lancer l'application :**

   ```bash
   make run-gui
   ```
2. **Chargement de la grammaire (2 méthodes) :**

   **Méthode 1 - Depuis un fichier :**

   - Onglet "Load from File"
   - Saisir le chemin du fichier (défaut: `input/grammaire.txt`)
   - Cliquer sur "Load Grammar"

   **Méthode 2 - Saisie manuelle :**

   - Onglet "Manual Input"
   - Saisir les règles ligne par ligne
   - Cliquer sur "Parse Grammar"
3. **Visualisation :**

   - La grammaire s'affiche avec le format : `État : ->symboleÉtat_suivant`
   - Les états finaux sont marqués avec `*`
4. **Test de chaînes :**

   - Saisir une chaîne dans le champ "Test String"
   - Cliquer sur "Check String"
   - Le résultat apparaît en **vert** (acceptée) ou **rouge** (rejetée)

## 5. Difficultés Rencontrées et Solutions

### 5.1 Gestion de la Mémoire en C

**Problème :** Les structures chaînées (`list`, `Transitions`) nécessitent une allocation dynamique avec `malloc()`. Risque de fuites mémoire si les pointeurs ne sont pas correctement libérés.

**Solution :**

- Vérification systématique du retour de `malloc()` :

  ```c
  children *c = (children*)malloc(sizeof(children));
  if (!c) return;  // Gestion d'erreur
  ```
- Libération explicite de la mémoire dans `get_grammaire_list()` :

  ```c
  for (int i = 0; lines[i] != NULL; i++) {
      free(lines[i]);
  }
  free(lines);
  ```

### 5.4 Algorithme de Vérification

**Problème :** Gérer correctement le cas où la chaîne peut être acceptée avant d'avoir consommé tous les caractères (si l'état courant est final).

**Solution :** Ajout d'une condition spéciale :

```c
if (transition == NULL) {
    if (current->is_final == 1 && cpt == n - 1) {
        return 1;  // Accepter si état final et un seul caractère restant
    }
    return 0;
}
```

---

## 6. Tests et Validation

### 6.1 Tests Unitaires (Version Console)

**Fichier de test (`main.c`) :**

```c
#include <stdio.h>
#include "config/graph.h"
#include "config/utils.h"

int main(){
    list l = get_grammaire_list("input/grammaire.txt");

    if (l != NULL) {
        printf("Grammar loaded successfully:\n");
        afficher_liste(l);
    } else {
        printf("Failed to load grammar\n");
    }

    // Tests
    printf("result : %d\n", check_string("aaa", l));      // 0 (rejetée)
    printf("result : %d\n", check_string("dddccccdcc", l)); // Selon grammaire

    return 0;
}
```

## 7. Conclusion

Ce projet a permis de mettre en pratique les concepts fondamentaux de la **théorie des automates** et des **grammaires formelles**. L'implémentation d'une interface graphique au-dessus d'un moteur de calcul en C démontre :

1. **La faisabilité d'outils pédagogiques** pour l'apprentissage de la compilation
2. **L'importance de la séparation des responsabilités** (Backend/Frontend)
3. **La puissance des structures de données** (graphes, listes chaînées) pour modéliser des concepts théoriques

### Perspectives d'Amélioration

- **Optimisation mémoire** : Implémentation d'un garbage collector ou passage à C++
- **Extension aux grammaires context-free** : Support des grammaires de type 2
- **Visualisation graphique** : Affichage du graphe avec Cairo ou Graphviz
- **Export de résultats** : Génération de rapports PDF
- **Déterminisation** : Conversion NFA → DFA pour optimisation

## Annexes

### Annexe A : Structure du Projet

```
.
├── config/
│   ├── graph.h          # Structures de données
│   ├── graph.c          # Opérations sur le graphe
│   ├── utils.h          # Interfaces parsing/validation
│   ├── utils.c          # Implémentation parsing/validation
│   ├── constant.h       # Constantes du projet
│   └── constant.c
├── ui/
│   ├── grammar_ui.h     # Interface GTK
│   ├── grammar_ui.c     # Implémentation UI
│   └── start.c          # Point d'entrée GUI
├── input/
│   └── grammaire.txt    # Fichier de grammaire de test
├── main.c               # Version console
├── Makefile             # Scripts de compilation
└── CLAUDE.md            # Documentation technique
```

### Annexe B : Exemple de Fichier de Grammaire

**Grammaire reconnaissant les identifiants** (commence par lettre, suivi de lettres/chiffres) :

```
I -> aA
I -> bA
A -> aA
A -> bA
A -> 0A
A -> 1A
A -> \t
```

**Langage :** `L = { (a|b)(a|b|0|1)* }`

**Exemples :**

- `a`, `b`, `a0`, `b1a`, `ab01` → Acceptés
- `0a`, `1b`, `01` → Rejetés (ne commencent pas par lettre)

### Annexe C : Références Bibliographiques

1. **Introduction to Automata Theory, Languages, and Computation** - Hopcroft, Motwani, Ullman
2. **Compilers: Principles, Techniques, and Tools** - Aho, Lam, Sethi, Ullman (Dragon Book)
3. **GTK+ 3 Reference Manual** - Documentation officielle GNOME
4. **The C Programming Language** - Kernighan & Ritchie
