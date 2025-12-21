#!/usr/bin/env python3
"""
Generador de instancias significativas para MCIS inducido.
Pensado para evaluación experimental (reporte/paper).
"""

import random
import os


class MCISGenerator:

    def __init__(self, seed=42):
        random.seed(seed)

    # ===================== PRIMITIVOS =====================

    def erdos_renyi(self, n, p):
        edges = []
        for i in range(n):
            for j in range(i + 1, n):
                if random.random() < p:
                    edges.append((i, j))
        return list(range(n)), edges

    def complete_graph(self, n):
        edges = [(i, j) for i in range(n) for j in range(i + 1, n)]
        return list(range(n)), edges

    def save_graph(self, filename, vertices, edges):
        with open(filename, "w") as f:
            f.write(f"# Grafo con {len(vertices)} vértices y {len(edges)} aristas\n")
            for u, v in edges:
                f.write(f"v{u} v{v}\n")

    # ===================== CASOS EXPERIMENTALES =====================

    def isomorphic_pair(self, n, p):
        verts, edges = self.erdos_renyi(n, p)

        perm = list(range(n))
        random.shuffle(perm)

        edges2 = []
        for u, v in edges:
            a, b = perm[u], perm[v]
            if a > b:
                a, b = b, a
            edges2.append((a, b))

        return verts, edges, list(range(n)), edges2

    def complete_vs_sparse(self, clique_size, sparse_size, p_sparse):
        v1, e1 = self.complete_graph(clique_size)
        v2, e2 = self.erdos_renyi(sparse_size, p_sparse)
        return v1, e1, v2, e2

    def common_core(self, core_size, extra1, extra2, p_core, p_noise):
        # Núcleo compartido (inducido)
        core_vertices, core_edges = self.erdos_renyi(core_size, p_core)

        # Ruido independiente
        _, e1_extra = self.erdos_renyi(extra1, p_noise)
        _, e2_extra = self.erdos_renyi(extra2, p_noise)

        # Reindexar ruido
        e1_extra = [(u + core_size, v + core_size) for u, v in e1_extra]
        e2_extra = [(u + core_size, v + core_size) for u, v in e2_extra]

        v1 = list(range(core_size + extra1))
        v2 = list(range(core_size + extra2))

        # ❗ NO conexiones núcleo–ruido → inducido garantizado
        e1 = core_edges + e1_extra
        e2 = core_edges + e2_extra

        return v1, e1, v2, e2


def generate_all(output_dir="instances_mcsi", seed=42):
    os.makedirs(output_dir, exist_ok=True)
    gen = MCISGenerator(seed)

    print("\nGenerando instancias MCIS significativas...\n")

    print("[1] Isomorfos grandes")
    v1, e1, v2, e2 = gen.isomorphic_pair(n=1000, p=0.4)
    gen.save_graph(f"{output_dir}/iso_large_g1.txt", v1, e1)
    gen.save_graph(f"{output_dir}/iso_large_g2.txt", v2, e2)

    print("[2] Completo vs casi plano")
    v1, e1, v2, e2 = gen.complete_vs_sparse(
        clique_size=25,
        sparse_size=60,
        p_sparse=0.05
    )
    gen.save_graph(f"{output_dir}/complete_g1.txt", v1, e1)
    gen.save_graph(f"{output_dir}/sparse_g2.txt", v2, e2)

    print("[3] Núcleo compartido + ruido")
    v1, e1, v2, e2 = gen.common_core(
        core_size=40,
        extra1=40,
        extra2=60,
        p_core=0.4,
        p_noise=0.1
    )
    gen.save_graph(f"{output_dir}/core_noise_g1.txt", v1, e1)
    gen.save_graph(f"{output_dir}/core_noise_g2.txt", v2, e2)

    print("\n✓ Instancias generadas en:", output_dir)
    print("✓ Casos: isomorfos, incompatibles, núcleo compartido\n")


if __name__ == "__main__":
    generate_all()