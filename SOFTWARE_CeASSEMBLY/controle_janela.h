#ifndef ZOOM_CONTROLLER_H
#define ZOOM_CONTROLLER_H

#include <stdint.h>
#include "funcoes_HPStoFPGA.h"

void configurar_terminal(int enable);

static void enviar_e_limpar(uint8_t *img_montada, uint8_t *img_do_fpga);

int ajustar_zoom(char tecla);

void aplicar_zoom_nni(uint8_t *img_original);

void aplicar_zoom_replicacao(uint8_t *img_original);

#endif // ZOOM_CONTROLLER_H
