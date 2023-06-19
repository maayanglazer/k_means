#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double get_eclidean_dist();
double **fit();
double update_clusters_and_delta();
void initLayer();
void print();
double *zeros();
double **zeros_2d();
double max();
double *add();
void free_2d();
void free_all();
double **matrix;
double **k_array;
double **sum_for_cluster;
double *delta_k;
double *elem_in_cluster;
int vectors_num = 0;
int dim = 0;
int K = 0;


void generalError()
{
    printf("An Error Has Occurred\n");
    free_all();
    exit(1);
}

void free_all()
{
    if (matrix != NULL)
    {
        free_2d(matrix, vectors_num);
    }
    if (k_array != NULL)
    {
        free_2d(k_array, K);
    }
    if (sum_for_cluster != NULL)
    {
        free_2d(sum_for_cluster, K);
    }
    if (elem_in_cluster != NULL)
    {
        free(elem_in_cluster);
    }
    if (delta_k != NULL)
    {
        free(delta_k);
    }
}

void free_2d(double **ptr, int N)
{
    int i;
    double *currDoublePtr;
    for (i = 0; i < N; i++)
    {
        currDoublePtr = ptr[i];
        free(currDoublePtr);
    }
    free(ptr);
}

double update_clusters_and_delta(double *delta_k, double **sum_for_cluster, double *elem_in_cluster, double **k_array)
{
    double *new_cluster;
    int i, j;
    for (i = 0; i < K; i++)
    {
        new_cluster = zeros(dim);
        for (j = 0; j < dim; j++)
        {
            new_cluster[j] = sum_for_cluster[i][j] / elem_in_cluster[i];
        }
        delta_k[i] = get_eclidean_dist(new_cluster, k_array[i]);
        free(k_array[i]);
        k_array[i] = new_cluster;
    }
    return max(delta_k, K);
}

double get_eclidean_dist(double *vector1, double *vector2)
{
    double sum = 0;
    int i;
    for (i = 0; i < dim; i++)
    {
        sum += pow((vector1[i] - vector2[i]), 2);
    }
    return sqrt(sum);
}

double max(double *vector, int vec_len)
{
    double max_num = vector[0];
    int i;
    for (i = 0; i < vec_len; i++)
    {
        if (vector[i] > max_num)
        {
            max_num = vector[i];
        }
    }
    return max_num;
}

double *add(double *vector1, double *vector2)
{
    int p;
    double *res = zeros(dim);
    for (p = 0; p < dim; p++)
    {
        res[p] = vector1[p] + vector2[p];
    }
    return res;
}

double *zeros(int n)
{
    double *pointer;
    int i;
    pointer = (double *)calloc(n, sizeof(double));
    if (pointer == NULL)
    {
        generalError();
    }
    for (i = 0; i < n; i++)
    {
        pointer[i] = 0.0;
    }
    return pointer;
}

void print(double **arr, int m, int n)
{
    int i, j;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (j == n - 1)
            {
                printf("%.4f\n", arr[i][j]);
            }
            else
            {
                printf("%.4f,", arr[i][j]);
            }
        }
    }
    printf("\n");
}

double **fit(int K, int iter, double **k_array, double **vectors, int dim, double EPS)
{
    double min_dist, dist;
    int min_elem;
    double *tmp_vector;
    double max_delta = 0.02;
    int iteration_number = 0;
    int i, j;
    delta_k = zeros(K);
    elem_in_cluster = zeros(K);
    initLayer(&sum_for_cluster, K, dim);

    do
    {
        iteration_number++;
        for (i = 0; i < K; i++)
        {
            delta_k[i] = 0.0;
            elem_in_cluster[i] = 0.0;
            for (j = 0; j < dim; j++)
            {
                sum_for_cluster[i][j] = 0.0;
            }
        }

        for (i = 0; i < vectors_num; i++)
        {
            min_dist = get_eclidean_dist(vectors[i], k_array[0]);
            min_elem = 0;
            for (j = 0; j < K; j++)
            {
                dist = get_eclidean_dist(vectors[i], k_array[j]);
                if (dist < min_dist)
                {
                    min_dist = dist;
                    min_elem = j;
                }
            }
            tmp_vector = sum_for_cluster[min_elem];
            sum_for_cluster[min_elem] = add(vectors[i], sum_for_cluster[min_elem]);
            free(tmp_vector);
            elem_in_cluster[min_elem]++;
        }
        max_delta = update_clusters_and_delta(delta_k, sum_for_cluster, elem_in_cluster, k_array);

    } while (!(max_delta < EPS || iteration_number == iter));

    return k_array;
}

void initLayer(double ***layer, int rows, int cols)
{
    int i;
    *layer = malloc(sizeof(double *) * rows);
    if (*layer == NULL)
    {
        generalError();
    }

    for (i = 0; i < rows; i++)
    {
        *(*layer + i) = malloc(sizeof(double) * cols);
        if (*(*layer + i) == NULL)
        {
            generalError();
        }
    }
}

PyObject *makelist(double *array, int size)
{
    PyObject *l = PyList_New(size);
    for (int i = 0; i != size; ++i)
    {
        PyList_SET_ITEM(l, i, PyFloat_FromDouble(array[i]));
    }
    return l;
}

PyObject *makemat(double **mat, int rows, int cols)
{
    PyObject *l = PyList_New(rows);
    for (int i = 0; i < rows; i++)
    {
        PyList_SET_ITEM(l, i, makelist(mat[i], cols));
    }
    return l;
}

static PyObject *kmeans(PyObject *self, PyObject *args)
{
    int K_input;
    int iter;
    PyObject *k_array_input;
    PyObject *vectors_input;
    PyObject *item;
    double **k_array;
    double **vectors;
    PyObject *input_vector;
    int dim_input;
    int n, i, j;
    double EPS;

    /* This parses the Python arguments into a double (d)  variable named z and int (i) variable named n*/
    if (!PyArg_ParseTuple(args, "iiOOiid", &K_input, &iter, &k_array_input, &vectors_input, &dim_input, &n, &EPS))
    {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

    K = K_input;
    dim = dim_input;
    vectors_num = n;

    initLayer(&k_array, K, dim);
    for (i = 0; i < K; i++)
    {
        input_vector = PyList_GetItem(k_array_input, i);
        for (j = 0; j < dim; j++)
        {
            item = PyList_GetItem(input_vector, j);
            k_array[i][j] = PyFloat_AsDouble(item);

        }
    }

    initLayer(&vectors, n, dim);
    for (i = 0; i < n; i++)
    {
        input_vector = PyList_GetItem(vectors_input, i);
        for (j = 0; j < dim; j++)
        {
            item = PyList_GetItem(input_vector, j);
            vectors[i][j] = PyFloat_AsDouble(item);
   
        }
    }

    /* This builds the answer ("d" = Convert a C double to a Python floating point number) back into a python object */
    PyObject *res = makemat(fit(K, iter, k_array, vectors, dim, EPS), K, dim);
    free_2d(k_array, K);
    free_2d(vectors, n);
    return res; /*  Py_BuildValue(...) returns a PyObject*  */
}

static PyMethodDef kmeansMethods[] = {
    {"fit",                        /* the Python method name that will be used */
     (PyCFunction)kmeans,          /* the C-function that implements the Python function and returns static PyObject*  */
     METH_VARARGS,                 /* flags indicating parameters
     accepted for this function */
     PyDoc_STR("get the kmeans")}, /*  The docstring for the function */
    {NULL, NULL, 0, NULL}          /* The last entry must be all NULL as shown to act as a
                                      sentinel. Python looks for this entry to know that all
                                      of the functions for the module have been defined. */
};

static struct PyModuleDef mykmeanssp_module = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp", /* name of module */
    NULL,         /* module documentation, may be NULL */
    -1,           /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    kmeansMethods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&mykmeanssp_module);
    if (!m)
    {
        return NULL;
    }
    return m;
}