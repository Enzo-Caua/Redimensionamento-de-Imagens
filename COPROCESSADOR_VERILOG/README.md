# Coprocessador Gráfico (Hardware - FPGA)

Este diretório contém os códigos-fonte em **Verilog HDL** responsáveis pela implementação do hardware dedicado na FPGA Cyclone V da placa DE1-SoC. O sistema atua como um acelerador gráfico, realizando armazenamento de vídeo, geração de sinal VGA e cálculos aritméticos para zoom.

## 📂 Estrutura de Arquivos

*   **`ghrd_top.v`**: Arquivo de topo (Top-Level Entity). Instancia o sistema Qsys (`soc_system`), o controlador gráfico e conecta os pinos físicos da FPGA (VGA, Clocks, LEDs, Chaves).
*   **`controlador_JER.v`**: Núcleo do IP proprietário. Contém:
    *   Gerenciamento de escrita na RAM.
    *   Máquina de estados para o algoritmo de **Média de Blocos**.
    *   Lógica de feedback para o **Modo Janela** (cálculo reverso de coordenadas).
*   **`vga_driver.v`**: Gera os sinais de sincronismo horizontal (`hsync`) e vertical (`vsync`) para o monitor (640x480 @ 60Hz).
*   **`seletor_algoritmo.v`**: Multiplexador lógico que determina qual coordenada da memória RAM deve ser lida com base na coordenada VGA atual e no nível de zoom selecionado (NNI ou Replicação).
*   **`ram_in.v` / `ram1p.v`**: Módulos de memória RAM (Block RAM) gerados pelo Quartus/IP Catalog para armazenar a imagem de entrada (160x120) e o buffer de saída.
*   **`divisor_freq.v`**: Reduz o clock de entrada (50MHz) para o pixel clock do VGA (25MHz).

## ⚙️ Especificações Técnicas

*   **Resolução de Entrada:** 160x120 pixels (8-bit Grayscale).
*   **Resolução de Saída:** 640x480 pixels (Upscaling via Hardware).
*   **Interface:** Ponte Lightweight AXI (sinais `controle_ram`, `algoritmo`, `zoom` mapeados em PIOs).
*   **Algoritmos em Hardware:**
    1.  Vizinho Mais Próximo (NNI).
    2.  Replicação de Pixels.
    3.  Média de Blocos (Filtro espacial 2x2 e 4x4).

## 🛠️ Como Sintetizar (Quartus Prime)

1.  Abra o **Quartus Prime Lite 23.1**.
2.  Crie um novo projeto ou abra o projeto existente apontando para este diretório.
3.  Certifique-se de que o dispositivo selecionado é **Cyclone V 5CSEMA5F31C6**.
4.  Adicione todos os arquivos `.v` listados acima ao projeto.
5.  Adicione o arquivo Qsys (`soc_system.qsys`) ao projeto.
6.  Execute a **Analysis & Synthesis**.
7.  Execute **Compile Design** para gerar o arquivo `.sof`.
8.  Programar a FPGA via **Programmer** (JTAG).

---
**Nota:** O hardware deve estar programado na FPGA **antes** de executar o software no HPS.
