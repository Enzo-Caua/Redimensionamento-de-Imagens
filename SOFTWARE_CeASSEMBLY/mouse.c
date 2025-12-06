#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    int x1, y1;
    int x2, y2;
} Retangulo;

Retangulo iniciar_modulo_mouse(void) {

    Retangulo r = {-1, -1, -1, -1};

    int fd = open("/dev/input/mice", O_RDONLY);

    if (fd == -1) {
        perror("Erro ao abrir /dev/input/mice");
        return r;
    }

    printf("Lendo dados do mouse...\n");
    printf("Clique ESQUERDO para registrar os pontos.\n");
    printf("Primeiro clique → canto superior esquerdo\n");
    printf("Segundo clique  → canto inferior direito\n");

    int pos_x = 0;
    int pos_y = 0;

    const int MAX_X = 159;
    const int MAX_Y = 119;

    int pontos = 0;

    while (pontos < 2) {
        unsigned char data[3];

        ssize_t bytes = read(fd, data, sizeof(data));
        if (bytes < 0) {
            perror("Falha na leitura");
            break;
        }

        int left = data[0] & 0x1;

        int dx = (int8_t)data[1];
        int dy = (int8_t)data[2];

        pos_x += dx;
        pos_y -= dy;

        if (pos_x < 0) pos_x = 0;
        if (pos_x > MAX_X) pos_x = MAX_X;

        if (pos_y < 0) pos_y = 0;
        if (pos_y > MAX_Y) pos_y = MAX_Y;

        printf("\rX=%3d  Y=%3d  [Clique-esq:%d]   ", pos_x, pos_y, left);
        fflush(stdout);

        // --- REGISTRA OS PONTOS ---
        if (left) {
            if (pontos == 0) {
                r.x1 = pos_x;
                r.y1 = pos_y;
                printf("\nPonto 1 registrado em (%d, %d)\n", r.x1, r.y1);
                pontos++;
                usleep(200000); // evita múltiplos cliques
            }
            else if (pontos == 1) {
                r.x2 = pos_x;
                r.y2 = pos_y;
                printf("\nPonto 2 registrado em (%d, %d)\n", r.x2, r.y2);
                pontos++;
                usleep(200000);
            }
        }
    }

    close(fd);

    // --- GARANTE ORDEM DAS COORDENADAS ---
    if (r.x2 < r.x1) {
        int tmp = r.x1; r.x1 = r.x2; r.x2 = tmp;
    }
    if (r.y2 < r.y1) {
        int tmp = r.y1; r.y1 = r.y2; r.y2 = tmp;
    }

    printf("\nRetângulo final:\n");
    printf("(%d, %d) até (%d, %d)\n", r.x1, r.y1, r.x2, r.y2);

    return r;
}
