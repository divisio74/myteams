# Git — Travailler avec les branches

Quand tu travailles seul ou en équipe, tu n'édites jamais directement `main`. Tu crées une branche, tu travailles dedans, puis tu reintègres ton travail une fois qu'il est prêt. Ce guide couvre ce cycle complet, en expliquant *pourquoi* chaque étape existe, pas seulement *comment* la faire.

---

## Pourquoi des branches ?

`main` est censé rester stable — c'est ce qui tourne en production, ce que les collègues clonent comme base de travail. Si tout le monde pushait directement dessus, le moindre bug en cours de dev casserait tout le monde.

Les branches sont des pointeurs vers un commit. Créer une branche ne copie rien — c'est juste un curseur qui va avancer indépendamment de `main` pendant que tu travailles. C'est pour ça que c'est quasi-instantané.

---

## 1. Créer une branche

Avant de toucher quoi que ce soit, tu crées une branche dédiée :

```bash
git switch -c feat/nom-de-la-fonctionnalité
```

**ATTENTION : La branche créée partira de la branche à laquelle vous êtes actuellement, il est donc généralement conseillé de partir de `main` (donc la commande est à faire sur la branche `main`)**# main est maintenant en retard, ta branche a l'avance


`-c` signifie *create*. La branche est créée à partir du commit sur lequel tu te trouvais (généralement la tête de `main`). Tu y es basculé immédiatement.

> **Sur le nommage** : le préfixe `feat/`, `fix/`, `chore/`, `refactor/` aide à comprendre d'un coup d'œil l'intention d'une branche dans la liste des PR, sans avoir à lire la description.

`git switch` est la commande moderne. `git checkout -b` fait la même chose et tu la croiseras souvent dans la doc et les projets existants — les deux sont corrects.

Pour vérifier où tu es :

```bash
git branch
```

La branche active a une étoile. Pour voir aussi les branches distantes (sur GitHub/GitLab) : `git branch -a`.

---

## 2. Travailler et committer

Dans ta branche, tu travailles normalement. Les commits que tu crées n'existent que là — `main` ne les voit pas.

```bash
git add .
git commit -m "feat: ajout du provider Google OAuth"
```

Une fois que tu veux partager ta branche (pour une review, ou juste pour avoir une sauvegarde distante) :

```bash
git push -u origin feat/auth-google
```
**La commande ci-dessus est évitable avec le bouton push de VSCode**

Le `-u` (ou `--set-upstream`) lie ta branche locale à la branche distante du même nom. Les fois suivantes, un simple `git push` suffit — Git sait où envoyer.

---

## 3. Garder ta branche synchronisée avec `main`

C'est souvent la partie qu'on néglige, et qui génère des conflits massifs au moment du merge. Pendant que tu travailles, `main` avance : des collègues mergent leurs features, des hotfixes sont poussés, des dépendances changent. Si tu laisses ta branche diverger trop longtemps, la réconciliation devient pénible.

La bonne pratique : resynchroniser régulièrement, au moins une fois par jour sur des projets actifs :

### Avec rebase (recommandée pour les feature branches)

```bash
git fetch origin
git rebase origin/main
```

`git fetch` télécharge les changements distants sans modifier ta copie de travail. `git rebase` rejoue ensuite tes commits *par-dessus* la nouvelle tête de `main`.

Ce que ça donne visuellement :

```
Avant :

main : A — B — C
                \
feat :           X — Y

Après rebase :

main : A — B — C
                \
feat :           X' — Y'
```

Tes commits `X` et `Y` deviennent `X'` et `Y'` — leur contenu est identique, mais leur parent a changé. L'historique est linéaire : quand tu mergeras, il n'y aura pas de commit de merge disgracieux, juste une succession propre.

### Quand il y a des conflits

Cette commande peut s'arrêter sur des conflits. Git te liste les fichiers concernés :

```bash
git status
# "both modified: src/auth/config.ts"
```

Dans chaque fichier en conflit, Git insère des marqueurs :

```
  // le code de main
```

Tu édites le fichier pour garder ce qui doit l'être (les deux, l'un ou l'autre, ou une fusion des deux), tu supprimes les marqueurs **(OU ALORS L'OUTIL MERGE RESOLVE DE VSCODE T'ASSISTE DANS CE PROCESSUS)**, puis :

```bash
git add fichier-fixé

# Si tu étais en train de rebaser :
git rebase --continue
```

Si tu réalises que tu t'es planté et que tu veux repartir de zéro :

```bash
git rebase --abort
```

Cette commande remetq tout dans l'état qu'il avait avant que tu lances l'opération.

---

## 4. Merger ta branche dans `main`

### Via une Pull Request (le workflow standard en équipe)

C'est le chemin habituel. Tu pousses ta branche, tu ouvres une PR sur GitHub ou GitLab, un ou plusieurs collègues la relisent, et le merge se fait via l'interface web.

Ce workflow a un avantage important : avant que le merge soit accepté, la plateforme peut vérifier que ta branche est à jour avec `main` et que les tests passent. Tu n'as pas à t'en souvenir manuellement.

### En local (projets perso, scripts de release, urgences)

Si tu merges toi-même :

```bash
# D'abord, revenir sur main et s'assurer qu'il est à jour
git switch main
git pull origin main

# Merger ta branche
git merge feat/auth-google
```

Si ta branche était proprement rebasée sur `main` (cf. étape 3), Git va faire un *fast-forward* : il avance simplement le pointeur de `main` jusqu'au dernier commit de ta branche. Pas de commit de merge, historique parfaitement linéaire.

Si tu préfères écraser tous tes commits de travail en un seul commit propre :

```bash
git merge --squash feat/auth-google
git commit -m "feat: authentification Google OAuth"
```

Utile quand ta branche contient beaucoup de "wip", "fix typo", "oups" — tu livres un historique propre sur `main` même si ton historique de travail était chaotique.

***__Ne pas oublier de faire un push après__***
```
git push
```

---

## 5. Nettoyer après le merge

Une branche mergée n'a plus de raison d'exister. La laisser traîner encombre la liste et peut prêter à confusion.

```bash
# Supprimer la branche locale
git branch -d feat/auth-google

# Supprimer la branche distante
git push origin --delete feat/auth-google
```

`-d` (minuscule) refuse de supprimer une branche non mergée — c'est un filet de sécurité. Si tu veux forcer la suppression d'une branche dont tu n'as plus besoin même si elle n'est pas mergée : `-D` (majuscule).

Pour nettoyer les références locales vers des branches distantes qui ont déjà été supprimées côté serveur :

```bash
git fetch --prune
```

---

## Le cycle complet en un coup d'œil

```
main:  ●─────────────────────────────────●  (stable, ne touche pas directement)
            \                           /
feat:        ●────●────●────●────●────●
             ↑                        ↑
         git switch -c          PR / git merge
```

Ce schéma, répété pour chaque feature, chaque fix, chaque changement significatif, c'est l'essentiel du workflow Git quotidien.

---

## Quelques situations courantes

**Tu as commencé à travailler sur `main` par erreur.**
Pas de panique. Crée la branche maintenant — elle va récupérer tes commits en cours :

```bash
git switch -c feat/ma-feature
# main est maintenant en retard, ta branche a l'avance
```

**Tu veux voir ce que ta branche a changé par rapport à `main`.**

```bash
git diff main...HEAD
```

Les trois points sont importants : ça te montre uniquement les changements que *ta branche* a introduits, pas les changements de `main` depuis la divergence.

**Tu as besoin d'un commit spécifique d'une autre branche.**

```bash
git cherry-pick <sha>
```

Applique ce commit précis dans ta branche courante, sans merger tout le reste.

**Tu veux visualiser l'arbre de commits.**

```bash
git log --oneline --graph --all
```

Merci Leo Cottaz !