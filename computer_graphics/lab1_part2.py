import matplotlib.pyplot as plt
import random
import time 

def line_formula(x1, y1, x2, y2):
    points = []

 
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


plt.figure(figsize=(10, 8))

start_time = time.perf_counter()

for _ in range(100):


    x1 = random.randint(0, 100)
    y1 = random.randint(0, 100)

    x2 = random.randint(0, 100)
    y2 = random.randint(0, 100)

    line_pts = line_formula(x1, y1, x2, y2)

    x, y = zip(*line_pts)
    plt.plot(x, y)     
    
end_time = time.perf_counter()
print("time taken here in ms", abs(start_time-end_time)*1000)


plt.xlabel("X")
plt.ylabel("Y")
plt.grid(True)
plt.axis("equal")
plt.show()