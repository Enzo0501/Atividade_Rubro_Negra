#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Usuario {
    char nome[100];
    int id;
    char email[100];
} Usuario;

typedef struct Node {
    Usuario user;
    int altura;
    struct Node *esq;
    struct Node *dir;
} Node;

// Função para retornar o máximo entre dois números
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Altura de um nó
int altura(Node* n) {
    return (n == NULL) ? 0 : n->altura;
}

// Fator de balanceamento
int fatorBalanceamento(Node* n) {
    return (n == NULL) ? 0 : altura(n->esq) - altura(n->dir);
}

// Alocação de um novo nó
Node* novoNode(char nome[], int id, char email[]) {
    Node* node = (Node*)malloc(sizeof(Node));
    strcpy(node->user.nome, nome);
    node->user.id = id;
    strcpy(node->user.email, email);
    node->esq = node->dir = NULL;
    node->altura = 1;
    return node;
}

// Rotação à direita
Node* rotacaoDireita(Node* y) {
    Node* x = y->esq;
    Node* T2 = x->dir;

    x->dir = y;
    y->esq = T2;

    y->altura = max(altura(y->esq), altura(y->dir)) + 1;
    x->altura = max(altura(x->esq), altura(x->dir)) + 1;

    return x;
}

// Rotação à esquerda
Node* rotacaoEsquerda(Node* x) {
    Node* y = x->dir;
    Node* T2 = y->esq;

    y->esq = x;
    x->dir = T2;

    x->altura = max(altura(x->esq), altura(x->dir)) + 1;
    y->altura = max(altura(y->esq), altura(y->dir)) + 1;

    return y;
}

// Inserção na árvore AVL
Node* inserir(Node* raiz, char nome[], int id, char email[]) {
    if (raiz == NULL) {
        return novoNode(nome, id, email);
    }

    if (strcmp(nome, raiz->user.nome) < 0)
        raiz->esq = inserir(raiz->esq, nome, id, email);
    else if (strcmp(nome, raiz->user.nome) > 0)
        raiz->dir = inserir(raiz->dir, nome, id, email);
    else {
        printf("Usuario com nome '%s' ja existe.\n", nome);
        return raiz;
    }

    raiz->altura = 1 + max(altura(raiz->esq), altura(raiz->dir));
    int balance = fatorBalanceamento(raiz);

    // Rotação simples ou dupla
    if (balance > 1 && strcmp(nome, raiz->esq->user.nome) < 0)
        return rotacaoDireita(raiz);
    if (balance < -1 && strcmp(nome, raiz->dir->user.nome) > 0)
        return rotacaoEsquerda(raiz);
    if (balance > 1 && strcmp(nome, raiz->esq->user.nome) > 0) {
        raiz->esq = rotacaoEsquerda(raiz->esq);
        return rotacaoDireita(raiz);
    }
    if (balance < -1 && strcmp(nome, raiz->dir->user.nome) < 0) {
        raiz->dir = rotacaoDireita(raiz->dir);
        return rotacaoEsquerda(raiz);
    }

    return raiz;
}

// Mínimo valor de um nó (utilizado na remoção)
Node* menorValor(Node* node) {
    Node* atual = node;
    while (atual->esq != NULL)
        atual = atual->esq;
    return atual;
}

// Remoção na árvore AVL
Node* remover(Node* raiz, char nome[]) {
    if (raiz == NULL)
        return raiz;

    if (strcmp(nome, raiz->user.nome) < 0)
        raiz->esq = remover(raiz->esq, nome);
    else if (strcmp(nome, raiz->user.nome) > 0)
        raiz->dir = remover(raiz->dir, nome);
    else {
        if (raiz->esq == NULL || raiz->dir == NULL) {
            Node* temp = (raiz->esq) ? raiz->esq : raiz->dir;
            if (temp == NULL) {
                temp = raiz;
                raiz = NULL;
            } else {
                *raiz = *temp;
            }
            free(temp);
        } else {
            Node* temp = menorValor(raiz->dir);
            raiz->user = temp->user;
            raiz->dir = remover(raiz->dir, temp->user.nome);
        }
    }

    if (raiz == NULL)
        return raiz;

    raiz->altura = 1 + max(altura(raiz->esq), altura(raiz->dir));
    int balance = fatorBalanceamento(raiz);

    // Rebalanceamento
    if (balance > 1 && fatorBalanceamento(raiz->esq) >= 0)
        return rotacaoDireita(raiz);
    if (balance > 1 && fatorBalanceamento(raiz->esq) < 0) {
        raiz->esq = rotacaoEsquerda(raiz->esq);
        return rotacaoDireita(raiz);
    }
    if (balance < -1 && fatorBalanceamento(raiz->dir) <= 0)
        return rotacaoEsquerda(raiz);
    if (balance < -1 && fatorBalanceamento(raiz->dir) > 0) {
        raiz->dir = rotacaoDireita(raiz->dir);
        return rotacaoEsquerda(raiz);
    }

    return raiz;
}

// Busca usuário
Node* buscar(Node* raiz, char nome[]) {
    if (raiz == NULL || strcmp(nome, raiz->user.nome) == 0)
        return raiz;

    if (strcmp(nome, raiz->user.nome) < 0)
        return buscar(raiz->esq, nome);
    else
        return buscar(raiz->dir, nome);
}

// Listagem em ordem alfabética (inordem)
void listar(Node* raiz) {
    if (raiz != NULL) {
        listar(raiz->esq);
        printf("Nome: %s | ID: %d | Email: %s\n", raiz->user.nome, raiz->user.id, raiz->user.email);
        listar(raiz->dir);
    }
}

// Menu principal
void menu() {
    printf("\n===== Sistema de Gerenciamento de Usuarios (AVL) =====\n");
    printf("1. Cadastrar Usuario\n");
    printf("2. Remover Usuario\n");
    printf("3. Buscar Usuario\n");
    printf("4. Listar Usuarios (Ordem Alfabetica)\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
}

int main() {
    Node* raiz = NULL;
    int opcao;
    char nome[100], email[100];
    int id;

    do {
        menu();
        scanf("%d", &opcao);
        getchar(); // consumir '\n'

        switch (opcao) {
            case 1:
                printf("Nome: ");
                fgets(nome, 100, stdin); nome[strcspn(nome, "\n")] = 0;
                printf("ID: ");
                scanf("%d", &id);
                getchar();
                printf("Email: ");
                fgets(email, 100, stdin); email[strcspn(email, "\n")] = 0;
                raiz = inserir(raiz, nome, id, email);
                break;
            case 2:
                printf("Nome para remover: ");
                fgets(nome, 100, stdin); nome[strcspn(nome, "\n")] = 0;
                raiz = remover(raiz, nome);
                break;
            case 3:
                printf("Nome para buscar: ");
                fgets(nome, 100, stdin); nome[strcspn(nome, "\n")] = 0;
                Node* encontrado = buscar(raiz, nome);
                if (encontrado) {
                    printf("Encontrado: Nome: %s | ID: %d | Email: %s\n", encontrado->user.nome, encontrado->user.id, encontrado->user.email);
                } else {
                    printf("Usuario nao encontrado.\n");
                }
                break;
            case 4:
                printf("\nUsuarios em Ordem Alfabetica:\n");
                listar(raiz);
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    return 0;
}
