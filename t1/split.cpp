#include "split.hpp"

#include "btree.h"

void Node::split(Node *left, Node *right, std::pair<int, float> *par_mediano) {
    // do nothing
}


void Node::split_interno(
    Node *left,
    Node *right,
    std::pair<int, float> *par_mediano
) {
    constexpr int m = N_ELEMS / 2;

    std::pair<int, float> r_par = llaves_valores[m];

    Node *r_left = new NodeBTree();
    Node *r_right = new NodeBTree();

    r_left->k  = m - 1;
    r_right->k = m;

    // Copiar pares a left
    for (int i = 0; i < r_left->k; ++i) {
        r_left->llaves_valores[i] = this->llaves_valores[i];
    }
    // Copiar pares a right
    for (int i = 0; i < r_right->k; ++i) {
        r_right->llaves_valores[i] = this->llaves_valores[m + i];
    }

    // Si era interno, repartir hijos
    if (!this->es_interno) {
        // left: primeros b/2 hijos
        for (int i = 0; i < m + 1; ++i) {
            r_left->hijos[i] = this->hijos[i];
        }
        // right: siguientes b/2 + 1 hijos
        for (int i = 0; i < m + 2; ++i) {
            r_right->hijos[i] = this->hijos[m + i];
        }
    }

    *left = *r_left;
    *right = *r_right;
    *par_mediano = r_par;
}


void NodeBTree::split(
    Node *left,
    Node *right,
    std::pair<int,float> *par_mediano
) {
    this->split_interno(left, right, par_mediano);
}

void NodeBTreePlus::split(Node *left, Node *right, std::pair<int,float> *par_mediano) {
    if (!this->es_interno) {
        // En internos, B+ se comporta igual que B
        this->split_interno(left, right, par_mediano);
    }
    constexpr int m = N_ELEMS / 2;

    std::pair<int, float> r_par = this->llaves_valores[m];
    Node *r_left = new NodeBTreePlus(true);
    Node *r_right = new NodeBTreePlus(true);

    r_left->k = m;
    r_right->k = m;

    // Copiar a left
    for (int i = 0; i < r_left->k; ++i) {
        r_left->llaves_valores[i] = this->llaves_valores[i];
    }
    // Copiar a right
    for (int i = 0; i < r_right->k; ++i) {
        r_right->llaves_valores[i] = this->llaves_valores[m + 1 + i];
    }

    *left = *r_left;
    *right = *r_right;
    *par_mediano = r_par;
}