#include <cassert>
#include <queue>
#include <set>
#include <string>
#include <vector>
// #include <filesystem>
#include <fstream>
// #include <iostream>

namespace vgraph {

class Vertex {
private:
    std::set<size_t> _in_edges_i;
    std::set<size_t> _out_edges_i;

public:
    bool allocated = false;

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

    const std::set<size_t> get_in_edges_i()
    {
        return _in_edges_i;
    }

    const std::set<size_t> get_out_edges_i()
    {
        return _out_edges_i;
    }

    const std::set<size_t> get_in_vertices_i()
    {
        // TODO: add
        return std::set<size_t>{};
    }

    const std::set<size_t> get_out_vertices_i()
    {
        // TODO: add
        return std::set<size_t>{};
    }
};

class Edge {
public:
    size_t src_vertex_i;
    size_t dst_vertex_i;
    bool allocated = false;

    Edge(size_t in_src_i, size_t in_dst_i) :
        src_vertex_i(in_src_i),
        dst_vertex_i(in_dst_i)
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

    const std::vector<TVertex> get_vertices()
    {
        std::vector<TVertex> vertices;
        for (auto& v : _vertices) {
            if (v.allocated) {
                vertices.push_back(v);
            }
        }
        return vertices;
    }

    const std::vector<TEdge> get_edges()
    {
        std::vector<TEdge> edges;
        for (auto& e : _edges) {
            if (e.allocated) {
                edges.push_back(e);
            }
        }
        return edges;
    }

    size_t add_vertex(TVertex v)
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

    size_t add_vertex()
    {
        TVertex v{};
        return add_vertex(v);
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

    size_t add_edge(TEdge e)
    {
        // check input
        assert(e.src_vertex_i < _vertices.size());
        assert(e.dst_vertex_i < _vertices.size());
        assert(_vertices[e.src_vertex_i].allocated);
        assert(_vertices[e.dst_vertex_i].allocated);

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

    size_t add_edge(size_t src_i, size_t dst_i)
    {
        TEdge e(src_i, dst_i);
        return add_edge(e);
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
        fv << "vertex_i" << std::endl;
        for (size_t i = 0; i < _vertices.size(); i++) {
            if (!_vertices[i].allocated) {
                continue;
            }
            fv << i << std::endl;
        }
        fv.close();

        std::ofstream fe(edge_filepath);
        fe.is_open();
        fe << "edge_i,src_vertex_i,dst_vertex_i" << std::endl;
        for (size_t i = 0; i < _edges.size(); i++) {
            if (!_edges[i].allocated) {
                continue;
            }
            fe << i << ",";
            fe << _edges[i].src_vertex_i << ",";
            fe << _edges[i].dst_vertex_i << std::endl;
        }
        fe.close();
    }
};

template <typename TVertex, typename TEdge>
class DAG : Graph<TVertex, TEdge> {};

}  // namespace vgraph
