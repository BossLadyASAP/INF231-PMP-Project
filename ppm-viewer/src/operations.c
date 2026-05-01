/**
 * @file operations.c
 * @brief Implémentation des opérations de base sur les images PPM
 * @author OUMAROU MVONGO CALEB ISRAEL (24G2771)
 * @date Octobre 2025
 * 
 * Ce fichier contient les implémentations des opérations de base du processeur d'images PPM.
 */

#include "image.h"
#include <stdio.h>
#include <stdlib.h>

// ============================================================================
// OPÉRATIONS DE BASE
// ============================================================================

/**
 * @brief Applique la dominante de couleur sur une image
 * @param img Image à traiter
 * @param channel Canal de couleur ('R', 'G', ou 'B')
 * @param value Valeur à ajouter/soustraire
 * @return 0 si succès, -1 si erreur
 */
int apply_dominant_color(Image* img, char channel, int value) {
    if (!img) {
        fprintf(stderr, "Erreur: Image invalide\n");
        return -1;
    }

    // Valider le canal
    if (channel != 'R' && channel != 'G' && channel != 'B') {
        fprintf(stderr, "Erreur: Canal invalide. Utilisez R, G ou B.\n");
        return -1;
    }

    // Valider la valeur (entre -255 et 255)
    if (value < -255 || value > 255) {
        fprintf(stderr, "Erreur: La valeur doit être entre -255 et 255\n");
        return -1;
    }

    // Appliquer la dominante de couleur
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            Pixel* p = &img->pixels[i][j];
            
            // Trouver la composante dominante
            int max_val = p->r;
            char max_channel = 'R';
            
            if (p->g > max_val) {
                max_val = p->g;
                max_channel = 'G';
            }
            
            if (p->b > max_val) {
                max_val = p->b;
                max_channel = 'B';
            }
            
            // Si le canal correspond, appliquer la modification
            if (max_channel == channel) {
                if (channel == 'R') {
                    p->r += value;
                    if (p->r > img->max_value) p->r = img->max_value;
                    if (p->r < 0) p->r = 0;
                } else if (channel == 'G') {
                    p->g += value;
                    if (p->g > img->max_value) p->g = img->max_value;
                    if (p->g < 0) p->g = 0;
                } else if (channel == 'B') {
                    p->b += value;
                    if (p->b > img->max_value) p->b = img->max_value;
                    if (p->b < 0) p->b = 0;
                }
            }
        }
    }
    
    return 0;
}

/**
 * @brief Convertit une image en niveaux de gris
 * @param img Image à convertir
 * @return 0 si succès, -1 si erreur
 */
int convert_to_grayscale(Image* img) {
    if (!img) {
        fprintf(stderr, "Erreur: Image invalide\n");
        return -1;
    }

    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            // Calcul de la moyenne des composantes RVB (méthode de la luminance)
            unsigned char gray = (unsigned char)(0.299 * img->pixels[i][j].r + 
                                                0.587 * img->pixels[i][j].g + 
                                                0.114 * img->pixels[i][j].b);
            
            // Appliquer la même valeur aux trois canaux
            img->pixels[i][j].r = gray;
            img->pixels[i][j].g = gray;
            img->pixels[i][j].b = gray;
        }
    }
    
    return 0;
}

/**
 * @brief Crée le négatif d'une image
 * @param src Image source
 * @param dst Image de destination (doit être allouée)
 * @return 0 si succès, -1 si erreur
 */
int create_negative(const Image* src, Image* dst) {
    if (!src || !dst) {
        fprintf(stderr, "Erreur: Images source ou destination invalides\n");
        return -1;
    }
    
    if (src->width != dst->width || src->height != dst->height) {
        fprintf(stderr, "Erreur: Les dimensions des images source et destination ne correspondent pas\n");
        return -1;
    }
    
    // Copier les métadonnées
    strcpy(dst->format, src->format);
    dst->max_value = src->max_value;
    
    // Créer le négatif
    for (int i = 0; i < src->height; i++) {
        for (int j = 0; j < src->width; j++) {
            dst->pixels[i][j].r = src->max_value - src->pixels[i][j].r;
            dst->pixels[i][j].g = src->max_value - src->pixels[i][j].g;
            dst->pixels[i][j].b = src->max_value - src->pixels[i][j].b;
        }
    }
    
    return 0;
}

/**
 * @brief Affiche les dimensions d'une image
 * @param img Image à analyser
 */
void display_size(const Image* img) {
    if (!img) {
        printf("Image: invalide\n");
        return;
    }
    
    printf("Dimensions: %d x %d pixels\n", img->width, img->height);
}

// ============================================================================
// FONCTIONS UTILITAIRES
// ============================================================================

/**
 * @brief Vérifie si une image est en niveaux de gris
 * @param img Image à vérifier
 * @return 1 si l'image est en niveaux de gris, 0 sinon
 */
int is_grayscale(const Image* img) {
    if (!img) return 0;
    
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            if (img->pixels[i][j].r != img->pixels[i][j].g || 
                img->pixels[i][j].g != img->pixels[i][j].b) {
                return 0;  // Les canaux ne sont pas égaux, pas en niveaux de gris
            }
        }
    }
    
    return 1;  // Tous les pixels sont en niveaux de gris
}

/**
 * @brief Calcule la luminosité moyenne d'une image
 * @param img Image à analyser
 * @return Luminosité moyenne (0-255) ou -1 en cas d'erreur
 */
float calculate_brightness(const Image* img) {
    if (!img) return -1.0f;
    
    unsigned long long sum = 0;
    int total_pixels = img->width * img->height;
    
    if (total_pixels == 0) return 0.0f;
    
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            // Utiliser la formule de luminance pour pondérer les canaux
            sum += (unsigned int)(0.299f * img->pixels[i][j].r + 
                                 0.587f * img->pixels[i][j].g + 
                                 0.114f * img->pixels[i][j].b);
        }
    }
    
    return (float)sum / total_pixels;
}
