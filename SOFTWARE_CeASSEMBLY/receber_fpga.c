#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "funcoes_FPGAtoHPS.h"
#include "funcoes_HPStoFPGA.h"
#include "carregar_BMP.h"

/* ---- Definições LW Bridge ---- */
#define LW_BRIDGE_BASE  0xFF200000
#define LW_BRIDGE_SPAN  0x1000

/* ---- Endereços dos PIOs (conforme Qsys) ---- */
#define PIO_FPGATOHPS_BASE   0x10   // pixel
#define PIO_STATUS_BASE      0x20   // status = pronto
#define PIO_NEXT_PIXEL_BASE  0x30 

void delay_simples() {
    volatile int k;
    for(k=0; k<50; k++); // Pequeno atraso
}

/* ========================================================
 * MAPEAMENTO DO LW BRIDGE
 * ======================================================== */
volatile uint32_t *mapear_lw() {

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) { perror("open(/dev/mem)"); exit(1); }

    void *lw = mmap(NULL, LW_BRIDGE_SPAN,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED,
                    fd,
                    LW_BRIDGE_BASE);

    if (lw == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    close(fd);
    return (volatile uint32_t*) lw;
}

uint8_t* receber_imagem_do_fpga(int largura, int altura, uint8_t *img_original) {
    uint32_t total_pixels = (uint32_t)largura * (uint32_t)altura;
    
    uint8_t *imagem = malloc(total_pixels);
    if (!imagem) { perror("malloc"); exit(1); }

    volatile uint32_t *lw = mapear_lw();
    
    // Mapeamento dos ponteiros
    volatile uint32_t *pio_pixel = (uint32_t *)((uint8_t*)lw + PIO_FPGATOHPS_BASE);
    // volatile uint32_t *pio_status = ... (opcional se modo janela não usa mais status para sync)
    
    volatile uint32_t *pio_next = (uint32_t *)((uint8_t*)lw + PIO_NEXT_PIXEL_BASE);

    // Garante estado inicial 0
    *pio_next = 0;
    delay_simples();

    for (uint32_t i = 0; i < total_pixels; ++i) {

        // 1. Lê a coordenada ATUAL
        uint32_t dados_raw = *pio_pixel; 

        int src_x = dados_raw & 0xFF;
        int src_y = (dados_raw >> 8) & 0x7F;

        // Proteção de limites
        if (src_x >= WIDTH) src_x = WIDTH - 1;
        if (src_y >= HEIGHT) src_y = HEIGHT - 1;

        imagem[i] = img_original[src_y * WIDTH + src_x];

        // 2. HANDSHAKE COM DELAY
        // Sobe o sinal
        *pio_next = 1;
        delay_simples(); // Segura o sinal alto um pouco
        
        // Desce o sinal
        *pio_next = 0;
        delay_simples(); // Espera FPGA processar borda de descida/preparar próxima
    }

    return imagem;
}

/* ========================================================
 * EXECUTA ALGORITMO
 * ======================================================== */
uint8_t* executar_algoritmo(int opcao, uint8_t *img_original)
{
    int largura, altura;

    /* 1 — Dispara o algoritmo no FPGA */
    switch (opcao) {

        case 0:
            largura = 160; altura = 120;
            exibir_original_janela();
            break;

        case 1:
            largura = 320; altura = 240;
            zoom_in_nni_2x_janela();
            break;

        case 2:
            largura = 640; altura = 480;
            zoom_in_nni_4x_janela();
            break;

        case 3:
            largura = 320; altura = 240;
            zoom_in_replicacao_2x_janela();
            break;

        case 4:
            largura = 640; altura = 480;
            zoom_in_replicacao_4x_janela();
            break;

        default:
            printf("[ERRO] Algoritmo inválido!\n");
            exit(1);
    }

    // Passa img_original para a função de reconstrução
    return receber_imagem_do_fpga(largura, altura, img_original);
}