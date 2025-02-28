////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 1999-2024 The Octave Project Developers
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

#if ! defined (octave_pt_cell_h)
#define octave_pt_cell_h 1

#include "octave-config.h"

#include <iosfwd>

class octave_value;
class octave_value_list;

#include "pt-mat.h"
#include "pt-walk.h"

OCTAVE_BEGIN_NAMESPACE(octave)

class symbol_scope;
class tree_argument_list;

// General cells.

class tree_cell : public tree_array_list
{
public:

  tree_cell (tree_argument_list *row = nullptr)
    : tree_array_list (row)
  { }

  OCTAVE_DISABLE_COPY_MOVE (tree_cell)

  ~tree_cell () = default;

  bool iscell () const { return true; }

  bool rvalue_ok () const { return true; }

  tree_expression * dup (symbol_scope& scope) const;

  octave_value evaluate (tree_evaluator&, int nargout = 1);

  octave_value_list evaluate_n (tree_evaluator& tw, int nargout = 1)
  {
    return ovl (evaluate (tw, nargout));
  }

  void accept (tree_walker& tw)
  {
    tw.visit_cell (*this);
  }
};

OCTAVE_END_NAMESPACE(octave)

#endif
