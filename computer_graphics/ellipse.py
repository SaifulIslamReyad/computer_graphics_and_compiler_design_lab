import matplotlib.pyplot as plt


def plot_ellipse_points(xc, yc, x, y, points):
    points.extend([
        (xc + x, yc + y),
        (xc - x, yc + y),
        (xc + x, yc - y),
        (xc - x, yc - y)
    ])


def midpoint_ellipse(xc, yc, rx, ry):
    points = []

    x = 0
    y = ry

    rx2 = rx * rx
    ry2 = ry * ry

    p1 = ry2 - (rx2 * ry) + (0.25 * rx2)

    dx = 2 * ry2 * x
    dy = 2 * rx2 * y

    while dx < dy:

        plot_ellipse_points(xc, yc, x, y, points)

        x += 1
        dx = dx + 2 * ry2

        if p1 < 0:
            p1 = p1 + dx + ry2
        else:
            y -= 1
            dy = dy - 2 * rx2
            p1 = p1 + dx - dy + ry2

    p2 = (
        ry2 * (x + 0.5) * (x + 0.5)
        + rx2 * (y - 1) * (y - 1)
        - rx2 * ry2
    )

    while y >= 0:

        plot_ellipse_points(xc, yc, x, y, points)

        y -= 1
        dy = dy - 2 * rx2

        if p2 > 0:
            p2 = p2 + rx2 - dy
        else:
            x += 1
            dx = dx + 2 * ry2
            p2 = p2 + dx - dy + rx2

    return points



ellipses = [
    (150, 150, 80, 50),
    (350, 150, 100, 60),
    (550, 150, 70, 100),
    (200, 400, 120, 70),
    (500, 400, 90, 120)
]


plt.figure(figsize=(10, 8))

for xc, yc, rx, ry in ellipses:

    points = midpoint_ellipse(xc, yc, rx, ry)

    x, y = zip(*points)

    plt.scatter(x, y, s=5)


plt.title("5 Ellipses - Midpoint Ellipse Drawing Algorithm")

plt.xlabel("X")
plt.ylabel("Y")

plt.axis("equal")
plt.grid(True)

plt.show()