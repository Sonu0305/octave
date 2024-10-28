########################################################################
##
## Copyright (C) 2018-2024 The Octave Project Developers
##
## See the file COPYRIGHT.md in the top-level directory of this
## distribution or <https://octave.org/copyright/>.
##
## This file is part of Octave.
##
## Octave is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, see
## <https://www.gnu.org/licenses/>.
##
########################################################################

## -*- texinfo -*-
## @deftypefn  {} {@var{y} =} movmean (@var{x}, @var{wlen})
## @deftypefnx {} {@var{y} =} movmean (@var{x}, [@var{nb}, @var{na}])
## @deftypefnx {} {@var{y} =} movmean (@dots{}, @var{dim})
## @deftypefnx {} {@var{y} =} movmean (@dots{}, @var{nancond})
## @deftypefnx {} {@var{y} =} movmean (@dots{}, @var{property}, @var{value})
## Calculate the moving average over a sliding window of length @var{wlen} on
## data @var{x}.
##
## The moving window length input @var{wlen} can either be a numeric scalar
## or a 2-element numeric array @w{@qcode{[@var{nb}, @var{na}]}}. The elements
## included in the moving window depend on the size and value of @var{wlen}
## as well as whether the @qcode{"SamplePoints"} option has been specified.
## For full details of element inclusion,
## @pxref{XREFmovslice,,@code{movslice}}.
##
## If the optional argument @var{dim} is given, operate along this dimension.
##
## The optional argument @var{nancond} is a string that controls how @code{NaN}
## and @code{NA} values affect the output of @qcode{"movmean"}. The value
## @qcode{"includenan"} (default) causes @code{NaN} and @code{NA} values to be
## included in the moving window, and any window slice containing @code{NaN} or
## @code{NA} values will return @code{NaN} for that element.  The value
## @qcode{"omitnan"} causes @qcode{"movmean"} to ignore any @code{NaN}
## or @code{NA} values resulting in fewer elements being used to calculate the
## mean for that window slice.  If @qcode{"omitnan"} is specified and a window
## slice contains all @code{NaN} or @code{NA} values, @qcode{"movmean"} returns
## @code{NaN} for that element.  The values @qcode{"includemissing"} and
## @qcode{"omitmissing"} may be used synonymously with @qcode{"includenan"} and
## @qcode{"omitnan"}, respectively.
##
## The calculation can be controlled by specifying @var{property}/@var{value}
## pairs.  Valid properties are @qcode{"Endpoints"} and
## @qcode{"SamplePoints"}.  For full descriptions of these properties and
## valid options, @pxref{XREFmovfun,,@code{movfun}}.
##
## Programming Note: This function is a wrapper which calls @code{movfun}.
## For full documentation of inputs and options,
## @pxref{XREFmovfun,,@code{movfun}}.
##
## @seealso{mean, movfun, movslice, movmad, movmax, movmedian, movmin,
## movprod, movstd, movsum, movvar}
## @end deftypefn

function y = movmean (x, wlen, varargin)

  if (nargin < 2)
    print_usage ();
  endif

  y = movfun (@mean, x, wlen, __parse_movargs__ ("movmean", varargin{:}){:});

endfunction


%!assert (movmean (1:5, 3), [1.5, 2, 3, 4, 4.5], eps)
%!assert (movmean (1:5, [1, 1]), [1.5, 2, 3, 4, 4.5], eps)
%!assert (movmean (1:5, 3, 2), [1.5, 2, 3, 4, 4.5], eps)
%!assert <*65928> (movmean (1:5, 3, 1), 1:5)
%!assert <*65928> (movmean (1:5, 3, 3), 1:5)

%!assert (movmean (magic (4), 3), ...
%!        [10.5, 6.5, 6.5, 10.5; 10, 20/3, 19/3, 11; ...
%!         6, 32/3, 31/3, 7; 6.5, 10.5, 10.5, 6.5], eps)
%!assert (movmean (magic (4), 3, 1), ...
%!        [10.5, 6.5, 6.5, 10.5; 10, 20/3, 19/3, 11; ...
%!         6, 32/3, 31/3, 7; 6.5, 10.5, 10.5, 6.5], eps)
%!assert (movmean (magic (4), 3, 2), ...
%!        [9, 7, 6, 8; 8, 26/3, 29/3, 9; ...
%!         8, 22/3, 25/3, 9; 9, 11, 10, 8], eps)
%!assert <*65928> (movmean (magic (4), 3, 3), magic (4))

%!assert <*55241> (movmean ((1:10).', 3), [1.5; (2:9).'; 9.5])

%!assert <66156> (movmean ([1:4, NaN(1,3), 8:10], 3), movmean ([1:4, NaN(1,3), 8:10], 3, "includenan"))
%!assert <66156> (movmean ([1:4, NaN(1,3), 8:10], 3, "includenan"), [1.5, 2, 3, NaN(1,5), 9, 9.5], eps)
%!assert <66156> (movmean ([1:4, NaN(1,3), 8:10], 3, "omitnan"), [1.5, 2, 3, 3.5, 4, NaN, 8, 8.5, 9, 9.5], eps)

## Test input validation
%!error <Invalid call> movmean ()
%!error <Invalid call> movmean (1)
