#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define HASH_SIZE 101        // tamanho da tabela hash (primo razoável)
#define MAX_NAME 64
#define MAX_INPUT 128

/* -------------------- utilitários -------------------- */

char *my_strdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = (char*)malloc(n);
    if (!p) { perror("malloc"); exit(1); }
    memcpy(p, s, n);
    return p;
}

void trim_newline(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n == 0) return;
    if (s[n-1] == '\n') s[n-1] = '\0';
}

/* -------------------- Árvore de cômodos (árvore binária) -------------------- */

typedef struct Sala {
    char *nome;             // identificador único do cômodo
    struct Sala *esq;       // filho esquerdo
    struct Sala *dir;       // filho direito
} Sala;

/*
 * criarSala() – cria dinamicamente um cômodo.
 * Parâmetros: nome (string), ponteiros para filhos esquerdo e direito (podem ser NULL).
 * Retorno: ponteiro para a Sala alocada dinamicamente.
 */
Sala *criarSala(const char *nome, Sala *esq, Sala *dir) {
    Sala *s = (Sala*)malloc(sizeof(Sala));
    if (!s) { perror("malloc"); exit(1); }
    s->nome = my_strdup(nome);
    s->esq = esq;
    s->dir = dir;
    return s;
}

/* -------------------- BST para pistas coletadas -------------------- */

typedef struct PistaNode {
    char *pista;
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

/*
 * inserirPista() / adicionarPista() – insere a pista coletada na árvore de pistas.
 * Implementado como inserirPista: insere em ordem lexicográfica.
 * Retorna ponteiro à raiz (possível nova raiz).
 * Observação: evita duplicatas (não insere a mesma pista duas vezes).
 */
PistaNode *inserirPista(PistaNode *raiz, const char *pista) {
    if (!pista) return raiz;
    if (!raiz) {
        PistaNode *n = (PistaNode*)malloc(sizeof(PistaNode));
        if (!n) { perror("malloc"); exit(1); }
        n->pista = my_strdup(pista);
        n->esq = n->dir = NULL;
        return n;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp == 0) {
        return raiz;
    } else if (cmp < 0) {
        raiz->esq = inserirPista(raiz->esq, pista);
    } else {
        raiz->dir = inserirPista(raiz->dir, pista);
    }
    return raiz;
}

void listarPistasInOrder(PistaNode *raiz) {
    if (!raiz) return;
    listarPistasInOrder(raiz->esq);
    printf(" - %s\n", raiz->pista);
    listarPistasInOrder(raiz->dir);
}

/* -------------------- Tabela Hash (encadeamento) -------------------- */

typedef struct HashNode {
    char *chave;            // pista
    char *suspeito;         // nome do suspeito associado
    struct HashNode *proximo;
} HashNode;

typedef struct {
    HashNode *t[HASH_SIZE];
} HashTable;

unsigned int hash_str(const char *s) {
    unsigned long hash = 5381;
    int c;
    while ((c = *s++)) hash = ((hash << 5) + hash) + (unsigned char)c;
    return (unsigned int)(hash % HASH_SIZE);
}

/*
 * inserirNaHash() – insere associação pista/suspeito na tabela hash.
 * Se a chave já existe, atualiza o suspeito.
 */
void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito) {
    if (!ht || !pista || !suspeito) return;
    unsigned int idx = hash_str(pista);
    HashNode *n = ht->t[idx];
    while (n) {
        if (strcmp(n->chave, pista) == 0) {
            free(n->suspeito);
            n->suspeito = my_strdup(suspeito);
            return;
        }
        n = n->proximo;
    }
    HashNode *novo = (HashNode*)malloc(sizeof(HashNode));
    if (!novo) { perror("malloc"); exit(1); }
    novo->chave = my_strdup(pista);
    novo->suspeito = my_strdup(suspeito);
    novo->proximo = ht->t[idx];
    ht->t[idx] = novo;
}

/*
 * encontrarSuspeito() – consulta o suspeito correspondente a uma pista.
 * Retorna ponteiro para string do suspeito (não liberar), ou NULL se não existe.
 */
const char *encontrarSuspeito(HashTable *ht, const char *pista) {
    if (!ht || !pista) return NULL;
    unsigned int idx = hash_str(pista);
    HashNode *n = ht->t[idx];
    while (n) {
        if (strcmp(n->chave, pista) == 0) return n->suspeito;
        n = n->proximo;
    }
    return NULL;
}

void initHash(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; ++i) ht->t[i] = NULL;
}

void liberarHash(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashNode *n = ht->t[i];
        while (n) {
            HashNode *next = n->proximo;
            free(n->chave);
            free(n->suspeito);
            free(n);
            n = next;
        }
        ht->t[i] = NULL;
    }
}

/* -------------------- Lógica de pistas por sala -------------------- */

const char *pistaParaSala(const char *nomeSala) {
    if (!nomeSala) return NULL;
    if (strcmp(nomeSala, "Hall") == 0) return "Pegada de lama pequeno tamanho";
    if (strcmp(nomeSala, "Biblioteca") == 0) return "Fio de cabelo ruivo preso ao livro";
    if (strcmp(nomeSala, "Cozinha") == 0) return "Pegada de lama grande";
    if (strcmp(nomeSala, "Escritorio") == 0) return "Bilhete rasgado com iniciais R.J.";
    if (strcmp(nomeSala, "Jardim") == 0) return "Frasco de perfume caro";
    if (strcmp(nomeSala, "Sala de Jantar") == 0) return "Mancha de vinho tinto";
    if (strcmp(nomeSala, "Quarto1") == 0) return "Botão preto perdido";
    if (strcmp(nomeSala, "Quarto2") == 0) return "Anel com pedra azul";
    if (strcmp(nomeSala, "Porão") == 0) return "Ferramenta com marcas de sangue";
    return NULL;
}

/* -------------------- Exploração interativa -------------------- */

/*
 * explorarSalas() – navega pela árvore e ativa o sistema de pistas.
 * Correções:
 *  - coleta usa 'c' (coletar) e 'n' (não coletar) para evitar conflito com 's' (sair).
 *  - se a sala atual for folha (sem filhos), a função retorna automaticamente após tratamento.
 */
void explorarSalas(Sala *atual, PistaNode **coletadas, HashTable *ht) {
    if (!atual) return;
    char input[MAX_INPUT];

    printf("\nVocê está na sala: %s\n", atual->nome);

    const char *pista = pistaParaSala(atual->nome);
    if (pista) {
        printf("Você encontrou uma pista: \"%s\"\n", pista);
        printf("Deseja coletar essa pista? (c = coletar / n = não coletar): ");
        if (!fgets(input, sizeof(input), stdin)) { clearerr(stdin); }
        trim_newline(input);

        if (tolower(input[0]) == 'c') {
            *coletadas = inserirPista(*coletadas, pista);
            printf("✅ Pista coletada.\n");
        } else {
            printf("↩ Voltando para a sala anterior...\n");
            return; // <- AGORA RETORNA E NÃO REPETE A SALA
        }
    } else {
        printf("Nenhuma pista nesta sala.\n");
    }

    // Se a sala é folha, retorna automaticamente
    if (!atual->esq && !atual->dir) {
        printf("Esta sala não tem saídas. Voltando...\n");
        return;
    }

    while (1) {
        printf("\nMovimente-se: (e) esquerda, (d) direita, (s) sair desta sala\n");
        printf("Escolha: ");
        if (!fgets(input, sizeof(input), stdin)) { clearerr(stdin); }
        trim_newline(input);

        switch (tolower(input[0])) {
            case 'e':
                if (atual->esq) explorarSalas(atual->esq, coletadas, ht);
                else printf("Não há caminho à esquerda.\n");
                return;

            case 'd':
                if (atual->dir) explorarSalas(atual->dir, coletadas, ht);
                else printf("Não há caminho à direita.\n");
                return;

            case 's':
                printf("↩ Voltando para a sala anterior...\n");
                return;

            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    }
}


/* -------------------- Verificação final da acusação -------------------- */

void contar_pistas_para(PistaNode *n, HashTable *ht, const char *acusado, int *contador) {
    if (!n) return;
    contar_pistas_para(n->esq, ht, acusado, contador);
    const char *sus = encontrarSuspeito(ht, n->pista);
    if (sus && strcmp(sus, acusado) == 0) {
        (*contador)++;
    }
    contar_pistas_para(n->dir, ht, acusado, contador);
}

/*
 * verificarSuspeitoFinal() – conduz à fase de julgamento final.
 * Retorna o número de pistas que apontam para o acusado.
 */
int verificarSuspeitoFinal(PistaNode *coletadas, HashTable *ht, const char *acusado) {
    if (!acusado) return 0;
    int contador = 0;
    contar_pistas_para(coletadas, ht, acusado, &contador);
    return contador;
}

/* -------------------- Funções de limpeza -------------------- */

void liberarSalas(Sala *r) {
    if (!r) return;
    liberarSalas(r->esq);
    liberarSalas(r->dir);
    free(r->nome);
    free(r);
}

void liberarBST(PistaNode *r) {
    if (!r) return;
    liberarBST(r->esq);
    liberarBST(r->dir);
    free(r->pista);
    free(r);
}

/* -------------------- Main: montar mansão e mapa de pistas->suspeitos -------------------- */

int main(void) {
    printf("=== Bem-vindo ao Detetive na Mansão ===\n\n");

    /* Montagem fixa do mapa (árvore) */
    Sala *quarto1 = criarSala("Quarto1", NULL, NULL);
    Sala *quarto2 = criarSala("Quarto2", NULL, NULL);
    Sala *escritorio = criarSala("Escritorio", quarto1, quarto2);
    Sala *jardim = criarSala("Jardim", NULL, NULL);
    Sala *biblioteca = criarSala("Biblioteca", escritorio, jardim);

    Sala *cozinha = criarSala("Cozinha", NULL, NULL);
    Sala *porao = criarSala("Porão", NULL, NULL);
    Sala *salaJantar = criarSala("Sala de Jantar", cozinha, porao);

    Sala *hall = criarSala("Hall", biblioteca, salaJantar);

    PistaNode *coletadas = NULL;
    HashTable ht;
    initHash(&ht);

    /* mapa pista -> suspeito */
    inserirNaHash(&ht, "Pegada de lama pequeno tamanho", "Alice");
    inserirNaHash(&ht, "Pegada de lama grande", "Carlos");
    inserirNaHash(&ht, "Fio de cabelo ruivo preso ao livro", "Bianca");
    inserirNaHash(&ht, "Bilhete rasgado com iniciais R.J.", "Rafael");
    inserirNaHash(&ht, "Frasco de perfume caro", "Bianca");
    inserirNaHash(&ht, "Mancha de vinho tinto", "Carlos");
    inserirNaHash(&ht, "Botão preto perdido", "Alice");
    inserirNaHash(&ht, "Anel com pedra azul", "Diana");
    inserirNaHash(&ht, "Ferramenta com marcas de sangue", "Rafael");

    printf("Instruções: explore a mansão. Em cada sala poderá haver uma pista.\n");
    printf("Coletar: digite 'c' quando perguntado. Movimente-se com 'e' (esquerda), 'd' (direita), 's' (sair).\n\n");

    explorarSalas(hall, &coletadas, &ht);

    printf("\n=== Exploração encerrada ===\n");
    printf("Pistas que você coletou:\n");
    if (!coletadas) {
        printf("Nenhuma pista coletada.\n");
    } else {
        listarPistasInOrder(coletadas);
    }

    printf("\nSuspeitos conhecidos:\n");
    printf(" - Alice\n");
    printf(" - Bianca\n");
    printf(" - Carlos\n");
    printf(" - Diana\n");
    printf(" - Rafael\n");


    char acusado[MAX_INPUT];
    printf("\nQuem você acusa? Digite o nome do suspeito (ex: Alice): ");
    if (!fgets(acusado, sizeof(acusado), stdin)) { clearerr(stdin); }
    trim_newline(acusado);
    if (strlen(acusado) == 0) {
        printf("Nenhum acusado informado. Encerrando.\n");
    } else {
        int total = verificarSuspeitoFinal(coletadas, &ht, acusado);
        printf("\nResultado da acusação contra '%s':\n", acusado);
        if (total >= 2) {
            printf("SUCESSO: %d pista(s) apontam para %s. Há evidências suficientes para a acusação.\n", total, acusado);
        } else if (total == 1) {
            printf("FRACO: %d pista aponta para %s. Não há pistas suficientes (mínimo 2).\n", total, acusado);
        } else {
            printf("INOCENTE (pelo menos com as pistas coletadas): %d pistas apontam para %s.\n", total, acusado);
        }
    }

    liberarBST(coletadas);
    liberarHash(&ht);
    liberarSalas(hall);

    printf("\nObrigado por jogar!\n");
    return 0;
}
