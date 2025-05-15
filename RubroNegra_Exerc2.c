#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { VERMELHO, PRETO } Cor;

typedef struct Produto {
    int codigo;
    char nome[100];
    int quantidade;
    float preco;
} Produto;

typedef struct Node {
    Produto prod;
    Cor cor;
    struct Node *esq, *dir, *pai;
} Node;

Node* raiz = NULL;

// Função utilitária
Node* criarNo(int codigo, char nome[], int quantidade, float preco) {
    Node* novo = (Node*)malloc(sizeof(Node));
    novo->prod.codigo = codigo;
    strcpy(novo->prod.nome, nome);
    novo->prod.quantidade = quantidade;
    novo->prod.preco = preco;
    novo->cor = VERMELHO;
    novo->esq = novo->dir = novo->pai = NULL;
    return novo;
}

// Funções auxiliares
Node* avo(Node* n) {
    return (n && n->pai) ? n->pai->pai : NULL;
}

Node* tio(Node* n) {
    Node* a = avo(n);
    if (!a) return NULL;
    return (n->pai == a->esq) ? a->dir : a->esq;
}

void rotacaoEsquerda(Node** raiz, Node* x) {
    Node* y = x->dir;
    x->dir = y->esq;
    if (y->esq) y->esq->pai = x;
    y->pai = x->pai;
    if (!x->pai)
        *raiz = y;
    else if (x == x->pai->esq)
        x->pai->esq = y;
    else
        x->pai->dir = y;
    y->esq = x;
    x->pai = y;
}

void rotacaoDireita(Node** raiz, Node* y) {
    Node* x = y->esq;
    y->esq = x->dir;
    if (x->dir) x->dir->pai = y;
    x->pai = y->pai;
    if (!y->pai)
        *raiz = x;
    else if (y == y->pai->dir)
        y->pai->dir = x;
    else
        y->pai->esq = x;
    x->dir = y;
    y->pai = x;
}

// Corrige a árvore após inserção
void corrigirInsercao(Node** raiz, Node* z) {
    while (z != *raiz && z->pai->cor == VERMELHO) {
        Node* p = z->pai;
        Node* a = avo(z);
        Node* t = tio(z);

        if (t && t->cor == VERMELHO) {
            p->cor = PRETO;
            t->cor = PRETO;
            a->cor = VERMELHO;
            z = a;
        } else {
            if (p == a->esq) {
                if (z == p->dir) {
                    z = p;
                    rotacaoEsquerda(raiz, z);
                }
                p->cor = PRETO;
                a->cor = VERMELHO;
                rotacaoDireita(raiz, a);
            } else {
                if (z == p->esq) {
                    z = p;
                    rotacaoDireita(raiz, z);
                }
                p->cor = PRETO;
                a->cor = VERMELHO;
                rotacaoEsquerda(raiz, a);
            }
        }
    }
    (*raiz)->cor = PRETO;
}

// Inserção do produto
void inserirProduto(int codigo, char nome[], int quantidade, float preco) {
    Node *z = criarNo(codigo, nome, quantidade, preco);
    Node *y = NULL;
    Node *x = raiz;

    while (x != NULL) {
        y = x;
        if (codigo < x->prod.codigo)
            x = x->esq;
        else if (codigo > x->prod.codigo)
            x = x->dir;
        else {
            printf("Produto com codigo %d ja existe.\n", codigo);
            free(z);
            return;
        }
    }

    z->pai = y;
    if (y == NULL)
        raiz = z;
    else if (codigo < y->prod.codigo)
        y->esq = z;
    else
        y->dir = z;

    corrigirInsercao(&raiz, z);
}

// Busca por produto
Node* buscar(Node* raiz, int codigo) {
    if (raiz == NULL || raiz->prod.codigo == codigo)
        return raiz;

    if (codigo < raiz->prod.codigo)
        return buscar(raiz->esq, codigo);
    else
        return buscar(raiz->dir, codigo);
}

// Mínimo de uma subárvore (utilizado na remoção)
Node* minimo(Node* node) {
    while (node->esq != NULL)
        node = node->esq;
    return node;
}

// Transplanta um nó por outro
void transplantar(Node** raiz, Node* u, Node* v) {
    if (!u->pai)
        *raiz = v;
    else if (u == u->pai->esq)
        u->pai->esq = v;
    else
        u->pai->dir = v;
    if (v) v->pai = u->pai;
}

// Corrige após remoção (simplificada)
void corrigirRemocao(Node** raiz, Node* x) {
    while (x != *raiz && (!x || x->cor == PRETO)) {
        if (x == x->pai->esq) {
            Node* w = x->pai->dir;
            if (w && w->cor == VERMELHO) {
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                rotacaoEsquerda(raiz, x->pai);
                w = x->pai->dir;
            }
            if ((!w->esq || w->esq->cor == PRETO) && (!w->dir || w->dir->cor == PRETO)) {
                w->cor = VERMELHO;
                x = x->pai;
            } else {
                if (!w->dir || w->dir->cor == PRETO) {
                    if (w->esq) w->esq->cor = PRETO;
                    w->cor = VERMELHO;
                    rotacaoDireita(raiz, w);
                    w = x->pai->dir;
                }
                w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                if (w->dir) w->dir->cor = PRETO;
                rotacaoEsquerda(raiz, x->pai);
                x = *raiz;
            }
        } else {
            Node* w = x->pai->esq;
            if (w && w->cor == VERMELHO) {
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                rotacaoDireita(raiz, x->pai);
                w = x->pai->esq;
            }
            if ((!w->dir || w->dir->cor == PRETO) && (!w->esq || w->esq->cor == PRETO)) {
                w->cor = VERMELHO;
                x = x->pai;
            } else {
                if (!w->esq || w->esq->cor == PRETO) {
                    if (w->dir) w->dir->cor = PRETO;
                    w->cor = VERMELHO;
                    rotacaoEsquerda(raiz, w);
                    w = x->pai->esq;
                }
                w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                if (w->esq) w->esq->cor = PRETO;
                rotacaoDireita(raiz, x->pai);
                x = *raiz;
            }
        }
    }
    if (x) x->cor = PRETO;
}

// Remoção de produto
void removerProduto(int codigo) {
    Node* z = buscar(raiz, codigo);
    if (!z) {
        printf("Produto com codigo %d nao encontrado.\n", codigo);
        return;
    }

    Node* y = z;
    Cor yOriginalCor = y->cor;
    Node* x;

    if (z->esq == NULL) {
        x = z->dir;
        transplantar(&raiz, z, z->dir);
    } else if (z->dir == NULL) {
        x = z->esq;
        transplantar(&raiz, z, z->esq);
    } else {
        y = minimo(z->dir);
        yOriginalCor = y->cor;
        x = y->dir;
        if (y->pai == z) {
            if (x) x->pai = y;
        } else {
            transplantar(&raiz, y, y->dir);
            y->dir = z->dir;
            if (y->dir) y->dir->pai = y;
        }
        transplantar(&raiz, z, y);
        y->esq = z->esq;
        if (y->esq) y->esq->pai = y;
        y->cor = z->cor;
    }

    if (yOriginalCor == PRETO && x)
        corrigirRemocao(&raiz, x);

    free(z);
}

// Impressão em ordem
void listar(Node* r) {
    if (r != NULL) {
        listar(r->esq);
        printf("Codigo: %d | Nome: %s | Qtde: %d | Preco: %.2f | Cor: %s\n",
               r->prod.codigo, r->prod.nome, r->prod.quantidade, r->prod.preco,
               r->cor == VERMELHO ? "Vermelho" : "Preto");
        listar(r->dir);
    }
}

// Menu principal
void menu() {
    printf("\n==== Sistema de Inventario (Arvore Rubro-Negra) ====\n");
    printf("1. Cadastrar Produto\n");
    printf("2. Remover Produto\n");
    printf("3. Buscar Produto\n");
    printf("4. Listar Produtos (ordem crescente)\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
}

int main() {
    int opcao, codigo, quantidade;
    float preco;
    char nome[100];

    do {
        menu();
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                printf("Codigo: ");
                scanf("%d", &codigo);
                getchar();
                printf("Nome: ");
                fgets(nome, 100, stdin); nome[strcspn(nome, "\n")] = 0;
                printf("Quantidade: ");
                scanf("%d", &quantidade);
                printf("Preco: ");
                scanf("%f", &preco);
                inserirProduto(codigo, nome, quantidade, preco);
                break;
            case 2:
                printf("Codigo do produto a remover: ");
                scanf("%d", &codigo);
                removerProduto(codigo);
                break;
            case 3:
                printf("Codigo para buscar: ");
                scanf("%d", &codigo);
                Node* encontrado = buscar(raiz, codigo);
                if (encontrado) {
                    printf("Produto encontrado:\nCodigo: %d | Nome: %s | Qtde: %d | Preco: %.2f\n",
                           encontrado->prod.codigo, encontrado->prod.nome,
                           encontrado->prod.quantidade, encontrado->prod.preco);
                } else {
                    printf("Produto nao encontrado.\n");
                }
                break;
            case 4:
                printf("Lista de Produtos:\n");
                listar(raiz);
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}
