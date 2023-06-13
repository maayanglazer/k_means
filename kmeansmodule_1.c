#define PY_SSIZE_T_CLEAN
#include <Python.h>

void fit(int K)
{
    printf("what what \n");
}

typedef struct _object
{
    Py_ssize_t ob_refcnt;
    struct _typeobject *ob_type;
} PyObject;

static PyObject *kmeans(PyObject *self, PyObject *args)
{
    int K;
    /* This parses the Python arguments into a double (d)  variable named z and int (i) variable named n*/
    if (!PyArg_ParseTuple(args, "di", &K))
    {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

    /* This builds the answer ("d" = Convert a C double to a Python floating point number) back into a python object */
    return Py_BuildValue("d", fit(K)); /*  Py_BuildValue(...) returns a PyObject*  */
}

static PyMethodDef kmeansMethods[] = {
    {"fit",                        /* the Python method name that will be used */
     (PyCFunction)fit,             /* the C-function that implements the Python function and returns static PyObject*  */
     METH_VARARGS,                 /* flags indicating parameters
     accepted for this function */
     PyDoc_STR("get the kmeans")}, /*  The docstring for the function */
    {NULL, NULL, 0, NULL}          /* The last entry must be all NULL as shown to act as a
                                      sentinel. Python looks for this entry to know that all
                                      of the functions for the module have been defined. */
};

static struct PyModuleDef mykmeanssp = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp", /* name of module */
    NULL,         /* module documentation, may be NULL */
    -1,           /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    kmeansMethods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&mykmeanssp);
    if (!m)
    {
        return NULL;
    }
    return m;
}