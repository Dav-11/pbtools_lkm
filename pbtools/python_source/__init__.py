import os

from ..parser import SCALAR_VALUE_TYPES
from ..parser import camel_to_snake_case


SOURCE_FMT = '''\
/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * This file was generated by pbtools.
 */

#include <Python.h>
#include "c/{name}.h"
#include "pbtools_py.h"

static PyMethodDef methods[] = {{
{methods}\
    {{ NULL }}
}};

static PyModuleDef module = {{
    PyModuleDef_HEAD_INIT,
    .m_name = "{name}",
    .m_doc = NULL,
    .m_size = -1,
    .m_methods = methods
}};

PyMODINIT_FUNC PyInit_{name}(void)
{{
    PyObject *module_p;

    module_p = PyModule_Create(&module);

    if (module_p == NULL) {{
        return (NULL);
    }}

    return (module_p);
}}
'''


class Generator:

    def __init__(self, namespace, parsed):
        if parsed.package is not None:
            namespace = camel_to_snake_case(parsed.package)

        self.namespace = namespace
        self.parsed = parsed

    def generate(self):
        return SOURCE_FMT.format(name=self.namespace,
                                 methods='')


def generate(name, parsed):
    """Generate Python source code from given parsed proto-file.

    """

    return Generator(name, parsed).generate()
