#include <stdio.h>

#define TAMANHO_TABULEIRO 10
#define TAMANHO_NAVIO     3
#define AGUA              0
#define NAVIO             3

/* Inicializa todas as células do tabuleiro com AGUA (0). */
void inicializarTabuleiro(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
            tabuleiro[i][j] = AGUA;
}

/* Retorna 1 se as coordenadas e orientação do navio são válidas (dentro dos
   limites e sem sobreposição), 0 caso contrário.
   orientacao: 'H' = horizontal, 'V' = vertical. */
int posicionamentoValido(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO],
                         int linha, int coluna, char orientacao) {
    for (int k = 0; k < TAMANHO_NAVIO; k++) {
        int l = linha   + (orientacao == 'V' ? k : 0);
        int c = coluna  + (orientacao == 'H' ? k : 0);

        /* Verifica limites do tabuleiro */
        if (l < 0 || l >= TAMANHO_TABULEIRO || c < 0 || c >= TAMANHO_TABULEIRO)
            return 0;

        /* Verifica sobreposição com outro navio */
        if (tabuleiro[l][c] == NAVIO)
            return 0;
    }
    return 1;
}

/* Posiciona o navio no tabuleiro copiando o valor NAVIO (3) para cada célula
   que ele ocupa, de acordo com a orientação informada. */
void posicionarNavio(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO],
                     int navio[TAMANHO_NAVIO],
                     int linha, int coluna, char orientacao) {
    for (int k = 0; k < TAMANHO_NAVIO; k++) {
        int l = linha  + (orientacao == 'V' ? k : 0);
        int c = coluna + (orientacao == 'H' ? k : 0);
        tabuleiro[l][c] = navio[k];
    }
}

/* Exibe o tabuleiro no console, separando cada célula por espaço. */
void exibirTabuleiro(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    printf("\n  ");
    for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        printf("%d ", j);
    printf("\n");

    for (int i = 0; i < TAMANHO_TABULEIRO; i++) {
        printf("%d ", i);
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
            printf("%d ", tabuleiro[i][j]);
        printf("\n");
    }
}

int main(void) {
    /* Tabuleiro 10x10, inicializado com AGUA */
    int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
    inicializarTabuleiro(tabuleiro);

    /* Vetor que representa um navio (3 segmentos, todos marcados com NAVIO) */
    int navio[TAMANHO_NAVIO] = {NAVIO, NAVIO, NAVIO};

    /* --- Navio 1: horizontal, linha 2, coluna 4 --- */
    int linha1 = 2, coluna1 = 4;
    char orientacao1 = 'H';

    if (posicionamentoValido(tabuleiro, linha1, coluna1, orientacao1)) {
        posicionarNavio(tabuleiro, navio, linha1, coluna1, orientacao1);
        printf("Navio 1 posicionado: horizontal, inicio em (%d, %d)\n", linha1, coluna1);
    } else {
        printf("Erro: posicao invalida para o Navio 1.\n");
        return 1;
    }

    /* --- Navio 2: vertical, linha 5, coluna 7 --- */
    int linha2 = 5, coluna2 = 7;
    char orientacao2 = 'V';

    if (posicionamentoValido(tabuleiro, linha2, coluna2, orientacao2)) {
        posicionarNavio(tabuleiro, navio, linha2, coluna2, orientacao2);
        printf("Navio 2 posicionado: vertical,     inicio em (%d, %d)\n", linha2, coluna2);
    } else {
        printf("Erro: posicao invalida para o Navio 2.\n");
        return 1;
    }

    /* Exibe o tabuleiro final com os navios posicionados */
    printf("\nTabuleiro (0 = agua, 3 = navio):\n");
    exibirTabuleiro(tabuleiro);

    return 0;
}

