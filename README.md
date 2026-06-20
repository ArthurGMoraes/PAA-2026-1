# Redução Transitiva de Grafos

## 1. O Problema

Dado um grafo direcionado G = (V, E), a **redução transitiva** produz um
grafo G′ com o **menor número possível de arestas**, mantendo exatamente
a mesma **atingibilidade**: se u alcança v em G, u também alcança v em G′,
e vice-versa.

Uma aresta (u -> v) é considerada **redundante** quando existe algum outro
caminho de u até v além da aresta direta. Se existir, a aresta pode ser
removida sem alterar quem alcança quem.

Importante: o algoritmo **não se importa com o tamanho dos caminhos**.
Ele não procura o caminho mais curto nem o mais longo, só responde
"existe algum jeito alternativo de chegar lá?". Se a resposta for sim,
a aresta direta é descartável.

---

## 2. Decisão: Representação do Grafo

Usamos **lista de adjacência** (`vector<list<int>>`), uma lista por vértice
contendo seus vizinhos diretos.

**Por que não matriz de adjacência?**

| Aspecto | Lista de Adjacência | Matriz de Adjacência |
|---|---|---|
| Espaço | O(V + E) | O(V²) |
| Percorrer vizinhos de um vértice | O(grau do vértice) | O(V) |
| Verificar se aresta existe | O(grau do vértice) | O(1) |

Como o algoritmo é baseado em **caminhamento** (DFS), o que mais importa é
percorrer os vizinhos de um vértice rapidamente, o que é mais eficiente na lista de adjacência. Além disso, grafos reais costumam ser **esparsos** (poucas arestas comparado a V²), então a matriz desperdiçaria
muita memória à toa.

---

## 3. Decisão: Algoritmo Baseado em Caminhamento (DFS)

O enunciado pede explicitamente um algoritmo baseado em caminhamento no
grafo. A estratégia escolhida:

> Para cada aresta (u → v): finja que essa aresta não existe e faça uma
> busca em profundidade (DFS) a partir de u. Se v ainda for alcançado,
> a aresta era redundante e remove. Se não for alcançado, a aresta é
> necessária e mantém.

Isso é repetido para **todas** as arestas do grafo.

### Por que ignorar a aresta direta durante a busca?

Esse é o ponto mais sutil do algoritmo. Se não ignorássemos a aresta
direta, a pergunta "u alcança v?" seria **sempre verdadeira** (porque a
aresta u->v existe no grafo) e o algoritmo removeria todas as arestas,
o que é errado.

A pergunta certa não é "existe algum caminho de u até v?" — isso é
trivial, já que a própria aresta é um caminho. A pergunta certa é:
"existe um caminho **alternativo**, diferente da aresta direta?".
Por isso a busca ignora especificamente o primeiro passo u->v.

### Complexidade

Para cada uma das |E| arestas, fazemos uma busca O(V + E). No total:

```
O(E × (V + E))
```

---

## 4. Decisão: Grafos com Ciclos

O algoritmo, como implementado, assume que o grafo de entrada é um
**DAG** (grafo acíclico direcionado) ou então não se preocupa em tratar
cuidadosamente os ciclos. Em um ciclo (ex: 0->1->2->0), toda aresta parece
redundante porque dá pra "dar a volta", o que tecnicamente quebraria a
ideia de redução transitiva única dentro de uma componente fortemente
conexa. 

---

## 5. Decisão: Grafos Não Direcionados

O enunciado pede para discutir se a implementação funcionaria em um grafo
não direcionado. Funcionaria, mas o resultado é pouco
interessante já que em um grafo não direcionado conexo, toda aresta que faz
parte de um ciclo é "redundante" pela mesma lógica, então a redução
transitiva de um grafo não-direcionado conexo é simplesmente uma
**árvore geradora** (spanning tree). Para grafos desconexos, vira uma
**floresta geradora**. Isso já é resolvido de forma mais direta por
algoritmos clássicos como Prim ou Kruskal.

---

## 6. Explicação da Implementação

### Classe `Graph`

```cpp
class Graph {
public:
    int V;
    vector<list<int>> adj;
    Graph(int v) : V(v), adj(v) {}
```

`V` guarda o número de vértices. `adj` é o vetor de listas: `adj[u]`
contém todos os vértices que `u` alcança diretamente (uma aresta de
distância). O construtor recebe o número de vértices e já inicializa o
vetor com `v` listas vazias.

### `addEdge`

```cpp
void addEdge(int u, int v) {
    auto& lst = adj[u];
    if (find(lst.begin(), lst.end(), v) == lst.end())
        lst.push_back(v);
}
```

Adiciona a aresta u->v, mas só se ela ainda não existir (evita arestas
duplicadas, que não fazem sentido em grafo simples e atrapalhariam a
contagem de arestas).

### `removeEdge`

```cpp
void removeEdge(int u, int v) {
    adj[u].remove(v);
}
```

Remove `v` da lista de adjacência de `u`.

### `isReachable` — a busca em profundidade

```cpp
//dfs
bool isReachable(int src, int target) const {
    vector<bool> visited(V, false);
    stack<int> stk;
    stk.push(src);
    visited[src] = true;
    while (!stk.empty()) {
        int u = stk.top(); stk.pop();
        for (int w : adj[u]) {
            //ignora aresta direta
            if (u == src && w == target)
                continue;
            if (w == target)
                return true;
            if (!visited[w]) {
                visited[w] = true;
                stk.push(w);
            }
        }
    }
    return false;
}
```

Essa é a função central do algoritmo. É uma DFS iterativa (usando uma
pilha explícita, ao invés de recursão) que verifica se `target` é
alcançável a partir de `src`.

Ignoramos a aresta direta, verificado pela condição `u == src && w == target`.
Qualquer outro caminho que passe por outros vértices até chegar em
`target` continua sendo considerado normalmente.

### `transitiveReduction` — o algoritmo principal

```cpp
void transitiveReduction() {
    vector<pair<int,int>> toRemove;

    for (int u = 0; u < V; u++) {
        vector<int> neighbors(adj[u].begin(), adj[u].end());
        for (int v : neighbors) {
            if (isReachable(u, v)) {
                toRemove.push_back({u, v});
            }
        }
    }

    for (auto& [u, v] : toRemove) {
        removeEdge(u, v);
    }
}
```

Para cada vértice `u`, percorremos seus vizinhos diretos `v`. Para cada
par (u, v), perguntamos: existe um caminho alternativo de u até v (sem
contar a aresta direta)? Se sim, marcamos a aresta para remoção.

Note que **não removemos a aresta na hora**. Primeiro guardamos todas as
arestas redundantes em `toRemove`, e só depois de testar tudo é que
removemos de fato. Isso evita um problema sutil: se removêssemos durante
o laço, estaríamos alterando o grafo enquanto ainda o estamos analisando,
o que poderia mudar o resultado de buscas seguintes de forma incorreta.

### `print`, `edgeCount`

Funções auxiliares: `print` mostra a lista de adjacência no terminal; `edgeCount` soma o tamanho de
todas as listas de adjacência para saber quantas arestas o grafo tem no
total para comparar "antes" e "depois" da redução.

### `readGraph` — leitura da entrada

```cpp
Graph readGraph(istream& in) {
    int V, E;
    in >> V >> E;
    Graph g(V);
    for (int i = 0; i < E; i++) {
        int u, v;
        in >> u >> v;
        g.addEdge(u, v);
    }
    return g;
}
```

Lê o número de vértices e arestas na primeira linha, depois lê cada
aresta (par u v) e adiciona ao grafo. Repare que a função recebe um
`istream&` genérico, não importa se é do terminal (`cin`) ou um arquivo
(`ifstream`), o código de leitura é o mesmo. Isso é o que permite o
programa funcionar tanto digitando no terminal quanto lendo de um
arquivo.

### `main` — leitura flexível (terminal ou arquivo)

```cpp
istream* input = &cin;
ifstream file;
if (argc > 1) {
    file.open(argv[1]);
    if (!file) {
        cerr << "Erro ao abrir arquivo " << argv[1] << "\n";
        return 1;
    }
    input = &file;
}
```

`istream` é a classe-base tanto de
`cin` quanto de `ifstream` (arquivo), então um ponteiro `istream*` pode
apontar para qualquer um dos dois. Por padrão, `input` aponta para `cin`
(o terminal). Se o usuário passar um nome de arquivo como
argumento na linha de comando, o programa tenta abrir esse arquivo;
se conseguir, troca o ponteiro para apontar para o arquivo ao invés do
terminal.

```cpp
Graph g = readGraph(*input);
```

Lê o grafo da fonte escolhida (arquivo ou terminal).

```cpp
g.transitiveReduction();
```

Executa o algoritmo.

O restante do `main` imprime o grafo antes e depois da redução, conta
arestas para comparação.

---

## 7. Formato de Entrada

```
V E
u1 v1
u2 v2
...
```

- Primeira linha: número de vértices `V` e número de arestas `E`
- As próximas `E` linhas: uma aresta por linha, `u v`
- Vértices numerados de `0` a `V-1`

Exemplo:
```
4 5
0 1
0 2
1 2
2 3
0 3
```

## 8. Compilar e Rodar

```bash
g++ -o transitive_reduction transitive_reduction.cpp
./transitive_reduction input.txt < output.txt
```

---

## 9. Pontos para Discutir no Relatório

- Justificativa da lista de adjacência (esparsidade, custo de
  percorrer vizinhos).
- Explicação do algoritmo de redução via DFS ignorando a aresta direta.
- Complexidade O(E·(V+E)).
- Discussão sobre grafos não direcionados (resultado = árvore/floresta
  geradora).
- Limitação conhecida: ciclos em grafos direcionados não são tratados
  de forma especial (cada aresta de um ciclo pode parecer redundante).
- Sugestão de trabalho futuro: detectar componentes fortemente conexas
  (Tarjan/Kosaraju) antes de aplicar a redução, para tratar ciclos
  corretamente.