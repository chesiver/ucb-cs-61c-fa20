#include "numc.h"
#include <structmember.h>

PyTypeObject Matrix61cType;

/* Helper functions for initalization of matrices and vectors */

/*
 * Return a tuple given rows and cols
 */
PyObject *get_shape(int rows, int cols) {
  if (rows == 1 || cols == 1) {
    return PyTuple_Pack(1, PyLong_FromLong(rows * cols));
  } else {
    return PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
  }
}
/*
 * Matrix(rows, cols, low, high). Fill a matrix random double values
 */
int init_rand(PyObject *self, int rows, int cols, unsigned int seed, double low,
              double high) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    rand_matrix(new_mat, seed, low, high);
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(rows, cols, val). Fill a matrix of dimension rows * cols with val
 */
int init_fill(PyObject *self, int rows, int cols, double val) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed)
        return alloc_failed;
    else {
        fill_matrix(new_mat, val);
        ((Matrix61c *)self)->mat = new_mat;
        ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    }
    return 0;
}

/*
 * Matrix(rows, cols, 1d_list). Fill a matrix with dimension rows * cols with 1d_list values
 */
int init_1d(PyObject *self, int rows, int cols, PyObject *lst) {
    if (rows * cols != PyList_Size(lst)) {
        PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list");
        return -1;
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j, PyFloat_AsDouble(PyList_GetItem(lst, count)));
            count++;
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(2d_list). Fill a matrix with dimension len(2d_list) * len(2d_list[0])
 */
int init_2d(PyObject *self, PyObject *lst) {
    int rows = PyList_Size(lst);
    if (rows == 0) {
        PyErr_SetString(PyExc_ValueError,
                        "Cannot initialize numc.Matrix with an empty list");
        return -1;
    }
    int cols;
    if (!PyList_Check(PyList_GetItem(lst, 0))) {
        PyErr_SetString(PyExc_ValueError, "List values not valid");
        return -1;
    } else {
        cols = PyList_Size(PyList_GetItem(lst, 0));
    }
    for (int i = 0; i < rows; i++) {
        if (!PyList_Check(PyList_GetItem(lst, i)) ||
                PyList_Size(PyList_GetItem(lst, i)) != cols) {
            PyErr_SetString(PyExc_ValueError, "List values not valid");
            return -1;
        }
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j,
                PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(lst, i), j)));
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * This deallocation function is called when reference count is 0
 */
void Matrix61c_dealloc(Matrix61c *self) {
    deallocate_matrix(self->mat);
    Py_TYPE(self)->tp_free(self);
}

/* For immutable types all initializations should take place in tp_new */
PyObject *Matrix61c_new(PyTypeObject *type, PyObject *args,
                        PyObject *kwds) {
    /* size of allocated memory is tp_basicsize + nitems*tp_itemsize*/
    Matrix61c *self = (Matrix61c *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

/*
 * This matrix61c type is mutable, so needs init function. Return 0 on success otherwise -1
 */
int Matrix61c_init(PyObject *self, PyObject *args, PyObject *kwds) {
    /* Generate random matrices */
    if (kwds != NULL) {
        PyObject *rand = PyDict_GetItemString(kwds, "rand");
        if (!rand) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (!PyBool_Check(rand)) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (rand != Py_True) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        PyObject *low = PyDict_GetItemString(kwds, "low");
        PyObject *high = PyDict_GetItemString(kwds, "high");
        PyObject *seed = PyDict_GetItemString(kwds, "seed");
        double double_low = 0;
        double double_high = 1;
        unsigned int unsigned_seed = 0;

        if (low) {
            if (PyFloat_Check(low)) {
                double_low = PyFloat_AsDouble(low);
            } else if (PyLong_Check(low)) {
                double_low = PyLong_AsLong(low);
            }
        }

        if (high) {
            if (PyFloat_Check(high)) {
                double_high = PyFloat_AsDouble(high);
            } else if (PyLong_Check(high)) {
                double_high = PyLong_AsLong(high);
            }
        }

        if (double_low >= double_high) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        // Set seed if argument exists
        if (seed) {
            if (PyLong_Check(seed)) {
                unsigned_seed = PyLong_AsUnsignedLong(seed);
            }
        }

        PyObject *rows = NULL;
        PyObject *cols = NULL;
        if (PyArg_UnpackTuple(args, "args", 2, 2, &rows, &cols)) {
            if (rows && cols && PyLong_Check(rows) && PyLong_Check(cols)) {
                return init_rand(self, PyLong_AsLong(rows), PyLong_AsLong(cols), unsigned_seed, double_low,
                                 double_high);
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    }
    PyObject *arg1 = NULL;
    PyObject *arg2 = NULL;
    PyObject *arg3 = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 3, &arg1, &arg2, &arg3)) {
        /* arguments are (rows, cols, val) */
        if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && (PyLong_Check(arg3)
                || PyFloat_Check(arg3))) {
            if (PyLong_Check(arg3)) {
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyLong_AsLong(arg3));
            } else
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyFloat_AsDouble(arg3));
        } else if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && PyList_Check(arg3)) {
            /* Matrix(rows, cols, 1D list) */
            return init_1d(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), arg3);
        } else if (arg1 && PyList_Check(arg1) && arg2 == NULL && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_2d(self, arg1);
        } else if (arg1 && arg2 && PyLong_Check(arg1) && PyLong_Check(arg2) && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), 0);
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return -1;
    }
}

/*
 * List of lists representations for matrices
 */
PyObject *Matrix61c_to_list(Matrix61c *self) {
    int rows = self->mat->rows;
    int cols = self->mat->cols;
    PyObject *py_lst = NULL;
    if (self->mat->is_1d) {  // If 1D matrix, print as a single list
        py_lst = PyList_New(rows * cols);
        int count = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(py_lst, count, PyFloat_FromDouble(get(self->mat, i, j)));
                count++;
            }
        }
    } else {  // if 2D, print as nested list
        py_lst = PyList_New(rows);
        for (int i = 0; i < rows; i++) {
            PyList_SetItem(py_lst, i, PyList_New(cols));
            PyObject *curr_row = PyList_GetItem(py_lst, i);
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(curr_row, j, PyFloat_FromDouble(get(self->mat, i, j)));
            }
        }
    }
    return py_lst;
}

PyObject *Matrix61c_class_to_list(Matrix61c *self, PyObject *args) {
    PyObject *mat = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 1, &mat)) {
        if (!PyObject_TypeCheck(mat, &Matrix61cType)) {
            PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
            return NULL;
        }
        Matrix61c* mat61c = (Matrix61c*)mat;
        return Matrix61c_to_list(mat61c);
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
}

/*
 * Add class methods
 */
PyMethodDef Matrix61c_class_methods[] = {
    {"to_list", (PyCFunction)Matrix61c_class_to_list, METH_VARARGS, "Returns a list representation of numc.Matrix"},
    {NULL, NULL, 0, NULL}
};

/*
 * Matrix61c string representation. For printing purposes.
 */
PyObject *Matrix61c_repr(PyObject *self) {
    PyObject *py_lst = Matrix61c_to_list((Matrix61c *)self);
    return PyObject_Repr(py_lst);
}

/* NUMBER METHODS */

/*
 * Add the second numc.Matrix (Matrix61c) object to the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_add(Matrix61c* self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }
    Matrix61c* mat61c = (Matrix61c*)args;
    if (self->mat->rows != mat61c->mat->rows || self->mat->cols != mat61c->mat->cols) {
        PyErr_SetString(PyExc_ValueError, "Matrices must have same dimensions!");
        return NULL;
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed) {
        PyErr_SetString(PyExc_RuntimeError, "Allocation failed!");
        return NULL;
    }
    add_matrix(new_mat, self->mat, mat61c->mat);
    Matrix61c *res = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    res->mat = new_mat;
    res->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *)res;
}

/*
 * Substract the second numc.Matrix (Matrix61c) object from the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_sub(Matrix61c* self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }
    Matrix61c* mat61c = (Matrix61c*)args;
    if (self->mat->rows != mat61c->mat->rows || self->mat->cols != mat61c->mat->cols) {
        PyErr_SetString(PyExc_ValueError, "Matrices must have same dimensions!");
        return NULL;
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed) {
        PyErr_SetString(PyExc_RuntimeError, "Allocation failed!");
        return NULL;
    }
    sub_matrix(new_mat, self->mat, mat61c->mat);
    Matrix61c *res = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    res->mat = new_mat;
    res->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *)res;
}

/*
 * NOT element-wise multiplication. The first operand is self, and the second operand
 * can be obtained by casting `args`.
 */
PyObject *Matrix61c_multiply(Matrix61c* self, PyObject *args) {
    /* TODO: YOUR CODE HERE */
    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }
    Matrix61c* mat61c = (Matrix61c*)args;
    if (self->mat->cols != mat61c->mat->rows) {
        PyErr_SetString(PyExc_ValueError, "Col / Row not matched!");
        return NULL;
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, self->mat->rows, mat61c->mat->cols);
    if (alloc_failed) {
        PyErr_SetString(PyExc_RuntimeError, "Allocation failed!");
        return NULL;
    }
    mul_matrix(new_mat, self->mat, mat61c->mat);
    Matrix61c *res = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    res->mat = new_mat;
    res->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *)res;
}

/*
 * Negates the given numc.Matrix.
 */
PyObject *Matrix61c_neg(Matrix61c* self) {
    /* TODO: YOUR CODE HERE */
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed) {
        PyErr_SetString(PyExc_RuntimeError, "Allocation failed!");
        return NULL;
    }
    neg_matrix(new_mat, self->mat);
    Matrix61c *res = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    res->mat = new_mat;
    res->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *)res;
}

/*
 * Take the element-wise absolute value of this numc.Matrix.
 */
PyObject *Matrix61c_abs(Matrix61c *self) {
    /* TODO: YOUR CODE HERE */
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed) {
        PyErr_SetString(PyExc_RuntimeError, "Allocation failed!");
        return NULL;
    }
    abs_matrix(new_mat, self->mat);
    Matrix61c *res = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    res->mat = new_mat;
    res->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *)res;
}

/*
 * Raise numc.Matrix (Matrix61c) to the `pow`th power. You can ignore the argument `optional`.
 */
PyObject *Matrix61c_pow(Matrix61c *self, PyObject *pow, PyObject *optional) {
    /* TODO: YOUR CODE HERE */
    if (!PyLong_Check(pow)) {
        PyErr_SetString(PyExc_TypeError, "Pow must of type Integer!");
        return NULL;
    }
    long primitive_pow = PyLong_AsLong(pow);
    if (primitive_pow < 0) {
        PyErr_SetString(PyExc_ValueError, "Pow must be positive!");
        return NULL;
    }
    if (self->mat->rows != self->mat->cols) {
        PyErr_SetString(PyExc_ValueError, "Matrix must be square!");
        return NULL;
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, self->mat->rows, self->mat->cols);
    if (alloc_failed) {
        PyErr_SetString(PyExc_RuntimeError, "Allocation failed!");
        return NULL;
    }
    pow_matrix(new_mat, self->mat, (int)primitive_pow);
    Matrix61c *res = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    res->mat = new_mat;
    res->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *)res;
}

/*
 * Create a PyNumberMethods struct for overloading operators with all the number methods you have
 * define. You might find this link helpful: https://docs.python.org/3.6/c-api/typeobj.html
 */
PyNumberMethods Matrix61c_as_number = {
    /* TODO: YOUR CODE HERE */
    .nb_add = Matrix61c_add,
    .nb_subtract = Matrix61c_sub,
    .nb_multiply = Matrix61c_multiply,
    .nb_negative = Matrix61c_neg,
    .nb_absolute = Matrix61c_abs,
    .nb_power = Matrix61c_pow
};


/* INSTANCE METHODS */

/*
 * Given a numc.Matrix self, parse `args` to (int) row, (int) col, and (double/int) val.
 * Return None in Python (this is different from returning null).
 */
PyObject *Matrix61c_set_value(Matrix61c *self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    PyObject *row = NULL;
    PyObject *col = NULL;
    PyObject *val = NULL;
    if (PyArg_UnpackTuple(args, "args", 3, 3, &row, &col, &val)) {
        if (!PyLong_Check(row)) {
            PyErr_SetString(PyExc_TypeError, "Row must of type Integer!");
            return NULL;
        }
        if (!PyLong_Check(col)) {
            PyErr_SetString(PyExc_TypeError, "Col must of type Integer!");
            return NULL;
        }
        if (!PyNumber_Check(val)) {
            PyErr_SetString(PyExc_TypeError, "Val must of type Number!");
            return NULL;
        }
        long r = PyLong_AS_LONG(row), c = PyLong_AS_LONG(col);
        if (r < 0 || r >= self->mat->rows || c < 0 || c >= self->mat->cols) {
            PyErr_SetString(PyExc_IndexError, "Index out of range!");
            return NULL;
        }
        set(self->mat, r, c, PyFloat_AsDouble(val));
        return Py_None;
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
}

/*
 * Given a numc.Matrix `self`, parse `args` to (int) row and (int) col.
 * Return the value at the `row`th row and `col`th column, which is a Python
 * float/int.
 */
PyObject *Matrix61c_get_value(Matrix61c *self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    PyObject *row = NULL;
    PyObject *col = NULL;
    if (PyArg_UnpackTuple(args, "args", 2, 2, &row, &col)) {
        if (!PyLong_Check(row)) {
            PyErr_SetString(PyExc_TypeError, "Row must of type Integer!");
            return NULL;
        }
        if (!PyLong_Check(col)) {
            PyErr_SetString(PyExc_TypeError, "Col must of type Integer!");
            return NULL;
        }
        long r = PyLong_AS_LONG(row), c = PyLong_AS_LONG(col);
        if (r < 0 || r >= self->mat->rows || c < 0 || c >= self->mat->cols) {
            PyErr_SetString(PyExc_IndexError, "Index out of range!");
            return NULL;
        }
        double res = get(self->mat, r, c);
        return (PyObject *)PyFloat_FromDouble(res);
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
}

/*
 * Create an array of PyMethodDef structs to hold the instance methods.
 * Name the python function corresponding to Matrix61c_get_value as "get" and Matrix61c_set_value
 * as "set"
 * You might find this link helpful: https://docs.python.org/3.6/c-api/structures.html
 */
PyMethodDef Matrix61c_methods[] = {
    /* TODO: YOUR CODE HERE */
    {"set", (PyCFunction)Matrix61c_set_value, METH_VARARGS, "Set one element"},
    {"get", (PyCFunction)Matrix61c_get_value, METH_VARARGS, "Get one element"},
    {NULL, NULL, 0, NULL}
};

/* INDEXING */

/*
 * Given a numc.Matrix `self`, index into it with `key`. Return the indexed result.
 */
PyObject *Matrix61c_subscript(Matrix61c* self, PyObject* key) {
    /* TODO: YOUR CODE HERE */
    if (!self->mat->is_1d) {
        if (PyLong_Check(key)) {
            long r = PyLong_AsLong(key);
            if (r < 0 || r >= self->mat->rows) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return NULL;
            }
            matrix *sub = NULL;
            int alloc_failed = allocate_matrix_ref(&sub, self->mat, r, 0, 1, self->mat->cols);
            if (alloc_failed) {
                PyErr_SetString(PyExc_RuntimeError, "Allocation failed!");
                return NULL;
            }
            Matrix61c *res = (Matrix61c *)Matrix61c_new(&Matrix61cType, NULL, NULL);
            res->mat = sub;
            res->shape = get_shape(sub->rows, sub->cols);
            return (PyObject *)res;
        }
        if (PySlice_Check(key)) {
            Py_ssize_t start, end, step = 0;
            if (PySlice_Unpack(key, &start, &end, &step) < 0) {
                PyErr_SetString(PyExc_ValueError, "Slice unpack failed!");
                return NULL;
            }
            if (start >= end || (step != 1 && step != 0)) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }
            int row_offset = start, rows = end - start;
            if (row_offset < 0 || row_offset + rows > self->mat->rows) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return NULL;
            }
            matrix *sub = NULL;
            int alloc_failed = allocate_matrix_ref(&sub, self->mat, row_offset, 0, rows, self->mat->cols);
            if (alloc_failed) {
                PyErr_SetString(PyExc_RuntimeError, "Allocation failed!");
                return NULL;
            }
            Matrix61c *res = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
            res->mat = sub;
            res->shape = get_shape(sub->rows, sub->cols);
            return (PyObject *)res;
        }
        if (PyTuple_Check(key)) {
            if (PyTuple_Size(key) != 2) {
                PyErr_SetString(PyExc_TypeError, "Key tuple size not equal to 2!");
                return NULL;
            }
            PyObject *row = PyTuple_GetItem(key, 0);
            PyObject *col = PyTuple_GetItem(key, 1);
            int row_offset, col_offset, rows, cols;
            if (PyLong_Check(row)) {
                row_offset = PyLong_AsLong(row);
                rows = 1;
            } else if (PySlice_Check(row)) {
                Py_ssize_t row_start, row_end, row_step = 0;
                if (PySlice_Unpack(row, &row_start, &row_end, &row_step) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Slice unpack failed!");
                    return NULL;
                }
                if (row_start >= row_end || (row_step != 1 && row_step != 0)) {
                    PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                    return NULL;
                }
                row_offset = row_start;
                rows = row_end - row_start;
            } else {
                PyErr_SetString(PyExc_TypeError, "Tuple key type error!");
                return NULL;
            }
            if (row_offset < 0 || row_offset + rows > self->mat->rows) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return NULL;
            }
            if (PyLong_Check(col)) {
                col_offset = PyLong_AsLong(col);
                cols = 1;
            } else if (PySlice_Check(col)) {
                Py_ssize_t col_start, col_end, col_step = 0;
                if (PySlice_Unpack(col, &col_start, &col_end, &col_step) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Slice unpack failed!");
                    return NULL;
                }
                if (col_start >= col_end || (col_step != 1 && col_step != 0)) {
                    PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                    return NULL;
                }
                col_offset = col_start;
                cols = col_end - col_start;
            } else {
                PyErr_SetString(PyExc_TypeError, "Tuple key type error!");
                return NULL;
            }
            if (col_offset < 0 || col_offset + cols > self->mat->cols) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return NULL;
            }
            matrix *sub = NULL;
            int alloc_failed = allocate_matrix_ref(&sub, self->mat, row_offset, col_offset, rows, cols);
            if (alloc_failed) {
                PyErr_SetString(PyExc_RuntimeError, "Allocation failed!");
                return NULL;
            }
            Matrix61c *res = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
            res->mat = sub;
            res->shape = get_shape(sub->rows, sub->cols);
            return (PyObject *)res;
        }
        PyErr_SetString(PyExc_TypeError, "Key type error!");
        return NULL;
    } else {
        /*1D matrix*/
        if (PyLong_Check(key)) {
            PyObject *res;
            long k = PyLong_AsLong(key);
            if (self->mat->rows == 1) {
                if (k >= self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range!");
                    return NULL;
                }
                res = PyFloat_FromDouble(self->mat->data[0][PyLong_AsLong(key)]);
            } else {
                if (k >= self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range!");
                    return NULL;
                }
                res = PyFloat_FromDouble(self->mat->data[PyLong_AsLong(key)][0]);
            }
            return (PyObject *)res;
        }
        if (PySlice_Check(key)) {
            Py_ssize_t start, end, step = 0;
            if (PySlice_Unpack(key, &start, &end, &step) < 0) {
                PyErr_SetString(PyExc_ValueError, "Slice unpack failed!");
                return NULL;
            }
            if (start >= end || (step != 1 && step != 0)) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }
            matrix *sub = NULL;
            if (self->mat->rows == 1) {
                if (start < 0 || end > self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range!");
                    return NULL;
                }
                int alloc_failed = allocate_matrix_ref(&sub, self->mat, 0, (int)start, 1, (int)(end - start));
                if (alloc_failed) {
                    PyErr_SetString(PyExc_RuntimeError, "Allocation failed!");
                    return NULL;
                }
            } else {
                if (start < 0 || end > self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range!");
                    return NULL;
                }
                int alloc_failed = allocate_matrix_ref(&sub, self->mat, (int)start, 0, (int)(end - start), 1);
                if (alloc_failed) {
                    PyErr_SetString(PyExc_RuntimeError, "Allocation failed!");
                    return NULL;
                }
            }
            Matrix61c *res = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
            res->mat = sub;
            res->shape = get_shape(sub->rows, sub->cols);
            return (PyObject *)res;
        }
        PyErr_SetString(PyExc_TypeError, "Key type error!");
        return NULL;
    }
}

/*
 * Given a numc.Matrix `self`, index into it with `key`, and set the indexed result to `v`.
 */
int Matrix61c_set_subscript(Matrix61c* self, PyObject *key, PyObject *v) {
    /* TODO: YOUR CODE HERE */
    int row_start, col_start, row_end, col_end;
    if (!self->mat->is_1d) {
        if (PyLong_Check(key)) {
            long r = PyLong_AsLong(key);
            if (r < 0 || r >= self->mat->rows) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return -1;
            }
            row_start = r; 
            row_end = r + 1;
            col_start = 0; 
            col_end = self->mat->cols;
        }
        else if (PySlice_Check(key)) {
            Py_ssize_t start, end, step = 0;
            if (PySlice_Unpack(key, &start, &end, &step) < 0) {
                PyErr_SetString(PyExc_ValueError, "Slice unpack failed!");
                return 1;
            }
            if (start >= end || (step != 1 && step != 0)) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return 1;
            }
            row_start = start; 
            row_end = end;
            col_start = 0; 
            col_end = self->mat->cols;
        } else if (PyTuple_Check(key)) {
            if (PyTuple_Size(key) != 2) {
                PyErr_SetString(PyExc_TypeError, "Key tuple size not equal to 2!");
                return -1;
            }
            PyObject *row = PyTuple_GetItem(key, 0);
            PyObject *col = PyTuple_GetItem(key, 1);
            if (PyLong_Check(row)) {
                row_start = PyLong_AsLong(row); 
                row_end = row_start + 1;
            } else if (PySlice_Check(row)) {
                Py_ssize_t r_start, r_end, r_step = 0;
                if (PySlice_Unpack(row, &r_start, &r_end, &r_step) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Slice unpack failed!");
                    return -1;
                }
                if (r_start >= r_end || (r_step != 1 && r_step != 0)) {
                    PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                    return -1;
                }
                row_start = r_start;
                row_end = r_end;
            } else {
                PyErr_SetString(PyExc_TypeError, "Tuple key type error!");
                return -1;
            }
            if (row_start < 0 || row_end > self->mat->rows) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return -1;
            }
            if (PyLong_Check(col)) {
                col_start = PyLong_AsLong(col); 
                col_end = col_start + 1;
            } else if (PySlice_Check(col)) {
                Py_ssize_t c_start, c_end, c_step = 0;
                if (PySlice_Unpack(col, &c_start, &c_end, &c_step) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Slice unpack failed!");
                    return -1;
                }
                if (c_start >= c_end || (c_step != 1 && c_step != 0)) {
                    PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                    return -1;
                }
                col_start = c_start;
                col_end = c_end;
            } else {
                PyErr_SetString(PyExc_TypeError, "Tuple key type error!");
                return -1;
            }
            if (col_start < 0 || col_end > self->mat->cols) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return -1;
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Key type error!");
            return -1;
        }
    } else {
        /*1D matrix*/
        if (PyLong_Check(key)) {
            long k = PyLong_AsLong(key);
            if (self->mat->rows == 1) {
                if (k >= self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range!");
                    return -1;
                }
                row_start = 0;
                row_end = 1;
                col_start = k;
                col_end = k + 1;
            } else {
                if (k >= self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range!");
                    return -1;
                }
                row_start = k;
                row_end = k + 1;
                col_start = 0;
                col_end = 1;
            }
        } else if (PySlice_Check(key)) {
            Py_ssize_t start, end, step = 0;
            if (PySlice_Unpack(key, &start, &end, &step) < 0) {
                PyErr_SetString(PyExc_ValueError, "Slice unpack failed!");
                return -1;
            }
            if (start >= end || (step != 1 && step != 0)) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }
            if (self->mat->rows == 1) {
                if (start < 0 || end > self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range!");
                    return -1;
                }
                row_start = 0;
                row_end = 1;
                col_start = start;
                col_end = end; 
            } else {
                if (start < 0 || end > self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range!");
                    return -1;
                }
                row_start = start;
                row_end = end;
                col_start = 0;
                col_end = 1; 
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Key type error!");
            return -1;
        }
    }
    if (PyNumber_Check(v)) {
        if (row_end - row_start != 1 || col_end - col_start != 1) {
            PyErr_SetString(PyExc_ValueError, "Dimensions not matched!");
            return -1;
        }
        set(self->mat, row_start, col_start, PyFloat_AsDouble(PyNumber_Float(v)));
        return 0;
    }
    if (PyList_Check(v)) {
        int row_len = PyList_Size(v);
        if (row_len == 0) {
            PyErr_SetString(PyExc_ValueError, "Empty list!");
            return -1;
        }
        /* Dimension check for value*/
        PyObject *first_item = PyList_GetItem(v, 0);
        int col_len;
        if (PyNumber_Check(first_item)) {
            col_len = 1;
        } else if (PyList_Check(first_item)) {
            col_len = PyList_Size(first_item);
        } else {
            PyErr_SetString(PyExc_TypeError, "Value type error!");
            return -1;
        }
        for (int i = 1; i < row_len; i += 1) {
            PyObject *item = PyList_GetItem(v, i);
            if (PyNumber_Check(item) && col_len != 1) {
                PyErr_SetString(PyExc_ValueError, "Value dimension error!");
                return -1;
            }
            if (PyList_Check(item) && col_len != PyList_Size(item)) {
                PyErr_SetString(PyExc_ValueError, "Value dimension error!");
                return -1;
            }
        }
        /* One-Dimension still needs special handling */
        if (row_end - row_start == 1 || col_end - col_start == 1) {
            if (row_end - row_start == 1) {
                if (col_end - col_start != row_len || 1 != col_len) {
                    PyErr_SetString(PyExc_ValueError, "Value dimension not matched!");
                    return -1;
                }
                for (int i = 0; i < row_len; i += 1) {
                    set(self->mat, 0, col_start + i, PyFloat_AsDouble(PyNumber_Float(PyList_GetItem(v, i))));
                }
                return 0;
            } else {
                if (row_end - row_start != row_len || 1 != col_len) {
                    PyErr_SetString(PyExc_ValueError, "Value dimension not matched!");
                    return -1;
                }
                for (int i = 0; i < row_len; i += 1) {
                    set(self->mat, row_start + i, 0, PyFloat_AsDouble(PyNumber_Float(PyList_GetItem(v, i))));
                }
                return 0;
            }
        }
        if (row_end - row_start != row_len || col_end - col_start != col_len) {
            PyErr_SetString(PyExc_ValueError, "Value dimension not matched!");
            return -1;
        }
        for (int i = 0; i < row_len; i += 1) {
            PyObject *item = PyList_GetItem(v, i);
            for (int j = 0; j < col_len; j += 1) {
                set(self->mat, row_start + i, col_start + j, PyFloat_AsDouble(PyNumber_Float(PyList_GetItem(item, j))));
            }
        }
        return 0;
    }
    PyErr_SetString(PyExc_TypeError, "Value type error!");
    return -1;
}

PyMappingMethods Matrix61c_mapping = {
    NULL,
    (binaryfunc) Matrix61c_subscript,
    (objobjargproc) Matrix61c_set_subscript,
};

/* INSTANCE ATTRIBUTES*/
PyMemberDef Matrix61c_members[] = {
    {
        "shape", T_OBJECT_EX, offsetof(Matrix61c, shape), 0,
        "(rows, cols)"
    },
    {NULL}  /* Sentinel */
};

PyTypeObject Matrix61cType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "numc.Matrix",
    .tp_basicsize = sizeof(Matrix61c),
    .tp_dealloc = (destructor)Matrix61c_dealloc,
    .tp_repr = (reprfunc)Matrix61c_repr,
    .tp_as_number = &Matrix61c_as_number,
    .tp_flags = Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,
    .tp_doc = "numc.Matrix objects",
    .tp_methods = Matrix61c_methods,
    .tp_members = Matrix61c_members,
    .tp_as_mapping = &Matrix61c_mapping,
    .tp_init = (initproc)Matrix61c_init,
    .tp_new = Matrix61c_new
};


struct PyModuleDef numcmodule = {
    PyModuleDef_HEAD_INIT,
    "numc",
    "Numc matrix operations",
    -1,
    Matrix61c_class_methods
};

/* Initialize the numc module */
PyMODINIT_FUNC PyInit_numc(void) {
    PyObject *m;

    if (PyType_Ready(&Matrix61cType) < 0)
        return NULL;

    m = PyModule_Create(&numcmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&Matrix61cType);
    PyModule_AddObject(m, "Matrix", (PyObject *)&Matrix61cType);
    fflush(stdout);
    return m;
}

PyMODINIT_FUNC PyInit_numc_naive(void) {
    PyObject *m;

    if (PyType_Ready(&Matrix61cType) < 0)
        return NULL;

    m = PyModule_Create(&numcmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&Matrix61cType);
    PyModule_AddObject(m, "Matrix", (PyObject *)&Matrix61cType);
    fflush(stdout);
    return m;
}