#ifndef RECEBER_FPGA_H
#define RECEBER_FPGA_H

#include <stdint.h>

/* ---- Definições LW Bridge ---- */
#define LW_BRIDGE_BASE  0xFF200000
#define LW_BRIDGE_SPAN  0x1000

#define PIO_FPGATOHPS_BASE   0x10
#define PIO_STATUS_BASE      0x20

#define PIO_NEXT_PIXEL_BASE  0x30 
#define PIO_NEXT_PIXEL_SPAN  16

volatile uint32_t* mapear_lw(void);

// Alterado para receber a imagem original como referência
uint8_t* receber_imagem_do_fpga(int largura, int altura, uint8_t *img_original);

// Alterado para receber a imagem original como referência
uint8_t* executar_algoritmo(int opcao, uint8_t *img_original);

#endif