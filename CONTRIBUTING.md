# Contribuer à R-Type

Ce guide explique comment on s'organise côté dev : le GitHub Project, les issues, les branches et les commits.

## 1. Le GitHub Project

Le kanban se trouve ici : [R-Type - Organisation](https://github.com/users/Yoyott29/projects/3).

Chaque carte correspond à une issue (ou une PR) du repo `Yoyott29/R-Type`. Elle a les champs suivants :

| Champ    | Valeurs                                                | Rôle                      |
| -------- | ------------------------------------------------------ | ------------------------- |
| Status   | `Backlog` → `Ready` → `In progress` → `In review` → `Done` | où en est la tâche        |
| Priority | `P0` (urgent) · `P1` · `P2`                            | ce qu'on traite en premier |
| Size     | `XS` · `S` · `M` · `L` · `XL`                          | estimation de l'effort     |

### Ce qui est automatique

| Action                           | Effet sur le kanban                                |
| -------------------------------- | -------------------------------------------------- |
| Créer une issue sur le repo      | la carte est ajoutée automatiquement au project    |
| Créer une sous-issue             | la carte est ajoutée aussi, et la parente affiche l'avancement (`Sub-issues progress`) |
| Lier une PR à l'issue            | la PR apparaît sur la carte (`Linked pull requests`) |
| Merger la PR / fermer l'issue    | la carte passe en `Done`                           |

### Ce qui reste manuel

- Remplir `Priority` et `Size` à la création.
- Déplacer la carte en `Ready`, `In progress` et `In review` au fil du travail.
- S'assigner l'issue quand on la prend.


## 2. Créer une issue

Une issue = une tâche = une branche.

### Titre

```
[type](scope): action à l'infinitif, courte
```

- **type** : `feat` · `fix` · `refactor` · `perf` · `test` · `docs` · `chore` · `ci`
- **scope** : la partie du projet concernée (`server`, `client`, `engine`, `network`, `ecs`, `ci`, `docs`…)
- 72 caractères maximum, pas de point final, en français.

Exemples :

```
[fix](network): corriger la perte de paquets à la reconnexion
[feat](client): afficher le score en fin de partie
```

### Corps

```markdown
Description courte du problème ou de la fonctionnalité.

**Branche :** `Nom_De_La_Branche`

### Tâches
- [ ] étape 1
- [ ] étape 2
- [ ] vérifier que les tests passent
```

Pour une tâche normale (jusqu'à `L`), les étapes vont dans une **checklist** (`- [ ]`).

### Grosse fonctionnalité : issue parente + sous-issues

Quand une fonctionnalité est trop grosse pour une seule branche (`XL`, plusieurs personnes dessus), on la découpe :

- une **issue parente** en `XL`, qui décrit l'ensemble et liste ses sous-issues ;
- des **sous-issues** liées à la parente, une par branche, avec le label `sub-issue`.

Exemple : l'ECS, avec la parente #24 et les sous-issues #25, #26 et #27.

Corps de l'issue parente :

```markdown
Description de la fonctionnalité.

### Sous-issues
- #25 — partie 1 (`Branche_1`)
- #26 — partie 2 (`Branche_2`)

### Branches
- La branche `Manage_Xxx` doit être créée depuis `dev`.
- La branche de chaque sous-issue doit être créée depuis `Manage_Xxx`, puis mergée dans `Manage_Xxx` par une PR.
- `Manage_Xxx` est mergée dans `dev` uniquement quand toutes les sous-issues sont terminées.

### Tâches
- [ ] Toutes les sous-issues sont fermées
- [ ] PR `Manage_Xxx` → `dev` mergée
```

Une sous-issue rappelle sa parente et ses dépendances en première ligne (`Partie 2 de l'ECS (#24). Dépend de #25.`), puis suit le corps normal.

### Ajouter un label

`bug` pour un problème, `enhancement` pour une nouvelle fonctionnalité, `documentation` pour la doc, et `sub-issue` en plus pour toute sous-issue.

### En ligne de commande

```sh
gh issue create -R Yoyott29/R-Type \
  --title "[fix](network): corriger la perte de paquets à la reconnexion" \
  --label bug \
  --body-file issue.md

# sous-issue : --parent lie l'issue à sa parente
gh issue create -R Yoyott29/R-Type \
  --title "[feat](ecs): ajouter le registry pour gérer entités et composants" \
  --label enhancement --label sub-issue \
  --parent 24 \
  --body-file issue.md
```

## 3. Branches

```
Ma_Branche  ──PR──▶  dev  ──PR──▶  main
```

Pour une issue parente avec sous-issues :

```
dev
 └── Manage_Ecs                 (issue parente)
      ├── Ecs_Core      ──PR──▶ Manage_Ecs
      ├── Ecs_Registry  ──PR──▶ Manage_Ecs
      └── Ecs_Systems   ──PR──▶ Manage_Ecs
Manage_Ecs  ──PR──▶  dev        (quand toutes les sous-issues sont terminées)
```

- On ne peut pas pousser directement sur `main` ni sur `dev` : on passe toujours par une PR.
- Les branches sont nommées en `Mots_Avec_Majuscules` (ex : `Manage_Documentation`, `Add_Useful_Workflows`).
- Les tests (`tests/tests.sh`) se lancent sur la PR `dev` → `main`. Ils ne tournent pas sur les PR vers une branche parente : il faut les lancer en local avant de merger.

## 4. Norme de commit

```
[X.YY] [branche] Type: message en anglais au passé
```

- **`[X.YY]`** : les initiales de la personne qui commit, avec la première lettre du prénom et les deux premières lettres du nom, en majuscules. Exemple : Eliott Duchene donne `[E.DU]`.
- **`[branche]`** : la branche sur laquelle on commit.

| Type     | Quand l'utiliser                  |
| -------- | --------------------------------- |
| `Feat`   | nouvelle fonctionnalité           |
| `Fix`    | correction de bug                 |
| `Test`   | ajout ou modification de tests    |
| `Merge`  | merge d'une branche dans une autre |
| `Init`   | initialisation                    |

Exemples :

```
[E.DU] [Manage_Documentation] Feat: added developer about page
[E.DU] [Manage_Documentation] Fix: fixed node version
[E.DU] [dev] Merge: merged dev into main
```

Les messages de commit servent aussi à générer les notes de release (voir le workflow `auto-version`) : un message clair donne un changelog clair.

## 5. Lier son travail à l'issue

- Dans un commit ou une PR, écrire `#23` crée un lien vers l'issue 23 (visible dans l'historique de l'issue).
- Dans la description de la PR, écrire `Closes #23` ferme l'issue **quand la PR est mergée dans `main`** (la branche par défaut). Une PR mergée dans `dev` ne ferme pas l'issue : il faut la fermer à la main ou attendre la PR `dev` → `main`.
- Même chose pour les sous-issues : une PR mergée dans la branche parente ne les ferme pas. On ferme la sous-issue à la main une fois sa PR mergée.

## Récap

1. Créer l'issue (titre normé, checklist, label) → la carte arrive sur le kanban.
2. Remplir `Priority` et `Size`, s'assigner, passer la carte en `In progress`.
3. Créer la branche et committer en suivant la norme.
4. Ouvrir la PR vers `dev` avec `#numéro` dans la description → passer la carte en `In review`.
5. Merge → la carte passe en `Done`.

Pour une grosse fonctionnalité : issue parente `XL` + sous-issues (label `sub-issue`), sous-branches mergées dans la branche parente, puis la branche parente mergée dans `dev`.
