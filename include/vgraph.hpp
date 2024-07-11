#include <cassert>
#include <queue>
#include <set>
#include <vector>

namespace vgraph {

class Vertex {
private:
    std::set<size_t> _in_edges_i;
    std::set<size_t> _out_edges_i;

public:
    size_t i;
    bool allocated = false;

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
private:
    size_t _src_vertex_i;
    size_t _dst_vertex_i;

public:
    size_t i;
    bool allocated = false;

    size_t get_src_vertex_i()
    {
        return _src_vertex_i;
    }

    size_t get_dst_vertex_i()
    {
        return _dst_vertex_i;
    }
};

template <typename TVertex, typename TEdge>
class Graph {
private:
    std::vector<TVertex> _vertices;
    std::vector<TEdge> _edges;
    std::queue<size_t> _not_allocated_vertices_i;
    std::queue<size_t> _not_allocated_edges_i;

public:
    const std::vector<TVertex> get_vertices()
    {
        return _vertices;
    }

    const std::vector<TEdge> get_edges()
    {
        return _edges;
    }

    size_t add_vertex(TVertex)
    {
        // TODO: add
        return 0;
    }

    size_t add_edge(TVertex src, TVertex dst)
    {
        // TODO: add
        return 0;
    }

    void remove_vertex(size_t i)
    {
        // TODO: add
    }

    void remove_edge(size_t i)
    {
        // TODO: add
    }
};

template <typename TVertex, typename TEdge>
class DAG : Graph<TVertex, TEdge> {};

}  // namespace vgraph
