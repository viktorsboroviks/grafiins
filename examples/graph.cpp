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

    assert(g.add_edge(0, 1) == 0);
    assert(g.get_n_edges() == 1);
    assert(g.get_edges().size() == 1);

    return 0;
}
