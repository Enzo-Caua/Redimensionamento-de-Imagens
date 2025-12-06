#ifndef FUNCOES_FPGATOHPS_H
#define FUNCOES_FPGATOHPS_H

#include <stdint.h>

/* ============================================================
 *  FUNÇÕES ASSEMBLY (FPGA → HPS)
 * ============================================================ */

/**
 * Define o endereço (virtual!) do PIO de entrada que fornece pixels.
 */
void set_pio_in_address(uint32_t addr);

/**
 * Define o endereço (virtual!) do PIO de status.
 * Status == 1 → pixel válido disponível.
 */
void set_pio_status_address(uint32_t addr);

/**
 * Define o ponteiro do buffer onde os pixels serão armazenados.
 */
void set_buffer_address(uint8_t *buf);

/**
 * Aguarda o status do PIO ficar igual a 1.
 * Chamado automaticamente antes de cada pixel.
 */
void wait_for_fpga(void);

/**
 * Lê 1 pixel (8 bits) do FPGA.
 * Retorno em r0.
 */
uint32_t read_pixel_from_fpga(void);

/**
 * Lê um frame de N pixels do FPGA.
 * Entrada: r0 = quantidade total de pixels.
 */
void read_frame_from_fpga(uint32_t total_pixels);

#endif
