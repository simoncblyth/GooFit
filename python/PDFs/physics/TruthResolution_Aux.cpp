#include <goofit/Python.h>

#include <pybind11/stl.h>

#include <goofit/PDFs/physics/TruthResolution_Aux.h>
#include <goofit/Variable.h>
#include <goofit/docs/PDFs/physics/TruthResolution_Aux.h>

using namespace GooFit;

void init_TruthResolution(py::module &m) {
    py::class_<TruthResolution, MixingTimeResolution>(m, "TruthResolution")
        .def(py::init<>())
        .def_static("help", []() { return HelpPrinter(TruthResolution_docs); })

        ;
}