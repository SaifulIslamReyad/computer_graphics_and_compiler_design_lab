import matplotlib.pyplot as plt



def plot_circle_points(xc, yc, x, y, points):
    points.extend([
        (xc + x, yc + y),
        (xc - x, yc + y),
        (xc + x, yc - y),
        (xc - x, yc - y),
        (xc + y, yc + x),
        (xc - y, yc + x),
        (xc + y, yc - x),
        (xc - y, yc - x)
    ])



def bresenham_circle(xc, yc, r):
    x = 0
    y = r
    d = 3 - 2 * r

    points = []

    while x <= y:
        plot_circle_points(xc, yc, x, y, points)

        if d < 0:
            d = d + 4 * x + 6
        else:
            d = d + 4 * (x - y) + 10
            y -= 1

        x += 1

    return points



def midpoint_circle(xc, yc, r):
    x = 0
    y = r
    p = 1 - r

    points = []

    while x <= y:
        plot_circle_points(xc, yc, x, y, points)

        x += 1

        if p < 0:
            p = p + 2 * x + 1
        else:
            y -= 1
            p = p + 2 * (x - y) + 1

    return points


xc = 250
yc = 250
r = 100

bres_points = bresenham_circle(xc, yc, r)
mid_points = midpoint_circle(xc, yc, r)


plt.figure(figsize=(8, 8))



x2, y2 = zip(*mid_points)
plt.scatter(x2, y2, s=5, marker="x", label="Midpoint")

x1, y1 = zip(*bres_points)
plt.scatter(x1, y1, s=5, label="Bresenham")

plt.title("Bresenham vs Midpoint Circle Algorithm")
plt.xlabel("X")
plt.ylabel("Y")

plt.axis("equal")
plt.grid(True)
plt.legend()

plt.show()



