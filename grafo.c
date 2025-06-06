/**
 * @file grafo.c
 * @brief Implementação de um grafo não orientado com funcionalidades de leitura,
 *        manipulação e análise, como componentes conexas, bipartição, diâmetros,
 *        pontos de articulação e arestas de corte.
 *
 * @author Julio Martins
 * @date 2023-10-01
 */

#include "grafo.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

/**
 * Remove espaços em branco no início e no final de uma string.
 * @param str  string a ser "limpa"
 * @return ponteiro para a string limpa
 */
static char *trim(char *str) {
    char *end;
    // Trim leading space
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0')
        return str;
    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    return str;
}

/**
 * Procura um vértice pelo nome.
 * @param g grafo a ser pesquisado
 * @param name  nome do vértice a ser pesquisado
 * @return  índice do vértice ou -1 se não existir
 */
static int find_vertex_index(struct grafo *g, const char *name) {
    for (int i = 0; i < g->n_vertices; i++) {
        if (strcmp(g->vertices[i], name) == 0) {
            return i;
        }
    }
    return -1;
}
/**
 * Adiciona um novo vértice ao grafo.
 * @param g grafo onde o vértice será adicionado
 * @param name nome do vértice a ser adicionado
 * @return índice do novo vértice
 */
static int add_vertex(struct grafo *g, const char *name) {
    int idx = g->n_vertices;
    g->vertices = realloc(g->vertices, (idx + 1) * sizeof(char *));
    g->adj = realloc(g->adj, (idx + 1) * sizeof(adj_node *));
    if (!g->vertices || !g->adj) exit(EXIT_FAILURE);

    g->vertices[idx] = malloc(strlen(name) + 1);
    if (!g->vertices[idx]) exit(EXIT_FAILURE);
    strcpy(g->vertices[idx], name);

    g->adj[idx] = NULL;
    g->n_vertices++;
    return idx;
}

/**
 * Retorna o índice de um vértice pelo nome, adicionando-o se necessário.
 * @param g grafo onde o vértice será adicionado
 * @param name nome do vértice a ser adicionado
 * @return índice do vértice
 */
static int get_vertex_index(struct grafo *g, const char *name) {
    int idx = find_vertex_index(g, name);
    if (idx < 0) {
        idx = add_vertex(g, name);
    }
    return idx;
}

/**
 * Adiciona uma aresta não orientada entre dois vértices com um peso especificado.
 * @param g  grafo onde a aresta será adicionada
 * @param u  vertice de origem
 * @param v  vertice de destino
 * @param weight  peso da aresta
 */
static void add_edge(struct grafo *g, int u, int v, int weight) {
    // u → v
    adj_node *node = malloc(sizeof(adj_node));
    if (!node) exit(EXIT_FAILURE);
    node->v      = v;
    node->weight = weight;
    node->next   = g->adj[u];
    g->adj[u]    = node;
    // v → u
    node = malloc(sizeof(adj_node));
    if (!node) exit(EXIT_FAILURE);
    node->v      = u;
    node->weight = weight;
    node->next   = g->adj[v];
    g->adj[v]    = node;

    g->n_arestas++;
}

struct grafo *le_grafo(FILE *f) {
    char buffer[4096];
    struct grafo *g = malloc(sizeof(struct grafo));
    if (!g) return NULL;
    g->name       = NULL;
    g->n_vertices = 0;
    g->n_arestas  = 0;
    g->vertices   = NULL;
    g->adj        = NULL;

    int have_name = 0;
    while (fgets(buffer, sizeof(buffer), f)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        char *line = trim(buffer);
        if (line[0] == '\0' || (line[0] == '/' && line[1] == '/')) {
            continue;
        }
        if (!have_name) {
            // Primeira linha não-comentário é o nome do grafo
            g->name = malloc(strlen(line) + 1);
            if (!g->name) exit(EXIT_FAILURE);
            strcpy(g->name, line);
            have_name = 1;
            continue;
        }
        // Verifica se é linha de aresta (contém "--")
        char *d = strstr(line, "--");
        if (d) {
            // separar "v1 -- v2 [peso]"
            *d = '\0';
            char *v1 = trim(line);
            char *right = trim(d + 2);
            // right tem "v2 [peso]" ou só "v2"
            char vname2[256];
            int weight = 1;  // peso padrão = 1
            char *tok = strtok(right, " \t");
            if (tok) {
                strcpy(vname2, tok);
                char *tok2 = strtok(NULL, " \t");
                if (tok2) {
                    weight = atoi(tok2);
                }
            } else {
                continue; // linha malformada → ignora
            }
            int i1 = get_vertex_index(g, v1);
            int i2 = get_vertex_index(g, vname2);
            add_edge(g, i1, i2, weight);
        } else {
            // Linha de vértice isolado
            char *v = line;
            if (find_vertex_index(g, v) < 0) {
                add_vertex(g, v);
            }
        }
    }
    return g;
}

unsigned int destroi_grafo(struct grafo *g) {
    if (!g) return 0;
    for (int i = 0; i < g->n_vertices; i++) {
        adj_node *p = g->adj[i];
        while (p) {
            adj_node *tmp = p;
            p = p->next;
            free(tmp);
        }
        free(g->vertices[i]);
    }
    free(g->vertices);
    free(g->adj);
    free(g->name);
    free(g);
    return 1;
}

char *nome(struct grafo *g) {
    if (!g) return NULL;
    return g->name;
}

unsigned int n_vertices(struct grafo *g) {
    if (!g) return 0;
    return g->n_vertices;
}


unsigned int n_arestas(struct grafo *g) {
    if (!g) return 0;
    return g->n_arestas;
}

/**
 * Realiza uma busca em profundidade (DFS) para marcar todos os vértices
 * alcançados a partir de um vértice inicial.
 * @param g grafo a ser percorrido
 * @param u vértice inicial
 * @param visited vetor de controle de visitados
 */
static void dfs_comp(struct grafo *g, int u, int *visited) {
    visited[u] = 1;
    adj_node *p = g->adj[u];
    while (p) {
        int v = p->v;
        if (!visited[v]) dfs_comp(g, v, visited);
        p = p->next;
    }
}

unsigned int n_componentes(struct grafo *g) {
    if (!g) return 0;
    int n = g->n_vertices;
    int *visited = calloc(n, sizeof(int));
    if (!visited) exit(EXIT_FAILURE);
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            count++;
            dfs_comp(g, i, visited);
        }
    }
    free(visited);
    return count;
}

/**
 * Realiza uma busca em profundidade (DFS) para verificar se o grafo é bipartido.
 * @param g  grafo a ser analisado
 * @param u  vértice atual
 * @param color  vetor de cores (-1 = não visitado, 0 = cor 1, 1 = cor 2)
 * @return  1 se g é bipartido, 0 caso contrário
 */
static int dfs_bipartite(struct grafo *g, int u, int *color) {
    adj_node *p = g->adj[u];
    while (p) {
        int v = p->v;
        if (color[v] == -1) {
            color[v] = 1 - color[u];
            if (!dfs_bipartite(g, v, color)) return 0;
        } else if (color[v] == color[u]) {
            return 0;
        }
        p = p->next;
    }
    return 1;
}

unsigned int bipartido(struct grafo *g) {
    if (!g) return 0;
    int n = g->n_vertices;
    int *color = malloc(n * sizeof(int));
    if (!color) exit(EXIT_FAILURE);
    for (int i = 0; i < n; i++) color[i] = -1;
    for (int i = 0; i < n; i++) {
        if (color[i] == -1) {
            color[i] = 0;
            if (!dfs_bipartite(g, i, color)) {
                free(color);
                return 0;
            }
        }
    }
    free(color);
    return 1;
}

static const int INF = INT_MAX / 2;

/**
 * Calcula as distâncias mínimas de um vértice de origem para todos os outros vértices
 * @param g  grafo a ser analisado
 * @param src  índice do vértice de origem
 * @return  um vetor de distâncias do vértice src para todos os outros vértices
 */
static int *dijkstra(struct grafo *g, int src) {
    int n = g->n_vertices;
    int *dist    = malloc(n * sizeof(int));
    int *visited = calloc(n, sizeof(int));
    if (!dist || !visited) exit(EXIT_FAILURE);
    for (int i = 0; i < n; i++) dist[i] = INF;
    dist[src] = 0;
    for (int _ = 0; _ < n; _++) {
        int u = -1, min = INF;
        for (int j = 0; j < n; j++) {
            if (!visited[j] && dist[j] < min) {
                min = dist[j];
                u = j;
            }
        }
        if (u < 0) break;
        visited[u] = 1;
        adj_node *p = g->adj[u];
        while (p) {
            int v = p->v;
            int w = p->weight;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
            }
            p = p->next;
        }
    }
    free(visited);
    return dist;
}

char *diametros(struct grafo *g) {
    if (!g) return NULL;
    int n = g->n_vertices;
    int *visited = calloc(n, sizeof(int));
    if (!visited) exit(EXIT_FAILURE);

    // Máximo de um componente por vértice
    int *diam = malloc(n * sizeof(int));
    if (!diam) exit(EXIT_FAILURE);
    int comp_count = 0;

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            // Descobre todos os vértices desse componente
            int *stack   = malloc(n * sizeof(int));
            int  top     = 0;
            visited[i]   = 1;
            stack[top++] = i;
            int comp_size = 0;
            int *members  = malloc(n * sizeof(int));
            members[comp_size++] = i;

            while (top) {
                int u = stack[--top];
                adj_node *p = g->adj[u];
                while (p) {
                    int v = p->v;
                    if (!visited[v]) {
                        visited[v] = 1;
                        stack[top++]    = v;
                        members[comp_size++] = v;
                    }
                    p = p->next;
                }
            }
            // Calcular o diâmetro deste componente
            int maxdist = 0;
            for (int j = 0; j < comp_size; j++) {
                int u = members[j];
                int *dist = dijkstra(g, u);
                for (int k = 0; k < comp_size; k++) {
                    int v = members[k];
                    if (dist[v] < INF && dist[v] > maxdist) {
                        maxdist = dist[v];
                    }
                }
                free(dist);
            }
            diam[comp_count++] = maxdist;
            free(stack);
            free(members);
        }
    }
    free(visited);

    // Ordena diametros em ordem não decrescente
    for (int i = 0; i < comp_count - 1; i++) {
        for (int j = i + 1; j < comp_count; j++) {
            if (diam[i] > diam[j]) {
                int tmp = diam[i];
                diam[i] = diam[j];
                diam[j] = tmp;
            }
        }
    }

    // Monta a string de saída
    int bufsize = comp_count * 12 + 1;
    char *res = malloc(bufsize);
    if (!res) exit(EXIT_FAILURE);
    res[0] = '\0';

    char temp[32];
    for (int i = 0; i < comp_count; i++) {
        if (i > 0) strcat(res, " ");
        snprintf(temp, sizeof(temp), "%d", diam[i]);
        strcat(res, temp);
    }
    free(diam);
    return res;
}

//------------------------------------------------------------------------------
// Articulation points (vértices de corte)
static int time_dfs_ap;

/**
 * Realiza uma busca em profundidade (DFS) para encontrar pontos de articulação no grafo.
 * @param g  grafo a ser analisado
 * @param u  vértice atual
 * @param visited  vetor de controle de visitados
 * @param disc  vetor de descoberta (tempo de visita)
 * @param low  vetor de menor descoberta alcançada
 * @param parent  vetor de pais na DFS
 * @param ap  vetor de pontos de articulação (1 se é ponto de articulação, 0 caso contrário)
 */
static void ap_dfs(struct grafo *g, int u, int *visited, int *disc, int *low, int *parent, int *ap) {
    visited[u] = 1;
    disc[u] = low[u] = ++time_dfs_ap;
    int children = 0;

    adj_node *p = g->adj[u];
    while (p) {
        int v = p->v;
        if (!visited[v]) {
            children++;
            parent[v] = u;
            ap_dfs(g, v, visited, disc, low, parent, ap);
            // atualiza low[u]
            low[u] = (low[u] < low[v]) ? low[u] : low[v];
            // u é raiz e tem 2 ou mais filhos
            if (parent[u] == -1 && children > 1) {
                ap[u] = 1;
            }
            // u não é raiz e low[v] >= disc[u]
            if (parent[u] != -1 && low[v] >= disc[u]) {
                ap[u] = 1;
            }
        } else if (v != parent[u]) {
            // v é um back-edge
            low[u] = (low[u] < disc[v]) ? low[u] : disc[v];
        }
        p = p->next;
    }
}

char *vertices_corte(struct grafo *g) {
    if (!g) return NULL;
    int n = g->n_vertices;
    int *visited = calloc(n, sizeof(int));
    int *disc    = malloc(n * sizeof(int));
    int *low     = malloc(n * sizeof(int));
    int *parent  = malloc(n * sizeof(int));
    int *ap      = calloc(n, sizeof(int));
    if (!visited || !disc || !low || !parent || !ap) exit(EXIT_FAILURE);

    for (int i = 0; i < n; i++) {
        parent[i]  = -1;
        visited[i] = 0;
        ap[i]      = 0;
    }
    time_dfs_ap = 0;
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            ap_dfs(g, i, visited, disc, low, parent, ap);
        }
    }
    // Conta quantos vértices de corte existem
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (ap[i]) count++;
    }
    if (count == 0) {
        free(visited); free(disc); free(low); free(parent); free(ap);
        char *res = malloc(1);
        res[0] = '\0';
        return res;
    }
    // Coleta nomes e ordena
    char **names = malloc(count * sizeof(char *));
    int idx = 0;
    for (int i = 0; i < n; i++) {
        if (ap[i]) {
            names[idx++] = g->vertices[i];
        }
    }
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(names[i], names[j]) > 0) {
                char *tmp = names[i];
                names[i] = names[j];
                names[j] = tmp;
            }
        }
    }
    // Construir string
    int length = 0;
    for (int i = 0; i < count; i++) {
        length += strlen(names[i]) + 1;
    }
    char *res = malloc(length + 1);
    if (!res) exit(EXIT_FAILURE);
    res[0] = '\0';
    for (int i = 0; i < count; i++) {
        if (i > 0) strcat(res, " ");
        strcat(res, names[i]);
    }
    free(names);
    free(visited); free(disc); free(low); free(parent); free(ap);
    return res;
}

/**
 *  Estrutura para armazenar pares de arestas (u, v) onde u < v
 *  representa uma aresta de corte.
 */
typedef struct edge_pair {
    char *u;
    char *v;
} edge_pair;

static int time_dfs_br;

/**
 * Realiza uma busca em profundidade (DFS) para encontrar pontes no grafo.
 * @param g  grafo a ser analisado
 * @param u  vértice atual
 * @param visited  vetor de controle de visitados
 * @param disc  vetor de descoberta (tempo de visita)
 * @param low  vetor de menor descoberta alcançada
 * @param parent  vetor de pais na DFS
 * @param bridges  vetor de ponte (pares de arestas)
 * @param bcount  contador de pontes encontradas
 */
static void bridge_dfs(struct grafo *g, int u, int *visited, int *disc, int *low, int *parent, edge_pair **bridges, int *bcount) {
    visited[u] = 1;
    disc[u] = low[u] = ++time_dfs_br;

    adj_node *p = g->adj[u];
    while (p) {
        int v = p->v;
        if (!visited[v]) {
            parent[v] = u;
            bridge_dfs(g, v, visited, disc, low, parent, bridges, bcount);
            low[u] = (low[u] < low[v]) ? low[u] : low[v];
            if (low[v] > disc[u]) {
                // (u,v) é ponte — grava par de nomes em ordem alfabética
                edge_pair *ep = malloc(sizeof(edge_pair));
                if (!ep) exit(EXIT_FAILURE);
                if (strcmp(g->vertices[u], g->vertices[v]) < 0) {
                    ep->u = g->vertices[u];
                    ep->v = g->vertices[v];
                } else {
                    ep->u = g->vertices[v];
                    ep->v = g->vertices[u];
                }
                bridges[(*bcount)++] = ep;
            }
        } else if (v != parent[u]) {
            low[u] = (low[u] < disc[v]) ? low[u] : disc[v];
        }
        p = p->next;
    }
}

char *arestas_corte(struct grafo *g) {
    if (!g) return NULL;
    int n = g->n_vertices;
    int *visited = calloc(n, sizeof(int));
    int *disc    = malloc(n * sizeof(int));
    int *low     = malloc(n * sizeof(int));
    int *parent  = malloc(n * sizeof(int));
    if (!visited || !disc || !low || !parent) exit(EXIT_FAILURE);

    // Máximo de pontes em grafo simples << n*(n-1)/2
    edge_pair **bridges = malloc((n * (n - 1) / 2) * sizeof(edge_pair *));
    if (!bridges) exit(EXIT_FAILURE);

    for (int i = 0; i < n; i++) {
        visited[i] = 0;
        parent[i]  = -1;
    }
    time_dfs_br = 0;
    int bcount = 0;
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            bridge_dfs(g, i, visited, disc, low, parent, bridges, &bcount);
        }
    }
    if (bcount == 0) {
        free(visited); free(disc); free(low); free(parent); free(bridges);
        char *res = malloc(1);
        res[0] = '\0';
        return res;
    }
    // Ordena pares de pontes lexicograficamente pelo primeiro nome, depois pelo segundo
    for (int i = 0; i < bcount - 1; i++) {
        for (int j = i + 1; j < bcount; j++) {
            if (strcmp(bridges[i]->u, bridges[j]->u) > 0 ||
               (strcmp(bridges[i]->u, bridges[j]->u) == 0 &&
                strcmp(bridges[i]->v, bridges[j]->v) > 0)) {
                edge_pair *tmp = bridges[i];
                bridges[i] = bridges[j];
                bridges[j] = tmp;
            }
        }
    }
    // Calcula tamanho total da string
    int length = 0;
    for (int i = 0; i < bcount; i++) {
        length += strlen(bridges[i]->u) + 1 + strlen(bridges[i]->v) + (i < bcount - 1 ? 1 : 0);
    }
    char *res = malloc(length + 1);
    if (!res) exit(EXIT_FAILURE);
    res[0] = '\0';
    for (int i = 0; i < bcount; i++) {
        if (i > 0) strcat(res, " ");
        strcat(res, bridges[i]->u);
        strcat(res, " ");
        strcat(res, bridges[i]->v);
    }
    for (int i = 0; i < bcount; i++) {
        free(bridges[i]);
    }
    free(bridges);
    free(visited); free(disc); free(low); free(parent);
    return res;
}
