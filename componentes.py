import sys
from collections import defaultdict, deque

def read_graph(path):
    edges = []
    vertices = set()

    with open(path, "r", encoding="utf8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue

            a, b = line.split()
            vertices.add(a)
            vertices.add(b)
            edges.append((a, b))

    return vertices, edges


def connected_components(vertices, edges):
    adj = defaultdict(set)
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    visited = set()
    components = []

    for v in vertices:
        if v in visited:
            continue

        comp = set()
        q = deque([v])
        visited.add(v)

        while q:
            x = q.popleft()
            comp.add(x)
            for y in adj[x]:
                if y not in visited:
                    visited.add(y)
                    q.append(y)

        components.append(comp)

    return components


def main():
    if len(sys.argv) != 2:
        print("Uso: python components_mcis.py <archivo_mcis.txt>")
        sys.exit(1)

    vertices, edges = read_graph(sys.argv[1])
    components = connected_components(vertices, edges)

    print(f"#vertices = {len(vertices)}")
    print(f"#edges    = {len(edges)}")
    print(f"#components = {len(components)}\n")

    for i, comp in enumerate(sorted(components, key=len, reverse=True)):
        print(f"Componente {i}: tamaño = {len(comp)}")
        # descomenta si quieres ver los vértices
        # for v in sorted(comp):
        #     print(" ", v)
        print()


if __name__ == "__main__":
    main()
