#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#include "hps_0.h"
#include "carregar_BMP.h"
#include "enviar_fpga.h"
#include "menu.h"
#include "funcoes_HPStoFPGA.h"

int main() {
    // ---- Buffer para o nome do arquivo ----
    char nome_arquivo[100];

    // ---- Solicita o nome do arquivo ao usuário ----
    printf("===================================\n");
    printf(" Sistema de Processamento de Zoom  \n");
    printf("===================================\n");
    printf("Digite o nome do arquivo BMP (ex: jer.bmp): ");

    // Lê a entrada do teclado
    if (fgets(nome_arquivo, sizeof(nome_arquivo), stdin) != NULL) {
        nome_arquivo[strcspn(nome_arquivo, "\n")] = 0; 
    } else {
        printf("Erro ao ler o nome do arquivo.\n");
        return 1;
    }

    // ---- Carrega imagem BMP com o nome informado ----
    uint8_t *img_original = ler_bmp_grayscale(nome_arquivo); 
    
    if (!img_original) {
        printf("Falha ao carregar '%s'. Verifique se o arquivo existe.\n", nome_arquivo);
        return 1;
    }

    inverter_vertical(img_original);

    // ---- Mapeamento LW Bridge ----
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) { perror("open"); exit(1); }

    void *lw = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE,
                    MAP_SHARED, fd, 0xFF200000);
    if (lw == MAP_FAILED) { perror("mmap"); exit(1); }

    // -----------------------------------------------------------
    //  Define o endereço do PIO_HPStoFPGA para o Assembly usar
    // -----------------------------------------------------------
    volatile uint32_t *pio_hpstofpga_addr = (volatile uint32_t*)((char*)lw + PIO_HPSTOFPGA_BASE);
    set_pio_address(pio_hpstofpga_addr);   // <-- AGORA O ASSEMBLY SABE O ENDEREÇO

    // ---- PIO_STATUS (somente leitura) ----
    volatile uint32_t *pio_status =
        (volatile uint32_t*)((char*)lw + PIO_STATUS_BASE);
    
    // 1. Envia um estado "falso" (ex: Zoom 2x - 0x0011)
    write_to_pio(0xFFFF);
    usleep(20000); // Aguarda 20ms para o FPGA registrar
    
    // 2. Envia o estado "correto" inicial (Original - 0x0000)
    // Isso cria a transição 0x0011 -> 0x0000, disparando 'config_mudou'
    // e zerando o contador_escrita_ext no Verilog.

    // ---- Envia imagem para FPGA ----
    enviar_para_fpga(img_original, pio_status);
    printf("FPGA Configurada.\n");

    write_to_pio(0x0000);
    //usleep(20000); // Aguarda estabilização

    // ---- Menu principal ----
    menu_principal(img_original);

    printf("===================================\n");
    printf("======== Sistema Encerrado ========\n");
    printf("===================================\n");
    limpar_fpga(pio_status);
    write_to_pio(0x0021);
    write_to_pio(0x0000);

    // ---- Finalização ----
    free(img_original);
    munmap(lw, 0x1000);
    close(fd);

    return 0;
}