#ifndef GRAFO_H
#define GRAFO_H

#include <stdio.h>


/**
 * Estrutura de dados para representar um nó na lista de adjacência.
 * Cada nó contém o índice do vértice adjacente, o peso da aresta
 * e um ponteiro para o próximo nó na lista.
 */
typedef struct adj_node {
    int v;
    int weight;
    struct adj_node *next;
} adj_node;

/**
 * Estrutura de dados para representar um grafo.
 * Contém o nome do grafo, número de vértices, número de arestas,
 * array de nomes dos vértices e listas de adjacência.
 */
struct grafo {
    char *name;
    int n_vertices;
    int n_arestas;
    char **vertices;      // array de nomes de vértice
    adj_node **adj;       // listas de adjacência
};

/**
 * Lê um grafo de um arquivo e o armazena em uma estrutura de dados.
 * @param f arquivo de onde o grafo será lido
 * @return ponteiro para a estrutura do grafo lido ou NULL em caso de erro
 */
struct grafo *le_grafo(FILE *f);


/**
 * Destrói o grafo e libera toda a memória alocada.
 * @param g grafo a ser destruído
 * @return 1 em caso de sucesso, 0 se g for NULL
 */
unsigned int destroi_grafo(struct grafo *g);


/**
 * Devolve o nome do grafo g.
 * @param g grafo cujo nome será devolvido
 * @return  nome do grafo ou NULL se g for NULL
 */
char *nome(struct grafo *g);

/**
 * Verifica se o grafo g é bipartido.
 * @param g grafo a ser verificado
 * @return  1 se g é bipartido, 0 caso contrário
 */
unsigned int bipartido(struct grafo *g);

/**
 * Devolve o número de vértices em g.
 * @param g  grafo cujo número de vértices será devolvido
 * @return  número de vértices em g ou 0 se g for NULL
 */
unsigned int n_vertices(struct grafo *g);

/**
 * Devolve o número de arestas em g.
 * @param g grafo cujo número de arestas será devolvido
 * @return  número de arestas em g ou 0 se g for NULL
 */
unsigned int n_arestas(struct grafo *g);


/**
 * Devolve o número de componentes conexas em g.
 * @param g grafo a ser analisado
 * @return  número de componentes conexas em g
 */
unsigned int n_componentes(struct grafo *g);

/**
 * Calcula os diâmetros dos componentes conexos do grafo g.
 * @param g grafo a ser analisado
 * @return  uma string com os diâmetros dos componentes em ordem não decrescente
 */
char *diametros(struct grafo *g);

/**
 * Devolve uma string com os vértices de corte do grafo g em ordem alfabética.
 * Cada vértice é representado pelo seu nome.
 * @param g grafo a ser analisado
 * @return  string com os vértices de corte ou NULL se g for NULL
 */
char *vertices_corte(struct grafo *g);

/**
 * Devolve uma string com as arestas de corte do grafo g em ordem alfabética.
 * Cada aresta é representada por um par de nomes de vértices em ordem alfabética.
 * @param g grafo a ser analisado
 * @return  string com as arestas de corte ou NULL se g for NULL
 */
char *arestas_corte(struct grafo *g);

#endif
