import argparse
import json
import os
import pandas as pd
import jsonschema

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

# TODO: add vgraph.py
# TODO: add csv_to_graphviz
