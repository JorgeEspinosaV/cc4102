#include "node.h"
#include <array>

Node::Node(bool es_interno, bool is_btree_plus) {
    this->es_interno = es_interno;
    // valor por defecto b/2 - 1
    this->k = 169;
    if (is_btree_plus)
        this->siguiente = 0;
    else
        this->siguiente = -1;
    // creamos el array de data
    this->llaves_valores = std::array<std::pair<int, float>, 340>();
    // creamos el array de hijos
    this->hijos = std::array<int, 340>();
}
