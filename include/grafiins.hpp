#include <cassert>
#include <fstream>
#include <map>
#include <optional>
#include <queue>
#include <set>
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

    std::map<std::string, std::string> serialize()
    {
        std::map<std::string, std::string> m;
        m["label"] = label;
        m["graphviz_shape"] = graphviz_shape;
        m["graphviz_cluster"] = graphviz_cluster;
        m["graphviz_width"] = std::to_string(graphviz_width);
        m["graphviz_height"] = std::to_string(graphviz_height);
        return m;
    }
};

struct Edge {
    std::optional<size_t> _src_vertex_i;
    std::optional<size_t> _dst_vertex_i;

    std::string label;

    Edge(size_t src_i, size_t dst_i, std::string label = "") :
        _src_vertex_i(src_i),
        _dst_vertex_i(dst_i),
        label(label)
    {
    }

    // constructor w/o arguments is required to be able to
    // resize garaza::Storage<Edge>
    Edge(std::string label = "") :
        label(label)
    {
    }

    std::map<std::string, std::string> serialize()
    {
        std::map<std::string, std::string> m;
        m["label"] = label;

        if (_src_vertex_i.has_value()) {
            m["src_vertex_i"] = std::to_string(_src_vertex_i.value());
        }
        else {
            m["src_vertex_i"] = "unassigned";
        }

        if (_dst_vertex_i.has_value()) {
            m["dst_vertex_i"] = std::to_string(_dst_vertex_i.value());
        }
        else {
            m["dst_vertex_i"] = "unassigned";
        }

        return m;
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

    const std::list<TVertex> all_vertices()
    {
        return _vertices.list();
    }

    bool contains_vertex_i(size_t i)
    {
        return _vertices.contains_i(i);
    }

    size_t rnd_vertex_i()
    {
        return _vertices.rnd_i();
    }

    TVertex* vertex_at(size_t i)
    {
        return _vertices.at(i);
    }

    const std::vector<size_t> all_edges_i()
    {
        return _edges.all_i();
    }

    const std::list<TEdge> all_edges()
    {
        return _edges.list();
    }

    bool contains_edge_i(size_t i)
    {
        return _edges.contains_i(i);
    }

    size_t rnd_edge_i()
    {
        return _edges.rnd_i();
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
        for (auto& oei : v->_out_edges_i) {
            const TEdge* e = edge_at(oei);
            assert(e != nullptr);
            assert(e->_dst_vertex_i.has_value());
            out_vertices_i.push_back(e->_dst_vertex_i.value());
        }

        return out_vertices_i;
    }

    const std::vector<size_t> in_vertices_i(size_t vertex_i)
    {
        const TVertex* v = vertex_at(vertex_i);
        assert(v != nullptr);

        std::vector<size_t> in_vertices_i;
        for (auto& iei : v->_in_edges_i) {
            const TEdge* e = edge_at(iei);
            assert(e != nullptr);
            assert(e->_src_vertex_i.has_value());
            in_vertices_i.push_back(e->_src_vertex_i.value());
        }

        return in_vertices_i;
    }

    size_t add_vertex(TVertex v)
    {
        const size_t i = _vertices.add(v);
        return i;
    }

    void remove_vertex(size_t i)
    {
        assert(_vertices.contains_i(i));

        // remove connected edges
        const TVertex* v = _vertices.at(i);
        assert(v != nullptr);
        const std::set<size_t> in_ei = v->_in_edges_i;
        const std::set<size_t> out_ei = v->_out_edges_i;
        for (size_t iei : in_ei) {
            assert(!out_ei.contains(iei));
            remove_edge(iei);
        }
        for (size_t oei : out_ei) {
            assert(!in_ei.contains(oei));
            remove_edge(oei);
        }

        _vertices.remove(i);
    }

    std::optional<size_t> add_edge(TEdge e)
    {
        // check input
        assert(e._src_vertex_i.has_value());
        assert(e._dst_vertex_i.has_value());
        assert(_vertices.contains_i(e._src_vertex_i.value()));
        assert(_vertices.contains_i(e._dst_vertex_i.value()));

        if (!allow_parallel_edges) {
            for (size_t vi : out_vertices_i(e._src_vertex_i.value())) {
                if (vi == e._dst_vertex_i.value()) {
                    // parallel edges not allowed
                    return {};
                }
            }
        }

        // add edge
        const size_t i = _edges.add(e);

        // update connected vertices
        _vertices.at(e._src_vertex_i.value())->_out_edges_i.insert(i);
        _vertices.at(e._dst_vertex_i.value())->_in_edges_i.insert(i);

        return i;
    }

    size_t remove_edge(size_t i)
    {
        assert(_edges.contains_i(i));

        // disconnect vertices
        const TEdge* e = _edges.at(i);
        assert(e->_src_vertex_i.has_value());
        const size_t src_i = e->_src_vertex_i.value();
        assert(_vertices.contains_i(src_i));
        _vertices.at(src_i)->_out_edges_i.erase(i);

        assert(e->_dst_vertex_i.has_value());
        const size_t dst_i = e->_dst_vertex_i.value();
        assert(_vertices.contains_i(dst_i));
        _vertices.at(dst_i)->_in_edges_i.erase(i);

        // remove edge
        _edges.remove(i);
        return i;
    }

    void to_csv(const std::string& vertex_filepath,
                const std::string& edge_filepath)
    {
        // vertices
        std::ofstream fv(vertex_filepath);
        fv.is_open();

        // generate title row
        fv << "vertex_i";
        std::map<std::string, std::string> mvh = TVertex().serialize();
        for (std::map<std::string, std::string>::iterator it = mvh.begin();
             it != mvh.end();
             it++) {
            fv << ",";
            fv << it->first;
        }
        fv << std::endl;

        // generate content
        for (size_t i : _vertices.all_i()) {
            fv << i;
            std::map<std::string, std::string> mv =
                    _vertices.at(i)->serialize();
            for (std::map<std::string, std::string>::iterator it = mv.begin();
                 it != mv.end();
                 it++) {
                fv << ",";
                fv << it->second;
            }
            fv << std::endl;
        }
        fv.close();

        // edges
        std::ofstream fe(edge_filepath);
        fe.is_open();

        // generate title row
        fe << "edge_i";
        std::map<std::string, std::string> meh = TEdge().serialize();
        for (std::map<std::string, std::string>::iterator it = meh.begin();
             it != meh.end();
             it++) {
            fe << ",";
            fe << it->first;
        }
        fe << std::endl;

        // generate content
        for (size_t i : _edges.all_i()) {
            fe << i;
            std::map<std::string, std::string> me = _edges.at(i)->serialize();
            for (std::map<std::string, std::string>::iterator it = me.begin();
                 it != me.end();
                 it++) {
                fe << ",";
                fe << it->second;
            }
            fe << std::endl;
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
    std::optional<size_t> add_edge(TEdge e)
    {
        assert(e._src_vertex_i.value() != e._dst_vertex_i.value());
        const std::optional<size_t> opt = Graph<TVertex, TEdge>::add_edge(e);
        if (!opt.has_value()) {
            return {};
        }
        const size_t edge_i = opt.value();

        const TEdge* edge = this->edge_at(edge_i);
        assert(edge != nullptr);
        assert(edge->_src_vertex_i.has_value());
        const size_t vertex_i = edge->_src_vertex_i.value();
        std::set<size_t> visited_i;
        std::set<size_t> searched_i;

        if (!this->dfs_cycle_found(vertex_i, visited_i, searched_i)) {
            return edge_i;
        }

        this->remove_edge(edge_i);
        return {};
    }
};

}  // namespace grafiins
