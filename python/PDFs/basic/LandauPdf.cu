#include <pybind11/pybind11.h>

#include <goofit/PDFs/basic/LandauPdf.h>
#include <goofit/Variable.h>

using namespace GooFit;
namespace py = pybind11;

void init_LandauPdf(py::module &m) {
    py::class_<LandauPdf, GooPdf>(m, "LandauPdf")
            .def(py::init<std::string, Variable *, Variable *, Variable *>(),
                 py::keep_alive<1,3>(),
                 py::keep_alive<1,4>(),
                 py::keep_alive<1,5>());
}
