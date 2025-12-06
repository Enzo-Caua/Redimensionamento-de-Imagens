# Aplicação de Controle e Interface (Software - HPS)

Este diretório contém a aplicação de software desenvolvida para rodar no processador **ARM Cortex-A9** (Linux Embarcado). O software é responsável pela interface com o usuário, leitura de arquivos, drivers de mouse e comunicação de baixo nível com a FPGA.

## 📂 Estrutura de Arquivos

### Camada de Aplicação (C)
*   **`main.c`**: Ponto de entrada. Configura o mapeamento de memória e inicia o menu.
*   **`menu.c`**: Interface textual para seleção de algoritmos.
*   **`controle_janela.c`**: Implementa o loop interativo do **Modo Janela**, gerenciando o feedback entre o mouse e a FPGA.
*   **`mouse.c`**: Driver de espaço de usuário (User Space Driver) para ler `/dev/input/mice`, interpretar o protocolo PS/2 e calcular coordenadas absolutas $(X, Y)$.
*   **`carregar_BMP.c`**: Parser para leitura e tratamento de imagens `.bmp` (8-bit grayscale).
*   **`montar_imagem.c`**: Lógica de composição (overlay) que insere a imagem ampliada sobre a original.

### Camada de Driver (Assembly & Baixo Nível)
*   **`funcoes_HPStoFPGA.s`**: Funções em Assembly ARMv7 para escrever nos registradores de controle da FPGA (`exibir_imagem`, `zoom_in`, etc.).
*   **`funcoes_FPGAtoHPS.s`**: Funções em Assembly para ler dados retornados pela FPGA (status e coordenadas).
*   **`enviar_fpga.c`**: Implementa o protocolo de envio serial de pixels (Bit-banging) via PIO.
*   **`receber_fpga.c`**: Implementa o handshake para ler coordenadas calculadas pelo hardware.

### Definições
*   **`hps_0.h`**: Cabeçalho gerado pelo Quartus com os endereços base (Offsets) dos periféricos no barramento Avalon.

## 🚀 Requisitos de Execução

1.  **Hardware:** Kit DE1-SoC com a FPGA já programada (ver diretório `COPROCESSADOR_VERILOG`).
2.  **Sistema Operacional:** Linux Embarcado (Angstrom ou Debian) rodando no HPS.
3.  **Periféricos:**
    *   Mouse USB conectado à placa.
    *   Monitor VGA conectado.
4.  **Arquivo:** Uma imagem no formato `.bmp` (160x120, 8-bit, não comprimida) no mesmo diretório do executável.

## 📦 Compilação e Execução

O projeto utiliza um `Makefile` para automatizar o processo de build.

1.  Transfira esta pasta para a DE1-SoC via SSH/SCP.
2.  No terminal da placa, navegue até o diretório:
    ```bash
    cd SOFTWARE_CeASSEMBLY
    ```
3.  Compile o projeto e execute:
    ```bash
    make run
    ```
    *(Isso irá gerar o executável `programa`)*.

4. Garanta a permissão de super usuario

5.  Siga as instruções no terminal:
    *   Digite o nome do arquivo BMP (ex: `imagem.bmp`).
    *   Use o teclado para navegar no menu.
    *   No Modo Janela, use o mouse para desenhar a área de zoom.
