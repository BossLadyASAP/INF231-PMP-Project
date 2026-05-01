/**
 * @file ppm_io.c
 * @brief Gestion de la lecture/écriture des fichiers PPM
 * @author HENDRIX MENOUNGA LEGRAND (24G2579)
 * @date Octobre 2025
 * 
 * Implémentation des fonctions de lecture/écriture des fichiers PPM
 * pour le projet PPM Image Processor.
 */

#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// FONCTIONS D'AIDE
// ============================================================================

/**
 * @brief Ignore les commentaires dans un fichier PPM
 * @param file Pointeur vers le fichier
 */
void skip_comments(FILE* file) {
    if (!file) return;
    
    int c;
    while ((c = fgetc(file)) != EOF) {
        // Si on trouve un caractère de commentaire
        if (c == '#') {
            // Lire jusqu'à la fin de la ligne
            while ((c = fgetc(file)) != '\n' && c != EOF);
        } else if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            // On est sur un caractère qui n'est pas un espace ni un commentaire
            // On revient en arrière pour que le prochain fscanf lise ce caractère
            ungetc(c, file);
            break;
        }
    }
}

// ============================================================================
// LECTURE D'UN FICHIER PPM
// ============================================================================

/**
 * @brief Lit un fichier PPM et charge l'image en mémoire
 * @param filename Nom du fichier PPM à lire
 * @return Pointeur vers l'image chargée ou NULL en cas d'erreur
 */
Image* read_ppm(const char* filename) {
    if (!filename) {
        fprintf(stderr, "Erreur: Nom de fichier invalide\n");
        return NULL;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    // Lire l'en-tête
    char format[3];
    if (fscanf(file, "%2s", format) != 1 || strcmp(format, "P3") != 0) {
        fprintf(stderr, "Erreur: Format PPM invalide. Seul P3 est supporté.\n");
        fclose(file);
        return NULL;
    }

    // Lire les dimensions
    int width, height, max_value;
    
    skip_comments(file);  // Passer les commentaires
    if (fscanf(file, "%d %d", &width, &height) != 2 || width <= 0 || height <= 0) {
        fprintf(stderr, "Erreur: Dimensions d'image invalides\n");
        fclose(file);
        return NULL;
    }
    
    skip_comments(file);  // Passer les commentaires
    if (fscanf(file, "%d", &max_value) != 1 || max_value <= 0 || max_value > 255) {
        fprintf(stderr, "Erreur: Valeur maximale de couleur invalide\n");
        fclose(file);
        return NULL;
    }

    // Créer l'image
    Image* img = create_image(width, height);
    if (!img) {
        fclose(file);
        return NULL;
    }
    
    strcpy(img->format, format);
    img->max_value = max_value;

    // Lire les pixels
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int r, g, b;
            skip_comments(file);
            if (fscanf(file, "%d %d %d", &r, &g, &b) != 3) {
                fprintf(stderr, "Erreur: Format de pixel invalide à la position (%d, %d)\n", i, j);
                free_image(img);
                fclose(file);
                return NULL;
            }
            
            // Valider les valeurs des couleurs
            if (r < 0 || r > max_value || g < 0 || g > max_value || b < 0 || b > max_value) {
                fprintf(stderr, "Erreur: Valeur de couleur hors limites à la position (%d, %d)\n", i, j);
                free_image(img);
                fclose(file);
                return NULL;
            }
            
            // Stocker le pixel
            img->pixels[i][j].r = r;
            img->pixels[i][j].g = g;
            img->pixels[i][j].b = b;
        }
    }

    fclose(file);
    return img;
}

// ============================================================================
// ÉCRITURE D'UN FICHIER PPM
// ============================================================================

/**
 * @brief Écrit une image en mémoire dans un fichier PPM
 * @param img Image à sauvegarder
 * @param filename Nom du fichier de sortie
 * @return 0 si succès, -1 si erreur
 */
int write_ppm(const Image* img, const char* filename) {
    if (!img || !filename) {
        fprintf(stderr, "Erreur: Paramètres invalides pour l'écriture\n");
        return -1;
    }

    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Erreur lors de la création du fichier");
        return -1;
    }

    // Écrire l'en-tête
    fprintf(file, "%s\n", img->format);
    fprintf(file, "# Fichier généré par PPM Image Processor\n");
    fprintf(file, "%d %d\n", img->width, img->height);
    fprintf(file, "%d\n", img->max_value);

    // Écrire les pixels
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            fprintf(file, "%d %d %d\t", 
                   img->pixels[i][j].r, 
                   img->pixels[i][j].g, 
                   img->pixels[i][j].b);
            
            // Nouvelle ligne tous les 5 pixels pour une meilleure lisibilité
            if (j > 0 && j % 5 == 0) {
                fprintf(file, "\n");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}

// ============================================================================
// FONCTIONS DE DÉBOGAGE
// ============================================================================

/**
 * @brief Affiche les informations d'une image (pour débogage)
 * @param img Image à afficher
 */
void print_image_info(const Image* img) {
    if (!img) {
        printf("Image: NULL\n");
        return;
    }
    
    printf("=== Image Info ===\n");
    printf("Format: %s\n", img->format);
    printf("Dimensions: %d x %d\n", img->width, img->height);
    printf("Valeur max: %d\n", img->max_value);
    
    // Afficher un aperçu des premiers pixels (2x2)
    if (img->width >= 2 && img->height >= 2) {
        printf("\nAperçu des pixels (2x2):\n");
        for (int i = 0; i < 2 && i < img->height; i++) {
            for (int j = 0; j < 2 && j < img->width; j++) {
                printf("(%3d,%3d,%3d) ", 
                       img->pixels[i][j].r, 
                       img->pixels[i][j].g, 
                       img->pixels[i][j].b);
            }
            printf("\n");
        }
    }
    printf("=================\n");
}
