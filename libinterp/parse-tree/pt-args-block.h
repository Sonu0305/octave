////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2021-2024 The Octave Project Developers
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

#if ! defined (octave_pt_args_block_h)
#define octave_pt_args_block_h 1

#include "octave-config.h"

#include <list>

#include "pt-arg-list.h"
#include "pt-cmd.h"
#include "pt-exp.h"
#include "pt-id.h"
#include "pt-walk.h"

// FIXME: We could maybe re-think the naming of some of these objects
// before releasing a version that contains these new classes...

OCTAVE_BEGIN_NAMESPACE(octave)

class comment_list;

class tree_arg_size_spec
{
public:

  tree_arg_size_spec (tree_argument_list *size_args)
    : m_size_args (size_args)
  { }

  OCTAVE_DISABLE_CONSTRUCT_COPY_MOVE (tree_arg_size_spec)

  ~tree_arg_size_spec ()
  {
    delete m_size_args;
  }

  tree_argument_list * size_args () { return m_size_args; }

  void accept (tree_walker& tw)
  {
    tw.visit_arg_size_spec (*this);
  }

private:

  tree_argument_list *m_size_args;
};

class tree_arg_validation_fcns
{
public:

  tree_arg_validation_fcns (tree_argument_list *fcn_args)
    : m_fcn_args (fcn_args)
  { }

  OCTAVE_DISABLE_CONSTRUCT_COPY_MOVE (tree_arg_validation_fcns)

  ~tree_arg_validation_fcns ()
  {
    delete m_fcn_args;
  }

  tree_argument_list * fcn_args () { return m_fcn_args; }

  void accept (tree_walker& tw)
  {
    tw.visit_arg_validation_fcns (*this);
  }

private:

  tree_argument_list *m_fcn_args;
};

class tree_arg_validation
{
public:

  tree_arg_validation (tree_arg_size_spec *size_spec,
                       tree_identifier *class_name,
                       tree_arg_validation_fcns *validation_fcns,
                       const token& eq_tok,
                       tree_expression *default_value)
    : m_arg_name (nullptr), m_size_spec (size_spec),
      m_class_name (class_name), m_validation_fcns (validation_fcns),
      m_eq_tok (eq_tok), m_default_value (default_value)
  { }

  OCTAVE_DISABLE_CONSTRUCT_COPY_MOVE (tree_arg_validation)

  ~tree_arg_validation ()
  {
    delete m_arg_name;
    delete m_size_spec;
    delete m_class_name;
    delete m_validation_fcns;
    delete m_default_value;
  }

  void arg_name (tree_expression *name)
  {
    m_arg_name = name;
  }

  tree_expression * identifier_expression () { return m_arg_name; }

  tree_arg_size_spec * size_spec () { return m_size_spec; }

  tree_identifier * class_name () { return m_class_name; }

  tree_arg_validation_fcns *
  validation_fcns () { return m_validation_fcns; }

  tree_expression *
  initializer_expression () { return m_default_value; }

  void accept (tree_walker& tw)
  {
    tw.visit_arg_validation (*this);
  }

private:

  // May be a simple identifier or an identifier followed by a single
  // field name.
  tree_expression *m_arg_name;
  tree_arg_size_spec *m_size_spec;
  tree_identifier *m_class_name;
  tree_arg_validation_fcns *m_validation_fcns;
  token m_eq_tok;
  tree_expression *m_default_value;
};

class tree_args_block_validation_list
  : public std::list<tree_arg_validation *>
{
public:

  tree_args_block_validation_list () { }

  tree_args_block_validation_list (tree_arg_validation *a) { push_back (a); }

  tree_args_block_validation_list (const std::list<tree_arg_validation *>& a)
    : std::list<tree_arg_validation *> (a)
  { }

  OCTAVE_DISABLE_COPY_MOVE (tree_args_block_validation_list)

  ~tree_args_block_validation_list ();

  void accept (tree_walker& tw)
  {
    tw.visit_args_block_validation_list (*this);
  }
};

// FIXME: Maybe make this object an actual list even though we don't
// currently need it?

class tree_args_block_attribute_list
{
public:

  tree_args_block_attribute_list (tree_identifier *attr = nullptr)
    : m_attr (attr)
  { }

  OCTAVE_DISABLE_COPY_MOVE (tree_args_block_attribute_list)

  ~tree_args_block_attribute_list ()
  {
    delete m_attr;
  }

  tree_identifier * attribute () { return m_attr; }

  void accept (tree_walker& tw)
  {
    tw.visit_args_block_attribute_list (*this);
  }

private:

  tree_identifier *m_attr;
};

// Arguments block.

class tree_arguments_block : public tree_command
{
public:

  tree_arguments_block (const token& args_tok, tree_args_block_attribute_list *attr_list, tree_args_block_validation_list *validation_list, const token& end_tok)
    : m_args_tok (args_tok), m_attr_list (attr_list), m_validation_list (validation_list), m_end_tok (end_tok)
  { }

  OCTAVE_DISABLE_CONSTRUCT_COPY_MOVE (tree_arguments_block)

  ~tree_arguments_block ()
  {
    delete m_attr_list;
    delete m_validation_list;
  }

  filepos beg_pos () const { return m_args_tok.beg_pos (); }
  filepos end_pos () const { return m_end_tok.end_pos (); }

  tree_args_block_attribute_list * attribute_list ()
  {
    return m_attr_list;
  }

  tree_args_block_validation_list * validation_list ()
  {
    return m_validation_list;
  }

  void accept (tree_walker& tw)
  {
    tw.visit_arguments_block (*this);
  }

private:

  token m_args_tok;

  tree_args_block_attribute_list *m_attr_list;

  tree_args_block_validation_list *m_validation_list;

  token m_end_tok;
};

OCTAVE_END_NAMESPACE(octave)

#endif
