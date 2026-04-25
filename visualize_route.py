"""
visualize_route.py
Visualizes the nearest-neighbor route-planning algorithm on a col x row grid.
Run: python visualize_route.py
You will be prompted for the same filenames as planRoute.
"""

import math
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import matplotlib.patheffects as pe

# ── data helpers ────────────────────────────────────────────────────────────

def fix_corruption(raw):
    s = raw
    while "XX" in s:
        s = s.replace("XX", "", 1)
    return s.replace("_", " ")


def read_locations(filename):
    with open(filename) as f:
        lines = [l.strip() for l in f if l.strip()]
    num_rows, num_cols = map(int, lines[0].split())
    start_r, start_c  = map(int, lines[1].split())
    end_r,   end_c    = map(int, lines[2].split())
    planets = []
    skipped = []
    for line in lines[3:]:
        parts = line.split()
        r, c, sym, pid = int(parts[0]), int(parts[1]), parts[2], int(parts[3])
        if r < 1 or r > num_rows or c < 1 or c > num_cols:
            skipped.append(pid)
        else:
            planets.append({"row": r, "col": c, "symbol": sym,
                            "id": pid, "name": "", "visited": False})
    return num_rows, num_cols, start_r, start_c, end_r, end_c, planets, skipped


def read_names(filename, planets):
    with open(filename) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split()
            pid, raw = int(parts[0]), parts[1]
            for p in planets:
                if p["id"] == pid:
                    p["name"] = fix_corruption(raw)


def dist_sq(r1, c1, r2, c2):
    return (r2 - r1) ** 2 + (c2 - c1) ** 2


def nearest_neighbor(start_r, start_c, end_r, end_c, planets):
    """Returns ordered list of (row, col, label) visit stops including S and E."""
    route = [(start_r, start_c, "START")]
    cur_r, cur_c = start_r, start_c

    # Build visit order step by step so we can record all candidates at each step
    steps = []   # each entry: list of (planet_index, dist_sq) sorted by dist

    while True:
        unvisited = [(i, p) for i, p in enumerate(planets) if not p["visited"]]
        if not unvisited:
            break
        candidates = sorted(unvisited,
                             key=lambda ip: (dist_sq(cur_r, cur_c, ip[1]["row"], ip[1]["col"]),
                                             ip[1]["id"]))
        steps.append({"from": (cur_r, cur_c), "candidates": candidates})
        best_i, best_p = candidates[0]
        planets[best_i]["visited"] = True
        cur_r, cur_c = best_p["row"], best_p["col"]
        route.append((cur_r, cur_c, best_p["name"]))

    route.append((end_r, end_c, "END"))
    return route, steps

# ── plotting ─────────────────────────────────────────────────────────────────

def plot_route(num_rows, num_cols, start_r, start_c, end_r, end_c,
               planets, route, steps):

    fig, ax = plt.subplots(figsize=(max(8, num_cols * 0.7),
                                    max(8, num_rows * 0.7)))

    # Grid lines
    for r in range(1, num_rows + 1):
        ax.axhline(r, color="#e0e0e0", lw=0.5, zorder=0)
    for c in range(1, num_cols + 1):
        ax.axvline(c, color="#e0e0e0", lw=0.5, zorder=0)

    # Draw faint "candidate search" lines (all distances considered at each step)
    for step in steps:
        fr, fc = step["from"]
        for rank, (_, cand) in enumerate(step["candidates"]):
            alpha = 0.08 if rank > 0 else 0.0   # chosen path drawn separately
            ax.plot([fc, cand["col"]], [fr, cand["row"]],
                    color="gray", lw=0.8, alpha=alpha, zorder=1, linestyle="--")

    # Draw route arrows
    arrow_props = dict(arrowstyle="-|>", color="#1f77b4",
                       lw=2, mutation_scale=18)
    for i in range(len(route) - 1):
        r1, c1, _ = route[i]
        r2, c2, _ = route[i + 1]
        ax.annotate("",
                    xy=(c2, r2), xytext=(c1, r1),
                    arrowprops=dict(arrowstyle="-|>", color="#1f77b4",
                                    lw=2, mutation_scale=16),
                    zorder=3)
        # Step number at midpoint
        mx, my = (c1 + c2) / 2, (r1 + r2) / 2
        ax.text(mx, my, str(i + 1), fontsize=7, color="#1f77b4",
                ha="center", va="center", zorder=4,
                path_effects=[pe.withStroke(linewidth=2, foreground="white")])

    # Planet markers
    for p in planets:
        ax.scatter(p["col"], p["row"], s=220, color="#ff7f0e",
                   edgecolors="black", lw=1.2, zorder=5)
        ax.text(p["col"], p["row"], p["symbol"],
                ha="center", va="center", fontsize=9,
                fontweight="bold", color="white", zorder=6)
        # Name label (offset so it doesn't overlap marker)
        ax.text(p["col"] + 0.15, p["row"] - 0.35,
                f'{p["name"]}\n(ID {p["id"]})',
                fontsize=6.5, color="#333333", zorder=6,
                path_effects=[pe.withStroke(linewidth=1.5, foreground="white")])

    # Start / End markers
    for (r, c, lbl, color) in [(start_r, start_c, "S", "#2ca02c"),
                                (end_r,   end_c,   "E", "#d62728")]:
        ax.scatter(c, r, s=320, color=color, edgecolors="black",
                   lw=1.5, zorder=5, marker="*")
        ax.text(c, r, lbl, ha="center", va="center",
                fontsize=9, fontweight="bold", color="white", zorder=6)
        ax.text(c + 0.15, r - 0.35, lbl, fontsize=7,
                color=color, fontweight="bold", zorder=6,
                path_effects=[pe.withStroke(linewidth=1.5, foreground="white")])

    # Axes formatting  (row 1 at top, matching the spec)
    ax.set_xlim(0.5, num_cols + 0.5)
    ax.set_ylim(num_rows + 0.5, 0.5)   # invert so row 1 is at top
    ax.set_xticks(range(1, num_cols + 1))
    ax.set_yticks(range(1, num_rows + 1))
    ax.set_xlabel("Column", fontsize=10)
    ax.set_ylabel("Row", fontsize=10)
    ax.set_title("Nearest-Neighbor Route Visualization", fontsize=13, fontweight="bold")

    # Legend
    handles = [
        mpatches.Patch(color="#2ca02c", label="Start (S)"),
        mpatches.Patch(color="#d62728", label="End (E)"),
        mpatches.Patch(color="#ff7f0e", label="Planet"),
        mpatches.Patch(color="#1f77b4", label="Chosen path (arrow = step order)"),
        mpatches.Patch(color="gray",    label="Rejected candidates (dashed)", alpha=0.5),
    ]
    ax.legend(handles=handles, loc="upper right", fontsize=8,
              framealpha=0.9)

    plt.tight_layout()
    plt.show()

# ── main ─────────────────────────────────────────────────────────────────────

if __name__ == "__main__":
    loc_file   = input("Enter Locations Filename: ").strip()
    names_file = input("Enter Names Filename: ").strip()

    num_rows, num_cols, sr, sc, er, ec, planets, skipped = read_locations(loc_file)
    for pid in skipped:
        print(f"{pid} out of range - ignoring")

    read_names(names_file, planets)

    route, steps = nearest_neighbor(sr, sc, er, ec, planets)

    print("\nRoute order:")
    for i, (r, c, lbl) in enumerate(route):
        print(f"  {i:2d}. ({r:2d}, {c:2d})  {lbl}")

    plot_route(num_rows, num_cols, sr, sc, er, ec, planets, route, steps)
