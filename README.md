# Projeto de Grafos em C

Este repositório contém uma implementação em C de uma biblioteca para manipulação de grafos genéricos, permitindo:

- Leitura de um grafo a partir de arquivo de texto (entrada padrão).
- Armazenamento interno via listas de adjacência.
- Cálculo de propriedades estruturais, como:
    - Número de vértices e arestas
    - Número de componentes conexas
    - Verificação se o grafo é bipartido
    - Cálculo dos diâmetros de cada componente conexa
    - Identificação de vértices de corte (articulação)
    - Identificação de arestas de corte (pontes)

---
## Estrutura de Arquivos

```
    graph_parser
    ├── assignment.txt
    ├── grafo.h
    ├── grafo.c
    └── teste.c
```

O projeto segue a especificação contida em `assignment.txt` e organiza o código nas seguintes unidades:

- **grafo.h** – Cabeçalho público que declara a interface “grafo”.
- **grafo.c** – Implementação das funções definidas em `grafo.h`, incluindo estruturas internas e algoritmos (DFS, Dijkstra ingênuo, busca de pontos de articulação e pontes).
- **teste.c** – Programa principal que demonstra o uso da biblioteca de grafos lendo um grafo da entrada padrão e exibindo as informações solicitadas.
- **assignment.txt** – Detalhes da especificação do projeto.


---
## Formato de Entrada do Grafo

O grafo deve ser fornecido em um arquivo de texto seguindo estas regras:

### 1. Comentários e linhas vazias
   * Linhas que começam com // (duas barras) são ignoradas por completo.
   * Linhas totalmente em branco (ou contendo apenas espaços/tabs) também são ignoradas.
### 2. Primeira linha não-comentário
   * Contém o nome do grafo (uma string qualquer, até o final da linha).
   * Esse nome é armazenado internamente e retornado por nome(g).
### 3. Linhas subsequentes
   * Vértice isolado: se a linha não contiver o delimitador --, interpreta-se como “declaração” de um vértice sem arestas, apenas para garantir que ele exista no grafo. <br>Exemplo:

     ```
       // Vertices isolados
       A
       B
       C
     ```
   * Aresta não-orientada: se a linha contiver o símbolo -- no formato
      ```
        V1 -- V2
      ```
      onde V1 e V2 são os nomes dos vértices (strings sem espaços), cria uma aresta não-orientada entre eles. 
   * O peso é opcional. Se não informado, assume-se peso = 1. Se informado, deve ser um inteiro ≥ 0. Se um dos vértices não existir, ele é criado automaticamente. <br>
     Exemplo:
     ```
       // Arestas
       A -- B
       B -- C 5
       C -- D 10
     ```
### 4. Regras adicionais

   * Espaços em branco nas extremidades das linhas são automaticamente “trimados” (prefixo/sufixo).
   * Se a mesma aresta for listada mais de uma vez, o grafo irá duplicar (ou seja, cada ocorrência é armazenada separadamente e conta no total de arestas).
   * Se um vértice isolado for declarado após ter aparecido em alguma aresta, simplesmente não gera duplicata de nome; o índice existente é mantido.

## Como compilar

- Para compilar o projeto, certifique-se de estar em um diretório que contenha os arquivos:

    ```
    grafo.h
    grafo.c
    teste.c
    ```
- Use o compilador GCC (ou outro compatível com C99 ou superior). <br>
Por exemplo:

    ```
    gcc -std=c99 -Wall -Wextra -o teste grafo.c teste.c
    ```
- Ou use o makefile disponivel neste projeto. <br> Exemplo:
    ```
    make all
    ```

## Exemplo de uso

### 1. Arquivo de Exemplo “grafo_exemplo.txt
```
grafo: Desordenado
10 vertices
7 arestas
4 componentes
não bipartido
diâmetros: 0 2 10 13
vértices de corte: a zzz
arestas de corte: 1 2 3 a aaa zzz abc zzz
```

### 2. Execução

```
./teste < grafo_exemplo.txt > saida.txt
```

### 3. Saida esperada

```
grafo: Desordenado
10 vertices
7 arestas
4 componentes
não bipartido
diâmetros: 0 2 10 13
vértices de corte: a zzz
arestas de corte: 1 2 3 a aaa zzz abc zzz
```