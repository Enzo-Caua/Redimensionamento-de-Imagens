/*
 * Funções Assembly para controle do PIO_HPStoFPGA
 * Baseado no header file hps_0.h
 * 
 * DATA_WIDTH: 16 bits
 */

    .section .data
    .align 4
pio_addr:
    .word 0                /* 32-bit storage para o endereço mapeado do PIO */

    .section .text
    .align 4

    /* ---------------------------
     * set_pio_address
     * --------------------------- */
    .global set_pio_address
    .type set_pio_address, %function
set_pio_address:
    push {lr}
    ldr r1, =pio_addr      /* r1 <- endereço do slot pio_addr */
    str r0, [r1]           /* *pio_addr = r0 */
    pop {pc}

    /* ---------------------------
     * write_to_pio
     * --------------------------- */
    .global write_to_pio
    .type write_to_pio, %function
write_to_pio:
    push {r4, lr}
    ldr r4, =pio_addr      /* r4 = &pio_addr */
    ldr r1, [r4]           /* r1 = valor armazenado em pio_addr (endereço mapeado) */
    str r0, [r1]           /* escreve valor em *r1 */
    pop {r4, pc}

    /* ========================================
     * FUNÇÕES PÚBLICAS
     * ======================================== */

    .global exibir_imagem_original
    .type exibir_imagem_original, %function
exibir_imagem_original:
    push {lr}
    mov r0, #0x0000
    bl write_to_pio
    pop {pc}

    .global zoom_in_nni_2x
    .type zoom_in_nni_2x, %function
zoom_in_nni_2x:
    push {lr}
    mov r0, #0x0011
    bl write_to_pio
    pop {pc}

    .global zoom_in_nni_4x
    .type zoom_in_nni_4x, %function
zoom_in_nni_4x:
    push {lr}
    mov r0, #0x0021
    bl write_to_pio
    pop {pc}

    .global zoom_in_replicacao_2x
    .type zoom_in_replicacao_2x, %function
zoom_in_replicacao_2x:
    push {lr}
    mov r0, #0x0012
    bl write_to_pio
    pop {pc}

    .global zoom_in_replicacao_4x
    .type zoom_in_replicacao_4x, %function
zoom_in_replicacao_4x:
    push {lr}
    mov r0, #0x0022
    bl write_to_pio
    pop {pc}

    .global zoom_out_nni_2x
    .type zoom_out_nni_2x, %function
zoom_out_nni_2x:
    push {lr}
    mov r0, #0x0014
    bl write_to_pio
    pop {pc}

    .global zoom_out_nni_4x
    .type zoom_out_nni_4x, %function
zoom_out_nni_4x:
    push {lr}
    mov r0, #0x0024
    bl write_to_pio
    pop {pc}

    .global zoom_out_media_2x
    .type zoom_out_media_2x, %function
zoom_out_media_2x:
    push {lr}
    mov r0, #0x0018
    bl write_to_pio
    pop {pc}

    .global zoom_out_media_4x
    .type zoom_out_media_4x, %function
zoom_out_media_4x:
    push {lr}
    mov r0, #0x0028
    bl write_to_pio
    pop {pc}

    .global exibir_original_janela
    .type exibir_original_janela, %function
exibir_original_janela:
    push {lr}
    movw r0, #0x8000   @ Bit 15 = 1 (Janela ativada), Algoritmo = 0 (Original)
    bl write_to_pio
    pop {pc}

  .global zoom_in_nni_2x_janela
    .type zoom_in_nni_2x_janela, %function
zoom_in_nni_2x_janela:
    push {lr}
    movw r0, #0x8011     /* 0x8000 (Bit 15) | 0x0011 */
    /* REMOVIDO: movt r0, #0x8000 */
    bl write_to_pio
    pop {pc}

    .global zoom_in_nni_4x_janela
    .type zoom_in_nni_4x_janela, %function
zoom_in_nni_4x_janela:
    push {lr}
    movw r0, #0x8021     /* 0x8000 | 0x0021 */
    bl write_to_pio
    pop {pc}

    .global zoom_in_replicacao_2x_janela
    .type zoom_in_replicacao_2x_janela, %function
zoom_in_replicacao_2x_janela:
    push {lr}
    movw r0, #0x8012     /* 0x8000 | 0x0012 */
    bl write_to_pio
    pop {pc}

    .global zoom_in_replicacao_4x_janela
    .type zoom_in_replicacao_4x_janela, %function
zoom_in_replicacao_4x_janela:
    push {lr}
    movw r0, #0x8022     /* 0x8000 | 0x0022 */
    bl write_to_pio
    pop {pc}

    .end


