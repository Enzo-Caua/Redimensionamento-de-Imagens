#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "carregar_BMP.h"

// ======================================================================
// Função: inverte verticalmente imagem grayscale 8bpp (160x120)
// ======================================================================
void inverter_vertical(uint8_t *img) {
for (int y = 0; y < HEIGHT / 2; y++) {
    uint8_t *linha_cima  = img + y * WIDTH;
    uint8_t *linha_baixo = img + (HEIGHT - 1 - y) * WIDTH;

    for (int x = 0; x < WIDTH; x++) {
        uint8_t temp = linha_cima[x];
        linha_cima[x] = linha_baixo[x];
        linha_baixo[x] = temp;
        }
    }
}

// ======================================================================
// Ler BMP 8-bit grayscale (fixo 160x120)
// ======================================================================
uint8_t* ler_bmp_grayscale(const char *arquivo) {
FILE *fp = fopen(arquivo, "rb");
if (!fp) {
printf("Erro ao abrir BMP.\n");
return NULL;
}


uint8_t header[54];
if (fread(header, 1, 54, fp) != 54) {
    printf("Erro ao ler cabeçalho BMP.\n");
    fclose(fp);
    return NULL;
}

int w      = *(int *)&header[18];
int h      = *(int *)&header[22];
int offset = *(int *)&header[10];

if (w != WIDTH || h != HEIGHT) {
    printf("ERRO: BMP precisa ser 160x120 (recebido: %dx%d)\n", w, h);
    fclose(fp);
    return NULL;
}

// pula paleta (256 cores)
if (fseek(fp, offset, SEEK_SET) != 0) {
    printf("Erro ao posicionar leitura no BMP.\n");
    fclose(fp);
    return NULL;
}

uint8_t *img = malloc(WIDTH * HEIGHT);
if (!img) {
    printf("Erro de alocação ao criar buffer da imagem.\n");
    fclose(fp);
    return NULL;
}

size_t total = WIDTH * HEIGHT;
if (fread(img, 1, total, fp) != total) {
    printf("Erro ao ler dados da imagem.\n");
    free(img);
    fclose(fp);
    return NULL;
}

fclose(fp);
printf("Imagem Carregada\n");
return img;

}
