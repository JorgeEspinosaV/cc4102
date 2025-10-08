//
// Created by jorge on 01/10/2025.
//
#include "btree.h"
#include <stdexcept>

int Node::kv_push_back(const KeyValue kv) {
    if (this->is_full()) {
        throw std::runtime_error("Node is full");
    }
    this->k++;
    this->llaves_valores[this->k] = kv;
    return this->k;
}

void Node::kv_push_first(const KeyValue kv) {
    this->k++;
    this->llaves_valores[0] = kv;
}

int Node::insertar_kv(const KeyValue kv) {
    if (this->is_full()) {
        throw std::runtime_error("Node is full");
    }
    int i = this->k - 1;
    while (i >= 0 && this->llaves_valores[i].first > kv.first) {
        this->llaves_valores[i + 1] = this->llaves_valores[i];
        i--;
    }
    this->llaves_valores[i + 1] = kv;
    this->k++;
    return i + 1;
}

int Node::child_push(const int idxNodo) {
    if (this->n_hijos >= N_ELEMS + 1) {
        throw std::runtime_error("Node is full of children");
    }
    this->n_hijos++;
    this->hijos[this->n_hijos] = idxNodo;
    return this->n_hijos;
}

int Node::find_child(const KeyValue kv) {
    // el nodo hijo[i] guarda valores cuya llave es:
    // mayor a llave_valores[i-1] y
    // menor o igual que llave_valores[i]
    int i = this->k - 1;
    while (i >= 0 && this->llaves_valores[i].first > kv.first) {
        i--;
    }
    return this->hijos[i + 1];
}

std::vector<KeyValue> Node::range(const int l, const int u) {
    std::vector<KeyValue> result;
    for (int i=0; i<this->k; i++) {
        if (this->llaves_valores[i].first >= l && this->llaves_valores[i].first <= u) {
            result.push_back(this->llaves_valores[i]);
        }
    }
    return result;
}


void BTree::replace_node(int idxNodo, Node *n) {
    this->nodes[idxNodo] = *n;
}


Node *BTree::get_root() {
    if (this->n_nodes == 0) {
        throw std::runtime_error("Tree is empty");
    }
    return &this->nodes[0];
}

Node *BTree::get_node(const int idxNodo) {
    if (idxNodo >= this->n_nodes) {
        throw std::runtime_error("Node does not exist");
    }
    return &this->nodes[idxNodo];
}

void BTree::set_root(Node *new_root) {
    this->nodes[0] = *new_root;
}


int BTree::push_back_node(Node *n) {
    this->nodes.push_back(*n);
    this->n_nodes++;
    return this->n_nodes - 1;
}

