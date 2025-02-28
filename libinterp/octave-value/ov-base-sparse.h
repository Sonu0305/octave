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

#if ! defined (octave_ov_base_sparse_h)
#define octave_ov_base_sparse_h 1

#include "octave-config.h"

#include <cstdlib>

#include <iosfwd>
#include <string>

#include "str-vec.h"

#include "error.h"
#include "ovl.h"
#include "ov-base.h"
#include "ov-typeinfo.h"

#include "boolSparse.h"
#include "MatrixType.h"

class octave_sparse_bool_matrix;

template <typename T>
class OCTINTERP_TEMPLATE_API octave_base_sparse : public octave_base_value
{
public:

  OCTINTERP_OVERRIDABLE_FUNC_API
  octave_base_sparse ()
    : octave_base_value (), matrix (), typ (MatrixType ())
  { }

  OCTINTERP_OVERRIDABLE_FUNC_API
  octave_base_sparse (const T& a)
    : octave_base_value (), matrix (a), typ (MatrixType ())
  {
    if (matrix.ndims () == 0)
      matrix.resize (dim_vector (0, 0));
  }

  OCTINTERP_OVERRIDABLE_FUNC_API
  octave_base_sparse (const T& a, const MatrixType& t)
    : octave_base_value (), matrix (a), typ (t)
  {
    if (matrix.ndims () == 0)
      matrix.resize (dim_vector (0, 0));
  }

  OCTINTERP_OVERRIDABLE_FUNC_API
  octave_base_sparse (const octave_base_sparse& a)
    : octave_base_value (), matrix (a.matrix), typ (a.typ) { }

  OCTINTERP_OVERRIDABLE_FUNC_API ~octave_base_sparse () = default;

  OCTINTERP_OVERRIDABLE_FUNC_API octave_idx_type numel () const
   { return dims ().safe_numel (); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_idx_type nnz () const
  { return matrix.nnz (); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_idx_type nzmax () const
  { return matrix.nzmax (); }

  OCTINTERP_OVERRIDABLE_FUNC_API std::size_t byte_size () const
  { return matrix.byte_size (); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value squeeze () const
  { return matrix.squeeze (); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value full_value () const
  { return matrix.matrix_value (); }

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
  subsasgn (const std::string& type, const std::list<octave_value_list>& idx,
            const octave_value& rhs);

  // FIXME: should we import the functions from the base class and
  // overload them here, or should we use a different name so we don't
  // have to do this?  Without the using declaration or a name change,
  // the base class functions will be hidden.  That may be OK, but it
  // can also cause some confusion.
  using octave_base_value::assign;

  template <typename RHS_T>
  void assign (const octave_value_list& idx, const RHS_T& rhs)
  {
    octave_idx_type len = idx.length ();

    // If we catch an indexing error in index_vector, we flag an error in
    // index k.  Ensure it is the right value before each idx_vector call.
    // Same variable as used in the for loop in the default case.

    octave_idx_type k = 0;

    try
      {
        switch (len)
          {
          case 1:
            {
              octave::idx_vector i = idx (0).index_vector ();

              matrix.assign (i, rhs);

              break;
            }

          case 2:
            {
              octave::idx_vector i = idx (0).index_vector ();

              k = 1;
              octave::idx_vector j = idx (1).index_vector ();

              matrix.assign (i, j, rhs);

              break;
            }

          default:
            error ("sparse indexing needs 1 or 2 indices");
          }
      }
    catch (octave::index_exception& ie)
      {
        // Rethrow to allow more info to be reported later.
        ie.set_pos_if_unset (len, k+1);
        throw;
      }

    // Invalidate matrix type.
    typ.invalidate_type ();
  }

  OCTINTERP_API void delete_elements (const octave_value_list& idx);

  OCTINTERP_OVERRIDABLE_FUNC_API dim_vector dims () const
  { return matrix.dims (); }

  OCTINTERP_API octave_value
  do_index_op (const octave_value_list& idx, bool resize_ok = false);

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value reshape (const dim_vector& new_dims) const
  { return T (matrix.reshape (new_dims)); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value
  permute (const Array<int>& vec, bool inv = false) const
  { return T (matrix.permute (vec, inv)); }

  OCTINTERP_API octave_value resize (const dim_vector& dv, bool = false) const;

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value all (int dim = 0) const
  { return matrix.all (dim); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value any (int dim = 0) const
  { return matrix.any (dim); }

  // We don't need to override both forms of the diag method.  The using
  // declaration will avoid warnings about partially-overloaded virtual
  // functions.
  using octave_base_value::diag;

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value diag (octave_idx_type k = 0) const
  { return octave_value (matrix.diag (k)); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value
  sort (octave_idx_type dim = 0, sortmode mode = ASCENDING) const
  { return octave_value (matrix.sort (dim, mode)); }

  OCTINTERP_OVERRIDABLE_FUNC_API octave_value
  sort (Array<octave_idx_type>& sidx, octave_idx_type dim = 0,
        sortmode mode = ASCENDING) const
  { return octave_value (matrix.sort (sidx, dim, mode)); }

  OCTINTERP_OVERRIDABLE_FUNC_API sortmode
  issorted (sortmode mode = UNSORTED) const
  { return full_value ().issorted (mode); }

  OCTINTERP_OVERRIDABLE_FUNC_API MatrixType matrix_type () const
  { return typ; }
  OCTINTERP_OVERRIDABLE_FUNC_API MatrixType
  matrix_type (const MatrixType& _typ) const
  { MatrixType ret = typ; typ = _typ; return ret; }

  OCTINTERP_OVERRIDABLE_FUNC_API bool is_matrix_type () const { return true; }

  OCTINTERP_OVERRIDABLE_FUNC_API bool isnumeric () const { return true; }

  OCTINTERP_OVERRIDABLE_FUNC_API bool issparse () const { return true; }

  OCTINTERP_OVERRIDABLE_FUNC_API bool is_defined () const { return true; }

  OCTINTERP_OVERRIDABLE_FUNC_API bool is_constant () const { return true; }

  OCTINTERP_API bool is_true () const;

  OCTINTERP_API bool print_as_scalar () const;

  OCTINTERP_API void print (std::ostream& os, bool pr_as_read_syntax = false);

  OCTINTERP_API void
  print_info (std::ostream& os, const std::string& prefix) const;

  OCTINTERP_API void
  print_raw (std::ostream& os, bool pr_as_read_syntax = false) const;

  OCTINTERP_API bool save_ascii (std::ostream& os);

  OCTINTERP_API bool load_ascii (std::istream& is);

  OCTINTERP_API float_display_format get_edit_display_format () const;

  OCTINTERP_API std::string
  edit_display (const float_display_format& fmt,
                octave_idx_type i, octave_idx_type j) const;

  // These functions exists to support the MEX interface.
  // You should not use them anywhere else.
  OCTINTERP_OVERRIDABLE_FUNC_API const void * mex_get_data () const
  { return matrix.data (); }

  OCTINTERP_OVERRIDABLE_FUNC_API const octave_idx_type * mex_get_ir () const
  { return matrix.ridx (); }

  OCTINTERP_OVERRIDABLE_FUNC_API const octave_idx_type * mex_get_jc () const
  { return matrix.cidx (); }

  OCTINTERP_API octave_value fast_elem_extract (octave_idx_type n) const;

protected:

  OCTINTERP_API octave_value
  map (octave_base_value::unary_mapper_t umap) const;

  T matrix;

  mutable MatrixType typ;
};

#endif
