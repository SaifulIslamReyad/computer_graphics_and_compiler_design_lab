import matplotlib.pyplot as plt




def line_formula(x1, y1, x2, y2):
    points = []
    if x1 == x2:
        for y in range(min(y1, y2), max(y1, y2) + 1):
            points.append((x1, y))
        return points
    m = (y2 - y1) / (x2 - x1)
    c= y1- m* x1
    for x in range(min(x1, x2), max(x1, x2)+1):
        y = c + m * (x)
        points.append((x, y))
    return points

line_pts = line_formula(10, 10, 80, 80)


plt.figure(figsize=(10, 5))

x, y = zip(*line_pts)
plt.scatter(x, y)

plt.xlabel("X")
plt.ylabel("Y")
plt.axis("equal")
plt.show()