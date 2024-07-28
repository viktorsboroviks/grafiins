#include <cassert>
#include <fstream>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "garaza.hpp"

namespace grafiins {

struct Vertex {
    std::set<size_t> _in_edges_i;
    std::set<size_t> _out_edges_i;

    std::string label;
    std::string graphviz_shape = "circle";
    std::string graphviz_cluster = "";
    double graphviz_width = 0;
    double graphviz_height = 0;

    Vertex(std::string in_label = "") :
        label(in_label)
    {
    }
};

struct Edge {
    size_t _src_vertex_i;
    size_t _dst_vertex_i;

    std::string label;

    Edge(size_t src_i, size_t dst_i, std::string label = "") :
        _src_vertex_i(src_i),
        _dst_vertex_i(dst_i),
        label(label)
    {
    }
};

template <typename TVertex, typename TEdge>
class Graph {
private:
    garaza::Storage<TVertex> _vertices;
    garaza::Storage<TEdge> _edges;

public:
    bool allow_parallel_edges = false;

    Graph(bool allow_parallel_edges = false) :
        allow_parallel_edges(allow_parallel_edges)
    {
    }

    size_t n_vertices()
    {
        return _vertices.size();
    }

    size_t n_edges()
    {
        return _edges.size();
    }

    const std::vector<size_t> all_vertices_i()
    {
        return _vertices.all_i();
    }

    TVertex* vertex_at(size_t i)
    {
        return _vertices.at(i);
    }

    const std::vector<size_t> all_edges_i()
    {
        return _edges.all_i();
    }

    TEdge* edge_at(size_t i)
    {
        return _edges.at(i);
    }

    const std::vector<size_t> out_vertices_i(size_t vertex_i)
    {
        const TVertex* v = vertex_at(vertex_i);
        assert(v != nullptr);

        std::vector<size_t> out_vertices_i;
        for (auto& oe_i : v->_out_edges_i) {
            out_vertices_i.push_back(edge_at(oe_i)->_dst_vertex_i);
        }

        return out_vertices_i;
    }

    const std::vector<size_t> in_vertices_i(size_t vertex_i)
    {
        const TVertex* v = vertex_at(vertex_i);
        assert(v != nullptr);

        std::vector<size_t> in_vertices_i;
        for (auto& oe_i : v->_in_edges_i) {
            in_vertices_i.push_back(edge_at(oe_i)->src_vertex_i);
        }

        return in_vertices_i;
    }

    size_t add_vertex(TVertex v)
    {
        return _vertices.add(v);
    }

    void remove_vertex(size_t i)
    {
        assert(_vertices.contains_i(i));

        // remove connected edges
        for (size_t ei : _vertices.at(i)->_in_edges_i) {
            remove_edge(ei);
        }
        for (size_t ei : _vertices.at(i)->_out_edges_i) {
            remove_edge(ei);
        }

        _vertices.remove(i);
    }

    size_t add_edge(TEdge e)
    {
        // check input
        assert(_vertices.contains_i(e._src_vertex_i));
        assert(_vertices.contains_i(e._dst_vertex_i));

        if (!allow_parallel_edges) {
            for (size_t vi : out_vertices_i(e._src_vertex_i)) {
                if (vi == e._dst_vertex_i) {
                    throw std::logic_error("Parallel edges not allowed.");
                }
            }
        }

        // add edge
        size_t i = _edges.add(e);

        // update connected vertices
        _vertices.at(e._src_vertex_i)->_out_edges_i.insert(i);
        _vertices.at(e._dst_vertex_i)->_in_edges_i.insert(i);

        return i;
    }

    void remove_edge(size_t i)
    {
        assert(_edges.contains_i(i));

        // disconnect vertices
        const size_t src_i = _edges.at(i)->_src_vertex_i;
        assert(_vertices.contains_i(src_i));
        _vertices.at(src_i)->_out_edges_i.erase(i);

        const size_t dst_i = _edges.at(i)->_dst_vertex_i;
        assert(_vertices.contains_i(dst_i));
        _vertices.at(dst_i)->_in_edges_i.erase(i);

        // remove edge
        _edges.remove(i);
    }

    void to_csv(std::string vertex_filepath, std::string edge_filepath)
    {
        std::ofstream fv(vertex_filepath);
        fv.is_open();
        fv << "vertex_i,label,";
        fv << "graphviz_shape,graphviz_cluster,";
        fv << "graphviz_width,graphviz_height";
        fv << std::endl;
        for (size_t i : _vertices.all_i()) {
            fv << i << ",";
            fv << _vertices.at(i)->label << ",";
            fv << _vertices.at(i)->graphviz_shape << ",";
            fv << _vertices.at(i)->graphviz_cluster << ",";
            fv << _vertices.at(i)->graphviz_width << ",";
            fv << _vertices.at(i)->graphviz_height << std::endl;
        }
        fv.close();

        std::ofstream fe(edge_filepath);
        fe.is_open();
        fe << "edge_i,src_vertex_i,dst_vertex_i,label" << std::endl;
        for (size_t i : _edges.all_i()) {
            fe << i << ",";
            fe << _edges.at(i)->_src_vertex_i << ",";
            fe << _edges.at(i)->_dst_vertex_i << ",";
            fe << _edges.at(i)->label << std::endl;
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

            for (size_t out_vertex_i : out_vertices_i(vertex_i)) {
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

        for (size_t out_vertex_i : out_vertices_i(vertex_i)) {
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
        const std::vector<size_t> vertices_i = all_vertices_i();
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
        assert(searched_i.size() == n_vertices());
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
    size_t add_edge(TEdge e)
    {
        const size_t edge_i = Graph<TVertex, TEdge>::add_edge(e);

        const TEdge* edge = this->edge_at(edge_i);
        assert(edge != nullptr);
        const size_t vertex_i = edge->_src_vertex_i;
        std::set<size_t> visited_i;
        std::set<size_t> searched_i;

        if (!this->dfs_cycle_found(vertex_i, visited_i, searched_i)) {
            return edge_i;
        }

        this->remove_edge(edge_i);
        throw std::logic_error("Adding edge creates cycle in DAG.");
    }
};

}  // namespace grafiins
