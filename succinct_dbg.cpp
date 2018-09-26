#include "succinct_dbg.h"


// void SuccinctDeBruijnGraph::construct_edges_static() {
//     cerr << "Contruct Static Edges vector..." << endl;
//     size_t index = 0;
//     edges_static.resize(edges.size());
//     for (auto it = edges.begin(); it != edges.end(); ++it, ++index) {
//         edges_static[index] = *it;
//     }
//     cerr << "End of Static Edges vector..." << endl;
// }


bit_vector SuccinctDeBruijnGraph::calculate_SBV(const bit_vector& pBL, uint8_t ptype) {
    cerr << "Generating Storage Bit Vector (SBV)..." << endl;
    bit_vector rv(pBL.size());
    bool prev_zero = false;
    for (size_t i = 0; i < pBL.size(); ++i) {
        if (prev_zero) {
            rv[i] = true;
            prev_zero = false;
        }
        else if (pBL[i] == 0) {
            rv[i] = true;
            prev_zero = true;
        }
        else if (ptype == 1 && edges[i] != 0 && indegree(forward(i, edges[i])) > 0) {

        }
        else if (ptype == 2) {

        }
        else {
            rv[i] = false;
        }
    }
    return rv;
}


bit_vector SuccinctDeBruijnGraph::calculate_SBV(uint8_t ptype) {
    cerr << "Generating Storage Bit Vector (SBV)..." << endl;
    bit_vector rv(BL.size());
    bool prev_zero = false;
    for (size_t i = 0; i < BL.size(); ++i) {
        if (prev_zero) {
            rv[i] = true;
            prev_zero = false;
        }
        else if (BL[i] == 0) {
            rv[i] = true;
            prev_zero = true;
        }
        else if (ptype == 1 && edges[i] != 0 && indegree(forward(i, edges[i])) > 0) {

        }
        else if (ptype == 2) {

        }
        else {
            rv[i] = false;
        }
    }
    return rv;
}


uint8_t SuccinctDeBruijnGraph::indegree(size_t index) {
    return 0;
}


// returns the node index (first outgoing edge index) when we read symbol c on the given position
size_t SuccinctDeBruijnGraph::forward(size_t index, uint8_t c) const {
    assert(c != 0);

    // uint8_t ac = edges[index];
    auto cid = (uint8_t) (bits_to_id(c) - 1);
    size_t j = edges.rank(index, c);
    size_t l = T_F[cid] + j;
    size_t node_cnt = BF_rank.rank(l) - F_node_cnt[cid];
    return BL_select.select(L_node_cnt[cid] + node_cnt) + 1;
}


size_t SuccinctDeBruijnGraph::get_next_symbol_index(size_t index, uint8_t c) const {
    // static wt_t::const_iterator start_it = edges.begin();
    static uint8_t first_char = symbol_to_bits('A');
    assert(c != 0);

    if (c == first_char) {
        return index;
    }
    return edges.select(edges.rank(index, c) + 1, c);
    // for (size_t i = index; i < edges.size(); ++i) {
    //     if (edges[i] == c) {
    //         return i;
    //     }
    // }
    //
    // // in normal circumstances this could never happen...
    // return edges.size();

}


size_t SuccinctDeBruijnGraph::get_label_index(size_t index) const {
    if (SBV[index] == 0) {
        return label_vect_size;
    }

    return SBV_rank.rank(index);
}


// getter for label_vect_size
size_t SuccinctDeBruijnGraph::get_label_vect_size() const {
    return label_vect_size;
}


size_t SuccinctDeBruijnGraph::save_dbg(ostream& out, structure_tree_node *v, string name) const {
    structure_tree_node *child = structure_tree::add_child(v, name, util::class_name(*this));
    size_t written_bytes = 0;
    written_bytes += write_member(n, out, child, "n");
    written_bytes += write_member(this->v, out, child, "v");
    written_bytes += write_member(k, out, child, "k");
    for (size_t i = 0; i < SIGMA; ++i) {
        written_bytes += write_member(T[i], out, child, "T[" + to_string(i) + "]");
    }
    written_bytes += BL.serialize(out, child, "BL");
    written_bytes += BL_rank.serialize(out, child, "BL_rank");
    written_bytes += BL_select.serialize(out, child, "BL_select");
    written_bytes += BF.serialize(out, child, "BF");
    written_bytes += BF_rank.serialize(out, child, "BF_rank");
    written_bytes += BF_select.serialize(out, child, "BF_select");
    written_bytes += edges.serialize(out, child, "edges");
    structure_tree::add_size(child, written_bytes);
    return written_bytes;
}


void SuccinctDeBruijnGraph::load_dbg(istream& in) {
    read_member(n, in);
    read_member(v, in);
    read_member(k, in);
    for (size_t i = 0; i < SIGMA; ++i) {
        read_member(T[i], in);
    }
    BL.load(in);
    BL_rank.load(in);
    BL_select.load(in);
    BF.load(in);
    BF_rank.load(in);
    BF_select.load(in);
    edges.load(in);
}


size_t SuccinctDeBruijnGraph::save_label_vect(ostream& out, structure_tree_node *v, string name) const {
    structure_tree_node *child = structure_tree::add_child(v, name, util::class_name(*this));
    size_t written_bytes = 0;
    written_bytes += X.serialize(out, child, "X");
    structure_tree::add_size(child, written_bytes);
    return written_bytes;
}


void SuccinctDeBruijnGraph::load_label_vect(istream& in) {
    X.load(in);
}


size_t SuccinctDeBruijnGraph::save_color_table(ostream& out, structure_tree_node *v, string name) const {
    structure_tree_node *child = structure_tree::add_child(v, name, util::class_name(*this));
    size_t written_bytes = 0;
    written_bytes += write_member(C, out, child, "C");
    written_bytes += CT.serialize(out, child, "CT");
    written_bytes += CT_rank.serialize(out, child, "CT_rank");
    written_bytes += CT_select.serialize(out, child, "CT_select");
    structure_tree::add_size(child, written_bytes);
    return written_bytes;
}


void SuccinctDeBruijnGraph::load_color_table(istream& in) {
    read_member(C, in);
    CT.load(in);
    CT_rank.load(in);
    CT_select.load(in);
}


size_t SuccinctDeBruijnGraph::save_storage_vect(ostream& out, structure_tree_node *v, string name) const {
    structure_tree_node *child = structure_tree::add_child(v, name, util::class_name(*this));
    size_t written_bytes = 0;
    written_bytes += SBV.serialize(out, child, "SBV");
    written_bytes += SBV_rank.serialize(out, child, "SBV_rank");
    written_bytes += SBV_select.serialize(out, child, "SBV_select");
    structure_tree::add_size(child, written_bytes);
    return written_bytes;
}


void SuccinctDeBruijnGraph::load_storage_vect(istream& in) {
    SBV.load(in);
    SBV_rank.load(in);
    SBV_select.load(in);
}


bool SuccinctDeBruijnGraph::save(const string& output_fname) const {
    auto save_to_bin_file = [](string fname, auto save_fn) {
        sdsl::osfstream out(fname, std::ios::binary | std::ios::trunc | std::ios::out);
        if (!out) {
            if (sdsl::util::verbose) {
                std::cerr << "ERROR: store_to_file not successful for: '" << fname << "'" << std::endl;
            }
            return false;
        }
        save_fn(out);
        out.close();
        if (util::verbose) {
            std::cerr << "INFO: store_to_file: `" << fname << "`" << std::endl;
        }

        return true;
    };

    cerr << "Saving DBG..." << endl;
    if (!save_to_bin_file(output_fname + ".dbg", [this](ostream& out) { save_dbg(out); }))
        return false;

    cerr << "Saving Label Vector..." << endl;
    if (!save_to_bin_file(output_fname + ".x", [this](ostream& out) { save_label_vect(out); }))
        return false;

    cerr << "Saving Color Table..." << endl;
    if (!save_to_bin_file(output_fname + ".ct", [this](ostream& out) { save_color_table(out); }))
        return false;

    cerr << "Saving Storage Vector..." << endl;
    if (!save_to_bin_file(output_fname + ".sbv", [this](ostream& out) { save_storage_vect(out); }))
        return false;

    return true;
}


bool SuccinctDeBruijnGraph::load(const string& input_fname) {
    auto load_from_bin_file = [](string fname, auto load_fn) {
        isfstream in(fname, std::ios::binary | std::ios::in);
        if (!in) {
            if (util::verbose) {
                std::cerr << "Could not load file `" << fname << "`" << std::endl;
            }
            return false;
        }
        load_fn(in);
        in.close();
        if (util::verbose) {
            std::cerr << "Load file `" << fname << "`" << std::endl;
        }
        return true;
    };

    cerr << "Loading DBG..." << endl;
    if (!load_from_bin_file(input_fname + ".dbg", [this](istream& in) { load_dbg(in); }))
        return false;

    cerr << "Loading Label Vector..." << endl;
    if (!load_from_bin_file(input_fname + ".x", [this](istream& in) { load_label_vect(in); }))
        return false;

    cerr << "Loading Color Table..." << endl;
    if (!load_from_bin_file(input_fname + ".ct", [this](istream& in) { load_color_table(in); }))
        return false;

    cerr << "Loading Storage Vector..." << endl;
    if (!load_from_bin_file(input_fname + ".sbv", [this](istream& in) { load_storage_vect(in); }))
        return false;

    return true;
}


void SuccinctDeBruijnGraph::print_stats(ostream& out) {
    out << "Number of edges:               " << n << endl;
    out << "Number of nodes:               " << v << endl;
    out << "k-mer size:                    " << k << endl;
    out << "Number of colors:              " << C << endl;
    out << "#Rows of Color Table (CT):     " << CT.size() / C << endl;
    out << "Size of original CT in bits:   " << CT.size() << endl;
    out << "Size of CT in bytes:           " << size_in_bytes(CT) << endl;
    out << "Length of Label Vector:        " << label_vect_size << endl;
    out << "Size of Label Vector in bytes: " << size_in_bytes(X) << endl;
    out << endl;

    for (size_t i = 0, prev = 0; i < label_vect_size; ++i) {
        size_t ai = X.select(i + 1) + 1;
        cout << (ai - prev - 1) << endl;
        prev = ai;
    }
}
