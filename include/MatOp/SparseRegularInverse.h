// Copyright (C) 2017 Yixuan Qiu <yixuan.qiu@cos.name>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef SPARSE_REGULAR_INVERSE_H
#define SPARSE_REGULAR_INVERSE_H

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>
#include <stdexcept>

namespace Spectra {


///
/// \ingroup MatOp
///
/// This class defines matrix operations related to a generalized eigen solver
/// in the regular inverse mode. For a generalized eigenvalue problem \f$Ax=\lambda Bx\f$,
/// where \f$A\f$ is symmetric and \f$B\f$ is sparse and positive definite,
/// it implements the matrix-vector product \f$y=Bx\f$ and the linear equation
/// solving operation \f$y=B^{-1}x\f$.
///
/// This class is intended to be used with the SymGEigsSolver generalized eigen solver
/// in the regular inverse mode.
///
template <typename Scalar, int Uplo = Eigen::Lower, int Flags = 0, typename StorageIndex = int>
class SparseRegularInverse
{
private:
    typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Vector;
    typedef Eigen::Map<const Vector> MapConstVec;
    typedef Eigen::Map<Vector> MapVec;
    typedef Eigen::SparseMatrix<Scalar, Flags, StorageIndex> SparseMatrix;

    const int m_n;
    const SparseMatrix& m_mat;
    Eigen::ConjugateGradient<SparseMatrix> m_cg;

public:
    ///
    /// Constructor to create the matrix operation object.
    ///
    /// \param mat_ An **Eigen** sparse matrix object, whose type is
    /// `Eigen::SparseMatrix<Scalar, ...>`.
    ///
    SparseRegularInverse(const SparseMatrix& mat_) :
        m_n(mat_.rows()), m_mat(mat_)
    {
        if(mat_.rows() != mat_.cols())
            throw std::invalid_argument("SparseRegularInverse: matrix must be square");

        m_cg.compute(mat_);
    }

    ///
    /// Return the number of rows of the underlying matrix.
    ///
    int rows() const { return m_n; }
    ///
    /// Return the number of columns of the underlying matrix.
    ///
    int cols() const { return m_n; }

    ///
    /// Perform the solving operation \f$y=B^{-1}x\f$.
    ///
    /// \param x_in  Pointer to the \f$x\f$ vector.
    /// \param y_out Pointer to the \f$y\f$ vector.
    ///
    // y_out = inv(B) * x_in
    void solve(const Scalar* x_in, Scalar* y_out) const
    {
        MapConstVec x(x_in,  m_n);
        MapVec      y(y_out, m_n);
        y.noalias() = m_cg.solve(x);
    }

    ///
    /// Perform the matrix-vector multiplication operation \f$y=Bx\f$.
    ///
    /// \param x_in  Pointer to the \f$x\f$ vector.
    /// \param y_out Pointer to the \f$y\f$ vector.
    ///
    // y_out = B * x_in
    void mat_prod(const Scalar* x_in, Scalar* y_out) const
    {
        MapConstVec x(x_in,  m_n);
        MapVec      y(y_out, m_n);
        y.noalias() = m_mat.template selfadjointView<Uplo>() * x;
    }
};


} // namespace Spectra

#endif // SPARSE_REGULAR_INVERSE_H
