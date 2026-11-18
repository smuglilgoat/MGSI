# MGSI

Travaux pratiques de modélisation géométrique et de synthèse d'image, en C++ et OpenGL.

## Contenu

| Dossier | Sujet |
| --- | --- |
| `Partie_C_Gentil/TP_Automate` | Automates fractals décrits dans un fichier texte, exportés en PostScript |
| `Partie_C_Gentil/WORK_TP_IFS` | Systèmes de fonctions itérées (IFS) affichés en OpenGL |
| `Partie_C_Gentil/TP_Chaos_Game` | Jeu du chaos |
| `Partie_C_Gentil/TP_Courbe` | Courbes de Bézier, repère de Frenet et tracé de tubes |
| `Partie_C_Gentil/WORK_TP_Surface` | Squelette de TP pour l'affichage de surfaces |
| `Partie_C_Gentil/WORK_TP_Terrain_Gen` | Génération de terrain par diamond-square |
| `Partie_C_Gentil/TP_Shader_Toon` | Shaders GLSL (Phong, Gooch, environment map) sur un tore en VBO |
| `Partie_S_Lanquetin/TP_Subdivision` | Subdivision de maillages : Doo-Sabin, Catmull-Clark, Loop et √3 |

## Compilation

Chaque TP dispose de son `Makefile` :

```bash
cd Partie_C_Gentil/TP_Chaos_Game
make
./main
```

Pour `TP_Subdivision`, l'exécutable s'appelle `surface_subdivision` :

```bash
cd Partie_S_Lanquetin/TP_Subdivision
make
./surface_subdivision
```

Dépendances : OpenGL, GLU, GLUT/freeglut et GLEW. Certains TPs utilisent Armadillo.

## Notes

- `TP_Automate/doc_automate.txt` documente le format des fichiers d'automates et l'usage de `./postscript fichier.txt nb_iterations`.
- Les maillages produits par `TP_Subdivision` sont au format Geomview (`.off`).
- Les binaires et fichiers objets compilés sont présents dans le dépôt.
