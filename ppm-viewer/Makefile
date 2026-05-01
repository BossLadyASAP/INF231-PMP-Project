# Makefile pour le projet PPM Image Processor
# Auteur: Équipe INF231 - 2025-2026

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I../src
LDFLAGS = -lm

# Dossiers (sources du parent)
SRC_DIR = ../src

# Cibles
TARGET = ppmviewer

# Fichiers sources (tous les .c du parent)
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Règles de compilation
.PHONY: all clean help run quick quick-fil quick-cut

# Construction: compiler puis lancer automatiquement ./ppmviewer
all: $(TARGET) run

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Lancer l'application interactivement
run: $(TARGET)
	@./$(TARGET)

# Tests rapides non interactifs
quick: $(TARGET)
	@printf "fil ../src/images/input/test.ppm ../src/images/output/test_fil.ppm\ncut ../src/images/input/test.ppm 1 2 1 2 ../src/images/output/test_cut.ppm\nquit\n" | ./$(TARGET)

quick-fil: $(TARGET)
	@printf "fil ../src/images/input/test.ppm ../src/images/output/test_fil.ppm\nquit\n" | ./$(TARGET)

quick-cut: $(TARGET)
	@printf "cut ../src/images/input/test.ppm 1 2 1 2 ../src/images/output/test_cut.ppm\nquit\n" | ./$(TARGET)

# Nettoyage minimal (pas de bin/ ni obj/)
clean:
	rm -f $(TARGET)

help:
	@echo "Utilisation dans ppm-viewer/:"
	@echo "  make        -> build puis lance ./ppmviewer"
	@echo "  make run    -> lance ./ppmviewer"
	@echo "  make quick  -> exécute fil et cut sur les fichiers de test"
	@echo "  make quick-fil  -> exécute seulement le filtre médian"
	@echo "  make quick-cut  -> exécute seulement le découpage"
	@echo "  make clean  -> supprime ./ppmviewer"
