#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura que representa um território
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// Cadastra cada território pedindo nome, cor do exército e tropas
void cadastrarTerritorios(Territorio* mapa, int total) {
    for (int i = 0; i < total; i++) {
        printf("\nCadastro do território %d:\n", i + 1);
        printf("  Nome: ");
        scanf(" %[^\n]", mapa[i].nome);
        printf("  Cor do exército: ");
        scanf(" %[^\n]", mapa[i].cor);
        printf("  Tropas iniciais: ");
        scanf("%d", &mapa[i].tropas);
    }
}

// Exibe o estado atual de todos os territórios
void exibirMapa(Territorio* mapa, int total) {
    printf("\n--- Mapa de Territórios ---\n");
    for (int i = 0; i < total; i++) {
        printf("ID %d | %s | Cor: %s | Tropas: %d\n",
               i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

// Função de ataque que rola um dado (1–6) para atacante e defensor
// e atualiza cores e tropas conforme resultado
void atacar(Territorio* atacante, Territorio* defensor) {
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("  Erro: não se pode atacar território da mesma cor.\n");
        return;
    }

    int dadoA = rand() % 6 + 1;
    int dadoD = rand() % 6 + 1;
    printf("  %s ataca %s → dadoA=%d vs dadoD=%d\n",
           atacante->nome, defensor->nome, dadoA, dadoD);

    if (dadoA > dadoD) {
        // atacante vence: assume metade das tropas
        strcpy(defensor->cor, atacante->cor);
        defensor->tropas = atacante->tropas / 2;
        printf("  Vitória do atacante! %s agora controla %s com %d tropas.\n",
               atacante->cor, defensor->nome, defensor->tropas);
    } else {
        // defensor resiste: atacante perde uma tropa
        atacante->tropas = (atacante->tropas > 0)
                          ? atacante->tropas - 1
                          : 0;
        printf("  Defesa bem-sucedida! %s perde 1 tropa.\n",
               atacante->nome);
    }
}

// Sorteia uma missão entre as pré-definidas e copia em destino
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int idx = rand() % totalMissoes;
    strcpy(destino, missoes[idx]);
}

// Exibe a missão de um jogador
void exibirMissao(const char* missao) {
    printf("→ Sua missão: %s\n", missao);
}

// Verifica se o jogador cuja cor é playerCor cumpriu a missão
int verificarMissao(const char* missao,
                    Territorio* mapa,
                    int total,
                    const char* playerCor,
                    const char* adversarioCor) {
    int countTerritorios = 0;
    int sumTrocasPlayer = 0, sumTrocasAdv = 0;

    for (int i = 0; i < total; i++) {
        if (strcmp(mapa[i].cor, playerCor) == 0) {
            countTerritorios++;
            sumTrocasPlayer += mapa[i].tropas;
        } else if (strcmp(mapa[i].cor, adversarioCor) == 0) {
            sumTrocasAdv += mapa[i].tropas;
        }
    }

    if (strcmp(missao, "Controlar todos os territórios") == 0) {
        return (countTerritorios == total);
    }
    if (strcmp(missao, "Controlar 3 ou mais territórios") == 0) {
        return (countTerritorios >= 3);
    }
    if (strcmp(missao, "Ter mais tropas que o adversário") == 0) {
        return (sumTrocasPlayer > sumTrocasAdv);
    }
    if (strcmp(missao, "Eliminar todas as tropas do adversário") == 0) {
        return (sumTrocasAdv == 0);
    }
    if (strcmp(missao, "Controlar mais da metade dos territórios") == 0) {
        return (countTerritorios > total / 2);
    }

    // Missão desconhecida: não cumprida
    return 0;
}

// Libera toda a memória alocada
void liberarMemoria(Territorio* mapa,
                    char* missao1,
                    char* missao2) {
    free(mapa);
    free(missao1);
    free(missao2);
}

int main() {
    srand((unsigned) time(NULL));

    // 1) Leitura e alocação dinâmica do mapa
    int totalTerritorios;
    printf("Quantos territórios deseja cadastrar? ");
    scanf("%d", &totalTerritorios);

    Territorio* mapa = calloc(totalTerritorios, sizeof(Territorio));
    cadastrarTerritorios(mapa, totalTerritorios);

    // 2) Definição de cores dos dois jogadores
    char corJog1[10], corJog2[10];
    printf("\nJogador 1, informe sua cor: ");
    scanf(" %9[^\n]", corJog1);
    printf("Jogador 2, informe sua cor: ");
    scanf(" %9[^\n]", corJog2);

    // 3) Vetor de missões pré-definidas
    char* missoesPredefinidas[] = {
        "Controlar todos os territórios",
        "Controlar 3 ou mais territórios",
        "Ter mais tropas que o adversário",
        "Eliminar todas as tropas do adversário",
        "Controlar mais da metade dos territórios"
    };
    int totalMissoes = 5;

    // 4) Alocação dinâmica para as missões
    char* missaoJog1 = malloc(100);
    char* missaoJog2 = malloc(100);

    atribuirMissao(missaoJog1, missoesPredefinidas, totalMissoes);
    atribuirMissao(missaoJog2, missoesPredefinidas, totalMissoes);

    printf("\n--- Missões Sorteadas ---\n");
    printf("Jogador 1 (%s): ", corJog1);
    exibirMissao(missaoJog1);
    printf("Jogador 2 (%s): ", corJog2);
    exibirMissao(missaoJog2);

    // 5) Laço de turnos com ataques e verificação de vitória
    int turno = 1, venceu = 0;
    while (!venceu) {
        printf("\n--- Turno do Jogador %d (%s) ---\n",
               turno, (turno == 1 ? corJog1 : corJog2));
        exibirMapa(mapa, totalTerritorios);

        int idA, idD;
        printf("Escolha ID do atacante: ");
        scanf("%d", &idA);
        printf("Escolha ID do defensor: ");
        scanf("%d", &idD);

        // Verifica se IDs são válidos e cores permitem ataque
        if (idA < 0 || idA >= totalTerritorios ||
            idD < 0 || idD >= totalTerritorios ||
            strcmp(mapa[idA].cor,
                   (turno == 1 ? corJog1 : corJog2)) != 0) {
            printf("  Escolha inválida. Tente novamente.\n");
            continue;
        }

        atacar(&mapa[idA], &mapa[idD]);

        // Após o ataque, checa vitória
        if (turno == 1) {
            if (verificarMissao(missaoJog1,
                                mapa,
                                totalTerritorios,
                                corJog1,
                                corJog2)) {
                printf("\nJogador 1 venceu ao cumprir sua missão!\n");
                venceu = 1;
            }
        } else {
            if (verificarMissao(missaoJog2,
                                mapa,
                                totalTerritorios,
                                corJog2,
                                corJog1)) {
                printf("\nJogador 2 venceu ao cumprir sua missão!\n");
                venceu = 1;
            }
        }

        // Alterna turno
        turno = (turno == 1 ? 2 : 1);
    }

    // 6) Limpeza de memória
    liberarMemoria(mapa, missaoJog1, missaoJog2);
    return 0;
}