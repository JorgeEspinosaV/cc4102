#include "btree.h"

void BTree::insertar(int llave, float valor) {
    // nodo actual es root R
    Node *rootNode = this->get_root();
    // 1. si root no esta lleno, insertar k, v en R
    if (!rootNode->is_full()) {
        insertar_no_lleno(root, {llave, valor});
    } else {
        // 2. si root full
        // 2.1 R.split, recibir R_left R_right y par mediano (k_r, v_r)
        Node left, right;
        KeyValue par_mediano;
        rootNode->split(&left, &right, &par_mediano);
        // 2.2 escribir ambos nuevos nodos al final del arreglo
        int idx_left = this->push_back_node(&left);
        int idx_right = this->push_back_node(&right);
        // 2.3 crear nueva raiz R' vacía
        Node *new_root = new NodeBTree(true);
        // 2.4 agregar (k_r, v_r) a arr de llaves valores de R'
        new_root->kv_push_first(par_mediano);
        // 2.5 insertar indices de nuevos hijos en R'
        new_root->child_push(idx_left);
        new_root->child_push(idx_right);
        // 2.6 actualizar elems en R', R'.k
        // 2.7 escribir R' en la primer pos del arr reemplazando la raíz antigua
        this->set_root(new_root);
        // 2.8 insertar nuevo par en:
        // R_left si llave del par es menor o igual a k_r, o
        if (llave <= par_mediano.first) {
            insertar_no_lleno(idx_left, {llave, valor});
        } else {
            // R_right si la llave es mayor a k_r.
            insertar_no_lleno(idx_right, {llave, valor});
        }
    }
}

void BTree::insertar_no_lleno(int idx, KeyValue par) {
    // Nodo V
    Node *nodo = this->get_node(idx);
    // si nodo actual H es hoja
    if (!nodo->es_interno) {
        // 1. insertar par en H
        // 2. actualizar H.k
        // 3. write hoja actualizada en pos correspondiente.
        nodo->insertar_kv(par);
    } else { // si nodo actual V es interno
        // 1. usando las llaves del nodo, encontrar hijo U en el cual insertar par.
        // nodo hijos[i] guarda unicamente valores cuya key es mayor a la key
        // de llaves_valores[i-1], y menor o igual que la llave de llaves_valores[i]
        int hijo_idx = nodo->find_child(par);
        Node *hijo = this->get_node(hijo_idx);
        if (!hijo->is_full()) {
            // 2. si U no esta lleno, insertar en U
            this->insertar_no_lleno(hijo_idx, par);
        } else {
            // 3. Si U está lleno (tiene b k-v):
            // 3.1 U.split, obteniendo U_left y U_right junto con el par mediano (k_u, v_u)
            Node left, right;
            KeyValue par_hijo;
            hijo->split(&left, &right, &par_hijo);
            // 3.2 insertar (k_u, v_u) en lista de pares valores de V,
            // recordando que estos pares deben quedar ordenados ascendentemente por llave
            nodo->insertar_kv(par_hijo);
            // 3.3 escribir los nodos de los nuevos hijos,
            // U_right al final del arr y
            int idx_right = this->push_back_node(&right);
            // U_left en el indice antiguo de U (como este se dividió, se puede reemplazar)
            int idx_left = hijo_idx;
            this->replace_node(hijo_idx, &left);
            // 3.4 agregar el indice U_right a la lista de hijos del nodo
            // (inmediatamente después del índice antiguo de U, ahora ocupado por U_left)
            // 3.5 actualizar V.k
            // 3.6 escribir V actualizado
            nodo->child_push(idx_right);
            // 3.7 insertar nuevo par en:
            // U_left si la llave del par es menor o igual a k_u, o
            // U_right si la llave es mayor a k_u
            if (par.first <= par_hijo.first) {
                insertar_no_lleno(idx_left, par);
            } else {
                insertar_no_lleno(idx_right, par);
            }

        }
    }
}