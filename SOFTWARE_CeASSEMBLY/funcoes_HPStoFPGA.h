#ifndef FUNCOES_HPSTOFPGA_H
#define FUNCOES_HPSTOFPGA_H

#include <stdint.h> 

/* Inicializa o endereço do PIO após mmap */
void set_pio_address(volatile uint32_t *addr);

/* Função auxiliar usada pelo Assembly para enviar dados */
void write_to_pio(uint32_t valor);

/* ------ Comandos principais ------ */
void exibir_imagem_original(void);

/* ------ Zoom IN ------ */
void zoom_in_nni_2x(void);
void zoom_in_nni_4x(void);
void zoom_in_replicacao_2x(void);
void zoom_in_replicacao_4x(void);

/* ------ Zoom OUT ------ */
void zoom_out_nni_2x(void);
void zoom_out_nni_4x(void);
void zoom_out_media_2x(void);
void zoom_out_media_4x(void);

/* ------ Comandos Modo Janela ------ */
void exibir_original_janela(void);
void zoom_in_nni_2x_janela(void);
void zoom_in_nni_4x_janela(void);
void zoom_in_replicacao_2x_janela(void);
void zoom_in_replicacao_4x_janela(void);

#endif
