#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // Necessário para malloc
#include <string.h> // Necessário para memcpy
#include "montar_imagem.h"

/*
 * Insere img2 dentro de um retângulo na img1.
 * Retorna um NOVO buffer contendo a composição.
 * A imagem original (img1) NÃO é alterada.
 */
uint8_t* montar_imagem(ImgGray *img1, ImgGray *img2,
                       int x0, int y0, int x1, int y1)
{
    if (!img1 || !img2) return NULL;
    if (!img1->data || !img2->data) return NULL;

    // 1. Aloca memória para a nova imagem (tamanho igual à img1)
    size_t tamanho_total = img1->width * img1->height;
    uint8_t *resultado = malloc(tamanho_total);
    
    if (!resultado) {
        printf("Erro de alocacao em montar_imagem\n");
        return NULL;
    }

    // 2. Copia a imagem de fundo (img1) para o buffer de resultado
    // Isso garante que a imagem original permaneça intacta
    memcpy(resultado, img1->data, tamanho_total);

    // === OTIMIZAÇÃO SOLICITADA ===
    // Se a imagem 1 (fundo) e a imagem 2 (processada) tiverem o mesmo tamanho,
    // retornamos a cópia da imagem 1 imediatamente (que já está no buffer 'resultado').
    if (img1->width == img2->width && img1->height == img2->height) {
        return resultado;
    }

    // Corrige se o usuário trocou coordenadas
    if (x1 < x0) { int t = x0; x0 = x1; x1 = t; }
    if (y1 < y0) { int t = y0; y0 = y1; y1 = t; }

    // ... código anterior (cálculo de rect_w e rect_h) ...

    int rect_w = x1 - x0 + 1;
    int rect_h = y1 - y0 + 1;

    // 1. Calcula o Fator de Zoom comparando as larguras
    int scale = img2->width / img1->width; 

    int start_x, start_y;

    // Se houver zoom (escala > 1), aplica efeito LUPA
    if (scale > 1) {
        // Calcula o centro geométrico da janela desenhada pelo mouse
        int center_win_x = x0 + rect_w / 2;
        int center_win_y = y0 + rect_h / 2;

        /* 
           FÓRMULA DA LUPA:
           Queremos que a coordenada 'center_win_x' na tela mostre 
           o pixel 'center_win_x * scale' da imagem ampliada.
           
           Na equação do loop: dest_x = start_x + x_da_imagem_src
           Substituindo: center_win_x = start_x + (center_win_x * scale)
           Isolando start_x:
        */
        start_x = center_win_x - (center_win_x * scale);
        start_y = center_win_y - (center_win_y * scale);
    } 
    else {
        // Lógica Original (Centralizar a imagem dentro do retângulo)
        // Mantido para o caso de escala 1x ou outros usos
        start_x = x0 + (rect_w - img2->width)  / 2;
        start_y = y0 + (rect_h - img2->height) / 2;
    }

    // ... continua para o loop "Varre imagem 2" ...

    // Varre imagem 2 (overlay)
    for (int y = 0; y < img2->height; y++)
    {
        for (int x = 0; x < img2->width; x++)
        {
            int dest_x = start_x + x;
            int dest_y = start_y + y;

            // Limita ao retângulo
            if (dest_x < x0 || dest_x > x1) continue;
            if (dest_y < y0 || dest_y > y1) continue;

            // Limita à imagem 1 (dimensões totais)
            if (dest_x < 0 || dest_x >= img1->width) continue;
            if (dest_y < 0 || dest_y >= img1->height) continue;

            int idx_src  = y * img2->width + x;
            int idx_dest = dest_y * img1->width + dest_x;

            // 3. Escreve no buffer de RESULTADO, não no img1->data
            resultado[idx_dest] = img2->data[idx_src];
        }
    }

    return resultado;
}