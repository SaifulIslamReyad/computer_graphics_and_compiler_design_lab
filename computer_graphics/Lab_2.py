import matplotlib.pyplot as plt
import random

def line_formula(x1, y1, x2, y2):

    points = []

    dy = y2 - y1
    dx = x2 - x1

    subtraction_count = 2
    division_count = 0
    multiplication_count = 0
    addition_count = 0

    if dx == 0:

        for y in range(min(y1, y2), max(y1, y2) + 1):
            points.append((x1, y))

        return points, (
            subtraction_count,
            division_count,
            multiplication_count,
            addition_count
        )

    m = dy / dx
    division_count += 1

    c = y1 - m * x1

    multiplication_count += 1
    subtraction_count += 1

    number_of_points = abs(x2 - x1) + 1

    for x in range(min(x1, x2), max(x1, x2) + 1):

        y = c + m * x

        multiplication_count += 1
        addition_count += 1

        points.append((x, y))

    return points, (
        subtraction_count,
        division_count,
        multiplication_count,
        addition_count
    )

NUMBER_OF_LINES = 100

total_subtractions = 0
total_divisions = 0
total_multiplications = 0
total_additions = 0

total_points = 0

all_lines = []


for i in range(NUMBER_OF_LINES):

    x1 = random.randint(0, 100)
    y1 = random.randint(0, 100)

    x2 = random.randint(0, 100)
    y2 = random.randint(0, 100)

    line_pts, operations = line_formula(
        x1, y1, x2, y2
    )

    all_lines.append(line_pts)

    total_points += len(line_pts)

    subtraction_count = operations[0]
    division_count = operations[1]
    multiplication_count = operations[2]
    addition_count = operations[3]
    
    total_subtractions += subtraction_count
    total_divisions += division_count
    total_multiplications += multiplication_count
    total_additions += addition_count
    
total_operations = (
    total_subtractions
    + total_divisions
    + total_multiplications
    + total_additions
)

computational_time = total_operations

print("Number of lines        :", NUMBER_OF_LINES)

print("Total points generated :", total_points)

print("\nArithmetic Operations")

print("Subtractions           :", total_subtractions)
print("Divisions              :", total_divisions)
print("Multiplications        :", total_multiplications)
print("Additions              :", total_additions)

print("Total operations       :", total_operations)

print("\nAssuming unit cost for each operation:")

print("Computational cost     :", computational_time, "time units")

plt.figure(figsize=(10, 8))

for line_pts in all_lines:

    x, y = zip(*line_pts)
    
    plt.scatter(x, y, s=5)


plt.title("100 Random Lines Using Mathematical Formula")

plt.xlabel("X")
plt.ylabel("Y")

plt.grid(True)

plt.axis("equal")

plt.show()