import numc as nc
import numc_naive as ncn
import time

mat1 = ncn.Matrix(4096, 4096, rand=True, seed=1)
mat2 = ncn.Matrix(4096, 4096, rand=True, seed=1)

start = time.time()
mat3 = mat1 + mat2
end = time.time()
print('Naive Addition: ', end - start)

mat1 = nc.Matrix(4096, 4096, rand=True, seed=1)
mat2 = nc.Matrix(4096, 4096, rand=True, seed=1)

start = time.time()
mat3 = mat1 + mat2
end = time.time()
print('Optimized Addition: ', end - start)