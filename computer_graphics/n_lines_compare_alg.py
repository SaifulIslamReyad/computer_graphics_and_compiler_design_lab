import matplotlib.pyplot as plt
import time
import random


def dda_keep_float(x1, y1, x2, y2):

    dx = x2 - x1
    dy = y2 - y1

    x_points = []
    y_points = []

    xp_append = x_points.append
    yp_append = y_points.append

    
    if abs(dy) <= abs(dx):

        if x1 > x2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1

        dx = x2 - x1
        dy = y2 - y1

        m = dy / dx
        steps = dx

        y0 = float(y1)

        for i in range(steps + 1):
            xp_append(round(x1 + i))
            yp_append(round(y0 + i * m))

    else:

        if y1 > y2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1

        dx = x2 - x1
        dy = y2 - y1

        m = dx / dy
        steps = dy

        x0 = float(x1)

        for i in range(steps + 1):
            xp_append(round(x0 + i * m))
            yp_append(round(y1 + i))

    return x_points, y_points


def dda_round_m(x1, y1, x2, y2):

    dx = x2 - x1
    dy = y2 - y1

    x_points = []
    y_points = []

    xp_append = x_points.append
    yp_append = y_points.append

    

    if abs(dy) <= abs(dx):

        if x1 > x2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1

        dx = x2 - x1
        dy = y2 - y1

        m = round(dy / dx)
        steps = dx

        y0 = float(y1)

        for i in range(steps + 1):
            xp_append(round(x1 + i))
            yp_append(round(y0 + i * m))

    else:

        if y1 > y2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1

        dx = x2 - x1
        dy = y2 - y1

        m = round(dx / dy)
        steps = dy

        x0 = float(x1)

        for i in range(steps + 1):
            xp_append(round(x0 + i * m))
            yp_append(y1 + i)

    return x_points, y_points

def line_formula(x1, y1, x2, y2):
 
    x_points = []
    y_points = []
 
    dx = x2 - x1
    dy = y2 - y1
 
    
 
    # x1_orig, y1_orig, x2_orig, y2_orig = x1, y1, x2, y2
 
    if abs(dy) <= abs(dx):
 
        if x1 > x2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1
 
        x = x1
 
        while x <= x2:
 
            dx = x2 - x1
            dy = y2 - y1
            m = dy / dx
            c = y1 - m * x1
 
            y = m * x + c
 
            x_points.append(x)
            y_points.append(round(y))
 
            x += 1
 
    else:
 
        if y1 > y2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1
 
        y = y1
 
        while y <= y2:
 
            dx = x2 - x1
            dy = y2 - y1
            m = dx / dy
            c = x1 - m * y1
 
            x = m * y + c
 
            x_points.append(round(x))
            y_points.append(y)
 
            y += 1
 
    return x_points, y_points




def bresenham_line(x1, y1, x2, y2):

    x_points = []
    y_points = []

    dx = x2 - x1
    dy = y2 - y1

    

    steep = abs(dy) > abs(dx)

    if steep:

        x1, y1 = y1, x1
        x2, y2 = y2, x2

        dx = x2 - x1
        dy = y2 - y1

    if x1 > x2:

        x1, x2 = x2, x1
        y1, y2 = y2, y1

    dx = x2 - x1
    dy = y2 - y1

    y_direction = 1

    if dy < 0:

        y_direction = -1
        dy = -dy

    x = x1
    y = y1

    d = 2 * dy - dx

    dS = 2 * dy
    dT = 2 * (dy - dx)

    if steep:

        x_points.append(y)
        y_points.append(x)

    else:

        x_points.append(x)
        y_points.append(y)

    while x < x2:

        x = x + 1

        if d < 0:

            d = d + dS

        else:

            y = y + y_direction

            d = d + dT

        if steep:

            x_points.append(y)
            y_points.append(x)

        else:

            x_points.append(x)
            y_points.append(y)

    return x_points, y_points


number_of_lines = int(input("Enter number of lines: "))

coordinate_limit = 500

lines = []

for i in range(number_of_lines):

    x1 = random.randint(0, coordinate_limit)
    y1 = random.randint(0, coordinate_limit)

    x2 = random.randint(0, coordinate_limit)
    y2 = random.randint(0, coordinate_limit)

    lines.append((x1, y1, x2, y2))


round_m_results = []
float_results = []
formula_results = []
bresenham_results = []


round_m_total_time = 0
float_total_time = 0
formula_total_time = 0
bresenham_total_time = 0


for x1, y1, x2, y2 in lines:

    start_time = time.perf_counter()

    round_m_x, round_m_y = dda_round_m(
        x1, y1, x2, y2
    )

    end_time = time.perf_counter()

    round_m_total_time += end_time - start_time

    round_m_results.append(
        (round_m_x, round_m_y)
    )


    start_time = time.perf_counter()

    float_x, float_y = dda_keep_float(
        x1, y1, x2, y2
    )

    end_time = time.perf_counter()

    float_total_time += end_time - start_time

    float_results.append(
        (float_x, float_y)
    )


    start_time = time.perf_counter()

    formula_x, formula_y = line_formula(
        x1, y1, x2, y2
    )

    end_time = time.perf_counter()

    formula_total_time += end_time - start_time

    formula_results.append(
        (formula_x, formula_y)
    )


    start_time = time.perf_counter()

    bresenham_x, bresenham_y = bresenham_line(
        x1, y1, x2, y2
    )

    end_time = time.perf_counter()

    bresenham_total_time += end_time - start_time

    bresenham_results.append(
        (bresenham_x, bresenham_y)
    )




times = [
    ("Bresenham", bresenham_total_time),
    ("DDA - Keep Float", float_total_time),
    ("DDA - Round m", round_m_total_time),
    ("Line Formula y = mx + c", formula_total_time)
]

times.sort(key=lambda x: x[1])

for i, (name, total_time) in enumerate(times, 1):

    print()
    print(f" {name} : {total_time * 1000:.6f} ms")

print()

plt.figure(figsize=(12, 12))


for i in range(number_of_lines):

    x1, y1, x2, y2 = lines[i]

    float_x, float_y = float_results[i]
    round_x, round_y = round_m_results[i]
    formula_x, formula_y = formula_results[i]
    bresenham_x, bresenham_y = bresenham_results[i]

  

    plt.scatter(
        round_x,
        round_y,
        s=15,
        color='blue',
        marker='.',
        alpha=0.5,
        label="DDA round" if i == 0 else ""
    )
    plt.scatter(
        float_x,
        float_y,
        s=15,
        color='red',
        marker='x',
        alpha=0.5,
        label="DDA float" if i == 0 else ""
    )

    plt.scatter(
        formula_x,
        formula_y,
        s=15,
        color='green',
        alpha=0.5,
        label="y = mx + c" if i == 0 else ""
    )

    plt.scatter(
        bresenham_x,
        bresenham_y,
        s=18,
        color='purple',
        marker='*',
        alpha=0.7,
        label="Bresenham" if i == 0 else ""
    )


plt.xlabel("X-axis")
plt.ylabel("Y-axis")

plt.title(
    f"Comparison of Line Drawing Algorithms "
    f"({number_of_lines} Random Lines)"
)

plt.grid(True)

plt.axis("equal")

plt.legend()

plt.show()


