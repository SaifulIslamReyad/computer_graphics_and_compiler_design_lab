import numpy as np
import matplotlib.pyplot as plt

LABELS = list("ABCDEFGH")
# A	0	0	0
# B	1	0	0
# C	1	1	0
# D	0	1	0
# E	0	1	1
# F	0	0	1
# G	1	0	1
# H	1	1	1

# The 12 edges of the cube
EDGES = [
    (0, 1), (1, 2), (2, 3), (3, 0),
    (4, 5), (5, 6), (6, 7), (7, 4),
    (0, 5), (1, 6), (2, 7), (3, 4)
]


def read_vertices():
    print("\nEnter coordinates for A, B, C, D, E, F, G, H.")
    print("Enter each vertex as: x y z")

    points = []

    for label in LABELS:
        while True:
            try:
                values = [
                    float(v)
                    for v in input(
                        f"{label} (x y z): "
                    ).replace(",", " ").split()
                ]

                if len(values) != 3:
                    raise ValueError

                points.append(values)
                break

            except ValueError:
                print("Enter exactly three numbers, e.g. 1 0 2.")

    return np.asarray(points, dtype=float)


# Perspective projection with a chosen vanishing point
def perspective(vertices, d, vp=(0.0, 0.0)):

    # Projection plane: z = 0
    # Center of projection: (vx, vy, -d)

    if d <= 0:
        raise ValueError("Perspective distance d must be positive.")

    vx, vy = vp
    x, y, z = vertices.T

    denominator = z + d

    if np.any(np.isclose(denominator, 0)):
        raise ValueError(
            "Projection is undefined when a vertex has z = -d."
        )

    # Perspective projection equations
    xp = vx + d * (x - vx) / denominator
    yp = vy + d * (y - vy) / denominator

    return np.column_stack((xp, yp))


# Select the projection method
def project(vertices, mode, d=10.0, vp=(0.0, 0.0)):

    x, y, z = vertices.T

    if mode == 1:
        # Chapter 7 standard perspective projection
        # Vanishing point = (0, 0)
        return perspective(vertices, d, (0.0, 0.0))

    elif mode == 2:
        # Orthographic parallel projection
        xp = x
        yp = y

        return np.column_stack((xp, yp))

    elif mode == 3:
        # Oblique cabinet projection
        theta = np.deg2rad(45)

        xp = x + 0.5 * z * np.cos(theta)
        yp = y + 0.5 * z * np.sin(theta)

        return np.column_stack((xp, yp))

    elif mode == 4:
        # One-point perspective with custom vanishing point
        return perspective(vertices, d, vp)

    else:
        raise ValueError("Invalid projection mode.")


# Draw the original 3D cube and its 2D projection
def draw(vertices, projected, title, vp=None, show_vp=False):

    fig = plt.figure(figsize=(12, 6))

    ax3 = fig.add_subplot(1, 2, 1, projection="3d")
    ax2 = fig.add_subplot(1, 2, 2)

    # Draw all edges in the original 3D cube
    for i, j in EDGES:

        ax3.plot(
            vertices[[i, j], 0],
            vertices[[i, j], 1],
            vertices[[i, j], 2],
            marker="o"
        )

        # Draw corresponding edges in 2D
        ax2.plot(
            projected[[i, j], 0],
            projected[[i, j], 1],
            marker="o"
        )

    # Label the vertices
    for i, label in enumerate(LABELS):

        ax3.text(
            vertices[i, 0],
            vertices[i, 1],
            vertices[i, 2],
            f" {label}"
        )

        ax2.annotate(
            label,
            projected[i],
            xytext=(4, 4),
            textcoords="offset points"
        )

    # Configure the 3D view
    ax3.set_title("Original 3D Wireframe")
    ax3.set_xlabel("X")
    ax3.set_ylabel("Y")
    ax3.set_zlabel("Z")

    mins = vertices.min(axis=0)
    maxs = vertices.max(axis=0)

    center = (mins + maxs) / 2
    half = max(float(np.max(maxs - mins)), 1.0) / 2

    ax3.set_xlim(center[0] - half, center[0] + half)
    ax3.set_ylim(center[1] - half, center[1] + half)
    ax3.set_zlim(center[2] - half, center[2] + half)

    ax3.set_box_aspect((1, 1, 1))

    # Show vanishing point and construction guides
    if show_vp and vp is not None:

        vx, vy = vp

        # Extend the projected depth edges toward the VP
        for i in (4, 5, 6, 7):

            ax2.plot(
                [projected[i, 0], vx],
                [projected[i, 1], vy],
                linestyle="--",
                linewidth=0.9
            )

        # Mark the vanishing point
        ax2.scatter(
            [vx],
            [vy],
            marker="x",
            s=85
        )

        ax2.annotate(
            f"VP ({vx:g}, {vy:g})",
            (vx, vy),
            xytext=(7, -12),
            textcoords="offset points"
        )

    # Configure the 2D projection
    ax2.set_title(title)
    ax2.set_xlabel("X'")
    ax2.set_ylabel("Y'")
    ax2.grid(True, alpha=0.3)
    ax2.set_aspect("equal", adjustable="box")

    lo = projected.min(axis=0)
    hi = projected.max(axis=0)

    if show_vp and vp is not None:
        lo = np.minimum(lo, vp)
        hi = np.maximum(hi, vp)

    padding = max(float(np.max(hi - lo)), 1.0) * 0.15

    ax2.set_xlim(lo[0] - padding, hi[0] + padding)
    ax2.set_ylim(lo[1] - padding, hi[1] + padding)

    fig.tight_layout()

    # Save the output image
    fig.savefig(
        "cube_projection_output.png",
        dpi=160,
        bbox_inches="tight"
    )

    print("\nSaved as cube_projection_output.png")

    plt.show()


def main():

    print("=" * 55)
    print("3D CUBE TO 2D PROJECTION - CHAPTER 7")
    print("=" * 55)

    print("1. Standard perspective projection")
    print("2. Orthographic parallel projection")
    print("3. Oblique cabinet projection")
    print("4. Perspective projection with custom vanishing point")

    # Select projection method
    while True:
        try:
            mode = int(input("Choose projection (1/2/3/4): "))

            if mode in (1, 2, 3, 4):
                break

        except ValueError:
            pass

        print("Please choose 1, 2, 3, or 4.")

    # Input the eight vertices
    vertices = read_vertices()

    vp = None
    show_vp = False

    if mode in (1, 4):

        while True:
            try:
                d = float(
                    input("Perspective distance d (default 10): ")
                    or "10"
                )

                if d <= 0:
                    print("d must be positive.")
                    continue

                if mode == 1:

                    # Chapter 7 standard vanishing point
                    vp = (0.0, 0.0)

                    projected = project(
                        vertices,
                        mode,
                        d=d
                    )

                    title = f"Standard Perspective (d={d:g})"

                else:

                    # Custom vanishing point input
                    raw = input(
                        "Enter vanishing point vx vy "
                        "(default 0.5 0.5): "
                    ).strip()

                    if raw:
                        values = [
                            float(v)
                            for v in raw.replace(",", " ").split()
                        ]
                    else:
                        values = [0.5, 0.5]

                    if len(values) != 2:
                        raise ValueError(
                            "Enter exactly two values for vx and vy."
                        )

                    vp = tuple(values)

                    projected = project(
                        vertices,
                        mode,
                        d=d,
                        vp=vp
                    )

                    title = f"One-Point Perspective (d={d:g})"

                show_vp = True
                break

            except ValueError as exc:
                print(f"Input error: {exc}")

    else:

        projected = project(vertices, mode)

        if mode == 2:
            title = "Orthographic Parallel Projection"
        else:
            title = "Oblique Cabinet Projection (45 degrees)"

    # Display the projected coordinates
    print("\nProjected 2D coordinates:")

    for label, point in zip(LABELS, projected):
        print(
            f"{label}' = "
            f"({point[0]:.4f}, {point[1]:.4f})"
        )

    # Display original cube and projected cube
    draw(
        vertices,
        projected,
        title,
        vp,
        show_vp
    )


if __name__ == "__main__":
    main()