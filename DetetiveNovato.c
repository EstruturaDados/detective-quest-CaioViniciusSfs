#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura que representa cada sala da mansão
typedef struct sala {
    char nome[50];
    struct sala *esq; // Caminho para a esquerda
    struct sala *dir; // Caminho para a direita
} Sala;

/*
 Função: criarSala
 Cria dinamicamente uma nova sala na memória e define seu nome.
*/
Sala* criarSala(char nome[]) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    nova->esq = NULL;
    nova->dir = NULL;
    return nova;
}

/*
 Função: explorarSalas
 Permite que o jogador navegue interativamente pela mansão,
 escolhendo seguir para a sala à esquerda (e), direita (d) ou sair (s).
*/
void explorarSalas(Sala *atual) {
    char escolha;

    while (atual != NULL) {
        printf("\nVocê está em: **%s**\n", atual->nome);

        // Quando a sala é folha (sem caminhos)
        if (atual->esq == NULL && atual->dir == NULL) {
            printf("Não há mais caminhos. Exploração encerrada!\n");
            return;
        }

        printf("Escolha o caminho:\n");
        if (atual->esq != NULL) printf(" (e) Ir para %s\n", atual->esq->nome);
        if (atual->dir != NULL) printf(" (d) Ir para %s\n", atual->dir->nome);
        printf(" (s) Sair da mansão\n");

        printf("Sua escolha: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' && atual->esq != NULL) {
            atual = atual->esq;
        }
        else if (escolha == 'd' && atual->dir != NULL) {
            atual = atual->dir;
        }
        else if (escolha == 's') {
            printf("Você decidiu sair. Até a próxima!\n");
            return;
        }
        else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

/*
 Função principal: monta o mapa (árvore) e inicia a exploração
*/
int main() {
    // Criando a mansão
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim = criarSala("Jardim");

    // Ligando salas (montando a árvore)
    hall->esq = salaEstar;
    hall->dir = cozinha;
    salaEstar->esq = biblioteca;
    salaEstar->dir = jardim;
    // Cozinha é nó com final (folha)

    // Inicia exploração
    explorarSalas(hall);

    return 0;
}
