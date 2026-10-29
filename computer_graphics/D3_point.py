import math

P = (1.0, 2.0, 3.0)       
V = (4.0, 5.0, 6.0)        
theta = 45.0              
Q = (7.0, 8.0, 9.0)        


def identity():
    return [[1.0, 0.0, 0.0, 0.0],
            [0.0, 1.0, 0.0, 0.0],
            [0.0, 0.0, 1.0, 0.0],
            [0.0, 0.0, 0.0, 1.0]]


def matrix_multiply(A, B):
    C = identity()
    for i in range(4):
        for j in range(4):
            s = 0.0
            for k in range(4):
                s = s + A[i][k] * B[k][j]
            C[i][j] = s
    return C


def transform(M, point):
    x = point[0]
    y = point[1]
    z = point[2]
    nx = M[0][0] * x + M[0][1] * y + M[0][2] * z + M[0][3]
    ny = M[1][0] * x + M[1][1] * y + M[1][2] * z + M[1][3]
    nz = M[2][0] * x + M[2][1] * y + M[2][2] * z + M[2][3]
    return (nx, ny, nz)


def translation(dx, dy, dz):
    T = identity()
    T[0][3] = dx
    T[1][3] = dy
    T[2][3] = dz
    return T


def rotation_x(c, s):                 
    R = identity()
    R[1][1] = c
    R[1][2] = -s
    R[2][1] = s
    R[2][2] = c
    return R


def rotation_y(c, s):
    R = identity()
    R[0][0] = c
    R[0][2] = s
    R[2][0] = -s
    R[2][2] = c
    return R


def rotation_z(c, s):
    R = identity()
    R[0][0] = c
    R[0][1] = -s
    R[1][0] = s
    R[1][1] = c
    return R


def show(name, point):
    print(f"  {name:<26} {point[0]:8.4f}, {point[1]:8.4f}, {point[2]:8.4f}")


def main():
    a = V[0]
    b = V[1]
    c = V[2]
    angle = theta * math.pi / 180.0

    mod = math.sqrt(a * a + b * b + c * c)         
    lam = math.sqrt(b * b + c * c)                 

    T_neg_P = translation(-P[0], -P[1], -P[2])
    A = matrix_multiply(rotation_y(lam / mod, -a / mod),
                        rotation_x(c / lam, b / lam))
    R_K = rotation_z(math.cos(angle), math.sin(angle))
    A_inv = matrix_multiply(rotation_x(c / lam, -b / lam),
                            rotation_y(lam / mod, a / mod))
    T_pos_P = translation(P[0], P[1], P[2])

    print("")
    print(f"  P = {P} , V = {V} , theta = {theta} degrees")
    print("")

    q = Q
    show("start", q)

    q = transform(T_neg_P, q)
    show("translate P to origin", q)

    q = transform(A, q)
    show("align V with K", q)

    q = transform(R_K, q)
    show("rotate theta about K", q)

    q = transform(A_inv, q)
    show("undo the alignment", q)

    q = transform(T_pos_P, q)
    show("translate back", q)

    print("")


if __name__ == "__main__":
    main()