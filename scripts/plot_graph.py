import argparse
import json
import os
import jsonschema
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

vertices_table = pd.read_csv(config_json["vertices_csv_path"])
edges_table = pd.read_csv(config_json["edges_csv_path"])
output_svg_path = config_json["output_svg_path"]

g = pydot.Dot(graph_type="digraph", rankdir="LR", ordering="in")

for i in vertices_table.index:
    g.add_node(pydot.Node(str(vertices_table["vertex_i"].iloc[i]), label=f"v{i}"))

for i in edges_table.index:
    g.add_edge(
        pydot.Edge(
            str(edges_table["src_vertex_i"].iloc[i]),
            str(edges_table["dst_vertex_i"].iloc[i]),
            label=f"e{i}",
        )
    )

g.write_svg(output_svg_path)

# TODO:
# - set from config
#   - rotate by 90 deg
#   - ordering = in/out/none
# - set from csv
#   - label
#   - node style
#     - https://graphviz.org/Gallery/directed/fsm.html
#   - grouping
#     - rank=same https://graphviz.readthedocs.io/en/stable/examples.html#rank-same-py
