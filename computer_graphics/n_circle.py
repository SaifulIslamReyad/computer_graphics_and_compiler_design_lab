import matplotlib.pyplot as plt
import time


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



def bresenham_circle_shift(xc, yc, r):
    x = 0
    y = r

    d = 3 - (r << 1)

    points = []

    while x <= y:
        plot_circle_points(xc, yc, x, y, points)

        if d < 0:

            d = d + (x << 2) + 6

        else:

            d = d + ((x - y) << 2) + 10

            y -= 1

        x += 1

    return points



def midpoint_circle_shift(xc, yc, r):
    x = 0
    y = r

    p = 1 - r

    points = []

    while x <= y:
        plot_circle_points(xc, yc, x, y, points)

        x += 1

        if p < 0:

            p = p + (x << 1) + 1

        else:

            y -= 1

            p = p + ((x - y) << 1) + 1

    return points



N = 5000

bres_circles = []
mid_circles = []



start = time.perf_counter()

for i in range(N):

    xc = 250 + (i % 10) * 30
    yc = 250 + (i // 10) * 30
    r = 20 + (i % 50)

    points = bresenham_circle_shift(xc, yc, r)

    bres_circles.append(points)

bres_time = time.perf_counter() - start



start = time.perf_counter()

for i in range(N):

    xc = 250 + (i % 10) * 30
    yc = 250 + (i // 10) * 30
    r = 20 + (i % 50)

    points = midpoint_circle_shift(xc, yc, r)

    mid_circles.append(points)

mid_time = time.perf_counter() - start




print(f"Number of circles : {N}")

print(f"\nBresenham Time    : {bres_time:.8f} seconds")
print(f"Midpoint Time     : {mid_time:.8f} seconds")

# print(f"\nBresenham Average : {(bres_time / N) * 1e6:.4f} µs/circle")
# print(f"Midpoint Average  : {(mid_time / N) * 1e6:.4f} µs/circle")



plt.figure(figsize=(10, 10))

for points in bres_circles:

    x, y = zip(*points)

    plt.scatter(x, y, s=1)


plt.title(f"{N} Circles - Bresenham Algorithm (Bit Shift)")
plt.xlabel("X")
plt.ylabel("Y")
plt.axis("equal")
plt.grid(True)

plt.show()



plt.figure(figsize=(10, 10))

for points in mid_circles:

    x, y = zip(*points)

    plt.scatter(x, y, s=1)


plt.title(f"{N} Circles - Midpoint Algorithm (Bit Shift)")
plt.xlabel("X")
plt.ylabel("Y")
plt.axis("equal")
plt.grid(True)

plt.show()