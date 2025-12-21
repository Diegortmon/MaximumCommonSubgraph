# Maximum Common Induced Subgraph - Ant Colony Optimization

Implementación de Ant Colony Optimization (ACO) para encontrar el Maximum Common Induced Subgraph (MCIS) entre dos gráficas no dirigidas.

## Características

- ✅ ACO optimizado para MCIS inducido
- ✅ Paralelización con OpenMP
- ✅ Exportación de resultados en formato `.mcis`
- ✅ Visualización SVG de gráficas y solución
- ✅ Parámetros configurables (α, β, ρ, hormigas, iteraciones)
- ✅ Scripts de validación y generación de instancias

## Requisitos

- C++17 o superior
- OpenMP (opcional, para paralelización)
- Meson + Ninja (para compilación) o g++

## Compilación

### Opción 1: Con Meson (recomendado)
```bash
meson setup builddir
meson compile -C builddir
```

### Opción 2: Con g++ directo
```bash
g++ -std=c++17 -O3 -fopenmp \
    src/main.cpp src/aco.cpp src/ant.cpp src/graph.cpp \
    src/mapping.cpp src/pheromone.cpp src/reader.cpp src/graphy.cpp \
    -o mcs_aco
```

## Uso

### Sintaxis básica
```bash
./mcs_aco <grafo1.txt> <grafo2.txt> --seed <n> [opciones]
```

### Ejemplos

**Ejecución básica:**
```bash
./mcs_aco grafo1.txt grafo2.txt --seed 42
```

**Con visualizaciones:**
```bash
./mcs_aco grafo1.txt grafo2.txt --seed 42 --output resultado
```
Genera: `resultado.mcis`, `resultado_g1.svg`, `resultado_g2.svg`, `resultado_solucion.svg`

**Parámetros personalizados:**
```bash
./mcs_aco g1.txt g2.txt --seed 123 \
    --ants 50 \
    --iterations 200 \
    --alpha 1.5 \
    --beta 4.0 \
    --rho 0.15 \
    --output experimento
```

### Parámetros

| Parámetro | Descripción | Default | Rango recomendado |
|-----------|-------------|---------|-------------------|
| `--seed` | Semilla aleatoria (OBLIGATORIO) | - | Cualquier entero |
| `--ants` | Número de hormigas | 20 | 10-100 |
| `--iterations` | Iteraciones máximas | 100 | 50-500 |
| `--alpha` | Peso de feromona (τ) | 1.0 | 0.5-3.0 |
| `--beta` | Peso de heurística (η) | 3.0 | 1.0-5.0 |
| `--rho` | Tasa de evaporación | 0.1 | 0.05-0.5 |
| `--output` | Nombre base para archivos de salida | - | - |

## Formato de entrada

Archivo de texto con una arista por línea:

```text
# Comentario (opcional)
verticeA verticeB
verticeC verticeD
verticeA verticeC
...
```

**Ejemplo** (`karate.txt`):
```text
John Mr_Hi
John Actor
Mr_Hi Officer
Actor Officer
```

## Archivos de salida

### Sin `--output`
- `resultado.mcis`: Grafo MCIS en formato texto

### Con `--output nombre`
- `nombre.mcis`: Grafo MCIS en formato texto
- `nombre_g1.svg`: Visualización completa de G1
- `nombre_g2.svg`: Visualización completa de G2
- `nombre_solucion.svg`: Visualización del MCIS encontrado

### Formato `.mcis`
```text
# Maximum Common Induced Subgraph (MCIS)
# Vertices: 28
# Edges: 78

# === VERTEX MAPPING ===
# 0: Alice|Ana
# 1: Bob|Bruno
...

# === EDGES ===
Alice|Ana Bob|Bruno
Alice|Ana Carol|Carla
...
```

## Algoritmo

El algoritmo implementa:

1. **Construcción de soluciones**: Cada hormiga construye un mapeo factible usando probabilidades ACO
2. **Heurística**: Basada en grados similares, aristas preservadas y conflictos
3. **Actualización de feromonas**: Refuerzo proporcional a la calidad + élite
4. **Reparación**: Garantiza MCIS inducido válido
5. **Optimización**: Pool sampling reducido (500 candidatos vs 40k)

## Ejemplo de ejecución

```bash
$ ./mcs_aco karate.txt zachary.txt --seed 42 --output karate_mcis

Leyendo grafos...
   G1: 34 vértices (desde karate.txt)
   G2: 34 vértices (desde zachary.txt)

═══════════════════════════════════════════════════════════
 PARÁMETROS ACO
═══════════════════════════════════════════════════════════
   Hormigas:     20
   Iteraciones:  100
   Alpha (τ):    1.00
   Beta (η):     3.00
   Rho (evap):   0.10
   Semilla:      42 ⭐
═══════════════════════════════════════════════════════════

Ejecutando ACO para MCIS...

Iter     0 | best_size = 28 | best_edges = 78
Iter    10 | best_size = 28 | best_edges = 78
...
Iter   100 | best_size = 28 | best_edges = 78

Final: 28 vertices, 78 edges

═══════════════════════════════════════════════════════════
 RESULTADO FINAL
═══════════════════════════════════════════════════════════

Estadísticas:
   Nodos mapeados: 28
   Aristas:        78
   Factible:       ✓ (MCIS inducido válido)

[Export] MCIS guardado en: karate_mcis.mcis
         Vértices: 28, Aristas: 78

[Graphy] Subgrafo MCIS guardado en: karate_mcis_g1.svg (34 vértices)
[Graphy] Subgrafo MCIS guardado en: karate_mcis_g2.svg (34 vértices)
[Graphy] Subgrafo MCIS guardado en: karate_mcis_solucion.svg (28 vértices)
```

## Estructura del proyecto

```
.
├── include/
│   ├── aco.hpp
│   ├── ant.hpp
│   ├── graph.hpp
│   ├── graphy.hpp
│   ├── mapping.hpp
│   ├── pheromone.hpp
│   └── reader.hpp
├── src/
│   ├── main.cpp
│   ├── aco.cpp
│   ├── ant.cpp
│   ├── graph.cpp
│   ├── graphy.cpp
│   ├── mapping.cpp
│   ├── pheromone.cpp
│   └── reader.cpp
├── scripts/
│   ├── generador.py       # Generador de instancias de prueba
│   ├── check.py           # Validador de soluciones MCIS
│   └── componentes.py     # Análisis de componentes conexas
├── meson.build
└── README.md
```

## Notas técnicas

- **Complejidad**: O(iteraciones × hormigas × K × |mapping|) donde K=500
- **Memoria**: O(n₁ × n₂) para matriz de feromonas
- **Paralelización**: Construcción de soluciones en paralelo con OpenMP
- **Optimización**: Pool sampling reduce tiempo ~98% vs versión naive

---

## Scripts auxiliares

El proyecto incluye tres scripts de Python para validación y experimentación:

### 1. `generador.py` - Generador de instancias

Genera casos de prueba controlados para evaluar el algoritmo.

**Uso:**
```bash
python scripts/generador.py
```

Genera automáticamente en `instances_mcsi/`:
- `iso_large_g1.txt`, `iso_large_g2.txt`: Gráficas isomorfas grandes (MCIS = 100%)
- `complete_g1.txt`, `sparse_g2.txt`: Completa vs dispersa (MCIS pequeño)
- `core_noise_g1.txt`, `core_noise_g2.txt`: Núcleo compartido + ruido

**Modificar dificultad de instancias:**

Edita los parámetros en `generate_all()`:

```python
# Caso 1: Gráficas isomorfas
v1, e1, v2, e2 = gen.isomorphic_pair(
    n=1000,    # ← Tamaño (vértices)
    p=0.4      # ← Densidad de aristas (0.0-1.0)
)
# Dificultad: Mayor n → más lento, mayor p → más denso

# Caso 2: Completa vs dispersa
v1, e1, v2, e2 = gen.complete_vs_sparse(
    clique_size=25,   # ← Tamaño de la clique completa
    sparse_size=60,   # ← Tamaño de gráfica dispersa
    p_sparse=0.05     # ← Densidad de la dispersa (más bajo = más difícil)
)
# Dificultad: p_sparse bajo → MCIS muy pequeño

# Caso 3: Núcleo compartido con ruido
v1, e1, v2, e2 = gen.common_core(
    core_size=40,   # ← Tamaño del MCIS esperado
    extra1=40,      # ← Vértices extra en G1
    extra2=60,      # ← Vértices extra en G2
    p_core=0.4,     # ← Densidad del núcleo
    p_noise=0.1     # ← Densidad del ruido (más alto = más confusión)
)
# Dificultad: Mayor extra1/extra2 → más búsqueda
#            Mayor p_noise → más falsos positivos
```

**Recomendaciones de dificultad:**

| Nivel | n | p | extra | p_noise | Tiempo estimado |
|-------|---|---|-------|---------|-----------------|
| Fácil | 50-100 | 0.3-0.5 | 10-20 | 0.05-0.1 | < 10s |
| Medio | 200-500 | 0.3-0.4 | 30-50 | 0.1-0.2 | 10-60s |
| Difícil | 500-1000 | 0.2-0.5 | 50-100 | 0.2-0.3 | 1-5min |
| Extremo | 1000+ | 0.4-0.6 | 100+ | 0.3+ | > 5min |

### 2. `check.py` - Validador de soluciones

Verifica que una solución sea un MCIS inducido válido.

**Uso:**
```bash
python scripts/check.py grafica1.txt grafica2.txt salida.txt
```

Valida:
- ✓ Inyectividad del mapeo
- ✓ Vértices existen en ambas gráficas
- ✓ Condición inducida: (u₁,u₂)∈E₁ ⟺ (v₁,v₂)∈E₂

**Ejemplo:**
```bash
# Generar instancias
python scripts/generador.py

# Resolver
./mcs_aco instances_mcsi/core_noise_g1.txt instances_mcsi/core_noise_g2.txt \
    --seed 42 --output resultado

# Validar
python scripts/check.py \
    instances_mcsi/core_noise_g1.txt \
    instances_mcsi/core_noise_g2.txt \
    resultado.txt
```

**Salida esperada:**
```
✅ La solución ES un MCIS inducido válido
✓ Tamaño del MCIS: 40
```

### 3. `componentes.py` - Análisis de componentes

Analiza la estructura de componentes conexas de una gráfica o solución MCIS.

**Uso:**
```bash
python scripts/componentes.py archivo.txt
```

**Ejemplo:**
```bash
python scripts/componentes.py resultado.mcis
```

**Salida:**
```
#vertices = 40
#edges    = 156
#components = 1

Componente 0: tamaño = 40
```

Útil para verificar:
- Si el MCIS es conexo (1 componente) o fragmentado (>1 componentes)
- Distribución de tamaños de componentes
- Calidad de la solución encontrada

---

## Flujo de trabajo experimental

### 1. Generar instancias de prueba
```bash
python scripts/generador.py
```

### 2. Ejecutar ACO con diferentes configuraciones
```bash
# Configuración conservadora
./mcs_aco instances_mcsi/core_noise_g1.txt instances_mcsi/core_noise_g2.txt \
    --seed 42 --ants 20 --iterations 100 --output exp1

# Configuración agresiva
./mcs_aco instances_mcsi/core_noise_g1.txt instances_mcsi/core_noise_g2.txt \
    --seed 42 --ants 50 --iterations 200 --alpha 1.5 --beta 4.0 --output exp2
```

### 3. Validar y analizar resultados
```bash
# Validar correctitud
python scripts/check.py \
    instances_mcsi/core_noise_g1.txt \
    instances_mcsi/core_noise_g2.txt \
    exp1.txt

# Analizar estructura
python scripts/componentes.py exp1.mcis
```

### 4. Comparar visualizaciones
Abre los archivos SVG generados:
- `exp1_g1.svg`: Gráfica G1 completa
- `exp1_g2.svg`: Gráfica G2 completa
- `exp1_solucion.svg`: MCIS encontrado (con mapeo combinado)

## Autores

Diego Iain Ortiz Montiel
Facultad de ciecias

## Referencias

- Dorigo, M., & Stützle, T. (2004). Ant Colony Optimization. MIT Press.
- [Otras referencias relevantes]