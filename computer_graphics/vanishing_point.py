import cv2
import numpy as np



vertices = np.array([
    [0, 1, 1, 0, 0, 0, 1, 1],
    [0, 0, 1, 1, 1, 0, 0, 1],
    [0, 0, 0, 0, 1, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1]
], dtype=float)

names = ["A", "B", "C", "D", "E", "F", "G", "H"]



cases = [
    {
        "id": "1_vanishing_point",
        "title": "Cube Projection - 1 Vanishing Point",
        "R": np.array([0.0, 0.0, 0.0]),
        "N": np.array([0.0, 0.0, 1.0]),
        "C": np.array([-1.2, 2.2, -2.5])
    },

    {
        "id": "2_vanishing_point",
        "title": "Cube Projection - 2 Vanishing Points (Reference Match)",
        "R": np.array([0.0, 0.0, 0.0]),
        "N": np.array([1.0, 0.0, 1.0]),
        "C": np.array([-2.0, 3.0, -2.0])
    },

    {
        "id": "3_vanishing_point",
        "title": "Cube Projection - 3 Vanishing Points",
        "R": np.array([0.0, 0.0, 0.0]),
        "N": np.array([1.0, -0.35, 1.0]),
        "C": np.array([-2.0, 3.0, -2.0])
    }
]



cube_faces = [
    ([0, 1, 2, 3], np.array([0, 0, -1]), np.array([0, 0, 0])),
    ([5, 6, 7, 4], np.array([0, 0, 1]),  np.array([0, 0, 1])),
    ([0, 3, 4, 5], np.array([-1, 0, 0]), np.array([0, 0, 0])),
    ([1, 6, 7, 2], np.array([1, 0, 0]),  np.array([1, 0, 0])),
    ([3, 2, 7, 4], np.array([0, 1, 0]),  np.array([0, 1, 0])),
    ([0, 5, 6, 1], np.array([0, -1, 0]), np.array([0, 0, 0]))
]



WIDTH = 500
HEIGHT = 520
BORDER = 85

positions = {
    "1_vanishing_point": (20, 60),
    "3_vanishing_point": (535, 60),
    "2_vanishing_point": (1050, 60)
}



def perspective_matrix(normal, reference, camera):

    nx, ny, nz = normal
    a, b, c = camera

    d0 = np.dot(normal, reference)
    d1 = np.dot(normal, camera)

    d = d0 - d1

    matrix = np.array([
        [d + a * nx, a * ny,       a * nz,       -a * d0],
        [b * nx,     d + b * ny,   b * nz,       -b * d0],
        [c * nx,      c * ny,     d + c * nz,    -c * d0],
        [nx,          ny,           nz,           -d1]
    ])

    return matrix, d0, d1, d



def project_points(matrix, points):

    transformed = np.matmul(matrix, points)

    result = []

    for j in range(transformed.shape[1]):

        w = transformed[3, j]

        x = transformed[0, j] / w
        y = transformed[1, j] / w
        z = transformed[2, j] / w

        result.append([x, y, z])

    return np.array(result)



def find_vanishing_points(normal, camera, d):

    nx, ny, nz = normal
    a, b, c = camera

    points = {}

    if nx != 0:
        points["VP1 (x-axis)"] = (
            round(a + d / nx, 3),
            round(b, 3),
            round(c, 3)
        )

    if ny != 0:
        points["VP2 (y-axis)"] = (
            round(a, 3),
            round(b + d / ny, 3),
            round(c, 3)
        )

    if nz != 0:
        points["VP3 (z-axis)"] = (
            round(a, 3),
            round(b, 3),
            round(c + d / nz, 3)
        )

    return points



def convert_to_pixels(points):

    x_values = points[:, 0]
    y_values = points[:, 1]

    xmin = np.min(x_values)
    xmax = np.max(x_values)

    ymin = np.min(y_values)
    ymax = np.max(y_values)

    width_range = xmax - xmin
    height_range = ymax - ymin

    if width_range == 0:
        width_range = 1

    if height_range == 0:
        height_range = 1

    sx = (WIDTH - 2 * BORDER) / width_range
    sy = (HEIGHT - 2 * BORDER) / height_range

    scale = min(sx, sy)

    center_x = (xmin + xmax) / 2
    center_y = (ymin + ymax) / 2

    pixels = []

    for point in points:

        x = point[0]
        y = point[1]

        screen_x = int(
            WIDTH / 2 + (x - center_x) * scale
        )

        screen_y = int(
            HEIGHT / 2 - (y - center_y) * scale
        )

        pixels.append((screen_x, screen_y))

    return pixels



def visible_face(normal, face_point, camera):

    direction = camera - face_point

    value = np.dot(normal, direction)

    return value > 0



def draw_cube(image, pixel_points, camera):

    for vertex_list, normal, face_point in cube_faces:

        if not visible_face(normal, face_point, camera):
            continue

        for i in range(4):

            first = vertex_list[i]
            second = vertex_list[(i + 1) % 4]

            p1 = pixel_points[first]
            p2 = pixel_points[second]

            cv2.line(
                image,
                p1,
                p2,
                (0, 0, 0),
                1,
                cv2.LINE_AA
            )



for item in cases:

    R = item["R"]
    N = item["N"]
    C = item["C"]

    P, d0, d1, d = perspective_matrix(N, R, C)

    projected = project_points(P, vertices)

    print("\n" + item["title"])

    for index, point in enumerate(projected):

        print(
            f"  {names[index]}' = "
            f"({point[0]:.3f}, "
            f"{point[1]:.3f}, "
            f"{point[2]:.3f})"
        )

    vanishing_points = find_vanishing_points(N, C, d)

    print("Vanishing Points:", vanishing_points)

    image_points = convert_to_pixels(projected)

    canvas = np.full(
        (HEIGHT, WIDTH, 3),
        255,
        dtype=np.uint8
    )

    draw_cube(
        canvas,
        image_points,
        C
    )

    cv2.imshow(
        item["title"],
        canvas
    )

    if item["id"] in positions:

        x, y = positions[item["id"]]

        cv2.moveWindow(
            item["title"],
            x,
            y
        )

    cv2.imwrite(
        item["id"] + ".png",
        canvas
    )


cv2.waitKey(0)
cv2.destroyAllWindows()