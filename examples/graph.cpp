#include "vgraph.hpp"

int main()
{
    vgraph::Graph<vgraph::Vertex, vgraph::Edge> g;
    assert(g.get_vertices().size() == 0);
    assert(g.get_edges().size() == 0);

    assert(g.add_vertex(vgraph::Vertex()) == 0);
    assert(g.add_vertex() == 1);
    assert(g.get_n_vertices() == 2);
    assert(g.get_vertices().size() == 2);

    g.remove_vertex(0);
    assert(g.get_n_vertices() == 1);
    assert(g.get_vertices().size() == 1);

    assert(g.add_vertex() == 0);
    assert(g.get_n_vertices() == 2);
    assert(g.get_vertices().size() == 2);

    assert(g.add_vertex() == 2);
    assert(g.get_n_vertices() == 3);
    assert(g.get_vertices().size() == 3);

    assert(g.add_vertex() == 3);
    assert(g.get_n_vertices() == 4);
    assert(g.get_vertices().size() == 4);

    assert(g.add_edge(vgraph::Edge(0, 1)) == 0);
    assert(g.add_edge(1, 2) == 1);
    assert(g.add_edge(2, 3) == 2);
    assert(g.get_n_edges() == 3);
    assert(g.get_edges().size() == 3);

    g.remove_edge(0);
    g.remove_edge(1);
    assert(g.get_n_edges() == 1);
    assert(g.get_edges().size() == 1);

    assert(g.add_vertex() == 4);
    assert(g.add_vertex() == 5);
    assert(g.add_vertex() == 6);
    assert(g.add_vertex() == 7);
    assert(g.add_vertex() == 8);
    assert(g.add_vertex() == 9);
    assert(g.add_vertex() == 10);
    assert(g.add_vertex() == 11);
    assert(g.add_vertex() == 12);
    assert(g.add_vertex() == 13);
    assert(g.get_n_vertices() == 14);

    assert(g.add_edge(3, 4) == 0);
    assert(g.add_edge(1, 2) == 1);
    assert(g.add_edge(5, 0) == 3);
    assert(g.add_edge(6, 3) == 4);
    assert(g.add_edge(7, 8) == 5);
    assert(g.add_edge(8, 9) == 6);
    assert(g.add_edge(9, 3) == 7);
    assert(g.add_edge(10, 11) == 8);
    assert(g.add_edge(11, 9) == 9);
    assert(g.add_edge(12, 11) == 10);
    assert(g.get_n_edges() == 11);

    g.to_csv("vertices.csv", "edges.csv");
    // TODO: add visualization at every step
    // - CLI/console/text
    // - other to be processed by plotly later
    // - dots/graphviz?
    // TODO: add check if 2 vertices are connected
    // TODO: add check if graph contains cycles
    // TODO: add DAG

    return 0;
}
