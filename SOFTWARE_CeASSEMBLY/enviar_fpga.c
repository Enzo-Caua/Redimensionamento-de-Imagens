#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "funcoes_HPStoFPGA.h"

void delay_minimo() {
    volatile int k;
    for(k=0; k<5; k++); // 5 iterações são suficientes para >40ns
}

// Envia pixel por pixel com pulso 0→1→0 no bit 6
void enviar_para_fpga(uint8_t *img, volatile uint32_t *pio_status)
{
    if (!img) {
        printf("Erro: ponteiro de imagem inválido\n");
        return;
    }

    if (!pio_status) {
        printf("Erro: ponteiro de pio_status inválido\n");
        return;
    }

    const int TOTAL_PIXELS = 160 * 120;

    for (int i = 0; i < TOTAL_PIXELS; i++)
    {
        uint8_t pixel = img[i];
        
        // Prepara os valores
        // Bits [14:7] = Pixel, Bit 6 = Clock
        uint32_t dados = ((uint32_t)pixel << 7);
        
        uint32_t val_high = dados | (1 << 6); // Bit 6 = 1
        uint32_t val_low  = dados;            // Bit 6 = 0

        // REMOVIDO: while ((*pio_status & 0x1) == 0) ... 
        // O FPGA é mais rápido que o HPS, não precisa esperar.

        // Gera a borda de subida (A lógica no FPGA detecta posedge no bit 6)
        // O estado anterior já era 0 (do final do loop anterior)
        write_to_pio(val_high); 
        
        if ((*pio_status & 0x1) == 0) {
            delay_minimo();
        }
        
        // Retorna para 0 para preparar a próxima borda
        write_to_pio(val_low);
    }

    write_to_pio(0x0000);

}

void limpar_fpga(volatile uint32_t *pio_status)
{
    if (!pio_status) {
        printf("Erro: ponteiro de pio_status inválido\n");
        return;
    }

    const int TOTAL_PIXELS = 160 * 120;
    
    uint32_t val_high = (1 << 6);
    uint32_t val_low  = 0;

    for (int i = 0; i < TOTAL_PIXELS; i++)
    {
        // Borda de subida (Escreve 0)
        write_to_pio(val_high);

        if ((*pio_status & 0x1) == 0) {
            delay_minimo();
        }

        // Borda de descida
        write_to_pio(val_low);
    }

    // Garante estado limpo
    write_to_pio(0x0000);

    printf("Memoria da FPGA limpa.\n");
}
