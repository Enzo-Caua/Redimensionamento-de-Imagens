#ifndef MONTAR_IMAGEM_H
#define MONTAR_IMAGEM_H

#include <stdint.h>

typedef struct {
    uint8_t *data;   // ponteiro para pixels
    int width;       // largura da imagem
    int height;      // altura da imagem
} ImgGray;

/* 
 * Retorna ponteiro para nova imagem montada.
 * O caller é responsável por dar free() no retorno.
 */
uint8_t* montar_imagem(ImgGray *img1, ImgGray *img2,
                       int x0, int y0, int x1, int y1);

#endif