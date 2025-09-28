#include <array>

class Node {
    public:
        Node(bool es_interno, bool is_btree_plus);
        bool es_interno;
        int k;
        std::array<std::pair<int, float>, 340> llaves_valores;
        std::array<int, 340> hijos;
        int siguiente;
};
