#include <vector>

extern "C" {
#include <Python.h>
}

namespace matrixops {	
	typedef std::vector<double>  row_t;
	typedef std::vector<row_t>   matrix_t;

	static matrix_t dot(const matrix_t &a)
	{
		matrix_t res;
		res.resize(a.size());

		for (size_t i = 0; i<a.size(); ++i)
		{
			res[i].resize(a.size());
			for (size_t j = 0; j < a.size(); ++j) 
				res[i][j] = a[i][j];
		}

		for (size_t m = 0; m < res.size(); ++m)
		for (size_t i = 0; i < res.size(); ++i) 
			for (size_t j = 0; j < res.size(); ++j) 
			{
				if ((res[i][m] == 0 && res[m][j] == 0) || res[i][j]==0)
					res[i][j] = 0;
				else
					res[i][j] = 1. / (1. / res[i][j] + 1. / (res[i][m] + res[m][j]) );
				}			
		return res;
	}
}

static matrixops::matrix_t pyobject_to_cxx(PyObject * py_matrix)
{
	matrixops::matrix_t result;
	result.resize(PyObject_Length(py_matrix));
	for (size_t i = 0; i<result.size(); ++i) {
		PyObject * py_row = PyList_GetItem(py_matrix, i);
		matrixops::row_t & row = result[i];
		row.resize(PyObject_Length(py_row));
		for (size_t j = 0; j<row.size(); ++j) {
			PyObject * py_elem = PyList_GetItem(py_row, j);
			const double elem = PyFloat_AsDouble(py_elem);
			row[j] = elem;
		}
	}
	return result;
}

static PyObject * cxx_to_pyobject(const matrixops::matrix_t &matrix)
{
	PyObject * result = PyList_New(matrix.size());
	for (size_t i = 0; i<matrix.size(); ++i) {
		const matrixops::row_t & row = matrix[i];
		PyObject * py_row = PyList_New(row.size());
		PyList_SetItem(result, i, py_row);
		for (size_t j = 0; j<row.size(); ++j) {
			const double elem = row[j];
			PyObject * py_elem = PyFloat_FromDouble(elem);
			PyList_SetItem(py_row, j, py_elem);
		}
	}
	return result;
}

static PyObject * algorithm_Fl_Warh(PyObject * module, PyObject * args)
{
	PyObject * py_a = PyTuple_GetItem(args, 0);

	/* Convert to C++ structure */
	const matrixops::matrix_t a = pyobject_to_cxx(py_a);

	/* Perform calculations */
	const matrixops::matrix_t result = matrixops::dot(a);

	/* Convert back to Python object */
	PyObject * py_result = cxx_to_pyobject(result);
	return py_result;
}

PyMODINIT_FUNC PyInit_matrixops()
{
	static PyMethodDef ModuleMethods[] = {
		{ "algorithm_Fl_Warh", algorithm_Fl_Warh, METH_VARARGS, "Floyd–Warshall algorithm" },
		{ NULL, NULL, 0, NULL }
	};
	static PyModuleDef ModuleDef = {
		PyModuleDef_HEAD_INIT,
		"matrixops",
		"resistance for nodes",
		-1, ModuleMethods, 
		NULL, NULL, NULL, NULL
	};
	PyObject * module = PyModule_Create(&ModuleDef);
	return module;
}