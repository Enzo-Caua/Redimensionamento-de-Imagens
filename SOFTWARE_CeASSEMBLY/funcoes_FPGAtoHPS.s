    .section .data
    .align 4

/* ponteiros virtuais vindos do mmap */
pio_in_addr:
    .word 0

pio_status_addr:
    .word 0

destino_buffer:
    .word 0

/* =========================================================
 * SETUP
 * ========================================================= */

    .section .text
    .align 4

    .global set_pio_in_address
    .type set_pio_in_address, %function
set_pio_in_address:
    push {lr}
    ldr r1, =pio_in_addr
    str r0, [r1]
    pop {pc}

    .global set_pio_status_address
    .type set_pio_status_address, %function
set_pio_status_address:
    push {lr}
    ldr r1, =pio_status_addr
    str r0, [r1]
    pop {pc}

    .global set_buffer_address
    .type set_buffer_address, %function
set_buffer_address:
    push {lr}
    ldr r1, =destino_buffer
    str r0, [r1]
    pop {pc}

/* =========================================================
 * read_pixel_from_fpga
 * ========================================================= */
    .global read_pixel_from_fpga
    .type read_pixel_from_fpga, %function
/* read pixel (coordinates) */
read_pixel_from_fpga:
    push {r4, lr}
    ldr  r4, =pio_in_addr
    ldr  r1, [r4]        @ r1 = endereço
    ldr  r0, [r1]        @ ALTERADO: ldr (32bit) em vez de ldrb (8bit)
    pop  {r4, pc}

/* wait */
    .global wait_for_fpga
    .type wait_for_fpga, %function
wait_for_fpga:
    push {r4, lr}
    ldr  r4, =pio_status_addr
    ldr  r1, [r4]
esperar:
    ldrb r2, [r1]        @ leitura segura de 1 byte
    cmp  r2, #1
    bne  esperar
    pop {r4, pc}

