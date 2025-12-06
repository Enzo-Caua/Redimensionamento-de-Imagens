#ifndef CARREGAR_BMP_H
#define CARREGAR_BMP_H

#include <stdint.h>

#define WIDTH 160
#define HEIGHT 120

uint8_t* ler_bmp_grayscale(const char *arquivo);
void inverter_vertical(uint8_t *img);

#endif
