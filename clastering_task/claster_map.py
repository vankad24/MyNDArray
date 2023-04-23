# https://www.naturalearthdata.com/downloads/
# https://gist.github.com/hrbrmstr/91ea5cc9474286c72838?short_path=f3fde31
# Continents JSON
import matplotlib.pyplot as plt
import json
import shapely
import math
import networkx as nx
from networkx.drawing.nx_pydot import graphviz_layout


def draw_graph(graph):
    pos = nx.spring_layout(graph)
    plt.figure(figsize=(7, 6))
    nx.draw_networkx_nodes(graph, pos)
    nx.draw_networkx_edges(graph, pos, graph.edges)
    nx.draw_networkx_labels(graph, pos, font_size=20, )
    labels = nx.get_edge_attributes(graph, "weight")
    nx.draw_networkx_edge_labels(graph, pos, edge_labels=labels)


def get_spanning_tree(graph):
    first_node = next(iter(graph.nodes))
    visited_nodes = {first_node}
    connected_nodes = set(map(lambda e: e[1], graph.edges(first_node)))
    edges = []
    while connected_nodes:

        edges_from_visited = []
        for visited_node in visited_nodes:
            for available_node in connected_nodes:
                if graph.has_edge(visited_node,available_node):
                    edges_from_visited.append((visited_node, available_node, graph.get_edge_data(visited_node, available_node)["weight"]))

        min_edge = min(edges_from_visited, key=lambda e: e[2])
        edges.append(min_edge)
        visited_node = min_edge[1]
        visited_nodes.add(visited_node)
        connected_nodes.remove(visited_node)
        for node in filter(lambda n: n not in visited_nodes,map(lambda e: e[1],graph.edges(visited_node))):
            connected_nodes.add(node)

    result = nx.Graph()
    result.add_weighted_edges_from(edges)
    result.add_nodes_from(visited_nodes)
    return result

def distance(site1, site2):
    loc1 = site1["location"]
    loc2 = site2["location"]
    r = 6371  # earth radius
    phi1 = math.radians(loc1["lat"])
    phi2 = math.radians(loc2["lat"])
    dphi = math.radians(loc2["lat"] - loc1["lat"])
    dlambda = math.radians(loc2["lon"] - loc1["lon"])
    a = math.sin(dphi / 2) ** 2 + math.cos(phi1) * math.cos(phi2) * math.sin(dlambda / 2) ** 2
    c = 2 * math.atan2(a ** .5, (1 - a) ** .5)
    return r * c

def make_graph(sites, max_distance=400):  # max_dis influences the number of edges
    edges = []
    nodes = []
    nodes_d = {}

    for i, site in enumerate(sites[:-1]):

        # print(f"{(i + 1)} / {len(sites) - 1}")
        nodes.append(site["code"])

        min_distance = 10 ** 16
        min_dest = None  # if we didn't find anything
        min_dest_flag = True

        nodes_d[site["code"]] = 1

        for dest in sites[i + 1:]:  # the point we are looking at

            if dest["code"] in nodes_d:
                continue

            d = distance(site, dest)
            if d <= max_distance:
                edges.append((site["code"], dest["code"], d))
                min_dest_flag = False
            if d < min_distance:
                min_dest = dest
                min_distance = d

        if min_dest_flag:
            edges.append((site["code"], min_dest["code"], min_distance))
    return edges, nodes


def get_clusters(spanning_tree, n_clusters):
    clusters = []
    deleted_edges = nx.Graph()
    deleted_edges.add_edges_from(
        sorted(spanning_tree.edges, key=lambda e: spanning_tree.get_edge_data(*e)["weight"])[-n_clusters + 1:])

    for tree_node in spanning_tree.nodes:
        # delete all unnecessary edges
        edges = filter(lambda e: not deleted_edges.has_edge(*e), spanning_tree.edges(tree_node))
        # add weights to edges
        edges = map(lambda e: e + (spanning_tree.get_edge_data(*e)["weight"],), edges)
        # convert to list
        edges = list(edges)
        nodes = list(map(lambda edge: edge[1], edges))

        need_create = True
        for cluster in clusters:
            for node in nodes:
                if node in cluster.nodes:
                    need_create = False
                    cluster.add_nodes_from(nodes)
                    cluster.add_weighted_edges_from(edges)
                    break
            if not need_create:
                break
        if need_create:
            g = nx.Graph()
            g.add_node(tree_node)
            g.add_nodes_from(nodes)
            g.add_weighted_edges_from(edges)
            clusters.append(g)
    return clusters


# print(distance({"location":{"lat":56.434038, "lon":36.425904}},{"location":{"lat":52.901612, "lon":104.207835}}))


with open("base.json", "r") as f:
    sites = json.load(f)  # места
    # print(len(sites))
    # print(sites[0])
    lons = []
    lats = []
    for site in sites:
        lons.append(site["location"]["lon"])
        lats.append(site["location"]["lat"])

with open("continents.json", "r") as f:
    continents = json.load(f)
    north_america_coords = continents["features"][1]["geometry"]["coordinates"]
# print(len(north_america_coords[0]))
# plt.scatter(lons,lats)
# plt.scatter(lons,lats)
# plt.show()

# coords = north_america_coords[3][0]
# xs,ys = zip(*coords)
# plt.plot(xs,ys)
# plt.show()

# for n in north_america_coords:
# print(len(n[0]))
max_area = -1
max_polygon = []

for i in range(len(north_america_coords)):
    points = north_america_coords[i][0]
    area = shapely.Polygon(points).area
    if area > max_area:
        max_area = area
        max_polygon = points

# xs,ys = zip(*max_polygon)
# print(max_polygon)
# plt.plot(xs,ys)


north_america = shapely.Polygon(max_polygon)
north_america_places = []
for place in sites:
    p = shapely.Point((place["location"]["lon"], place["location"]["lat"]))
    if north_america.contains(p):
        north_america_places.append(place)

print(len(north_america_places))

# lons = []
# lats = []
# for site in north_america_places:
# 	lons.append(site["location"]["lon"])
# 	lats.append(site["location"]["lat"])
# plt.scatter(lons,lats)


edges, nodes = make_graph(north_america_places)
print(len(edges))

graph = nx.Graph()
graph.add_nodes_from(nodes)
graph.add_weighted_edges_from(edges)
graphs = get_clusters(get_spanning_tree(graph), 40)
print(len(graphs))

# plt.show()
