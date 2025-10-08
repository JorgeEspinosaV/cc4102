#pragma once
#include <array>
#include <vector>
#include <utility>
#include <stdexcept>

// =======================
// Constantes de configuración
// =======================
constexpr int N_ELEMS = 340; // capacidad máxima (ejemplo)

typedef std::pair<int, float> KeyValue;

// =======================
// Definición de Node
// =======================
struct Node {
    bool es_interno;   // true = interno, false = hoja
    int k;             // número de llaves
    std::array<KeyValue, N_ELEMS> llaves_valores;
    std::array<int, N_ELEMS+1> hijos{};  // índices de hijos
    int n_hijos;
    int siguiente;     // para B+Tree (enlace a siguiente hoja)

    explicit Node(bool interno=false)
        : es_interno(interno), k(0), siguiente(-1) {
        hijos.fill(-1), n_hijos = 0;
    }

    Node(bool interno, int siguiente) : Node(interno) {
        this->siguiente = siguiente;
    }

    virtual ~Node() = default;

    virtual int kv_push_back(KeyValue kv);

    virtual void kv_push_first(KeyValue kv);

    virtual int child_push(int idxNodo);

    virtual int find_child(KeyValue kv);

    virtual int insertar_kv(KeyValue kv);
    virtual std::vector<KeyValue> range(int l, int u);
    virtual void split(Node *left, Node *right, KeyValue *par_mediano);

    virtual void split_interno(Node *left, Node *right, KeyValue *par_mediano);

    [[nodiscard]] bool is_full() const { return k == N_ELEMS; }
};

struct NodeBTree : Node {
    explicit NodeBTree(bool interno=false) : Node(interno) {}

    void split(Node *left, Node *right, KeyValue *par_mediano) override;

};

struct NodeBTreePlus : Node {
    explicit NodeBTreePlus(bool interno) : Node(interno) {}

    NodeBTreePlus(bool interno, int siguiente) : Node(interno, siguiente) {}

    void split(Node *left, Node *right, KeyValue *par_mediano) override;
};

struct IOStats {
    unsigned long reads = 0;
    unsigned long writes = 0;
    void reset() { reads = writes = 0; }
};

// =======================
// Interfaz base de BTree
// =======================
struct BTree {
    std::vector<Node> nodes;  // almacenamiento global
    int root;
    int n_nodes;
    IOStats io;

    BTree() : root(0), n_nodes(0) {}
    virtual ~BTree() = default;
    virtual Node *get_root();
    virtual void set_root(Node *new_root);
    virtual Node *get_node(int idxNodo);
    virtual int push_back_node(Node *n);
    virtual void replace_node(int idxNodo, Node *n);
    virtual void serialize(const std::string& filepath) = 0;

    virtual void insertar(int llave, float valor);
    virtual void range(int l, int u, std::vector<KeyValue>& result);
    virtual void range_internal(int idx, int l, int u, std::vector<KeyValue>& result);
    void insertar_no_lleno(int idxNodo, KeyValue par);
    void reset_io() { io.reset(); }
    [[nodiscard]] IOStats get_io() const { return io; }
};

// Dentro de tus clases reales, llama a io.reads++
// cada vez que “accedes” un nodo (p. ej., en get_node)
// y io.writes++ cuando “escribes” un nodo
// (crear/partir/reemplazar/serializar).
// Eso hace trazable la métrica sin cambiar tu lógica.

// =======================
// BTree normal
// =======================
struct BTreeNormal : BTree {
    BTreeNormal() = default;

    void insertar(int llave, float valor) override;
    void range_internal(int idx, int l, int u, std::vector<KeyValue>& result) override;
};

// =======================
// B+Tree
// =======================
struct BTreePlus : BTree {
    BTreePlus() = default;

    void insertar(int llave, float valor) override;
    void range_internal(int idx, int l, int u, std::vector<KeyValue>& result) override;
};