# 📷 PPM Image Processor

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Language: C](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_library))
[![Status: Active](https://img.shields.io/badge/Status-Active-brightgreen.svg)](https://github.com/your-repo)
[![Build: Passing](https://img.shields.io/badge/Build-Passing-success.svg)](https://github.com/your-repo)

**Projet académique** - INF231 : Technique de Conception d'Algorithmes et Structures de Données  
**Coordinateur** : Pr Melatagia Yonta - Université de Yaoundé 1  
**Année académique** : 2025-2026

---

##  **Explication du Projet**

### **🎯 En quoi consiste le projet ?**
Nous développons une application en **langage C** qui permet de manipuler des images au format **PPM (Portable PixMap)**. Le programme offre une interface interactive où l'utilisateur peut taper des commandes pour effectuer diverses transformations sur les images.

### ** Termes Techniques Expliqués**

| Terme | Explication |
|-------|-------------|
| **PPM (Portable PixMap)** | Format d'image simple qui stocke les pixels en texte clair |
| **RGB** | Système de couleurs : Rouge, Vert, Bleu - chaque couleur entre 0 et 255 |
| **Pixel** | Point élémentaire d'une image avec 3 valeurs (R, G, B) |
| **P3** | Version "texte" du format PPM que nous utilisons |
| **Header** | En-tête du fichier avec le format, dimensions, valeur max |
| **Filtre médian** | Algorithme qui réduit le bruit en prenant la valeur du milieu des voisins |

### ** Exemple de Fichier PPM :**
```
P3                    # Format PPM version texte
# Image exemple       # Les commentaires commencent par #
4 4                   # Largeur: 4px, Hauteur: 4px
255                   # Valeur maximale des couleurs
255 0 0   0 255 0     # Pixel 1 (rouge), Pixel 2 (vert)
0 0 255   255 255 255 # Pixel 3 (bleu), Pixel 4 (blanc)
...                   # Suite des pixels
```

---

## 👥 Équipe de Développement

| Rôle | Membre | Matricule | Tâches Principales |
|------|---------|-----------|-------------------|
| **Leader** | MATAGNE DASSE PRESLIE CHANEL | 23V2294 | Coordination, structure données, intégration |
| **Développeur 2** | HENDRIX MENOUNGA LEGRAND | 24G2579 | Lecture/écriture fichiers PPM |
| **Développeur 3** | OUMAROU MVONGO CALEB ISRAEL | 24G2771 | Commandes de base (dom, gris, size, neg) |
| **Développeur 4** | VAMI NEGUEM YVO FREED | 24F2658 | Découpage et filtre médian |
| **Développeur 5** | FOKOU FOFOU STEDDY RONEL | 24G2579 | Interface utilisateur et tests |

---

## **Fonctionnalités Détaillées**

### **🎨 1. Dominante de Couleur** (`dom c val fichier.ppm`)
- **Explication** : Identifie pour chaque pixel sa couleur principale (R, G ou B) et l'ajuste
- **Exemple** : `dom R 50 image.ppm` → Augmente le rouge de 50 pour les pixels rouges dominants
- **Technique** : Compare les 3 valeurs RGB et applique la modification seulement à la couleur dominante

### **⚫ 2. Noir et Blanc** (`gris fichier.ppm`)
- **Explication** : Convertit l'image en niveaux de gris
- **Technique** : Pour chaque pixel, calcule la moyenne (R+G+B)/3 et l'applique aux 3 couleurs

### **🌗 3. Négatif** (`neg in.ppm out.ppm`)
- **Explication** : Crée l'inverse des couleurs (comme un photo-négatif)
- **Technique** : Nouvelle valeur = 255 - ancienne valeur

### **📐 4. Dimensions** (`size fichier.ppm`)
- **Explication** : Affiche la taille de l'image en pixels
- **Sortie** : `Largeur: 800 px, Hauteur: 600 px`

### **✂️ 5. Découpage** (`cut f.ppm l1 l2 c1 c2 res.ppm`)
- **Explication** : Extrait une partie rectangulaire de l'image
- **Paramètres** : l1→l2 (lignes), c1→c2 (colonnes)
- **Vérification** : S'assure que les coordonnées sont valides

### **🔍 6. Filtre Médian** (`fil in.ppm out.ppm`)
- **Explication** : Réduit le bruit en "lissant" l'image
- **Technique** : Pour chaque pixel, regarde ses 8 voisins, trie les valeurs et prend celle du milieu
- **Effet** : Supprime les pixels aberrants tout en préservant les contours

---

## 🏗️ Architecture du Projet

```
ppm-viewer/
├── images/
│   ├── input/                 # Images sources pour tests
│   └── output/                # Images générées par l'application
├── src/
│   ├── main.c                 # Interface utilisateur principale
│   ├── image.h                # Structures Image/Pixel et prototypes
│   ├── ppm_io.c               # Gestion lecture/écriture PPM
│   ├── operations.c           # Implémentation des commandes de base
│   └── filters.c              # Algorithmes de découpage et filtres
├── Makefile                   # Système de compilation
└── README.md
```

---

##  **Installation & Utilisation Rapide**

### **Compilation**
```bash
git clone https://github.com/[username]/ppm-viewer.git
cd ppm-viewer
make
```

### **Utilisation Interactive**
```bash
./ppmviewer

ppmviewer> size image.ppm
100 x 200

ppmviewer> gris image.ppm
✅ Opération réussie - image_gris.ppm créé

ppmviewer> quit
```

---

##  **Comment tester le projet ?**

1. **Placez** une image PPM dans `images/input/`
2. **Lancez** l'application : `./ppmviewer`
3. **Testez** les commandes une par une
4. **Vérifiez** les résultats dans `images/output/`

---

##  Exemple de test pour `fil` et `cut`

```text
╔══════════════════════════════════════════════════════════════╗
║          📷  PPM IMAGE PROCESSOR  📷                         ║
╠══════════════════════════════════════════════════════════════╣
║  Projet INF231 - Université de Yaoundé 1                    ║
║  Équipe de développement 2025-2026                          ║
╚══════════════════════════════════════════════════════════════╝

Tapez 'help' pour voir les commandes disponibles
Tapez 'quit' pour quitter

ppmviewer > fil images/input/test.ppm images/output/test_fil.ppm
✅ Fichier créé: images/output/test_fil.ppm
opération effectuée

ppmviewer > cut images/input/test.ppm 1 2 1 2 images/output/test_cut.ppm
✅ Fichier créé: images/output/test_cut.ppm
opération effectuée

ppmviewer > quit 
Au revoir! 👋
```

Astuce: depuis le dossier `ppm-viewer/`, utilisez les chemins `images/...` pour vos entrées/sorties.

---

##  **Problèmes Courants & Solutions**

| Problème | Solution |
|----------|----------|
| **Fichier non trouvé** | Vérifier le chemin et l'extension `.ppm` |
| **Format non supporté** | S'assurer que c'est un PPM P3 (pas P6) |
| **Découpage impossible** | Vérifier que l1 < l2 et c1 < c2 dans les limites |

---

## 📞 **Contact Équipe**

**Leader** : MATAGNE DASSE PRESLIE CHANEL - 23V2294 - chanelledasse1@gmail.com    
**Encadrant** : Pr Melatagia Yonta - Université de Yaoundé 1

---

<div align="center">

**✨ Projet réalisé avec soin par l'équipe de développement ✨**

*Dernière mise à jour : Octobre 2025*

</div>
**Projet académique** - INF231 : Technique de Conception d'Algorithmes et Structures de Données  

**Coordinateur** : Pr Melatagia - Université de Yaoundé 1  

**Année académique** : 2025-2026
