#ifndef MOUSE_MODULO_H
#define MOUSE_MODULO_H

typedef struct {
    int x1, y1;
    int x2, y2;
} Retangulo;

// Inicia leitura do mouse e retorna duas coordenadas (retângulo)
Retangulo iniciar_modulo_mouse(void);

#endif
