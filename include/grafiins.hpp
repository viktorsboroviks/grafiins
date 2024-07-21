#include <cassert>
#include <fstream>
#include <queue>
#include <set>
#include <string>
#include <vector>

namespace grafiins {

class Vertex {
private:
    std::set<size_t> _in_edges_i;
    std::set<size_t> _out_edges_i;

public:
    bool allocated = false;
    std::string label;
    std::string graphviz_shape = "circle";
    std::string graphviz_cluster = "";
    double graphviz_width = 0;
    double graphviz_height = 0;

    Vertex(std::string in_label = "") :
        label(in_label)
    {
    }

    void connect_in_edge(size_t i)
    {
        assert(!_in_edges_i.contains(i));
        _in_edges_i.insert(i);
    }

    void connect_out_edge(size_t i)
    {
        assert(!_out_edges_i.contains(i));
        _out_edges_i.insert(i);
    }

    void disconnect_in_edge(size_t i)
    {
        assert(_in_edges_i.contains(i));
        _in_edges_i.erase(i);
    }

    void disconnect_out_edge(size_t i)
    {
        assert(_out_edges_i.contains(i));
        _out_edges_i.erase(i);
    }

    const std::set<size_t> get_in_edges_i() const
    {
        return _in_edges_i;
    }

    const std::set<size_t> get_out_edges_i() const
    {
        return _out_edges_i;
    }
};

class Edge {
public:
    size_t src_vertex_i;
    size_t dst_vertex_i;
    bool allocated = false;
    std::string label;

    Edge(size_t in_src_i, size_t in_dst_i, std::string in_label = "") :
        src_vertex_i(in_src_i),
        dst_vertex_i(in_dst_i),
        label(in_label)
    {
    }
};

template <typename TVertex, typename TEdge>
class Graph {
private:
    std::vector<TVertex> _vertices;
    std::vector<TEdge> _edges;
    std::queue<size_t> _unallocated_vertices_i;
    std::queue<size_t> _unallocated_edges_i;

public:
    bool allow_parallel_edges = false;

    Graph(bool allow_parallel_edges = false) :
        allow_parallel_edges(allow_parallel_edges)
    {
    }

    size_t get_n_vertices()
    {
        assert(_unallocated_vertices_i.size() <= _vertices.size());
        return _vertices.size() - _unallocated_vertices_i.size();
    }

    size_t get_n_edges()
    {
        assert(_unallocated_edges_i.size() <= _edges.size());
        return _edges.size() - _unallocated_edges_i.size();
    }

    const std::vector<size_t> get_vertices_i()
    {
        std::vector<size_t> idx;
        for (size_t i = 0; i < _vertices.size(); i++) {
            if (_vertices[i].allocated) {
                idx.push_back(i);
            }
        }
        return idx;
    }

    TVertex* get_vertex(size_t i)
    {
        assert(i < _vertices.size());
        if (_vertices[i].allocated)
            return &_vertices[i];

        return nullptr;
    }

    const std::vector<size_t> get_edges_i()
    {
        std::vector<size_t> idx;
        for (size_t i = 0; i < _edges.size(); i++) {
            if (_edges[i].allocated) {
                idx.push_back(i);
            }
        }
        return idx;
    }

    TEdge* get_edge(size_t i)
    {
        assert(i < _edges.size());
        if (_edges[i].allocated)
            return &_edges[i];

        return nullptr;
    }

    const std::vector<size_t> get_out_vertices_i(size_t vertex_i)
    {
        assert(vertex_i < _vertices.size());
        const TVertex* v = get_vertex(vertex_i);

        const std::set<size_t> out_edges_i = v->get_out_edges_i();
        std::vector<TEdge*> out_edges;
        for (auto& oe_i : out_edges_i) {
            out_edges.push_back(get_edge(oe_i));
        }

        std::vector<size_t> out_vertices_i;
        for (auto p_oe : out_edges) {
            out_vertices_i.push_back(p_oe->dst_vertex_i);
        }

        return out_vertices_i;
    }

    const std::vector<size_t> get_in_vertices_i(size_t vertex_i)
    {
        // TODO: add
        (void)vertex_i;
        std::vector<size_t> in_vertices_i;
        return in_vertices_i;
    }

    // int is used instead of size_t to be able to return -1
    int add_vertex(TVertex v)
    {
        // update vertex
        assert(_unallocated_vertices_i.size() <= _vertices.size());
        v.allocated = true;

        // first try to occupy unallocated indeces
        if (!_unallocated_vertices_i.empty()) {
            size_t i = _unallocated_vertices_i.front();
            _vertices[i] = v;
            _unallocated_vertices_i.pop();
            return i;
        }

        // if no unallocated indeces available, extend storage
        _vertices.push_back(v);
        return _vertices.size() - 1;
    }

    void remove_vertex(size_t i)
    {
        assert(i < _vertices.size());
        assert(_vertices[i].allocated);
        // remove connected edges
        for (size_t i : _vertices[i].get_in_edges_i()) {
            remove_edge(i);
        }
        for (size_t i : _vertices[i].get_out_edges_i()) {
            remove_edge(i);
        }
        // remove vertex
        _vertices[i].allocated = false;
        _unallocated_vertices_i.push(i);
        assert(_unallocated_vertices_i.size() <= _vertices.size());
    }

    // int is used instead of size_t to be able to return -1
    int add_edge(TEdge e)
    {
        // check input
        assert(e.src_vertex_i < _vertices.size());
        assert(e.dst_vertex_i < _vertices.size());
        assert(_vertices[e.src_vertex_i].allocated);
        assert(_vertices[e.dst_vertex_i].allocated);

        if (!allow_parallel_edges) {
            auto& out_vertices_i = get_out_vertices_i(e.src_vertex_i);
            for (size_t vi : out_vertices_i) {
                if (vi == e.dst_vertex_i) {
                    return -1;
                }
            }
        }

        // update edge
        assert(_unallocated_edges_i.size() <= _edges.size());
        e.allocated = true;
        size_t i;

        // first try to occupy unallocated indeces
        if (!_unallocated_edges_i.empty()) {
            i = _unallocated_edges_i.front();
            _edges[i] = e;
            _unallocated_edges_i.pop();
        }
        // if no unallocated indeces available, extend storage
        else {
            _edges.push_back(e);
            i = _edges.size() - 1;
        }

        // update connected vertices
        _vertices[e.src_vertex_i].connect_out_edge(i);
        _vertices[e.dst_vertex_i].connect_in_edge(i);

        return i;
    }

    void remove_edge(size_t i)
    {
        assert(i < _edges.size());
        assert(_edges[i].allocated);
        // disconnect vertices
        const size_t src_i = _edges[i].src_vertex_i;
        const size_t dst_i = _edges[i].dst_vertex_i;
        assert(src_i < _vertices.size());
        assert(_vertices[i].allocated);
        assert(dst_i < _vertices.size());
        assert(_vertices[i].allocated);
        _vertices[src_i].disconnect_out_edge(i);
        _vertices[dst_i].disconnect_in_edge(i);
        // remove edge
        _edges[i].allocated = false;
        _unallocated_edges_i.push(i);
        assert(_unallocated_edges_i.size() <= _edges.size());
    }

    void to_csv(std::string vertex_filepath, std::string edge_filepath)
    {
        std::ofstream fv(vertex_filepath);
        fv.is_open();
        fv << "vertex_i,label,";
        fv << "graphviz_shape,graphviz_cluster,";
        fv << "graphviz_width,graphviz_height";
        fv << std::endl;
        for (size_t i = 0; i < _vertices.size(); i++) {
            if (!_vertices[i].allocated) {
                continue;
            }
            fv << i << ",";
            fv << _vertices[i].label << ",";
            fv << _vertices[i].graphviz_shape << ",";
            fv << _vertices[i].graphviz_cluster << ",";
            fv << _vertices[i].graphviz_width << ",";
            fv << _vertices[i].graphviz_height << std::endl;
        }
        fv.close();

        std::ofstream fe(edge_filepath);
        fe.is_open();
        fe << "edge_i,src_vertex_i,dst_vertex_i,label" << std::endl;
        for (size_t i = 0; i < _edges.size(); i++) {
            if (!_edges[i].allocated) {
                continue;
            }
            fe << i << ",";
            fe << _edges[i].src_vertex_i << ",";
            fe << _edges[i].dst_vertex_i << ",";
            fe << _edges[i].label << std::endl;
        }
        fe.close();
    }

    // return true if any from src vertices is connected to
    // any from the dst vertices
    bool are_connected_any(const std::set<size_t> src_vertices_i,
                           const std::set<size_t> dst_vertices_i)
    {
        assert(src_vertices_i.size() > 0);
        assert(dst_vertices_i.size() > 0);

        std::set<size_t> togo_i{src_vertices_i};
        std::set<size_t> visited_i;
        while (!togo_i.empty()) {
            size_t vertex_i = *togo_i.begin();
            if (dst_vertices_i.contains(vertex_i)) {
                return true;
            }
            togo_i.erase(vertex_i);
            visited_i.insert(vertex_i);

            for (size_t out_vertex_i : get_out_vertices_i(vertex_i)) {
                if (!visited_i.contains(out_vertex_i)) {
                    togo_i.insert(out_vertex_i);
                }
            }
        }

        return false;
    }

    // depth first search function that returns true if cycle found
    // - visited = dfs is ongoing for this vetex
    // - searched = dfs complete for this vertex and no cycles found
    bool dfs_cycle_found(const size_t vertex_i,
                         std::set<size_t>& visited_i,
                         std::set<size_t>& searched_i)
    {
        if (searched_i.contains(vertex_i)) {
            // this vertex was already searched
            return false;
        }

        if (visited_i.contains(vertex_i)) {
            // vertex already visited -> cycle found
            return true;
        }

        visited_i.insert(vertex_i);

        const std::vector<size_t> out_vertices_i =
                get_out_vertices_i(vertex_i);
        for (size_t out_vertex_i : out_vertices_i) {
            if (dfs_cycle_found(out_vertex_i, visited_i, searched_i)) {
                return true;
            }
        }

        assert(visited_i.contains(vertex_i));
        visited_i.erase(vertex_i);
        assert(!searched_i.contains(vertex_i));
        searched_i.insert(vertex_i);
        return false;
    }

    // return true if this directed graph is cyclic
    bool is_cyclic()
    {
        const std::vector<size_t> vertices_i = get_vertices_i();
        std::set<size_t> search_i(vertices_i.begin(), vertices_i.end());
        std::set<size_t> searched_i;
        while (!search_i.empty()) {
            std::set<size_t> visited_i;
            size_t search_vertex_i = *search_i.begin();
            if (dfs_cycle_found(search_vertex_i, visited_i, searched_i)) {
                return true;
            }
            for (size_t i : searched_i) {
                search_i.erase(i);
            }
        }

        assert(search_i.size() == 0);
        assert(searched_i.size() == get_n_vertices());
        assert(searched_i.size() == vertices_i.size());
#ifndef NDEBUG
        for (size_t vi : vertices_i) {
            assert(searched_i.contains(vi));
        }
#endif
        return false;
    }
};

template <typename TVertex, typename TEdge>
class DAG : public Graph<TVertex, TEdge> {
public:
    // int is used instead of size_t to be able to return -1
    // - retval: -1 if adding edge creates cycle;
    //           i of the added edge otherwise
    int add_edge(TEdge e)
    {
        const int edge_i = Graph<TVertex, TEdge>::add_edge(e);
        if (edge_i < 0) {
            return -1;
        }

        const TEdge* edge = this->get_edge(edge_i);
        assert(edge != nullptr);
        const size_t vertex_i = edge->src_vertex_i;
        std::set<size_t> visited_i;
        std::set<size_t> searched_i;

        if (!this->dfs_cycle_found(vertex_i, visited_i, searched_i)) {
            return edge_i;
        }

        this->remove_edge(edge_i);
        return -1;
    }
};

}  // namespace grafiins
