////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2008-2024 The Octave Project Developers
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

#if ! defined (octave_ov_base_diag_h)
#define octave_ov_base_diag_h 1

#include "octave-config.h"

#include <cstdlib>

#include <iosfwd>
#include <string>

#include "mx-base.h"
#include "str-vec.h"

#include "ovl.h"
#include "ov-base.h"
#include "ov-typeinfo.h"

// Real matrix values.

template <typename DMT, typename MT>
class OCTINTERP_TEMPLATE_API octave_base_diag : public octave_base_value
{

public:

  OCTINTERP_OVERRIDABLE_FUNC_API 
  octave_base_diag ()
    : octave_base_value (), m_matrix (), m_dense_cache () { }

  OCTINTERP_OVERRIDABLE_FUNC_API 
  octave_base_diag (const DMT& m)
    : octave_base_value (), m_matrix (m), m_dense_cache () { }

  OCTINTERP_OVERRIDABLE_FUNC_API 
  octave_base_diag (const octave_base_diag& m)
    : octave_base_value (), m_matrix (m.m_matrix), m_dense_cache () { }

  OCTINTERP_OVERRIDABLE_FUNC_API  ~octave_base_diag () = default;

  OCTINTERP_OVERRIDABLE_FUNC_API std::size_t byte_size () const
  { return m_matrix.byte_size (); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value squeeze () const
  { return m_matrix; }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value full_value () const
  { return to_dense (); }

  // We don't need to override all three forms of subsref.  The using
  // declaration will avoid warnings about partially-overloaded virtual
  // functions.
  using octave_base_value::subsref;

  OCTINTERP_API octave_value
  subsref (const std::string& type, const std::list<octave_value_list>& idx);

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value_list
  subsref (const std::string& type, const std::list<octave_value_list>& idx,
           int)
  { return subsref (type, idx); }

  OCTINTERP_API octave_value
  do_index_op (const octave_value_list& idx, bool resize_ok = false);

  OCTINTERP_API octave_value
  subsasgn (const std::string& type, const std::list<octave_value_list>& idx,
            const octave_value& rhs);

  OCTINTERP_OVERRIDABLE_FUNC_API dim_vector dims () const
  { return m_matrix.dims (); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_idx_type nnz () const
  { return diag ().nnz (); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value reshape (const dim_vector& new_dims) const
  { return to_dense ().reshape (new_dims); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value
  permute (const Array<int>& vec, bool inv = false) const
  {
    if (vec.numel () == 2
        && ((vec.xelem (0) == 1 && vec.xelem (1) == 0)
            || (vec.xelem (0) == 0 && vec.xelem (1) == 1)))
      return DMT (m_matrix);
    else
      return to_dense ().permute (vec, inv);
  }

  OCTINTERP_API octave_value
  resize (const dim_vector& dv, bool fill = false) const;

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value all (int dim = 0) const
  { return MT (m_matrix).all (dim); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value any (int dim = 0) const
  { return MT (m_matrix).any (dim); }

  OCTINTERP_OVERRIDABLE_FUNC_API MatrixType matrix_type () const
  { return MatrixType::Diagonal; }
  OCTINTERP_OVERRIDABLE_FUNC_API MatrixType matrix_type (const MatrixType&) const
  { return matrix_type (); }

  // We don't need to override both forms of the diag method.  The using
  // declaration will avoid warnings about partially-overloaded virtual
  // functions.
  using octave_base_value::diag;

  OCTINTERP_API octave_value diag (octave_idx_type k = 0) const;

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value
  sort (octave_idx_type dim = 0, sortmode mode = ASCENDING) const
  { return to_dense ().sort (dim, mode); }
  OCTINTERP_OVERRIDABLE_FUNC_API octave_value
  sort (Array<octave_idx_type>& sidx, octave_idx_type dim = 0,
        sortmode mode = ASCENDING) const
  { return to_dense ().sort (sidx, dim, mode); }

  OCTINTERP_OVERRIDABLE_FUNC_API sortmode
  issorted (sortmode mode = UNSORTED) const
  { return to_dense ().issorted (mode); }

  OCTINTERP_OVERRIDABLE_FUNC_API Array<octave_idx_type>
  sort_rows_idx (sortmode mode = ASCENDING) const
  { return to_dense ().sort_rows_idx (mode); }

  OCTINTERP_OVERRIDABLE_FUNC_API sortmode
 is_sorted_rows (sortmode mode = UNSORTED) const
  { return to_dense ().is_sorted_rows (mode); }

  OCTINTERP_OVERRIDABLE_FUNC_API bool is_matrix_type () const
  { return true; }

  OCTINTERP_OVERRIDABLE_FUNC_API bool isnumeric () const { return true; }

  OCTINTERP_OVERRIDABLE_FUNC_API bool is_defined () const { return true; }

  OCTINTERP_OVERRIDABLE_FUNC_API bool is_constant () const { return true; }

  OCTINTERP_API bool is_true () const;

  OCTINTERP_OVERRIDABLE_FUNC_API bool is_diag_matrix () const { return true; }

  OCTINTERP_API double double_value (bool = false) const;

  OCTINTERP_API float float_value (bool = false) const;

  OCTINTERP_OVERRIDABLE_FUNC_API double
  scalar_value (bool frc_str_conv = false) const
  { return double_value (frc_str_conv); }

  OCTINTERP_API octave::idx_vector
  index_vector (bool /* require_integers */ = false) const;

  OCTINTERP_API Matrix matrix_value (bool = false) const;

  OCTINTERP_API FloatMatrix float_matrix_value (bool = false) const;

  OCTINTERP_API Complex complex_value (bool = false) const;

  OCTINTERP_API FloatComplex float_complex_value (bool = false) const;

  OCTINTERP_API ComplexMatrix complex_matrix_value (bool = false) const;

  OCTINTERP_API FloatComplexMatrix
  float_complex_matrix_value (bool = false) const;

  OCTINTERP_API ComplexNDArray complex_array_value (bool = false) const;

  OCTINTERP_API FloatComplexNDArray
  float_complex_array_value (bool = false) const;

  OCTINTERP_API boolNDArray bool_array_value (bool warn = false) const;

  OCTINTERP_API charNDArray char_array_value (bool = false) const;

  OCTINTERP_API NDArray array_value (bool = false) const;

  OCTINTERP_API FloatNDArray float_array_value (bool = false) const;

  OCTINTERP_API SparseMatrix sparse_matrix_value (bool = false) const;

  OCTINTERP_API SparseComplexMatrix
  sparse_complex_matrix_value (bool = false) const;

  OCTINTERP_OVERRIDABLE_FUNC_API int8NDArray int8_array_value () const
  { return to_dense ().int8_array_value (); }

  OCTINTERP_OVERRIDABLE_FUNC_API int16NDArray int16_array_value () const
  { return to_dense ().int16_array_value (); }

  OCTINTERP_OVERRIDABLE_FUNC_API int32NDArray int32_array_value () const
  { return to_dense ().int32_array_value (); }

  OCTINTERP_OVERRIDABLE_FUNC_API int64NDArray int64_array_value () const
  { return to_dense ().int64_array_value (); }

  OCTINTERP_OVERRIDABLE_FUNC_API uint8NDArray uint8_array_value () const
  { return to_dense ().uint8_array_value (); }

  OCTINTERP_OVERRIDABLE_FUNC_API uint16NDArray uint16_array_value () const
  { return to_dense ().uint16_array_value (); }

  OCTINTERP_OVERRIDABLE_FUNC_API uint32NDArray uint32_array_value () const
  { return to_dense ().uint32_array_value (); }

  OCTINTERP_OVERRIDABLE_FUNC_API uint64NDArray uint64_array_value () const
  { return to_dense ().uint64_array_value (); }

  OCTINTERP_API octave_value
  convert_to_str_internal (bool pad, bool force, char type) const;

  OCTINTERP_API void
  print_raw (std::ostream& os, bool pr_as_read_syntax = false) const;

  OCTINTERP_API float_display_format get_edit_display_format () const;

  OCTINTERP_API std::string
  edit_display (const float_display_format& fmt,
                octave_idx_type i, octave_idx_type j) const;

  OCTINTERP_API bool save_ascii (std::ostream& os);

  OCTINTERP_API bool load_ascii (std::istream& is);

  OCTINTERP_API int
  write (octave::stream& os, int block_size,
         oct_data_conv::data_type output_type, int skip,
         octave::mach_info::float_format flt_fmt) const;

  OCTINTERP_API mxArray * as_mxArray (bool interleaved) const;

  OCTINTERP_API bool print_as_scalar () const;

  OCTINTERP_API void print (std::ostream& os, bool pr_as_read_syntax = false);

  OCTINTERP_API void
  print_info (std::ostream& os, const std::string& prefix) const;

  OCTINTERP_API void short_disp (std::ostream& os) const;

  OCTINTERP_API octave_value fast_elem_extract (octave_idx_type n) const;

protected:

  DMT m_matrix;

  OCTINTERP_API octave_value to_dense () const;

  virtual bool chk_valid_scalar (const octave_value&,
                                 typename DMT::element_type&) const = 0;

private:

  mutable octave_value m_dense_cache;

};

#endif
