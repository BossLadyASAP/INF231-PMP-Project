/**
 * @file main.c
 * @brief Programme principal et parseur de commandes pour PPM Image Processor
 * @author MATAGNE DASSE PRESLIE CHANEL (23V2294) - Project Leader
 * @date Octobre 2025
 * 
 * Ce fichier contient le point d'entrée du programme et le système
 * d'intégration qui coordonne tous les modules du projet.
 */

#include "image.h"

// ============================================================================
// FONCTIONS UTILITAIRES DE GESTION MÉMOIRE
// ============================================================================

/**
 * @brief Alloue la mémoire pour une image
 */
Image* create_image(int width, int height) {
    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Erreur: Dimensions invalides\n");
        return NULL;
    }

    Image* img = (Image*)malloc(sizeof(Image));
    if (!img) {
        fprintf(stderr, ERR_MEMORY);
        return NULL;
    }

    img->width = width;
    img->height = height;
    strcpy(img->format, "P3");
    img->max_value = MAX_COLOR_VALUE;

    // Allocation du tableau 2D de pixels
    img->pixels = (Pixel**)malloc(height * sizeof(Pixel*));
    if (!img->pixels) {
        fprintf(stderr, ERR_MEMORY);
        free(img);
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        img->pixels[i] = (Pixel*)malloc(width * sizeof(Pixel));
        if (!img->pixels[i]) {
            fprintf(stderr, ERR_MEMORY);
            // Libérer la mémoire déjà allouée
            for (int j = 0; j < i; j++) {
                free(img->pixels[j]);
            }
            free(img->pixels);
            free(img);
            return NULL;
        }
    }

    return img;
}

/**
 * @brief Libère la mémoire utilisée par une image
 */
void free_image(Image* img) {
    if (!img) return;

    if (img->pixels) {
        for (int i = 0; i < img->height; i++) {
            if (img->pixels[i]) {
                free(img->pixels[i]);
            }
        }
        free(img->pixels);
    }
    free(img);
}

/**
 * @brief Copie une image
 */
Image* copy_image(const Image* src) {
    if (!src) return NULL;

    Image* dst = create_image(src->width, src->height);
    if (!dst) return NULL;

    strcpy(dst->format, src->format);
    dst->max_value = src->max_value;

    for (int i = 0; i < src->height; i++) {
        for (int j = 0; j < src->width; j++) {
            dst->pixels[i][j] = src->pixels[i][j];
        }
    }

    return dst;
}

// ============================================================================
// OUTILS D'INTERPRÉTATION DE CHEMINS
// ============================================================================

/**
 * @brief Remplace les backslashes Windows par des slashes Unix pour supporter
 *        des chemins saisis comme "..\\mesimages\\test1.ppm".
 */
static void normalize_path(char* path) {
    if (!path) return;
    for (char* p = path; *p; ++p) {
        if (*p == '\\') *p = '/';
    }
}

// ============================================================================
// INTERFACE UTILISATEUR
// ============================================================================

/**
 * @brief Affiche le message de bienvenue
 */
void display_welcome() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║          📷  PPM IMAGE PROCESSOR  📷                         ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  Projet INF231 - Université de Yaoundé 1                    ║\n");
    printf("║  Équipe de développement 2025-2026                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Tapez 'help' pour voir les commandes disponibles\n");
    printf("Tapez 'quit' pour quitter\n\n");
}

/**
 * @brief Affiche l'aide sur les commandes disponibles
 */
void display_help() {
    printf("\n=== COMMANDES DISPONIBLES ===\n\n");
    printf("📏 size <fichier.ppm>\n");
    printf("   Affiche les dimensions de l'image\n\n");
    
    printf("🎨 dom <R|G|B> <valeur> <fichier.ppm>\n");
    printf("   Applique la dominante de couleur\n");
    printf("   Exemple: dom R 50 image.ppm\n\n");
    
    printf("⚫ gris <fichier.ppm>\n");
    printf("   Convertit l'image en niveaux de gris\n");
    printf("   Sortie: <fichier>_gris.ppm\n\n");
    
    printf("🌗 neg <fichier_in.ppm> <fichier_out.ppm>\n");
    printf("   Crée le négatif de l'image\n\n");
    
    printf("✂️ cut <fichier.ppm> <l1> <l2> <c1> <c2> <fichier_out.ppm>\n");
    printf("   Découpe une région rectangulaire\n");
    printf("   l1,l2: lignes, c1,c2: colonnes (0-indexed)\n\n");
    
    printf("🔍 fil <fichier_in.ppm> <fichier_out.ppm>\n");
    printf("   Applique un filtre médian pour réduire le bruit\n\n");
    
    printf("❓ help\n");
    printf("   Affiche cette aide\n\n");
    
    printf("🚪 quit\n");
    printf("   Quitte le programme\n\n");

    printf("🧪 test\n");
    printf("   Lance la suite de tests internes\n\n");

    printf("✨ clear\n");
    printf("   Efface l'écran\n\n");
}

/**
 * @brief Valide le format d'un fichier PPM
 */
int validate_ppm_file(const char* filename) {
    if (!filename) return 0;
    
    size_t len = strlen(filename);
    if (len < 4) return 0;
    
    // Vérifier l'extension .ppm
    if (strcmp(filename + len - 4, ".ppm") != 0) {
        fprintf(stderr, "Erreur: Le fichier doit avoir l'extension .ppm\n");
        return 0;
    }
    
    return 1;
}

/**
 * @brief Génère un nom de fichier de sortie avec suffixe
 */
void generate_output_filename(const char* input, const char* suffix, char* output) {
    char base[MAX_FILENAME_LENGTH];
    strcpy(base, input);
    
    // Retirer l'extension .ppm
    char* dot = strrchr(base, '.');
    if (dot) *dot = '\0';
    
    // Ajouter le suffixe et l'extension
    sprintf(output, "%s_%s.ppm", base, suffix);
}

// ============================================================================
// PARSEUR DE COMMANDES
// ============================================================================

/**
 * @brief Parse et exécute une commande utilisateur
 */
int parse_command(char* command) {
    // Supprimer le retour à la ligne
    command[strcspn(command, "\n")] = 0;
    
    // Ignorer les lignes vides
    if (strlen(command) == 0) return 0;
    
    // Parser les tokens
    char* token = strtok(command, " ");
    if (!token) return 0;
    
    // Commande: quit
    if (strcmp(token, "quit") == 0) {
        printf("Au revoir! 👋\n");
        return QUIT;
    }
    
    // Commande: help
    if (strcmp(token, "help") == 0) {
        display_help();
        return SUCCESS;
    }
    
    // Commande: size <fichier.ppm>
    if (strcmp(token, "size") == 0) {
        char* filename = strtok(NULL, " ");
        normalize_path(filename);
        if (!filename || !validate_ppm_file(filename)) {
            fprintf(stderr, ERR_INVALID_PARAMS);
            printf("Usage: size <fichier.ppm>\n");
            return ERROR;
        }
        
        Image* img = read_ppm(filename);
        if (img) {
            display_size(img);
            free_image(img);
        }
        return SUCCESS;
    }
    
    // Commande: dom <R|G|B> <valeur> <fichier.ppm>
    if (strcmp(token, "dom") == 0) {
        char* channel_str = strtok(NULL, " ");
        char* value_str = strtok(NULL, " ");
        char* filename = strtok(NULL, " ");

        normalize_path(filename);
        if (!channel_str || !value_str || !filename || !validate_ppm_file(filename)) {
            fprintf(stderr, ERR_INVALID_PARAMS);
            printf("Usage: dom <R|G|B> <valeur> <fichier.ppm>\n");
            return ERROR;
        }
        
        Image* img = read_ppm(filename);
        if (img) {
            char channel = channel_str[0];
            int value = atoi(value_str);
            
            if (apply_dominant_color(img, channel, value) == SUCCESS) {
                char output[MAX_FILENAME_LENGTH];
                generate_output_filename(filename, "dom", output);
                if (write_ppm(img, output) == SUCCESS) {
                    printf(MSG_FILE_CREATED, output);
                    printf("opération effectuée\n");
                }
            }
            free_image(img);
        }
        return SUCCESS;
    }
    
    // Commande: neg <in.ppm> <out.ppm>
    if (strcmp(token, "neg") == 0) {
        char* input = strtok(NULL, " ");
        char* output = strtok(NULL, " ");

        normalize_path(input);
        normalize_path(output);
        if (!input || !output || !validate_ppm_file(input) || !validate_ppm_file(output)) {
            fprintf(stderr, ERR_INVALID_PARAMS);
            printf("Usage: neg <fichier_in.ppm> <fichier_out.ppm>\n");
            return ERROR;
        }
        
        Image* src = read_ppm(input);
        if (src) {
            Image* dst = create_image(src->width, src->height);
            if (dst) {
                if (create_negative(src, dst) == SUCCESS) {
                    if (write_ppm(dst, output) == SUCCESS) {
                        printf(MSG_FILE_CREATED, output);
                        printf("opération effectuée\n");
                    }
                }
                free_image(dst);
            }
            free_image(src);
        }
        return SUCCESS;
    }
    
    // Commande: cut <f.ppm> <l1> <l2> <c1> <c2> <out.ppm>
    if (strcmp(token, "cut") == 0) {
        char* filename = strtok(NULL, " ");
        char* l1_str = strtok(NULL, " ");
        char* l2_str = strtok(NULL, " ");
        char* c1_str = strtok(NULL, " ");
        char* c2_str = strtok(NULL, " ");
        char* output = strtok(NULL, " ");

        normalize_path(filename);
        normalize_path(output);
        if (!filename || !l1_str || !l2_str || !c1_str || !c2_str || !output ||
            !validate_ppm_file(filename) || !validate_ppm_file(output)) {
            fprintf(stderr, ERR_INVALID_PARAMS);
            printf("Usage: cut <fichier.ppm> <l1> <l2> <c1> <c2> <fichier_out.ppm>\n");
            return ERROR;
        }
        
        int l1 = atoi(l1_str);
        int l2 = atoi(l2_str);
        int c1 = atoi(c1_str);
        int c2 = atoi(c2_str);
        
        Image* src = read_ppm(filename);
        if (src) {
            Image* result = cut_image(src, l1, l2, c1, c2);
            if (result) {
                if (write_ppm(result, output) == SUCCESS) {
                    printf(MSG_FILE_CREATED, output);
                    printf("opération effectuée\n");
                }
                free_image(result);
            }
            free_image(src);
        }
        return SUCCESS;
    }
    
    // Commande: fil <in.ppm> <out.ppm>
    if (strcmp(token, "fil") == 0) {
        char* input = strtok(NULL, " ");
        char* output = strtok(NULL, " ");

        normalize_path(input);
        normalize_path(output);
        if (!input || !output || !validate_ppm_file(input) || !validate_ppm_file(output)) {
            fprintf(stderr, ERR_INVALID_PARAMS);
            printf("Usage: fil <fichier_in.ppm> <fichier_out.ppm>\n");
            return ERROR;
        }
        
        Image* src = read_ppm(input);
        if (src) {
            Image* dst = create_image(src->width, src->height);
            if (dst) {
                if (apply_median_filter(src, dst) == SUCCESS) {
                    if (write_ppm(dst, output) == SUCCESS) {
                        printf(MSG_FILE_CREATED, output);
                        printf("opération effectuée\n");
                    }
                }
                free_image(dst);
            }
            free_image(src);
        }
        return SUCCESS;
    }
    
    // Commande: test
    if (strcmp(token, "test") == 0) {
        test_median_calculation();
        return SUCCESS;
    }

    // Commande: clear
    if (strcmp(token, "clear") == 0) {
        system("cls || clear"); // Commande pour Windows et Linux/macOS
        return SUCCESS;
    }

    // Commande inconnue
    fprintf(stderr, ERR_INVALID_COMMAND);
    printf("Tapez 'help' pour voir les commandes disponibles\n");
    return ERROR;
}

// ============================================================================
// FONCTION PRINCIPALE
// ============================================================================

int main(int argc, char* argv[]) {
    display_welcome();
    
    char command[MAX_COMMAND_LENGTH];
    
    // Boucle principale d'interaction
    while (1) {
        printf("ppmviewer > ");
        fflush(stdout);
        
        if (!fgets(command, MAX_COMMAND_LENGTH, stdin)) {
            break;
        }
        
        int result = parse_command(command);
        if (result == QUIT) {
            break;
        }
        
        printf("\n");
    }
    
    return 0;
}
