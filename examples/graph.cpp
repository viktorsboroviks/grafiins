#include "vgraph.hpp"

int main()
{
    vgraph::Graph<vgraph::Vertex, vgraph::Edge> g;
    assert(g.get_vertices_i().size() == 0);
    assert(g.get_edges_i().size() == 0);

    assert(g.add_vertex(vgraph::Vertex("v0")) == 0);
    assert(g.add_vertex(vgraph::Vertex("v1")) == 1);
    assert(g.get_n_vertices() == 2);
    assert(g.get_vertices_i().size() == 2);

    g.remove_vertex(0);
    assert(g.get_n_vertices() == 1);
    assert(g.get_vertices_i().size() == 1);

    assert(g.add_vertex(vgraph::Vertex("v0")) == 0);
    assert(g.get_n_vertices() == 2);
    assert(g.get_vertices_i().size() == 2);

    assert(g.add_vertex(vgraph::Vertex("v2")) == 2);
    assert(g.get_n_vertices() == 3);
    assert(g.get_vertices_i().size() == 3);

    assert(g.add_vertex(vgraph::Vertex("v3")) == 3);
    assert(g.get_n_vertices() == 4);
    assert(g.get_vertices_i().size() == 4);

    assert(g.add_edge(vgraph::Edge(0, 1, "e0")) == 0);
    assert(g.add_edge(vgraph::Edge(1, 2, "e1")) == 1);
    assert(g.add_edge(vgraph::Edge(2, 3, "e1")) == 2);
    assert(g.get_n_edges() == 3);
    assert(g.get_edges_i().size() == 3);

    g.remove_edge(0);
    g.remove_edge(1);
    assert(g.get_n_edges() == 1);
    assert(g.get_edges_i().size() == 1);

    assert(g.add_vertex(vgraph::Vertex("v4")) == 4);
    assert(g.add_vertex(vgraph::Vertex("v5")) == 5);
    assert(g.add_vertex(vgraph::Vertex("v6")) == 6);
    assert(g.add_vertex(vgraph::Vertex("v7")) == 7);
    assert(g.add_vertex(vgraph::Vertex("v8")) == 8);
    assert(g.add_vertex(vgraph::Vertex("v9")) == 9);
    assert(g.add_vertex(vgraph::Vertex("v10")) == 10);
    assert(g.add_vertex(vgraph::Vertex("v11")) == 11);
    assert(g.add_vertex(vgraph::Vertex("v12")) == 12);
    assert(g.add_vertex(vgraph::Vertex("v13")) == 13);
    assert(g.get_n_vertices() == 14);

    assert(g.add_edge(vgraph::Edge(3, 4, "e0")) == 0);
    assert(g.add_edge(vgraph::Edge(1, 2, "e1")) == 1);
    assert(g.add_edge(vgraph::Edge(5, 0, "e3")) == 3);
    assert(g.add_edge(vgraph::Edge(6, 3, "e4")) == 4);
    assert(g.add_edge(vgraph::Edge(7, 8, "e5")) == 5);
    assert(g.add_edge(vgraph::Edge(8, 9, "e6")) == 6);
    assert(g.add_edge(vgraph::Edge(9, 3, "e7")) == 7);
    assert(g.add_edge(vgraph::Edge(10, 11, "e8")) == 8);
    assert(g.add_edge(vgraph::Edge(11, 9, "e9")) == 9);
    assert(g.add_edge(vgraph::Edge(12, 11, "e10")) == 10);
    assert(g.get_n_edges() == 11);

    for (size_t i : {1, 5, 6, 7, 10, 12}) {
        g.get_vertex(i)->graphviz_shape = "doublecircle";
        g.get_vertex(i)->graphviz_subgraph_same = "inputs";
        g.get_vertex(i)->graphviz_width = 0.4;
    }

    for (size_t i : {0, 4}) {
        g.get_vertex(i)->graphviz_shape = "doublecircle";
        g.get_vertex(i)->graphviz_subgraph_same = "outputs";
        g.get_vertex(i)->graphviz_width = 0.4;
    }

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
