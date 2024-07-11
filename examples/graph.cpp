#include "vgraph.hpp"

int main()
{
    vgraph::Graph<vgraph::Vertex, vgraph::Edge> g;
    assert(g.get_vertices().size() == 0);
    assert(g.get_edges().size() == 0);
    return 0;
}
