////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 1998-2024 The Octave Project Developers
//
// See the file COPYRIGHT.md in the top-level directory of this
// distribution or <https://octave.org/copyright/>.
//
// This file is part of Octave.
//
// Octave is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Octave is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Octave; see the file COPYING.  If not, see
// <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////

#if ! defined (octave_ov_re_sparse_h)
#define octave_ov_re_sparse_h 1

#include "octave-config.h"

#include <cstdlib>

#include <iosfwd>
#include <string>

#include "mx-base.h"
#include "str-vec.h"

#include "error.h"
#include "oct-stream.h"
#include "ov-base.h"
#include "ov-typeinfo.h"

#include "dSparse.h"
#include "MatrixType.h"
#include "ov-base-sparse.h"
#include "ov-cx-sparse.h"

class octave_value_list;

extern template class OCTINTERP_EXTERN_TEMPLATE_API
octave_base_sparse<SparseMatrix>;

class OCTINTERP_API octave_sparse_matrix : public octave_base_sparse<SparseMatrix>
{
public:

  octave_sparse_matrix ()
    : octave_base_sparse<SparseMatrix> () { }

  octave_sparse_matrix (const Matrix& m)
    : octave_base_sparse<SparseMatrix> (SparseMatrix (m)) { }

  octave_sparse_matrix (const NDArray& m)
    : octave_base_sparse<SparseMatrix> (SparseMatrix (m)) { }

  octave_sparse_matrix (const SparseMatrix& m)
    : octave_base_sparse<SparseMatrix> (m) { }

  octave_sparse_matrix (const SparseMatrix& m, const MatrixType& t)
    : octave_base_sparse<SparseMatrix> (m, t) { }

  octave_sparse_matrix (const MSparse<double>& m)
    : octave_base_sparse<SparseMatrix> (m) { }

  octave_sparse_matrix (const MSparse<double>& m, const MatrixType& t)
    : octave_base_sparse<SparseMatrix> (m, t) { }

  octave_sparse_matrix (const Sparse<double>& m)
    : octave_base_sparse<SparseMatrix> (SparseMatrix (m)) { }

  octave_sparse_matrix (const Sparse<double>& m, const MatrixType& t)
    : octave_base_sparse<SparseMatrix> (SparseMatrix (m), t) { }

  octave_sparse_matrix (const octave_sparse_matrix& m)
    : octave_base_sparse<SparseMatrix> (m) { }

  ~octave_sparse_matrix () = default;

  octave_base_value * clone () const
  { return new octave_sparse_matrix (*this); }
  octave_base_value * empty_clone () const
  { return new octave_sparse_matrix (); }

  octave::idx_vector index_vector (bool require_integers = false) const;

  builtin_type_t builtin_type () const { return btyp_double; }

  bool is_real_matrix () const { return true; }

  bool isreal () const { return true; }

  bool is_double_type () const { return true; }

  bool isfloat () const { return true; }

  double double_value (bool = false) const;

  double scalar_value (bool frc_str_conv = false) const
  { return double_value (frc_str_conv); }

  Matrix matrix_value (bool = false) const;

  Complex complex_value (bool = false) const;

  boolNDArray bool_array_value (bool warn = false) const;

  charNDArray char_array_value (bool = false) const;

  ComplexMatrix complex_matrix_value (bool = false) const;

  ComplexNDArray complex_array_value (bool = false) const;

  NDArray array_value (bool = false) const;

  SparseMatrix sparse_matrix_value (bool = false) const
  { return matrix; }

  SparseComplexMatrix sparse_complex_matrix_value (bool = false) const
  { return SparseComplexMatrix (matrix); }

  SparseBoolMatrix sparse_bool_matrix_value (bool warn = false) const;

  octave_value convert_to_str_internal (bool pad, bool force, char type) const;

  octave_value as_double () const;

  bool save_binary (std::ostream& os, bool save_as_floats);

  bool load_binary (std::istream& is, bool swap,
                    octave::mach_info::float_format fmt);

  bool save_hdf5 (octave_hdf5_id loc_id, const char *name, bool save_as_floats);

  bool load_hdf5 (octave_hdf5_id loc_id, const char *name);

  mxArray * as_mxArray (bool interleaved) const;

  octave_value map (unary_mapper_t umap) const;

private:
  octave_value map (double (*fcn) (double)) const;

  DECLARE_OV_TYPEID_FUNCTIONS_AND_DATA_API (OCTINTERP_API)
};

#endif
