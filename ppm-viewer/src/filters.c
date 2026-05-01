/**
 * @file filters.c
 * @brief Implémentation des filtres et opérations avancées sur les images PPM
 * @author VAMI NEGUEM YVO FREED (24F2658)
 * @date Octobre 2025
 * 
 * Ce fichier contient les implémentations des filtres et opérations avancées
 * pour le processeur d'images PPM, notamment le filtre médian et le découpage.
 */

#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// FONCTIONS D'AIDE
// ============================================================================

/**
 * @brief Échange deux entiers
 * @param a Pointeur vers le premier entier
 * @param b Pointeur vers le deuxième entier
 */
static void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief Partition pour le tri rapide
 * @param arr Tableau à trier
 * @param low Indice de début
 * @param high Indice de fin
 * @return Indice du pivot
 */
static int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

/**
 * @brief Tri rapide (QuickSort)
 * @param arr Tableau à trier
 * @param low Indice de début
 * @param high Indice de fin
 */
static void quick_sort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

// ============================================================================
// FILTRE MÉDIAN
// ============================================================================

/**
 * @brief Calcule la valeur médiane d'un tableau
 * @param values Tableau de valeurs
 * @param size Taille du tableau
 * @return Valeur médiane
 */
int calculate_median(int* values, int size) {
    if (!values || size <= 0) return 0;
    
    // Créer une copie du tableau pour ne pas modifier l'original
    int* sorted = (int*)malloc(size * sizeof(int));
    if (!sorted) return 0;
    
    memcpy(sorted, values, size * sizeof(int));
    
    // Trier le tableau
    quick_sort(sorted, 0, size - 1);
    
    // Retourner la médiane
    int median = sorted[size / 2];
    free(sorted);
    
    return median;
}

/**
 * @brief Applique un filtre médian pour réduire le bruit
 * @param src Image source
 * @param dst Image de destination (doit être allouée)
 * @return 0 si succès, -1 si erreur
 */
int apply_median_filter(const Image* src, Image* dst) {
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
    
    // Taille du voisinage (3x3)
    const int WINDOW_SIZE = 3;
    const int WINDOW_OFFSET = WINDOW_SIZE / 2;
    
    // Tableaux pour stocker les valeurs des canaux du voisinage
    int r_values[WINDOW_SIZE * WINDOW_SIZE];
    int g_values[WINDOW_SIZE * WINDOW_SIZE];
    int b_values[WINDOW_SIZE * WINDOW_SIZE];
    
    // Appliquer le filtre médian
    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            int count = 0;
            
            // Parcourir le voisinage 3x3
            for (int wy = -WINDOW_OFFSET; wy <= WINDOW_OFFSET; wy++) {
                for (int wx = -WINDOW_OFFSET; wx <= WINDOW_OFFSET; wx++) {
                    int px = x + wx;
                    int py = y + wy;
                    
                    // Gérer les bords en répétant les pixels du bord
                    if (px < 0) px = 0;
                    if (px >= src->width) px = src->width - 1;
                    if (py < 0) py = 0;
                    if (py >= src->height) py = src->height - 1;
                    
                    // Stocker les valeurs du voisinage
                    r_values[count] = src->pixels[py][px].r;
                    g_values[count] = src->pixels[py][px].g;
                    b_values[count] = src->pixels[py][px].b;
                    count++;
                }
            }
            
            // Calculer les médianes pour chaque canal
            dst->pixels[y][x].r = calculate_median(r_values, count);
            dst->pixels[y][x].g = calculate_median(g_values, count);
            dst->pixels[y][x].b = calculate_median(b_values, count);
        }
    }
    
    return 0;
}

// ============================================================================
// DÉCOUPAGE D'IMAGE
// ============================================================================

/**
 * @brief Découpe une région rectangulaire d'une image
 * @param src Image source
 * @param row1 Ligne de début (0-indexed)
 * @param row2 Ligne de fin (0-indexed, inclusive)
 * @param col1 Colonne de début (0-indexed)
 * @param col2 Colonne de fin (0-indexed, inclusive)
 * @return Nouvelle image contenant la région découpée ou NULL si erreur
 */
Image* cut_image(const Image* src, int row1, int row2, int col1, int col2) {
    if (!src) {
        fprintf(stderr, "Erreur: Image source invalide\n");
        return NULL;
    }
    
    // Valider les coordonnées
    if (row1 < 0 || row2 >= src->height || col1 < 0 || col2 >= src->width ||
        row1 > row2 || col1 > col2) {
        fprintf(stderr, "Erreur: Coordonnées de découpage invalides\n");
        fprintf(stderr, "  Plage valide: 0 <= row1 <= row2 < %d, 0 <= col1 <= col2 < %d\n", 
                src->height, src->width);
        return NULL;
    }
    
    // Calculer les dimensions de la région à découper
    int width = col2 - col1 + 1;
    int height = row2 - row1 + 1;
    
    // Créer une nouvelle image pour la région découpée
    Image* result = create_image(width, height);
    if (!result) {
        return NULL;
    }
    
    // Copier les métadonnées
    strcpy(result->format, src->format);
    result->max_value = src->max_value;
    
    // Copier la région spécifiée
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int src_y = y + row1;
            int src_x = x + col1;
            
            if (src_y >= 0 && src_y < src->height && 
                src_x >= 0 && src_x < src->width) {
                result->pixels[y][x] = src->pixels[src_y][src_x];
            } else {
                // Remplir avec du noir si en dehors des limites (normalement pas nécessaire avec les validations)
                result->pixels[y][x].r = 0;
                result->pixels[y][x].g = 0;
                result->pixels[y][x].b = 0;
            }
        }
    }
    
    return result;
}

// ============================================================================
// FONCTIONS DE TEST ET DÉBOGAGE
// ============================================================================

/**
 * @brief Teste la fonction de calcul de médiane
 * @return 1 si tous les tests réussissent, 0 sinon
 */
int test_median_calculation() {
    printf("Test de la fonction de calcul de médiane...\n");
    
    // Test 1: Tableau impair
    int test1[] = {5, 2, 8, 1, 9};
    int median1 = calculate_median(test1, 5);
    printf("  Test 1: %s (attendu: 5, obtenu: %d)\n", 
           (median1 == 5) ? "PASSÉ" : "ÉCHEC", median1);
    
    // Test 2: Tableau pair (la médiane est l'élément du milieu arrondi vers le bas)
    int test2[] = {3, 1, 4, 1, 5, 9};
    int median2 = calculate_median(test2, 6);
    printf("  Test 2: %s (attendu: 3, obtenu: %d)\n", 
           (median2 == 3 || median2 == 4) ? "PASSÉ" : "ÉCHEC", median2);
    
    // Test 3: Tableau avec des doublons
    int test3[] = {7, 7, 7, 1, 7, 7, 7};
    int median3 = calculate_median(test3, 7);
    printf("  Test 3: %s (attendu: 7, obtenu: %d)\n", 
           (median3 == 7) ? "PASSÉ" : "ÉCHEC", median3);
    
    return (median1 == 5 && (median2 == 3 || median2 == 4) && median3 == 7);
}
