#include <stdexcept>

#include "grafiins.hpp"

int main()
{
    grafiins::Graph<grafiins::Vertex, grafiins::Edge> g;
    assert(g.all_vertices_i().size() == 0);
    assert(g.all_edges_i().size() == 0);

    assert(g.add_vertex(grafiins::Vertex("v0")) == 0);
    assert(g.add_vertex(grafiins::Vertex("v1")) == 1);
    assert(g.n_vertices() == 2);
    assert(g.all_vertices_i().size() == 2);

    g.remove_vertex(0);
    assert(g.n_vertices() == 1);
    assert(g.all_vertices_i().size() == 1);

    assert(g.add_vertex(grafiins::Vertex("v0")) == 0);
    assert(g.n_vertices() == 2);
    assert(g.all_vertices_i().size() == 2);

    assert(g.add_vertex(grafiins::Vertex("v2")) == 2);
    assert(g.n_vertices() == 3);
    assert(g.all_vertices_i().size() == 3);

    assert(g.add_vertex(grafiins::Vertex("v3")) == 3);
    assert(g.n_vertices() == 4);
    assert(g.all_vertices_i().size() == 4);

    assert(g.add_edge(grafiins::Edge(0, 1, "e0")) == 0);
    assert(g.add_edge(grafiins::Edge(1, 2, "e1")) == 1);
    assert(g.add_edge(grafiins::Edge(2, 3, "e1")) == 2);
    assert(g.n_edges() == 3);
    assert(g.all_edges_i().size() == 3);

    g.remove_edge(0);
    g.remove_edge(1);
    assert(g.n_edges() == 1);
    assert(g.all_edges_i().size() == 1);

    assert(g.add_vertex(grafiins::Vertex("v4")) == 4);
    assert(g.add_vertex(grafiins::Vertex("v5")) == 5);
    assert(g.add_vertex(grafiins::Vertex("v6")) == 6);
    assert(g.add_vertex(grafiins::Vertex("v7")) == 7);
    assert(g.add_vertex(grafiins::Vertex("v8")) == 8);
    assert(g.add_vertex(grafiins::Vertex("v9")) == 9);
    assert(g.add_vertex(grafiins::Vertex("v10")) == 10);
    assert(g.add_vertex(grafiins::Vertex("v11")) == 11);
    assert(g.add_vertex(grafiins::Vertex("v12")) == 12);
    assert(g.add_vertex(grafiins::Vertex("v13")) == 13);
    assert(g.n_vertices() == 14);

    assert(g.add_edge(grafiins::Edge(3, 4, "e0")) == 0);
    assert(g.add_edge(grafiins::Edge(1, 2, "e1")) == 1);
    assert(g.add_edge(grafiins::Edge(5, 0, "e3")) == 3);
    assert(g.add_edge(grafiins::Edge(6, 3, "e4")) == 4);
    assert(g.add_edge(grafiins::Edge(7, 8, "e5")) == 5);
    assert(g.add_edge(grafiins::Edge(8, 9, "e6")) == 6);
    assert(g.add_edge(grafiins::Edge(9, 3, "e7")) == 7);
    assert(g.add_edge(grafiins::Edge(10, 11, "e8")) == 8);
    assert(g.add_edge(grafiins::Edge(11, 9, "e9")) == 9);
    assert(g.add_edge(grafiins::Edge(12, 11, "e10")) == 10);
    assert(g.n_edges() == 11);

    for (size_t i : {1, 5, 6, 7, 10, 12}) {
        g.vertex_at(i)->graphviz_shape = "doublecircle";
        g.vertex_at(i)->graphviz_width = 0.4;
        g.vertex_at(i)->graphviz_cluster = "inputs";
    }

    for (size_t i : {0, 4}) {
        g.vertex_at(i)->graphviz_shape = "doublecircle";
        g.vertex_at(i)->graphviz_width = 0.4;
        g.vertex_at(i)->graphviz_cluster = "outputs";
    }

    g.to_csv("vertices.csv", "edges.csv");

    assert(g.are_connected_any({9}, {9}));
    assert(g.are_connected_any({6}, {4}));
    assert(g.are_connected_any({6}, {3}));
    assert(g.are_connected_any({7}, {3, 9}));
    assert(g.are_connected_any({7}, {0, 3, 9}));
    assert(!g.are_connected_any({7}, {10, 0}));
    assert(!g.are_connected_any({8}, {7}));
    assert(g.are_connected_any({13, 7, 5}, {4}));
    assert(g.are_connected_any({13, 7, 5}, {4, 0}));
    assert(!g.are_connected_any({13, 7, 5}, {6}));
    assert(!g.are_connected_any({2, 8}, {0}));

    assert(!g.is_cyclic());
    assert(g.add_edge(grafiins::Edge(3, 10, "cyclyc edge!")) == 11);
    assert(g.is_cyclic());
    g.remove_edge(11);
    assert(!g.is_cyclic());

    grafiins::DAG<grafiins::Vertex, grafiins::Edge> dag;
    assert(dag.add_vertex(grafiins::Vertex("v0")) == 0);
    assert(dag.add_vertex(grafiins::Vertex("v1")) == 1);
    assert(dag.add_vertex(grafiins::Vertex("v2")) == 2);
    assert(dag.n_vertices() == 3);
    assert(dag.all_vertices_i().size() == 3);

    assert(dag.add_edge(grafiins::Edge(0, 1, "e0")) == 0);
    assert(dag.add_edge(grafiins::Edge(1, 2, "e1")) == 1);
    assert(dag.n_edges() == 2);
    assert(dag.all_edges_i().size() == 2);
    assert(!dag.is_cyclic());

    assert(!dag.add_edge(grafiins::Edge(2, 0, "cyclic edge!")).has_value());

    assert(!dag.is_cyclic());
    assert(dag.n_edges() == 2);

    return 0;
}
