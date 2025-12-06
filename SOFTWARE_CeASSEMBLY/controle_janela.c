#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <ctype.h>
#include <termios.h> // Necessário para controlar o terminal
#include <unistd.h>  // Necessário para STDIN_FILENO

#include "funcoes_HPStoFPGA.h"
#include "funcoes_FPGAtoHPS.h"
#include "receber_fpga.h"
#include "montar_imagem.h"
#include "mouse.h"
#include "enviar_fpga.h"
#include "controle_janela.h"

static int fator_zoom = 1; 
static const int ZOOM_MIN = 1;
static const int ZOOM_MAX = 3;

int g_x1, g_y1, g_x2, g_y2;
int largura, altura;

/* =========================================================
 * Função para configurar o terminal (Modo RAW)
 * enable = 1: Ativa leitura imediata (sem Enter)
 * enable = 0: Restaura terminal original
 * ========================================================= */
void configurar_terminal(int enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt); // Salva config atual
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // Desliga buffer e eco
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Aplica
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restaura
    }
}

/* =========================================================
 * Função auxiliar para enviar e limpar (Libera memória)
 * ========================================================= */
static void enviar_e_limpar(uint8_t *img_montada, uint8_t *img_do_fpga) {
    if (img_montada) {
        volatile uint32_t *lw = mapear_lw();
        volatile uint32_t *pio_status = (volatile uint32_t *)((uint8_t*)lw + PIO_STATUS_BASE);

        enviar_para_fpga(img_montada, pio_status);

        munmap((void*)lw, LW_BRIDGE_SPAN);
        free(img_montada);
    }

    if (img_do_fpga) {
        free(img_do_fpga);
    }
}

/* =========================================================
 * Função auxiliar para restaurar a imagem original (SEM FREE)
 * ========================================================= */
static void restaurar_imagem_original(uint8_t *img_original) {
    if (!img_original) return;

    printf("\n[Janela] Restaurando imagem original na FPGA...\n");
    
    volatile uint32_t *lw = mapear_lw();
    volatile uint32_t *pio_status = (volatile uint32_t *)((uint8_t*)lw + PIO_STATUS_BASE);

    // Envia a imagem original de volta para a RAM do FPGA
    enviar_para_fpga(img_original, pio_status);

    // Garante que o estado lógico do FPGA volte para o modo original
    // (Isso depende se você usa o write_to_pio dentro do enviar_para_fpga ou separado)
    // O enviar_para_fpga já deve deixar o barramento limpo, mas chamar a função
    // assembly de "Exibir Original" garante que os bits de controle fiquem em 0.
    exibir_imagem_original(); 

    munmap((void*)lw, LW_BRIDGE_SPAN);
}

/* =========================================================
 * AJUSTA FATOR DE ZOOM (Retorna 1 se mudou, 0 se não)
 * ========================================================= */
int ajustar_zoom(char tecla) {
    int mudou = 0;
    if (tecla == '+') {
        if (fator_zoom < ZOOM_MAX) {
            fator_zoom++;
            printf("\r[ZOOM] Aumentando para %dx   \n", (fator_zoom == 3 ? 4 : fator_zoom));
            mudou = 1;
        } else {
            printf("\r[ZOOM] Maximo alcancado (4x) \n");
        }
    }
    else if (tecla == '-') {
        if (fator_zoom > ZOOM_MIN) {
            fator_zoom--;
            printf("\r[ZOOM] Diminuindo para %dx   \n", (fator_zoom == 3 ? 4 : fator_zoom));
            mudou = 1;
        } else {
            printf("\r[ZOOM] Minimo alcancado (1x) \n");
        }
    }
    fflush(stdout);
    return mudou;
}

/* =========================================================
 * SELECIONA A FUNÇÃO DO FPGA PARA NNI
 * ========================================================= */
void aplicar_zoom_nni(uint8_t *img_original) {

    Retangulo R = iniciar_modulo_mouse();

    g_x1 = R.x1;
    g_y1 = R.y1;
    g_x2 = R.x2;
    g_y2 = R.y2;

    // Configura terminal para leitura imediata
    configurar_terminal(1);

    int precisa_atualizar = 1;
    char comando = 0;

    while (1) {
        
        printf("\n--- MODO JANELA NNI ---\n");
        printf("Comandos: (+) Zoom In | (-) Zoom Out | (s) Sair\n");

        if (precisa_atualizar) {
            uint8_t *img_ampliada = NULL;

            switch(fator_zoom) {
                case 1:
                    img_ampliada = executar_algoritmo(0, img_original);
                    largura = 160; altura = 120;
                    break;
                case 2:
                    img_ampliada = executar_algoritmo(1, img_original);
                    largura = 320; altura = 240;
                    break;
                case 3:
                    img_ampliada = executar_algoritmo(2, img_original);
                    largura = 640; altura = 480;
                    break;
            }

            ImgGray base = { .data = img_original, .width = 160, .height = 120 };
            ImgGray ampliada = { .data = img_ampliada, .width = largura, .height = altura };

            uint8_t *img_final = montar_imagem(&base, &ampliada, g_x1, g_y1, g_x2, g_y2);
            enviar_e_limpar(img_final, img_ampliada);
            
            precisa_atualizar = 0;
        }

        // Lê um caractere IMEDIATAMENTE (sem esperar enter)
        comando = getchar();

        if (comando == 's' || comando == 'S') {
            printf("\nSaindo do modo janela...\n");
            break;
        }

        // Se ajustar_zoom retornar 1 (houve mudança), marcamos para atualizar a tela
        if (ajustar_zoom(comando)) {
            precisa_atualizar = 1;
        }
    }

    // Restaura terminal ao normal antes de sair
    configurar_terminal(0);

    // *** RESTAURA A IMAGEM ORIGINAL NA FPGA ***
    restaurar_imagem_original(img_original);
}

/* =========================================================
 * SELECIONA A FUNÇÃO DO FPGA PARA REPLICAÇÃO
 * ========================================================= */
void aplicar_zoom_replicacao(uint8_t *img_original) {

    Retangulo R = iniciar_modulo_mouse();

    g_x1 = R.x1;
    g_y1 = R.y1;
    g_x2 = R.x2;
    g_y2 = R.y2;

    configurar_terminal(1);

    int precisa_atualizar = 1;
    char comando = 0;

    while (1) {

        printf("\n--- MODO JANELA REPLICACAO ---\n");
        printf("Comandos: (+) Zoom In | (-) Zoom Out | (s) Sair\n");


        if (precisa_atualizar) {
            uint8_t *img_ampliada = NULL;

            switch(fator_zoom) {
                case 1:
                    img_ampliada = executar_algoritmo(0, img_original);
                    largura = 160; altura = 120;
                    break;
                case 2:
                    img_ampliada = executar_algoritmo(3, img_original); 
                    largura = 320; altura = 240;
                    break;
                case 3:
                    img_ampliada = executar_algoritmo(4, img_original);
                    largura = 640; altura = 480;
                    break;
            }

            ImgGray base = { .data = img_original, .width = 160, .height = 120 };
            ImgGray ampliada = { .data = img_ampliada, .width = largura, .height = altura };

            uint8_t *img_final = montar_imagem(&base, &ampliada, g_x1, g_y1, g_x2, g_y2);
            enviar_e_limpar(img_final, img_ampliada);
            
            precisa_atualizar = 0;
        }

        comando = getchar();

        if (comando == 's' || comando == 'S') {
            printf("\nSaindo do modo janela...\n");
            break;
        }

        if (ajustar_zoom(comando)) {
            precisa_atualizar = 1;
        }
    }

    configurar_terminal(0);

    // *** RESTAURA A IMAGEM ORIGINAL NA FPGA ***
    restaurar_imagem_original(img_original);
}