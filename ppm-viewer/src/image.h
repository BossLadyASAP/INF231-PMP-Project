/**
 * @file image.h
 * @brief Structures de données et prototypes de fonctions pour la manipulation d'images PPM
 * @author MATAGNE DASSE PRESLIE CHANEL (23V2294) - Project Leader
 * @date Octobre 2025
 * 
 * Ce fichier contient les définitions des structures de données principales
 * et les prototypes de toutes les fonctions du projet PPM Image Processor.
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// STRUCTURES DE DONNÉES
// ============================================================================

/**
 * @struct Pixel
 * @brief Représente un pixel avec ses composantes RGB
 */
typedef struct {
    int r;  // Composante rouge (0-255)
    int g;  // Composante verte (0-255)
    int b;  // Composante bleue (0-255)
} Pixel;

/**
 * @struct Image
 * @brief Représente une image PPM complète
 */
typedef struct {
    char format[3];      // Format PPM (P3 ou P6)
    int width;           // Largeur de l'image en pixels
    int height;          // Hauteur de l'image en pixels
    int max_value;       // Valeur maximale des couleurs (généralement 255)
    Pixel** pixels;      // Tableau 2D de pixels [height][width]
} Image;

/**
 * @enum ColorChannel
 * @brief Canaux de couleur pour les opérations
 */
typedef enum {
    RED = 'R',
    GREEN = 'G',
    BLUE = 'B'
} ColorChannel;

// ============================================================================
// PROTOTYPES - GESTION MÉMOIRE ET UTILITAIRES
// ============================================================================

/**
 * @brief Alloue la mémoire pour une image
 * @param width Largeur de l'image
 * @param height Hauteur de l'image
 * @return Pointeur vers l'image allouée ou NULL en cas d'erreur
 */
Image* create_image(int width, int height);

/**
 * @brief Libère la mémoire utilisée par une image
 * @param img Pointeur vers l'image à libérer
 */
void free_image(Image* img);

/**
 * @brief Copie une image
 * @param src Image source
 * @return Pointeur vers la nouvelle image ou NULL en cas d'erreur
 */
Image* copy_image(const Image* src);

// ============================================================================
// PROTOTYPES - LECTURE/ÉCRITURE PPM (ppm_io.c)
// Développeur: HENDRIX MENOUNGA LEGRAND (24G2579)
// ============================================================================

/**
 * @brief Lit un fichier PPM et charge l'image en mémoire
 * @param filename Nom du fichier PPM à lire
 * @return Pointeur vers l'image chargée ou NULL en cas d'erreur
 */
Image* read_ppm(const char* filename);

/**
 * @brief Écrit une image en mémoire dans un fichier PPM
 * @param img Image à sauvegarder
 * @param filename Nom du fichier de sortie
 * @return 0 si succès, -1 si erreur
 */
int write_ppm(const Image* img, const char* filename);

/**
 * @brief Ignore les lignes de commentaires dans un fichier PPM
 * @param file Pointeur vers le fichier
 */
void skip_comments(FILE* file);

// ============================================================================
// PROTOTYPES - OPÉRATIONS DE BASE (operations.c)
// Développeur: OUMAROU MVONGO CALEB ISRAEL (24G2771)
// ============================================================================

/**
 * @brief Applique la dominante de couleur sur une image
 * @param img Image à traiter
 * @param channel Canal de couleur ('R', 'G', ou 'B')
 * @param value Valeur à ajouter/soustraire
 * @return 0 si succès, -1 si erreur
 */
int apply_dominant_color(Image* img, char channel, int value);

/**
 * @brief Convertit une image en niveaux de gris
 * @param img Image à convertir
 * @return 0 si succès, -1 si erreur
 */
int convert_to_grayscale(Image* img);

/**
 * @brief Affiche les dimensions d'une image
 * @param img Image à analyser
 */
void display_size(const Image* img);

/**
 * @brief Crée le négatif d'une image
 * @param src Image source
 * @param dst Image de destination
 * @return 0 si succès, -1 si erreur
 */
int create_negative(const Image* src, Image* dst);

// ============================================================================
// PROTOTYPES - DÉCOUPAGE ET FILTRES (filters.c)
// Développeur: VAMI NEGUEM YVO FREED (24F2658)
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
Image* cut_image(const Image* src, int row1, int row2, int col1, int col2);

/**
 * @brief Applique un filtre médian pour réduire le bruit
 * @param src Image source
 * @param dst Image de destination
 * @return 0 si succès, -1 si erreur
 */
int apply_median_filter(const Image* src, Image* dst);

/**
 * @brief Fonction auxiliaire pour calculer la médiane d'un tableau
 * @param values Tableau de valeurs
 * @param size Taille du tableau
 * @return Valeur médiane
 */
int calculate_median(int* values, int size);

// ============================================================================
// PROTOTYPES - INTERFACE UTILISATEUR (main.c)
// Développeur: FOKOU FOFOU STEDDY RONEL (24G2579)
// ============================================================================

/**
 * @brief Affiche le message de bienvenue
 */
void display_welcome();

/**
 * @brief Affiche l'aide sur les commandes disponibles
 */
void display_help();

/**
 * @brief Parse et exécute une commande utilisateur
 * @param command Chaîne de commande entrée par l'utilisateur
 * @return 0 pour continuer, 1 pour quitter
 */
int parse_command(char* command);

/**
 * @brief Valide le format d'un fichier PPM
 * @param filename Nom du fichier à valider
 * @return 1 si valide, 0 sinon
 */
int validate_ppm_file(const char* filename);

// ============================================================================
// CONSTANTES GLOBALES
// ============================================================================

#define MAX_COMMAND_LENGTH 256
#define MAX_FILENAME_LENGTH 128
#define MAX_COLOR_VALUE 255
#define MIN_COLOR_VALUE 0

// Codes de retour
#define SUCCESS 0
#define ERROR -1
#define QUIT 1

// Messages d'erreur
#define ERR_MEMORY "Erreur: Allocation mémoire échouée\n"
#define ERR_FILE_OPEN "Erreur: Impossible d'ouvrir le fichier\n"
#define ERR_INVALID_FORMAT "Erreur: Format PPM invalide\n"
#define ERR_INVALID_COMMAND "Erreur: Commande invalide\n"
#define ERR_INVALID_PARAMS "Erreur: Paramètres invalides\n"

// Messages de succès
#define MSG_SUCCESS "✅ Opération réussie\n"
#define MSG_FILE_CREATED "✅ Fichier créé: %s\n"

#endif // IMAGE_H
