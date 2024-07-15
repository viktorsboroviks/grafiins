import argparse
import json
import os
import jsonschema
import numpy as np
import pandas as pd
import pydot

CONFIG_SCHEMA_PATH = os.path.join(
    os.path.dirname(__file__),
    "plot_graph_config.schema.json",
)

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("config", help="path to a .json config file")
args = parser.parse_args()

with open(CONFIG_SCHEMA_PATH) as f:
    config_schema = json.load(f)
with open(args.config) as f:
    config_json = json.load(f)["plot_graph"]

jsonschema.validate(instance=args.config, schema=config_schema)

vertices_table = pd.read_csv(config_json["vertices_csv_path"]).replace(np.nan, "")
edges_table = pd.read_csv(config_json["edges_csv_path"]).replace(np.nan, "")
output_svg_path = config_json["output_svg_path"]

graphviz_ordering = None
if "ordering" in config_json["graphviz"]:
    graphviz_ordering = config_json["graphviz"]["ordering"]
graphviz_rankdir = None
if "rankdir" in config_json["graphviz"]:
    graphviz_rankdir = config_json["graphviz"]["rankdir"]

g = pydot.Dot(
    graph_type="digraph",
    rankdir=graphviz_rankdir,
    ordering=graphviz_ordering,
)

for i in vertices_table.index:
    g.add_node(
        pydot.Node(
            str(vertices_table["vertex_i"].iloc[i]),
            label=vertices_table["label"].iloc[i],
        )
    )

for i in edges_table.index:
    g.add_edge(
        pydot.Edge(
            str(edges_table["src_vertex_i"].iloc[i]),
            str(edges_table["dst_vertex_i"].iloc[i]),
            label=edges_table["label"].iloc[i],
        )
    )

g.write_svg(output_svg_path)

# TODO:
# - set from csv
#   - node style
#     - https://graphviz.org/Gallery/directed/fsm.html
#   - grouping
#     - rank=same https://graphviz.readthedocs.io/en/stable/examples.html#rank-same-py
