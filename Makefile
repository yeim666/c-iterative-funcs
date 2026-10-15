# Makefile - tres funciones mediante series (seno, logaritmo, arctangente)
#
#   make          compila los tres programas en build/
#   make test     ejecuta todas las pruebas del informe
#   make clean    borra build/

CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic
LDLIBS  = -lm
SRC     = src
BUILD   = build

# Programas principales (un .c por cada uno en src/)
PROGS   = seno logaritmo arctangente
BINS    = $(addprefix $(BUILD)/,$(PROGS))

# Variantes con maximo de 10 terminos, para probar el aviso de no convergencia
BINS_MAX10 = $(BUILD)/logaritmo_max10 $(BUILD)/arctangente_max10

.PHONY: all test test-seno test-log test-atan clean

all: $(BINS) $(BINS_MAX10)

# Crea la carpeta build/ si no existe
$(BUILD):
	mkdir -p $(BUILD)

# Regla general: build/nombre se obtiene de src/nombre.c (-lm va al final)
$(BUILD)/%: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

# Variantes con MAX_TERMINOS = 10
$(BUILD)/logaritmo_max10: $(SRC)/logaritmo.c | $(BUILD)
	$(CC) $(CFLAGS) -DMAX_TERMINOS=10 $< -o $@ $(LDLIBS)

$(BUILD)/arctangente_max10: $(SRC)/arctangente.c | $(BUILD)
	$(CC) $(CFLAGS) -DMAX_TERMINOS=10 $< -o $@ $(LDLIBS)

# ---------------------------------------------------------------------------
# Pruebas. Cada caso imprime sus datos, la salida del programa y el codigo
# de salida (0 = correcto, 1 = entrada invalida, 2 = sin convergencia).
# ---------------------------------------------------------------------------
test: test-seno test-log test-atan

test-seno: $(BUILD)/seno
	@echo "##### SENO #####"
	@for caso in "0 12" "0.5235987755982988 12" "-1.5707963267948966 12" \
	             "6.283185307179586 12" "0.5235987755982988 3" \
	             "0.5235987755982988 6" "1.5707963267948966 3" \
	             "1.5707963267948966 6" "1.5707963267948966 12" \
	             "100 12" "100 100" "-0.5235987755982988 12" \
	             "4.71238898038469 12" \
	             "101 10" "1 0" "1 101" "hola 5"; do \
	    echo; echo ">>> entrada: $$caso"; \
	    echo "$$caso" | ./$(BUILD)/seno; echo "codigo de salida: $$?"; \
	done

test-log: $(BUILD)/logaritmo $(BUILD)/logaritmo_max10
	@echo "##### LOGARITMO #####"
	@for caso in "0.1 1e-12" "0.5 1e-12" "1 1e-12" "2 1e-12" "10 1e-12" \
	             "0.5 1e-3" "10 1e-3" "10 1e-6" \
	             "0 1e-6" "-2 1e-6" "texto 1e-6" "2 1e-13" "2 0.01"; do \
	    echo; echo ">>> entrada: $$caso"; \
	    echo "$$caso" | ./$(BUILD)/logaritmo; echo "codigo de salida: $$?"; \
	done
	@echo; echo ">>> maximo reducido a 10 terminos, entrada: 10 1e-12"
	@echo "10 1e-12" | ./$(BUILD)/logaritmo_max10; echo "codigo de salida: $$?"

test-atan: $(BUILD)/arctangente $(BUILD)/arctangente_max10
	@echo "##### ARCTANGENTE #####"
	@for caso in "0 1e-4" "0.5 1e-4" "1 1e-4" "-0.5 1e-4" "-1 1e-4" "2 1e-4" "-2 1e-4" \
	             "100 1e-4" "-100 1e-4" "1 1e-2" "1 1e-3" \
	             "101 1e-3" "1 1e-5" "1 0.1" "texto 1e-3"; do \
	    echo; echo ">>> entrada: $$caso"; \
	    echo "$$caso" | ./$(BUILD)/arctangente; echo "codigo de salida: $$?"; \
	done
	@echo; echo ">>> maximo reducido a 10 terminos, entrada: 1 1e-4"
	@echo "1 1e-4" | ./$(BUILD)/arctangente_max10; echo "codigo de salida: $$?"

# Borra los binarios generados
clean:
	rm -rf $(BUILD)/*
