#include <bits/stdc++.h>
#include "btree.h"
using namespace std;

using KeyValue = pair<int32_t, float>;

static constexpr int32_t MIN_KEY = 1546300800;         // 2019-01-01 00:00:00
static constexpr int32_t MAX_KEY = 1754006400;         // 2025-08-31 00:00:00 (según enunciado)
static constexpr int32_t WEEK_SECS = 604800;           // tamaño del rango
static constexpr int32_t L_MAX = MAX_KEY - WEEK_SECS;  // para que u = l + WEEK no exceda

// N = {2^15, ..., 2^26}
vector<int> set_test() {
    vector<int> r;
    r.reserve(26 - 15 + 1);
    for (int i = 15; i <= 26; ++i) r.push_back(1 << i); // int es suficiente hasta 2^26
    return r;
}

// Carga los primeros N pares (int32_t key, float value) desde datos.bin
vector<KeyValue> load_first_N_pairs(const string& binpath, size_t N) {
    ifstream in(binpath, ios::binary);
    if (!in) throw runtime_error("No pude abrir " + binpath);
    vector<KeyValue> v;
    v.reserve(N);
    for (size_t i = 0; i < N && in; ++i) {
        int32_t k; float val;
        in.read(reinterpret_cast<char*>(&k), sizeof(k));
        in.read(reinterpret_cast<char*>(&val), sizeof(val));
        if (!in) break;
        v.emplace_back(k, val);
    }
    return v;
}

struct ExperimentRow {
    string tree_kind;     // "BTree" o "B+Tree"
    int N;                // cantidad insertada
    double build_ms;      // tiempo construir (ms)
    uint64_t build_reads;
    uint64_t build_writes;
    size_t size_nodes;    // nodos usados (o bytes si prefieres otra métrica)
    double avg_query_ms;  // tiempo promedio por búsqueda (ms)
    double avg_query_reads;
    double avg_query_writes;
};

ExperimentRow run_for_N(BTree& tree,
                        const vector<KeyValue>& data,
                        const string& serialize_path,
                        int N,
                        int n_queries = 50,
                        uint64_t seed = 12345)
{
    ExperimentRow row;
    row.N = N;

    // 1) Build
    tree.reset_io();
    auto t0 = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        tree.insertar(data[i].first, data[i].second);
    }
    auto t1 = chrono::high_resolution_clock::now();
    row.build_ms = chrono::duration<double, milli>(t1 - t0).count();
    row.build_reads  = tree.get_io().reads;
    row.build_writes = tree.get_io().writes;

    // 2) Serializar a disco (simplemente invoca tu serialize)
    //    IMPORTANTE: serialize también debe incrementar io.writes por nodo.
    tree.serialize(serialize_path);

    // 3) 50 búsquedas de rango (aleatorias uniformes)
    mt19937_64 rng(seed);
    uniform_int_distribution<int32_t> distL(MIN_KEY, L_MAX);

    double sum_ms = 0.0;
    uint64_t sum_reads = 0, sum_writes = 0;

    vector<KeyValue> sink; sink.reserve(1024);
    for (int q = 0; q < n_queries; ++q) {
        int32_t l = distL(rng);
        int32_t u = l + WEEK_SECS;

        tree.reset_io();
        sink.clear();
        auto q0 = chrono::high_resolution_clock::now();
        tree.range(l, u, sink);
        auto q1 = chrono::high_resolution_clock::now();

        sum_ms     += chrono::duration<double, milli>(q1 - q0).count();
        sum_reads  += tree.get_io().reads;
        sum_writes += tree.get_io().writes;  // en B+ es normal que sea 0 en queries
    }

    row.avg_query_ms     = sum_ms / n_queries;
    row.avg_query_reads  = static_cast<double>(sum_reads)  / n_queries;
    row.avg_query_writes = static_cast<double>(sum_writes) / n_queries;
    row.size_nodes       = tree.n_nodes;

    return row;
}
void write_csv(const string& path, const vector<ExperimentRow>& rows) {
    ofstream out(path);
    if (!out) throw runtime_error("No pude abrir " + path + " para escribir");
    out << "tree,N,build_ms,build_reads,build_writes,size_nodes,avg_query_ms,avg_query_reads,avg_query_writes\n";
    for (const auto& r : rows) {
        out << r.tree_kind << ","
            << r.N << ","
            << r.build_ms << ","
            << r.build_reads << ","
            << r.build_writes << ","
            << r.size_nodes << ","
            << r.avg_query_ms << ","
            << r.avg_query_reads << ","
            << r.avg_query_writes << "\n";
    }
}

struct SingleQueryRow {
    string tree_kind;
    int N;
    int32_t l, u;
    double query_ms;
    uint64_t reads, writes;
    size_t results;  // cantidad de pares devueltos (para graficar “temperaturas” si corresponde)
};

SingleQueryRow run_single_query(BTree& tree, int N, int32_t l, int32_t u) {
    SingleQueryRow r;
    r.N = N; r.l = l; r.u = u;

    vector<KeyValue> out;
    tree.reset_io();
    auto t0 = chrono::high_resolution_clock::now();
    tree.range(l, u, out);
    auto t1 = chrono::high_resolution_clock::now();

    r.query_ms = chrono::duration<double, milli>(t1 - t0).count();
    r.reads = tree.get_io().reads;
    r.writes = tree.get_io().writes;
    r.results = out.size();
    return r;
}

void write_csv_single(const string& path, const vector<SingleQueryRow>& rows) {
    ofstream out(path);
    out << "tree,N,l,u,query_ms,reads,writes,results\n";
    for (auto& r : rows) {
        out << r.tree_kind << ","
            << r.N << ","
            << r.l << ","
            << r.u << ","
            << r.query_ms << ","
            << r.reads << ","
            << r.writes << ","
            << r.results << "\n";
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const string binpath = "datos.bin";     // Ajusta ruta si corresponde
    const string out_csv = "metricas_bt_vs_bpt.csv";
    const string out_single_csv = "single_query.csv";

    // N set
    vector<int> Ns = set_test();

    // Cargamos TODOS los pares una vez (máximo N = 2^26)
    vector<KeyValue> all = load_first_N_pairs(binpath, static_cast<size_t>(1u << 26));
    if (all.size() < (1u << 26)) {
        cerr << "Advertencia: datos.bin tiene solo " << all.size() << " pares\n";
    }

    vector<ExperimentRow> rows;

    // Fijamos una búsqueda única (para comparar entre dos Ns)
    mt19937_64 rng(987654321);
    uniform_int_distribution<int32_t> distL(MIN_KEY, L_MAX);
    const int32_t L_fixed = distL(rng);
    const int32_t U_fixed = L_fixed + WEEK_SECS;

    vector<SingleQueryRow> single_rows;

    // Elige dos Ns para la comparativa “misma búsqueda”
    int N_a = Ns.front();       // 2^15
    int N_b = Ns.back();        // 2^26

    for (int N : Ns) {
        vector<KeyValue> data(all.begin(), all.begin() + min<size_t>(N, all.size()));

        // --- BTree normal ---
        // TODO: reemplaza por tu clase real
        unique_ptr<BTree> btree = /* new BTreeNormal(...) */ nullptr;
        // EJEMPLO: btree.reset(new BTreeNormal(/*config*/));

        // TODO: quita este guard si ya puedes construir
        if (!btree) {
            // Evitar que el ejemplo no compile si todavía no conectas tus clases:
            struct Dummy : BTree {
                void insertar(int32_t, float) override {}
                void serialize(const string&) override {}
                void range(int32_t, int32_t, vector<KeyValue>&) override {}
            };
            btree.reset(new Dummy());
        }

        auto r1 = run_for_N(*btree, data, "btree_" + to_string(N) + ".bin", N);
        r1.tree_kind = "BTree";
        rows.push_back(r1);

        if (N == N_a || N == N_b) {
            auto s = run_single_query(*btree, N, L_fixed, U_fixed);
            s.tree_kind = "BTree";
            single_rows.push_back(s);
        }

        // --- B+Tree ---
        unique_ptr<BTree> bptree = /* new BTreePlus(...) */ nullptr;
        if (!bptree) {
            struct Dummy : BTree {
                void insertar(int32_t, float) override {}
                void serialize(const string&) override {}
                void range(int32_t, int32_t, vector<KeyValue>&) override {}
            };
            bptree.reset(new Dummy());
        }

        auto r2 = run_for_N(*bptree, data, "btreeplus_" + to_string(N) + ".bin", N);
        r2.tree_kind = "B+Tree";
        rows.push_back(r2);

        if (N == N_a || N == N_b) {
            auto s = run_single_query(*bptree, N, L_fixed, U_fixed);
            s.tree_kind = "B+Tree";
            single_rows.push_back(s);
        }
    }

    write_csv(out_csv, rows);
    write_csv_single(out_single_csv, single_rows);

    cerr << "Listo. Escribí:\n - " << out_csv << "\n - " << out_single_csv << "\n";
    return 0;
}
