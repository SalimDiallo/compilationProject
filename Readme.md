# Grammar Checker - Projet de Compilation

Un outil de vérification de grammaires formelles avec interface graphique GTK3.

## Fonctionnalités

- ✅ Chargement de fichiers de grammaire
- ✅ **Saisie manuelle de grammaire** (nouveau!)
- ✅ Représentation en graphe d'automate fini
- ✅ Vérification de chaînes de caractères
- ✅ Interface en ligne de commande
- ✅ Interface graphique GTK3 avec onglets

## Installation

### Prérequis

```bash
sudo apt-get install build-essential libgtk-3-dev
```

### Compilation

```bash
make              # Compile tout
make grammar_app  # Interface graphique
make main         # Version console
```

## Utilisation

### Interface Graphique

```bash
make run-gui
# ou
./grammar_app
```

L'interface dispose de **deux onglets** pour charger la grammaire:

**Onglet "Load from File":**
- Charger un fichier de grammaire (par défaut: `input/grammaire.txt`)
- Bouton "Load Grammar" pour parser le fichier

**Onglet "Manual Input":**
- Zone de texte pour saisir la grammaire directement
- Format: une règle par ligne (ex: `A -> bB`)
- Bouton "Parse Grammar" pour analyser la grammaire saisie

**Fonctionnalités communes:**
- Visualisation de la grammaire chargée avec états et transitions
- Zone de saisie pour tester des chaînes
- Résultat coloré: ✓ Acceptée (vert) ou ✗ Rejetée (rouge)

### Version Console

```bash
make run
# ou
./main
```

## Format de Grammaire

Les fichiers de grammaire utilisent le format suivant:

```
A -> bB
B -> cB
B -> \t
```

- `A -> bB` : depuis l'état A, avec l'entrée 'b', transition vers l'état B
- `\t` : indique un état terminal/final

## Structure du Projet

```
.
├── config/          # Structures de données et utilitaires
│   ├── graph.{c,h}  # Graphe d'automate
│   └── utils.{c,h}  # Parsing et validation
├── ui/              # Interface graphique GTK
│   ├── start.c      # Point d'entrée GUI
│   └── grammar_ui.{c,h}  # Implémentation UI
├── input/           # Fichiers de grammaire
├── main.c           # Version console
└── Makefile         # Scripts de compilation
```

## Exemples

Avec la grammaire par défaut (`A -> bB`, `B -> cB`, `B -> dB`, `B -> \t`):

- ✅ "bccc" → Acceptée
- ✅ "bdddc" → Acceptée
- ✗ "abc" → Rejetée (ne commence pas par 'b')
- ✅ "b" → Acceptée (état B est terminal)

## Fonctions Principales

### config/utils.c

- `get_grammaire_file(char *url)` - Lit un fichier et retourne tableau de lignes
- `get_grammaire_list(char *file_path)` - Parse fichier → graphe
- `parse_grammaire_text(char *text)` - Parse texte → graphe (pour saisie manuelle)
- `check_string(char *str, list l)` - Vérifie si chaîne acceptée

### config/graph.c

- `add_children()` - Ajoute transition à un état
- `add_node_to_liste()` - Ajoute état au graphe
- `get_transition()` - Trouve transition depuis état
- `get_node()` - Trouve état dans graphe
- `afficher_liste()` - Affiche graphe (console)
