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

#if ! defined (octave_ov_bool_sparse_h)
#define octave_ov_bool_sparse_h 1

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

#include "boolSparse.h"
#include "ov-base-sparse.h"
#include "ov-re-sparse.h"

class octave_value_list;

extern template class OCTINTERP_EXTERN_TEMPLATE_API
octave_base_sparse<SparseBoolMatrix>;

class OCTINTERP_API octave_sparse_bool_matrix : public octave_base_sparse<SparseBoolMatrix>
{
public:

  octave_sparse_bool_matrix ()
    : octave_base_sparse<SparseBoolMatrix> () { }

  octave_sparse_bool_matrix (const SparseBoolMatrix& bnda)
    : octave_base_sparse<SparseBoolMatrix> (bnda) { }

  octave_sparse_bool_matrix (const SparseBoolMatrix& bnda,
                             const MatrixType& t)
    : octave_base_sparse<SparseBoolMatrix> (bnda, t) { }

  octave_sparse_bool_matrix (const boolNDArray& m)
    : octave_base_sparse<SparseBoolMatrix> (SparseBoolMatrix (m)) { }

  octave_sparse_bool_matrix (const boolMatrix& m)
    : octave_base_sparse<SparseBoolMatrix> (SparseBoolMatrix (m)) { }

  octave_sparse_bool_matrix (const Sparse<bool>& a)
    : octave_base_sparse<SparseBoolMatrix> (a) { }

  octave_sparse_bool_matrix (const octave_sparse_bool_matrix& bm)
    : octave_base_sparse<SparseBoolMatrix> (bm) { }

  ~octave_sparse_bool_matrix () = default;

  octave_base_value * clone () const
  { return new octave_sparse_bool_matrix (*this); }
  octave_base_value * empty_clone () const
  { return new octave_sparse_bool_matrix (); }

  type_conv_info numeric_conversion_function () const;

  // FIXME: Adapt idx_vector to allow sparse logical indexing without overflow!
  octave::idx_vector index_vector (bool /* require_integers */ = false) const
  {
    return octave::idx_vector (matrix);
  }

  builtin_type_t builtin_type () const { return btyp_bool; }

  bool is_bool_matrix () const { return true; }

  bool islogical () const { return true; }

  bool isreal () const { return true; }

  bool isnumeric () const { return false; }

  double double_value (bool = false) const;

  double scalar_value (bool frc_str_conv = false) const
  { return double_value (frc_str_conv); }

  Matrix matrix_value (bool = false) const;

  NDArray array_value (bool = false) const;

  Complex complex_value (bool = false) const;

  ComplexMatrix complex_matrix_value (bool = false) const;

  ComplexNDArray complex_array_value (bool = false) const;

  charNDArray char_array_value (bool = false) const;

  boolMatrix bool_matrix_value (bool = false) const;

  boolNDArray bool_array_value (bool = false) const;

  SparseMatrix sparse_matrix_value (bool = false) const;

  SparseComplexMatrix sparse_complex_matrix_value (bool = false) const;

  SparseBoolMatrix sparse_bool_matrix_value (bool = false) const
  { return matrix; }

  octave_value convert_to_str_internal (bool pad, bool force, char type) const;

  octave_value as_double () const;

  bool save_binary (std::ostream& os, bool save_as_floats);

  bool load_binary (std::istream& is, bool swap,
                    octave::mach_info::float_format fmt);

  bool save_hdf5 (octave_hdf5_id loc_id, const char *name, bool save_as_floats);

  bool load_hdf5 (octave_hdf5_id loc_id, const char *name);

  mxArray * as_mxArray (bool interleaved) const;

  // Mapper functions are converted to double for treatment
  octave_value map (unary_mapper_t umap) const
  {
    octave_sparse_matrix m (sparse_matrix_value ());
    return m.map (umap);
  }

protected:

  DECLARE_OV_TYPEID_FUNCTIONS_AND_DATA_API (OCTINTERP_API)
};

#endif
