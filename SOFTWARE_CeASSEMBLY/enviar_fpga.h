#ifndef ENVIAR_FPGA_H
#define ENVIAR_FPGA_H

#include <stdint.h>

void enviar_para_fpga(uint8_t *img, volatile uint32_t *pio_status);
void limpar_fpga(volatile uint32_t *pio_status);

#endif