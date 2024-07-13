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

# debug
print(vertices_table)
print(edges_table)

g = pydot.Dot(graph_type="digraph")

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

g.write_svg("test.svg")

# TODO: set output name from config
# TODO: beautify, test more complex
# TODO: rotate by 90 deg
