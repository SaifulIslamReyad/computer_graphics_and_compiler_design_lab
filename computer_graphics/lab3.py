import matplotlib.pyplot as plt
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

    # Round slope
    m = round(m)

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

    else:

        if y1 > y2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1

        dx = x2 - x1
        dy = y2 - y1

        m = dx / dy

        # Round slope
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



def line_formula(x1, y1, x2, y2):

    x_points = []
    y_points = []

    if x1 == x2:

        for y in range(
            min(y1, y2),
            max(y1, y2) + 1
        ):

            x_points.append(x1)
            y_points.append(y)

        return x_points, y_points

    m = (y2 - y1) / (x2 - x1)

    c = y1 - m * x1

    for x in range(
        min(x1, x2),
        max(x1, x2) + 1
    ):

        y = c + m * x

        x_points.append(x)
        y_points.append(y)

    return x_points, y_points




def bresenham_line(x1, y1, x2, y2):

    x_points = []
    y_points = []

    dx = x2 - x1
    dy = y2 - y1

 
    if dx == 0 and dy == 0:

        x_points.append(x1)
        y_points.append(y1)

        return x_points, y_points


 
    if dx == 0:

        if y1 > y2:
            y1, y2 = y2, y1

        y = y1

        while y <= y2:

            x_points.append(x1)
            y_points.append(y)

            y += 1

        return x_points, y_points


 
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


x1 = int(input("Enter x1: "))
y1 = int(input("Enter y1: "))

x2 = int(input("Enter x2: "))
y2 = int(input("Enter y2: "))


start_time = time.perf_counter()

round_m_x, round_m_y = dda_round_m(
    x1, y1, x2, y2
)

end_time = time.perf_counter()

round_m_time = end_time - start_time



start_time = time.perf_counter()

float_x, float_y = dda_keep_float(
    x1, y1, x2, y2
)

end_time = time.perf_counter()

float_time = end_time - start_time


start_time = time.perf_counter()

formula_x, formula_y = line_formula(
    x1, y1, x2, y2
)

end_time = time.perf_counter()

formula_time = end_time - start_time



start_time = time.perf_counter()

bresenham_x, bresenham_y = bresenham_line(
    x1, y1, x2, y2
)

end_time = time.perf_counter()

bresenham_time = end_time - start_time




print(
    f"Pixels generated Round m : {len(round_m_x)}"
)

print(
    f"Execution time Round m   : "
    f"{round_m_time:.9f} seconds"
)

print("----------------------------------------------")

print(
    f"Pixels generated Float   : {len(float_x)}"
)

print(
    f"Execution time Float     : "
    f"{float_time:.9f} seconds"
)

print("----------------------------------------------")

print(
    f"Pixels generated Formula : {len(formula_x)}"
)

print(
    f"Execution time Formula   : "
    f"{formula_time:.9f} seconds"
)

print("----------------------------------------------")

print(
    f"Pixels generated Bresenham: {len(bresenham_x)}"
)

print(
    f"Execution time Bresenham : "
    f"{bresenham_time:.9f} seconds"
)


plt.figure(figsize=(10, 10))


plt.scatter(
    round_m_x,
    round_m_y,
    s=25,
    marker='o',
    color='blue',
    label="DDA - Round m"
)



plt.scatter(
    float_x,
    float_y,
    s=25,
    marker='x',
    color='red',
    label="DDA - Keep Float"
)



plt.scatter(
    formula_x,
    formula_y,
    s=30,
    marker='o',
    color='green',
    label="y = mx + c"
)




plt.scatter(
    bresenham_x,
    bresenham_y,
    s=35,
    marker='*',
    color='purple',
    label="Bresenham"
)



margin = 20

plt.xlim(
    min(x1, x2) - margin,
    max(x1, x2) + margin
)

plt.ylim(
    min(y1, y2) - margin,
    max(y1, y2) + margin
)

plt.xlabel("X-axis")
plt.ylabel("Y-axis")

plt.title("Comparison of Line Drawing Algorithms")

plt.grid(True)

plt.axis("equal")

plt.legend()

plt.show()