#include "btree.h"

void BTree::range(int l, int u, std::vector<KeyValue> &result) {
    // comenzamos desde la raiz (idx 0)
    constexpr int idx_root = 0;
    // declaramos e inicializamos el vector de resultado
    result.clear();
    this->range_internal(idx_root, l, u, result);
}

void BTreeNormal::range_internal(
    int idx,
    int l,
    int u,
    std::vector<KeyValue> &result
) {
    Node *nodo = this->get_node(idx);
    if (!nodo) return; // seguridad
    // es hoja
    if (!nodo->es_interno) {
        std::vector<KeyValue> r = nodo->range(l, u);
        result.insert(result.end(), r.begin(), r.end());
    } else {
        for (int i = 0; i < nodo->k; i++) {
            int key = nodo->llaves_valores[i].first;

            if (nodo->hijos[i] != -1 && l <= key) {
                this->range_internal(nodo->hijos[i], l, u, result);
            }

            if (key >= l && key <= u) {
                result.push_back(nodo->llaves_valores[i]);
            }
        }
        if (nodo->hijos[nodo->k] != -1) {
            this->range_internal(nodo->hijos[nodo->k], l, u, result);
        }
    }
}

void BTreePlus::range_internal(
    int idx,
    int l,
    int u,
    std::vector<KeyValue> &result
) {
    Node *nodo = this->get_node(idx);
    if (!nodo) return;

    while (nodo->es_interno) {
        bool found = false;
        for (int i = 0; i < nodo->k; i++) {
            if (l < nodo->llaves_valores[i].first) {
                nodo = this->get_node(nodo->hijos[i]);
                found = true;
                break;
            }
        }
        if (!found) {
            nodo = this->get_node(nodo->hijos[nodo->k]);
        }
    }

    while (nodo != nullptr) {
        for (int i = 0; i < nodo->k; i++) {
            int key = nodo->llaves_valores[i].first;
            float value = nodo->llaves_valores[i].second;
            if (key >= l && key <= u) {
                result.emplace_back(key, value);
            } else if (key > u) {
                return;
            }
        }
        if (nodo->siguiente == -1)
            break;

        nodo = this->get_node(nodo->siguiente);
    }
}

