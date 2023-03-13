//
// Copyright (c) 2022 INRIA
//

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <proxsuite/proxqp/dense/wrapper.hpp>
#include <proxsuite/proxqp/sparse/wrapper.hpp>
#include <proxsuite/proxqp/status.hpp>
#include <proxsuite/serialization/archive.hpp>
#include <proxsuite/serialization/wrapper.hpp>

namespace proxsuite {
namespace proxqp {
using proxsuite::linalg::veg::isize;

namespace dense {

namespace python {

template<typename T>
void
exposeQpObjectDense(pybind11::module_ m)
{

  ::pybind11::class_<dense::QP<T>>(m, "QP")
    .def(::pybind11::init<i64, i64, i64>(),
         pybind11::arg_v("n", 0, "primal dimension."),
         pybind11::arg_v("n_eq", 0, "number of equality constraints."),
         pybind11::arg_v("n_in", 0, "number of inequality constraints."),
         "Default constructor using QP model dimensions.") // constructor
    .def_readwrite(
      "results",
      &dense::QP<T>::results,
      "class containing the solution or certificate of infeasibility, "
      "and "
      "information statistics in an info subclass.")
    .def_readwrite(
      "settings", &dense::QP<T>::settings, "Settings of the solver.")
    .def_readwrite(
      "model", &dense::QP<T>::model, "class containing the QP model")

    .def("init",
         static_cast<void (dense::QP<T>::*)(optional<dense::MatRef<T>>,
                                            optional<dense::VecRef<T>>,
                                            optional<dense::MatRef<T>>,
                                            optional<dense::VecRef<T>>,
                                            optional<dense::MatRef<T>>,
                                            optional<dense::VecRef<T>>,
                                            optional<dense::VecRef<T>>,
                                            bool compute_preconditioner,
                                            optional<T>,
                                            optional<T>,
                                            optional<T>)>(&dense::QP<T>::init),
         "function for initialize the QP model.",
         pybind11::arg_v("H", nullopt, "quadratic cost"),
         pybind11::arg_v("g", nullopt, "linear cost"),
         pybind11::arg_v("A", nullopt, "equality constraint matrix"),
         pybind11::arg_v("b", nullopt, "equality constraint vector"),
         pybind11::arg_v("C", nullopt, "inequality constraint matrix"),
         pybind11::arg_v("l", nullopt, "upper inequality constraint vector"),
         pybind11::arg_v("u", nullopt, "lower inequality constraint vector"),
         pybind11::arg_v("compute_preconditioner",
                         true,
                         "execute the preconditioner for reducing "
                         "ill-conditioning and speeding up solver execution."),
         pybind11::arg_v("rho", nullopt, "primal proximal parameter"),
         pybind11::arg_v(
           "mu_eq", nullopt, "dual equality constraint proximal parameter"),
         pybind11::arg_v(
           "mu_in", nullopt, "dual inequality constraint proximal parameter"))
    .def("solve",
         static_cast<void (dense::QP<T>::*)()>(&dense::QP<T>::solve),
         "function used for solving the QP problem, using default parameters.")
    .def("solve",
         static_cast<void (dense::QP<T>::*)(optional<dense::VecRef<T>> x,
                                            optional<dense::VecRef<T>> y,
                                            optional<dense::VecRef<T>> z)>(
           &dense::QP<T>::solve),
         "function used for solving the QP problem, when passing a warm start.")

    .def(
      "update",
      static_cast<void (dense::QP<T>::*)(optional<dense::MatRef<T>>,
                                         optional<dense::VecRef<T>>,
                                         optional<dense::MatRef<T>>,
                                         optional<dense::VecRef<T>>,
                                         optional<dense::MatRef<T>>,
                                         optional<dense::VecRef<T>>,
                                         optional<dense::VecRef<T>>,
                                         bool update_preconditioner,
                                         optional<T>,
                                         optional<T>,
                                         optional<T>)>(&dense::QP<T>::update),
      "function used for updating matrix or vector entry of the model using "
      "dense matrix entries.",
      pybind11::arg_v("H", nullopt, "quadratic cost"),
      pybind11::arg_v("g", nullopt, "linear cost"),
      pybind11::arg_v("A", nullopt, "equality constraint matrix"),
      pybind11::arg_v("b", nullopt, "equality constraint vector"),
      pybind11::arg_v("C", nullopt, "inequality constraint matrix"),
      pybind11::arg_v("l", nullopt, "upper inequality constraint vector"),
      pybind11::arg_v("u", nullopt, "lower inequality constraint vector"),
      pybind11::arg_v(
        "update_preconditioner",
        true,
        "update the preconditioner considering new matrices entries for "
        "reducing ill-conditioning and speeding up solver execution. If set up "
        "to false, use previous derived preconditioner."),
      pybind11::arg_v("rho", nullopt, "primal proximal parameter"),
      pybind11::arg_v(
        "mu_eq", nullopt, "dual equality constraint proximal parameter"),
      pybind11::arg_v(
        "mu_in", nullopt, "dual inequality constraint proximal parameter"))
    .def("cleanup",
         &dense::QP<T>::cleanup,
         "function used for cleaning the workspace and result "
         "classes.")
    .def(pybind11::self == pybind11::self)
    .def(pybind11::self != pybind11::self)
    .def(pybind11::pickle(

      [](const dense::QP<T>& qp) {
        return pybind11::bytes(proxsuite::serialization::saveToString(qp));
      },
      [](pybind11::bytes& s) {
        proxsuite::proxqp::dense::QP<T> qp(1, 1, 1);
        proxsuite::serialization::loadFromString(qp, s);
        return qp;
      }));
  ;
}
} // namespace python
} // namespace dense

namespace sparse {

namespace python {

template<typename T, typename I>
void
exposeQpObjectSparse(pybind11::module_ m)
{

  ::pybind11::class_<sparse::QP<T, I>>(m, "QP") //,pybind11::module_local()
    .def(::pybind11::init<i64, i64, i64>(),
         pybind11::arg_v("n", 0, "primal dimension."),
         pybind11::arg_v("n_eq", 0, "number of equality constraints."),
         pybind11::arg_v("n_in", 0, "number of inequality constraints."),
         "Constructor using QP model dimensions.") // constructor
    .def(
      ::pybind11::init<const sparse::SparseMat<bool, I>&,
                       const sparse::SparseMat<bool, I>&,
                       const sparse::SparseMat<bool, I>&>(),
      pybind11::arg_v("H_mask", nullopt, "mask of the quadratic cost."),
      pybind11::arg_v(
        "A_mask", nullopt, "mask of the equality constraint matrix."),
      pybind11::arg_v("C_mask", 0, "mask of the inequality constraint matrix."),
      "Constructor using QP model sparsity structure.") // constructor
    .def_readwrite(
      "model", &sparse::QP<T, I>::model, "class containing the QP model")
    .def_readwrite(
      "results",
      &sparse::QP<T, I>::results,
      "class containing the solution or certificate of infeasibility, "
      "and "
      "information statistics in an info subclass.")
    .def_readwrite(
      "settings", &sparse::QP<T, I>::settings, "Settings of the solver.")
    .def("init",
         &sparse::QP<T, I>::init,
         "function for initializing the model when passing sparse matrices in "
         "entry.",
         pybind11::arg_v("H", nullopt, "quadratic cost"),
         pybind11::arg_v("g", nullopt, "linear cost"),
         pybind11::arg_v("A", nullopt, "equality constraint matrix"),
         pybind11::arg_v("b", nullopt, "equality constraint vector"),
         pybind11::arg_v("C", nullopt, "inequality constraint matrix"),
         pybind11::arg_v("l", nullopt, "upper inequality constraint vector"),
         pybind11::arg_v("u", nullopt, "lower inequality constraint vector"),
         pybind11::arg_v("compute_preconditioner",
                         true,
                         "execute the preconditioner for reducing "
                         "ill-conditioning and speeding up solver execution."),
         pybind11::arg_v("rho", nullopt, "primal proximal parameter"),
         pybind11::arg_v(
           "mu_eq", nullopt, "dual equality constraint proximal parameter"),
         pybind11::arg_v(
           "mu_in", nullopt, "dual inequality constraint proximal parameter"))

    .def("update",
         &sparse::QP<T, I>::update,
         "function for updating the model when passing sparse matrices in "
         "entry.",
         pybind11::arg_v("H", nullopt, "quadratic cost"),
         pybind11::arg_v("g", nullopt, "linear cost"),
         pybind11::arg_v("A", nullopt, "equality constraint matrix"),
         pybind11::arg_v("b", nullopt, "equality constraint vector"),
         pybind11::arg_v("C", nullopt, "inequality constraint matrix"),
         pybind11::arg_v("l", nullopt, "upper inequality constraint vector"),
         pybind11::arg_v("u", nullopt, "lower inequality constraint vector"),
         pybind11::arg_v(
           "update_preconditioner",
           true,
           "update the preconditioner or re-use previous derived for reducing "
           "ill-conditioning and speeding up solver execution."),
         pybind11::arg_v("rho", nullopt, "primal proximal parameter"),
         pybind11::arg_v(
           "mu_eq", nullopt, "dual equality constraint proximal parameter"),
         pybind11::arg_v(
           "mu_in", nullopt, "dual inequality constraint proximal parameter"))
    .def("solve",
         static_cast<void (sparse::QP<T, I>::*)()>(&sparse::QP<T, I>::solve),
         "function used for solving the QP problem, using default parameters.")
    .def("solve",
         static_cast<void (sparse::QP<T, I>::*)(optional<sparse::VecRef<T>> x,
                                                optional<sparse::VecRef<T>> y,
                                                optional<sparse::VecRef<T>> z)>(
           &sparse::QP<T, I>::solve),
         "function used for solving the QP problem, when passing a warm start.")
    .def("cleanup",
         &sparse::QP<T, I>::cleanup,
         "function used for cleaning the result "
         "class.");
}

} // namespace python
} // namespace sparse

} // namespace proxqp
} // namespace proxsuite
