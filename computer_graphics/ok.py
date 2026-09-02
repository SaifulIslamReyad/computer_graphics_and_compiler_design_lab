import matplotlib.pyplot as plt
import random
import time


def line_formula(x1, y1, x2, y2):
    points = []

    # Handle vertical line
    if x1 == x2:
        for y in range(min(y1, y2), max(y1, y2) + 1):
            points.append((x1, y))
        return points

    m = (y2 - y1) / (x2 - x1)
    c = y1 - m * x1

    for x in range(min(x1, x2), max(x1, x2) + 1):
        y = c + m * x
        points.append((x, y))

    return points


# Start computational time
start_time = time.perf_counter()

plt.figure(figsize=(10, 8))

# Draw 100 random lines
for _ in range(1000):

    x1 = random.randint(0, 1000)
    y1 = random.randint(0, 1000)

    x2 = random.randint(0, 1000)
    y2 = random.randint(0, 1000)

    line_pts = line_formula(x1, y1, x2, y2)

    x, y = zip(*line_pts)

    plt.scatter(x, y, s=5)


plt.title("100 Random Lines")
plt.xlabel("X")
plt.ylabel("Y")
plt.grid(True)
plt.axis("equal")

# End computational time
end_time = time.perf_counter()

# Calculate execution time
computational_time = end_time - start_time

print("Number of lines:", 1000)
print("Computational time:", computational_time, "seconds")

plt.show()