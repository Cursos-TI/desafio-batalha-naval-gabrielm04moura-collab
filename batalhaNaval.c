#include <stdio.h>

#define TAMANHO_TABULEIRO 10
#define TAMANHO_NAVIO     3
#define AGUA              0
#define NAVIO             3

/*
 * Orientações suportadas:
 *   'H' = horizontal       (coluna +1 por segmento, linha fixa)
 *   'V' = vertical         (linha   +1 por segmento, coluna fixa)
 *   'D' = diagonal desc.   (linha +1 e coluna +1 por segmento)
 *   'E' = diagonal esp.    (linha +1 e coluna -1 por segmento)
 */

/* Calcula o delta de linha para cada orientação. */
int deltaLinha(char orientacao) {
    return (orientacao == 'V' || orientacao == 'D' || orientacao == 'E') ? 1 : 0;
}

/* Calcula o delta de coluna para cada orientação. */
int deltaColuna(char orientacao) {
    if (orientacao == 'H') return  1;
    if (orientacao == 'D') return  1;
    if (orientacao == 'E') return -1;
    return 0; /* 'V' */
}

/* Inicializa todas as células do tabuleiro com AGUA (0). */
void inicializarTabuleiro(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
            tabuleiro[i][j] = AGUA;
}

/* Retorna 1 se o navio cabe dentro dos limites e não sobrepõe outro navio,
   0 caso contrário. Funciona para todas as orientações (H, V, D, E). */
int posicionamentoValido(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO],
                         int linha, int coluna, char orientacao) {
    int dl = deltaLinha(orientacao);
    int dc = deltaColuna(orientacao);

    for (int k = 0; k < TAMANHO_NAVIO; k++) {
        int l = linha  + k * dl;
        int c = coluna + k * dc;

        if (l < 0 || l >= TAMANHO_TABULEIRO || c < 0 || c >= TAMANHO_TABULEIRO)
            return 0;

        if (tabuleiro[l][c] == NAVIO)
            return 0;
    }
    return 1;
}

/* Copia o valor NAVIO (3) de cada segmento do vetor navio para as células
   correspondentes no tabuleiro, respeitando a orientação. */
void posicionarNavio(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO],
                     int navio[TAMANHO_NAVIO],
                     int linha, int coluna, char orientacao) {
    int dl = deltaLinha(orientacao);
    int dc = deltaColuna(orientacao);

    for (int k = 0; k < TAMANHO_NAVIO; k++) {
        int l = linha  + k * dl;
        int c = coluna + k * dc;
        tabuleiro[l][c] = navio[k];
    }
}

/* Exibe o tabuleiro com cabeçalho de colunas e índice de linhas.
   0 = água, 3 = navio. */
void exibirTabuleiro(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    printf("\n   ");
    for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        printf("%d ", j);
    printf("\n");

    for (int i = 0; i < TAMANHO_TABULEIRO; i++) {
        printf("%d  ", i);
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
            printf("%d ", tabuleiro[i][j]);
        printf("\n");
    }
}

/* Tenta posicionar um navio e imprime resultado; encerra o programa se inválido. */
void posicionarOuAbortar(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO],
                         int navio[TAMANHO_NAVIO],
                         int num, int linha, int coluna, char orientacao) {
    const char *nomeOrientacao[] = {
        [' '] = "?",
        ['H'] = "horizontal",
        ['V'] = "vertical",
        ['D'] = "diagonal desc.",
        ['E'] = "diagonal esp."
    };

    if (posicionamentoValido(tabuleiro, linha, coluna, orientacao)) {
        posicionarNavio(tabuleiro, navio, linha, coluna, orientacao);
        printf("Navio %d: %-14s inicio em (%d, %d)\n",
               num, nomeOrientacao[(int)orientacao], linha, coluna);
    } else {
        printf("Erro: posicao invalida para o Navio %d.\n", num);
        /* Exibe o estado atual para ajudar no diagnóstico */
        exibirTabuleiro(tabuleiro);
        /* Em programa de produção usaríamos retorno de erro;
           aqui encerramos para sinalizar bug de configuração. */
        __builtin_trap();
    }
}

int main(void) {
    /* Tabuleiro 10x10 inicializado com água */
    int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
    inicializarTabuleiro(tabuleiro);

    /* Vetor base: todos os segmentos de um navio valem NAVIO (3) */
    int navio[TAMANHO_NAVIO] = {NAVIO, NAVIO, NAVIO};

    /* -------------------------------------------------------
     * Navio 1 — horizontal: ocupa (2,4), (2,5), (2,6)
     * ------------------------------------------------------- */
    posicionarOuAbortar(tabuleiro, navio, 1, 2, 4, 'H');

    /* -------------------------------------------------------
     * Navio 2 — vertical: ocupa (5,7), (6,7), (7,7)
     * ------------------------------------------------------- */
    posicionarOuAbortar(tabuleiro, navio, 2, 5, 7, 'V');

    /* -------------------------------------------------------
     * Navio 3 — diagonal descendente (linha+1, coluna+1):
     *   ocupa (0,0), (1,1), (2,2)
     * ------------------------------------------------------- */
    posicionarOuAbortar(tabuleiro, navio, 3, 0, 0, 'D');

    /* -------------------------------------------------------
     * Navio 4 — diagonal espelhada (linha+1, coluna-1):
     *   ocupa (0,9), (1,8), (2,7)
     * ------------------------------------------------------- */
    posicionarOuAbortar(tabuleiro, navio, 4, 0, 9, 'E');

    /* Exibe o tabuleiro final */
    printf("\nTabuleiro final (0 = agua, 3 = navio):\n");
    exibirTabuleiro(tabuleiro);

    return 0;
}


