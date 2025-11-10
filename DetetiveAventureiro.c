#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura da árvore de salas (mapa da mansão)
typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala *esq;
    struct Sala *dir;
} Sala;

// Estrutura da árvore BST de pistas
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

// Função: criarSala
// Cria dinamicamente um nó da árvore de salas
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esq = nova->dir = NULL;
    return nova;
}

// Função: inserirPista
// Insere uma nova pista na BST em ordem alfabética
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        strcpy(novo->pista, pista);
        novo->esq = novo->dir = NULL;
        return novo;
    }
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esq = inserirPista(raiz->esq, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->dir = inserirPista(raiz->dir, pista);
    return raiz;
}

// Função: exibirPistas
// Percorre a BST em ordem alfabética e imprime as pistas
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esq);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

// Função: explorarSalasComPistas
// Controla navegação e coleta automática de pistas
void explorarSalasComPistas(Sala *atual, PistaNode **pistas) {
    char opc;
    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: %s\n", atual->pista);
            *pistas = inserirPista(*pistas, atual->pista);
        } else {
            printf("Nenhuma pista neste cômodo.\n");
        }

        printf("\nDeseja ir para (e) esquerda, (d) direita ou (s) sair? ");
        scanf(" %c", &opc);

        if (opc == 's') return;
        else if (opc == 'e') atual = atual->esq;
        else if (opc == 'd') atual = atual->dir;
        else printf("Opção inválida!\n");
    }
    printf("\nNão há mais caminhos a seguir nesta direção.\n");
}

int main() {
    // Construção fixa da mansão
    Sala *hall = criarSala("Hall de Entrada", "Pegadas de botas sujas");
    hall->esq = criarSala("Sala de Estar", "Taça quebrada");
    hall->dir = criarSala("Cozinha", "Faca faltando no suporte");

    hall->esq->esq = criarSala("Biblioteca", "Livro arrancado da prateleira");
    hall->esq->dir = criarSala("Sala de Música", "Partitura rasgada");

    hall->dir->esq = criarSala("Despensa", "Embalagens abertas");
    hall->dir->dir = criarSala("Jardim", "Pegadas que levam ao portão");

    PistaNode *pistas = NULL;

    printf("\n=== Exploração da Mansão ===\n");
    explorarSalasComPistas(hall, &pistas);

    printf("\n=== Pistas Coletadas (ordem alfabética) ===\n");
    exibirPistas(pistas);

    return 0;
}
