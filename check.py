    #!/usr/bin/env python3
import sys
from collections import defaultdict

def read_graph(path):
    edges = set()
    vertices = set()
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            u, v = line.split()
            vertices.add(u)
            vertices.add(v)
            if u > v:
                u, v = v, u
            edges.add((u, v))
    return vertices, edges

def read_mapping(path):
    mapping = {}
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or "→" not in line:
                continue
            u, v = line.split("→")
            u = u.strip()
            v = v.strip()
            mapping[u] = v
    return mapping

def check_mcis(g1_v, g1_e, g2_v, g2_e, mapping):
    # 1. Inyectividad
    if len(set(mapping.values())) != len(mapping):
        return False, "❌ El mapeo no es inyectivo (dos vértices van al mismo)"

    # 2. Todos los vértices existen
    for u, v in mapping.items():
        if u not in g1_v:
            return False, f"❌ {u} no está en G1"
        if v not in g2_v:
            return False, f"❌ {v} no está en G2"

    mapped = list(mapping.items())

    # 3. Condición inducida
    for i in range(len(mapped)):
        u1, v1 = mapped[i]
        for j in range(i + 1, len(mapped)):
            u2, v2 = mapped[j]

            e1 = (u1, u2) if u1 < u2 else (u2, u1)
            e2 = (v1, v2) if v1 < v2 else (v2, v1)

            in_g1 = e1 in g1_e
            in_g2 = e2 in g2_e

            if in_g1 != in_g2:
                return False, (
                    "❌ Violación inducida:\n"
                    f"   G1: {e1} = {in_g1}\n"
                    f"   G2: {e2} = {in_g2}"
                )

    return True, "✅ La solución ES un MCIS inducido válido"

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Uso:")
        print("  python check_mcis.py instances_mcsi/core_noise_g1.txt instances_mcsi/core_noise_g2.txt salida.txt")
        sys.exit(1)

    g1_path, g2_path, sol_path = sys.argv[1:]

    g1_v, g1_e = read_graph(g1_path)
    g2_v, g2_e = read_graph(g2_path)
    mapping = read_mapping(sol_path)

    ok, msg = check_mcis(g1_v, g1_e, g2_v, g2_e, mapping)
    print(msg)
    if ok:
        print(f"✔ Tamaño del MCIS: {len(mapping)}")
