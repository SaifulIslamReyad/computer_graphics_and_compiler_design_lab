import matplotlib.pyplot as plt
import random
import time


def dda_round_m(x1, y1, x2, y2):

    dx = x2 - x1
    dy = y2 - y1

    x_points = []
    y_points = []

    if dx == 0:

        if y1 > y2:
            y1, y2 = y2, y1

        y = y1

        while y <= y2:
            x_points.append(round(x1))
            y_points.append(round(y))

            y += 1

        return x_points, y_points

    m = dy / dx
    m = round(m)

    # |m| <= 1
    if abs(dy) <= abs(dx):

        if x1 > x2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1

        x = x1
        y = y1

        while x <= x2:

            x_points.append(round(x))
            y_points.append(round(y))

            x += 1
            y += m

    # |m| > 1
    else:

        if y1 > y2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1

        dx = x2 - x1
        dy = y2 - y1

        m = dx / dy

        m = round(m)

        x = x1
        y = y1

        while y <= y2:

            x_points.append(round(x))
            y_points.append(round(y))

            y += 1
            x += m

    return x_points, y_points


def dda_keep_float(x1, y1, x2, y2):

    dx = x2 - x1
    dy = y2 - y1

    x_points = []
    y_points = []

    if dx == 0:

        if y1 > y2:
            y1, y2 = y2, y1

        y = y1

        while y <= y2:

            x_points.append(round(x1))
            y_points.append(round(y))

            y += 1

        return x_points, y_points

    # |m| <= 1
    if abs(dy) <= abs(dx):

        if x1 > x2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1

        dx = x2 - x1
        dy = y2 - y1

        m = dy / dx

        x = x1
        y = float(y1)

        while x <= x2:


            x_points.append(round(x))
            y_points.append(round(y))

            x += 1
            y += m

    # |m| > 1
    else:

        if y1 > y2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1

        dx = x2 - x1
        dy = y2 - y1


        m = dx / dy

        x = float(x1)
        y = y1

        while y <= y2:

            x_points.append(round(x))
            y_points.append(round(y))

            y += 1

           
            x += m

    return x_points, y_points



nl = 100

w = 600
h = 600

lines = []

for i in range(nl):

    # Starting point
    x1 = random.randint(0, w)
    y1 = random.randint(0, h)

    # Ending point
    x2 = random.randint(0, w)
    y2 = random.randint(0, h)

    lines.append((x1, y1, x2, y2))





all_lines = []

total_pixels = 0

start_time = time.perf_counter()


for x1, y1, x2, y2 in lines:

    x_points, y_points = dda_keep_float(
        x1, y1,
        x2, y2
    )

    all_lines.append(
        (x_points, y_points)
    )

    total_pixels += len(x_points)


end_time = time.perf_counter()



dda_time = end_time - start_time

print(f"Number of lines       : {nl}")
print(f"Total pixels generated: {total_pixels}")
print(f"DDA execution time    : {dda_time:.6f} seconds")


print(
    f"Average time per line : "
    f"{(dda_time / nl) * 1000:.6f} ms"
)



plt.figure(figsize=(10, 10))

for x_points, y_points in all_lines:

    plt.scatter(
        x_points,
        y_points,
        s=1
    )


plt.xlim(0, w)
plt.ylim(0, h)

plt.xlabel("X-axis")
plt.ylabel("Y-axis")


plt.grid(True)

plt.axis("equal")


plt.show()

