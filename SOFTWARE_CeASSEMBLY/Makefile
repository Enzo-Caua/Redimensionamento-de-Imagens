# ======================================
# COMPILADORES (HPS)
# ======================================
CC  = gcc          # Compila C e linka tudo
AS  = as           # Monta Assembly

CFLAGS = -Wall -Wextra -O2 -std=c99
ASFLAGS = 

# Nome do executável final
TARGET = programa

# ======================================
# ARQUIVOS FONTE
# ======================================
SRC_C = \
    main.c \
    menu.c \
    mouse.c \
    carregar_BMP.c \
    enviar_fpga.c \
    controle_janela.c \
    montar_imagem.c \
    receber_fpga.c

SRC_S = \
    funcoes_HPStoFPGA.s \
    funcoes_FPGAtoHPS.s

OBJ_C = $(SRC_C:.c=.o)
OBJ_S = $(SRC_S:.s=.o)
OBJ   = $(OBJ_C) $(OBJ_S)

# ======================================
# REGRAS PRINCIPAIS
# ======================================
all: $(TARGET)

# Link final usando GCC (NÃO usar ld!)
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compila .c → .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Monta .s → .o
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# ======================================
# EXECUTAR COM SUDO
# ======================================
run: $(TARGET)
	sudo ./$(TARGET)

# ======================================
# LIMPEZA
# ======================================
clean:
	rm -f *.o $(TARGET)

.PHONY: clean all run
