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

    assert(g.add_edge(vgraph::Edge(0, 1)) == 0);
    assert(g.add_edge(1, 2) == 1);
    assert(g.get_n_edges() == 2);
    assert(g.get_edges().size() == 2);

    g.remove_edge(1);
    assert(g.get_n_edges() == 1);
    assert(g.get_edges().size() == 1);

    // TODO: add visualization at every step
    // - CLI/console/text
    // - other to be processed by plotly later
    // - dots/graphviz?
    // TODO: add check if 2 vertices are connected
    // TODO: add check if graph contains cycles
    // TODO: add DAG

    return 0;
}
