#include <stdio.h>
#include <stdlib.h>
#include "menu.h"
#include "funcoes_HPStoFPGA.h"
#include "controle_janela.h"

// --- Função Auxiliar para Limpar Ruído de Transição ---
void aplicar_transicao_segura() {
    // 1. Reseta para o estado base (Original, sem zoom)
    exibir_imagem_original();
    
    // 2. Aguarda 20ms (um quadro de vídeo VGA ~16ms)
    // Isso garante que o FPGA termine qualquer ciclo pendente e estabilize.
    usleep(20000); 
}

// ======================================================================
// MENU PRINCIPAL
// ======================================================================
void menu_principal(uint8_t *img_original) {

    int opcao;
    char buffer[64];

    while (1) {
        printf("\n======= MENU DE OPERACOES =======\n");
        printf("0  - Exibir imagem original\n");
        printf("1  - Zoom IN NNI 2x\n");
        printf("2  - Zoom IN NNI 4x\n");
        printf("3  - Zoom IN Replicacao 2x\n");
        printf("4  - Zoom IN Replicacao 4x\n");
        printf("5  - Zoom OUT NNI 2x\n");
        printf("6  - Zoom OUT NNI 4x\n");
        printf("7  - Zoom OUT Media 2x\n");
        printf("8  - Zoom OUT Media 4x\n");
        printf("9  - Modo Janela NNI (Mouse)\n");
        printf("10 - Modo Janela Replicacao (Mouse)\n");
        printf("11 - Sair\n");
        printf("=================================\n");
        printf("Escolha uma opcao: ");

        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Erro ao ler entrada.\n");
            continue;
        }
        if (sscanf(buffer, "%d", &opcao) != 1) {
            printf("Entrada invalida! Digite um numero.\n");
            continue;
        }
        if (opcao < 0 || opcao > 11) {
            printf("Opcao fora do intervalo (0–11).\n");
            continue;
        }

         switch (opcao) {
            case 0:
                aplicar_transicao_segura(); // Garante limpeza
                exibir_imagem_original();
                break;

            case 1:
                aplicar_transicao_segura(); // Limpa antes de mudar
                zoom_in_nni_2x();
                break;

            case 2:
                aplicar_transicao_segura();
                zoom_in_nni_4x();
                break;

            case 3:
                aplicar_transicao_segura();
                zoom_in_replicacao_2x();
                break;

            case 4:
                aplicar_transicao_segura();
                zoom_in_replicacao_4x();
                break;

            case 5:
                aplicar_transicao_segura();
                zoom_out_nni_2x();
                break;

            case 6:
                aplicar_transicao_segura();
                zoom_out_nni_4x();
                break;

            case 7:
                aplicar_transicao_segura();
                zoom_out_media_2x();
                break;

            case 8:
                aplicar_transicao_segura();
                zoom_out_media_4x();
                break;

            case 9:
                // Modo Janela já gerencia suas próprias transições via software,
                // mas resetar o FPGA antes é boa prática.
                aplicar_transicao_segura();
                aplicar_zoom_nni(img_original);
                break;

            case 10:
                aplicar_transicao_segura();
                aplicar_zoom_replicacao(img_original);
                break;

            case 11:
                printf("Saindo...\n");
                return;
        }
    }
}
