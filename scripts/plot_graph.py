import argparse
import json
import os
import jsonschema
import numpy as np
import pandas as pd
import pathlib
import pydot

CONFIG_SCHEMA_PATH = os.path.join(
    os.path.dirname(__file__),
    "plot_graph_config.schema.json",
)

DEFAULT_CONFIG_SECTION = "plot_graph"

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--config", help="path to a .json config file")
parser.add_argument(
    "--config-section",
    help="name of the config section",
    default=DEFAULT_CONFIG_SECTION,
    required=False,
)
parser.add_argument("--vertices", help="path to a .csv vertices file")
parser.add_argument("--edges", help="path to a .csv edges file")
parser.add_argument("--output", help="path to a .svg output file")
args = parser.parse_args()

with open(CONFIG_SCHEMA_PATH) as f:
    config_schema = json.load(f)
with open(args.config) as f:
    config_json = json.load(f)
if args.config_section:
    config_json = config_json[args.config_section]

jsonschema.validate(instance=args.config, schema=config_schema)

vertices_table = pd.read_csv(args.vertices).replace(np.nan, "")
edges_table = pd.read_csv(args.edges).replace(np.nan, "")
output_path = args.output

graphviz_ordering = None
if "ordering" in config_json["graphviz"]:
    graphviz_ordering = config_json["graphviz"]["ordering"]
graphviz_rankdir = None
if "rankdir" in config_json["graphviz"]:
    graphviz_rankdir = config_json["graphviz"]["rankdir"]
graphviz_fontsize = None
if "fontsize" in config_json["graphviz"]:
    graphviz_fontsize = config_json["graphviz"]["fontsize"]
graphviz_fontname = None
if "fontname" in config_json["graphviz"]:
    graphviz_fontname = config_json["graphviz"]["fontname"]
graphviz_labelloc = None
if "labelloc" in config_json["graphviz"]:
    graphviz_labelloc = config_json["graphviz"]["labelloc"]

g = pydot.Dot(
    graph_type="digraph",
    forcelabels=True,
    rankdir=graphviz_rankdir,
    ordering=graphviz_ordering,
    fontsize=graphviz_fontsize,
    fontname=graphviz_fontname,
    labelloc=graphviz_labelloc,
)

clusters = {}
for i in vertices_table.index:
    # all nodes without graphviz_cluster are grouped under
    # a cluster with empty string name
    graphviz_cluster = vertices_table["graphviz_cluster"].iloc[i]
    if graphviz_cluster not in clusters:
        clusters[graphviz_cluster] = pydot.Subgraph(
            f"cluster_{graphviz_cluster}",
            label=graphviz_cluster,
            peripheries=0,
        )
        g.add_subgraph(clusters[graphviz_cluster])
    dst_graph = clusters[graphviz_cluster]

    graphviz_width = vertices_table["graphviz_width"].iloc[i]
    graphviz_height = vertices_table["graphviz_height"].iloc[i]
    # this property is required to enable width and/or height settings
    graphviz_fixedsize = bool(graphviz_width) or bool(graphviz_height)

    dst_graph.add_node(
        pydot.Node(
            str(vertices_table["vertex_i"].iloc[i]),
            label=vertices_table["graphviz_label"].iloc[i],
            xlabel=vertices_table["graphviz_xlabel"].iloc[i],
            shape=vertices_table["graphviz_shape"].iloc[i],
            fixedsize=graphviz_fixedsize,
            width=graphviz_width,
            height=graphviz_height,
            fontsize=graphviz_fontsize,
            fontname=graphviz_fontname,
            labelloc=graphviz_labelloc,
        )
    )

for i in edges_table.index:
    g.add_edge(
        pydot.Edge(
            str(edges_table["src_vertex_i"].iloc[i]),
            str(edges_table["dst_vertex_i"].iloc[i]),
            label=edges_table["graphviz_label"].iloc[i],
            xlabel=edges_table["graphviz_xlabel"].iloc[i],
            fontsize=graphviz_fontsize,
            fontname=graphviz_fontname,
            labelloc=graphviz_labelloc,
        )
    )

extension = pathlib.Path(output_path).suffix
if extension == ".svg":
    g.write_svg(output_path)
elif extension == ".png":
    g.write_png(output_path)
else:
    raise RuntimeError(f"{extension} not supported.")
