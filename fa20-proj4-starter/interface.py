import numc as nc

print(nc.Matrix(3, 3))

print(nc.Matrix(3, 3, 1))

print(nc.Matrix([[1, 2, 3], [4, 5, 6]]))

print(nc.Matrix(1, 2, [4, 5]))

mat = nc.Matrix(3, 3)

print(mat.shape)

mat1 = nc.Matrix([[1, 2, 3], [4, 5, 6]])
mat2 = nc.Matrix([[3, 2, 1], [6, 5, 4]])

print(mat1)
print(mat2)

print(mat1 + mat2)
print(mat1 - mat2)

mat1 = nc.Matrix([[1, 2, 3], [4, 5, 6]])
mat2 = nc.Matrix([[3, 2], [6, 5], [9, 8]])
print(mat1 * mat2)

print(-mat1)

mat = nc.Matrix([[-1, -2, -3], [4, 5, 6]])
print(abs(mat))

mat = nc.Matrix([[3, 2], [6, 5]])
print(mat ** 0)
print(mat ** 2)

mat = nc.Matrix([[0, 0], [6, 5]])
mat.set(0, 0, 2.0)
mat.set(0, 1, -2)
print(mat)
print(mat.get(0, 0))

mat = nc.Matrix([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
print(mat[0])
print(mat[0:2])
print(mat[1:2, 1:2])
print(mat[1, 0:2])
print(mat[0:2, 1])
print(mat[1, 1])

mat = nc.Matrix(1, 3)
print(mat[0])
print(mat[0:2])

mat = nc.Matrix(3, 3)
mat[0, 0] = 1
print(mat)
mat = nc.Matrix(3, 3)
mat[0:2, 0] = [2, 2]
print(mat)
mat = nc.Matrix(3, 3)
mat[0:2, 0:2] = [[1, 2], [3, 4]]
print(mat)