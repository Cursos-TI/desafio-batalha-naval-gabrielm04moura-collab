#include <stdio.h>
#include <stdlib.h>  /* abs() */

#define TAMANHO_TABULEIRO 10
#define TAMANHO_NAVIO     3
#define HAB_SIZE          5   /* tamanho das matrizes de habilidade (5x5) */
#define HAB_CENTRO        2   /* índice central de uma matriz 5x5 */
#define AGUA              0
#define NAVIO             3
#define EFEITO            5

/*
 * Orientações de navio suportadas:
 *   'H' = horizontal     (coluna +1 por segmento, linha fixa)
 *   'V' = vertical       (linha   +1 por segmento, coluna fixa)
 *   'D' = diagonal desc. (linha +1 e coluna +1 por segmento)
 *   'E' = diagonal esp.  (linha +1 e coluna -1 por segmento)
 */

/* =========================================================
 * FUNÇÕES AUXILIARES DE ORIENTAÇÃO
 * ========================================================= */

int deltaLinha(char orientacao) {
    return (orientacao == 'V' || orientacao == 'D' || orientacao == 'E') ? 1 : 0;
}

int deltaColuna(char orientacao) {
    if (orientacao == 'H') return  1;
    if (orientacao == 'D') return  1;
    if (orientacao == 'E') return -1;
    return 0;
}

const char *nomeOrientacao(char o) {
    if (o == 'H') return "horizontal";
    if (o == 'V') return "vertical";
    if (o == 'D') return "diagonal desc.";
    if (o == 'E') return "diagonal esp.";
    return "?";
}

/* =========================================================
 * TABULEIRO
 * ========================================================= */

void inicializarTabuleiro(int tab[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
            tab[i][j] = AGUA;
}

/* Retorna 1 se o navio cabe nos limites e não sobrepõe outro. */
int posicionamentoValido(int tab[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO],
                         int linha, int coluna, char orientacao) {
    int dl = deltaLinha(orientacao);
    int dc = deltaColuna(orientacao);

    for (int k = 0; k < TAMANHO_NAVIO; k++) {
        int l = linha  + k * dl;
        int c = coluna + k * dc;
        if (l < 0 || l >= TAMANHO_TABULEIRO || c < 0 || c >= TAMANHO_TABULEIRO)
            return 0;
        if (tab[l][c] == NAVIO)
            return 0;
    }
    return 1;
}

void posicionarNavio(int tab[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO],
                     int navio[TAMANHO_NAVIO],
                     int linha, int coluna, char orientacao) {
    int dl = deltaLinha(orientacao);
    int dc = deltaColuna(orientacao);
    for (int k = 0; k < TAMANHO_NAVIO; k++)
        tab[linha + k * dl][coluna + k * dc] = navio[k];
}

void posicionarOuAbortar(int tab[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO],
                         int navio[TAMANHO_NAVIO],
                         int num, int linha, int coluna, char orientacao) {
    if (posicionamentoValido(tab, linha, coluna, orientacao)) {
        posicionarNavio(tab, navio, linha, coluna, orientacao);
        printf("Navio %d: %-14s inicio em (%d, %d)\n",
               num, nomeOrientacao(orientacao), linha, coluna);
    } else {
        printf("Erro: posicao invalida para o Navio %d.\n", num);
        exit(1);
    }
}

/* =========================================================
 * HABILIDADES ESPECIAIS — matrizes 5x5 construídas com loops
 * ========================================================= */

/*
 * CONE: expande para baixo a partir do topo central.
 * Condição: a distância horizontal até o centro <= linha atual.
 * Linha 0: só o centro  (1 célula)
 * Linha 1: centro ± 1   (3 células)
 * Linha 2: centro ± 2   (5 células)
 * Linhas 3-4: toda a largura (já cobertas pela condição)
 *
 * Resultado:
 *   0 0 1 0 0
 *   0 1 1 1 0
 *   1 1 1 1 1
 *   1 1 1 1 1
 *   1 1 1 1 1
 */
void construirCone(int hab[HAB_SIZE][HAB_SIZE]) {
    for (int i = 0; i < HAB_SIZE; i++)
        for (int j = 0; j < HAB_SIZE; j++)
            hab[i][j] = (abs(j - HAB_CENTRO) <= i) ? 1 : 0;
}

/*
 * CRUZ: linha central completa + coluna central completa.
 * Condição: i == centro OU j == centro.
 *
 * Resultado:
 *   0 0 1 0 0
 *   0 0 1 0 0
 *   1 1 1 1 1
 *   0 0 1 0 0
 *   0 0 1 0 0
 */
void construirCruz(int hab[HAB_SIZE][HAB_SIZE]) {
    for (int i = 0; i < HAB_SIZE; i++)
        for (int j = 0; j < HAB_SIZE; j++)
            hab[i][j] = (i == HAB_CENTRO || j == HAB_CENTRO) ? 1 : 0;
}

/*
 * OCTAEDRO (losango / diamante): distância de Manhattan até o centro <= raio.
 * Condição: |i - centro| + |j - centro| <= centro.
 *
 * Resultado:
 *   0 0 1 0 0
 *   0 1 1 1 0
 *   1 1 1 1 1
 *   0 1 1 1 0
 *   0 0 1 0 0
 */
void construirOctaedro(int hab[HAB_SIZE][HAB_SIZE]) {
    for (int i = 0; i < HAB_SIZE; i++)
        for (int j = 0; j < HAB_SIZE; j++)
            hab[i][j] = (abs(i - HAB_CENTRO) + abs(j - HAB_CENTRO) <= HAB_CENTRO) ? 1 : 0;
}

/*
 * Sobrepõe a matriz de habilidade ao tabuleiro, centrada em (orig_l, orig_c).
 * Apenas células com hab[i][j] == 1 são marcadas como EFEITO (5).
 * Células fora dos limites do tabuleiro são ignoradas (clipping).
 * Navios já posicionados (valor NAVIO) não são sobrescritos.
 */
void aplicarHabilidade(int tab[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO],
                       int hab[HAB_SIZE][HAB_SIZE],
                       int orig_l, int orig_c) {
    for (int i = 0; i < HAB_SIZE; i++) {
        for (int j = 0; j < HAB_SIZE; j++) {
            /* Calcula a posição no tabuleiro relativa ao ponto de origem */
            int l = orig_l + (i - HAB_CENTRO);
            int c = orig_c + (j - HAB_CENTRO);

            /* Ignora se estiver fora do tabuleiro */
            if (l < 0 || l >= TAMANHO_TABULEIRO || c < 0 || c >= TAMANHO_TABULEIRO)
                continue;

            /* Marca apenas posições dentro do efeito, sem sobrescrever navios */
            if (hab[i][j] == 1 && tab[l][c] != NAVIO)
                tab[l][c] = EFEITO;
        }
    }
}

/* =========================================================
 * EXIBIÇÃO
 * ========================================================= */

/* Converte o valor numérico da célula em caractere visual:
 *   ~  = água (0)
 *   N  = navio (3)
 *   *  = área de efeito (5) */
char celulaParaChar(int valor) {
    if (valor == NAVIO)  return 'N';
    if (valor == EFEITO) return '*';
    return '~';
}

void exibirTabuleiro(int tab[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    /* Cabeçalho de colunas A-J */
    printf("\n    ");
    for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        printf("%c  ", 'A' + j);
    printf("\n");

    /* Separador superior */
    printf("   +");
    for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        printf("--+");
    printf("\n");

    for (int i = 0; i < TAMANHO_TABULEIRO; i++) {
        printf("%2d |", i + 1);
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
            printf("%c |", celulaParaChar(tab[i][j]));
        printf("\n");

        printf("   +");
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
            printf("--+");
        printf("\n");
    }

    printf("Legenda: ~ agua  N navio  * area de efeito\n");
}

/* =========================================================
 * MAIN
 * ========================================================= */

int main(void) {
    int tab[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
    inicializarTabuleiro(tab);

    int navio[TAMANHO_NAVIO] = {NAVIO, NAVIO, NAVIO};

    /* --- Navios (nível aventureiro) --- */
    posicionarOuAbortar(tab, navio, 1,  2, 4, 'H'); /* horizontal:     (2,4)-(2,6) */
    posicionarOuAbortar(tab, navio, 2,  5, 7, 'V'); /* vertical:       (5,7)-(7,7) */
    posicionarOuAbortar(tab, navio, 3,  0, 0, 'D'); /* diag. desc.:    (0,0)-(2,2) */
    posicionarOuAbortar(tab, navio, 4,  0, 9, 'E'); /* diag. esp.:     (0,9)-(2,7) */

    /* --- Habilidades especiais --- */
    int habCone[HAB_SIZE][HAB_SIZE];
    int habCruz[HAB_SIZE][HAB_SIZE];
    int habOct[HAB_SIZE][HAB_SIZE];

    /* Constrói as matrizes de efeito usando loops e condicionais */
    construirCone(habCone);
    construirCruz(habCruz);
    construirOctaedro(habOct);

    /* Aplica cada habilidade em um ponto de origem escolhido no tabuleiro */
    printf("\nHabilidades aplicadas:\n");
    printf("  Cone      — origem (4, 5) [indices 0-based]\n");
    printf("  Cruz      — origem (7, 2) [indices 0-based]\n");
    printf("  Octaedro  — origem (5, 4) [indices 0-based]\n");

    aplicarHabilidade(tab, habCone, 4, 5);
    aplicarHabilidade(tab, habCruz, 7, 2);
    aplicarHabilidade(tab, habOct,  5, 4);

    /* Exibe o tabuleiro com navios e áreas de efeito */
    printf("\nTabuleiro com habilidades:\n");
    exibirTabuleiro(tab);

    return 0;
}


