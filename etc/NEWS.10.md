Summary of important user-visible changes for version 10 (yyyy-mm-dd):
---------------------------------------------------------------------

### General improvements

- Three short form aliases have been added for long form options when starting
`octave`.
  * `-e CODE` for `--eval CODE`
  * `-g` for `--gui`
  * `-G` for `--no-gui`

- Three long form options have been introduced for clarity.
  * `--no-init-user` : Don't read user configuration files ~/.octaverc or
    .octaverc files at startup.
  * `--no-init-site` : Don't read site-wide configuration files at startup.
  * `--no-init-all` : Don't read any configuration files at startup.

- `nchoosek` algorithm is now ~2x faster and provides greater precision.

- `nextpow2` algorithm is now more accurate for inputs very close to a power
  of 2.  The output class now matches the input class for Matlab compatibility.
  The function no longer accepts complex inputs and will emit an error for
  these inputs.

- `jsonencode` now outputs integers and floating point integers without ".0"
  suffix.

- `hist` now accepts N-dimensional array inputs for input `Y` which is
  processed in columns as if the array was flattened to a 2-dimensional array.

- The third output for `unique` is now correct when `stable` sort option is
  used.

- Support setting breakpoints in `set` or `get` methods of `classdef`
  properties (bug #65610).

- `.mex` files now link to the new library `liboctmex` (instead of to
  `liboctinterp` and `liboctave`).  The SOVERSION of this new library is
  expected to be stable across multiple major versions of Octave.  So, `.mex`
  files might not need to be rebuilt for future major versions of Octave.

### Graphical User Interface

### Graphics backend

- `polar` plots now include the center tick mark value, typically 0, in the
  `'rtick'` parameter when the plot is created.  Subsequent modifications to
  `'rtick'` by the function `rticks` will only include the center tick mark
  value if it is specified.

- `view` correctly interprets Cartesian viewpoints on main axes (bug #65641).

- `plot3` now draws a single marker if only one data point is given.
  Previously the plot was blank (`marker` = "none") which was confusing.

### Matlab compatibility

- `height` and `width` are now aliases for the `rows` and `columns` functions.

- All colormaps now default to a size of 256 colors. (The previous default size
  was 64.)

- The first argument to `colormap` may now be a figure or axes object.  Calling
  `colormap` on a figure object will clear any "colormap" properties set at the
  axes level.

- `griddata` output size more consistently matches the input interpolation
  points when they are input as vectors.  When they are same-orientation
  vectors, the outputs will be the same size as those vectors.  When either is
  a row vector and the other is a column vector, the interpolating points are
  processed through meshgrid and the output is a matrix the same size as the
  meshgrid.

- `iqr` now provides compatible output for empty inputs.

- `cross` now produces row vector outputs when the inputs are a mix of row and
  column vectors (bug #61295).

- `rat` now accepts complex inputs.

- The optional second input argument of `system`, denoting whether to return
  the output as a variable, is now required to be a boolean value if used.

- Octave functions whose Matlab equivalents give errors when passed non-integer
  values as sizes or dimensions now also give similar errors.  For example,
  `cell (e, pi)` now gives an error in Octave about requiring integer sizes for
  the cell array, matching Matlab behavior.  Previously, Octave's conversion
  from non-integers to integers was more lenient.

- `issorted` now accepts the MODE option "monotonic", which has the same
  behavior as the option "either".
  
- `movfun` and `movslice`: Functions now accept `wlen` equal to 1 or [0,0],
  non-integer values of `wlen`, and values of `wlen` that create window lengths
  exceeding the size of the target array.  `movfun` also accepts values of `dim`
  larger than the number of non-singleton dimensions in the target array.  The
  `SamplePoints` option has been implemented for both functions.  Non-numeric
  input array handling has been improved.  These changes affect all moving
  window functions (`movmad`, `movmax`, `movmean`, `movmedian`, `movmin`,
  `movprod`, `movstd`, `movsum`, and `movvar`) (bug #65928 and bug #66025).

- `movfun`: The `nancond` property has been fully implemented and made MATLAB
  compatible.  The `omitnan` option will ignore NaN and NA values when
  calculating the function return, and, if all elements in a window slice are
  NaN or NA, it will return the value contained in a new property `nanval`
  (default NaN) for that element.  The `includenan` property (the default) has
  been updated for compatibility such that any window containing NaN or NA will
  return NaN rather than passing those values to the calculating function.
  `omitmissing` and `includemissing` are now accepted as aliases for `omitnan`
  and `includenan`.  These changes affect all moving window functions
  (`movmad`, `movmax`, `movmean`, `movmedian`, `movmin`, `movprod`, `movstd`,
  `movsum`, and `movvar`) (bug #66156).
  
- `movmin` and `movmax`: These functions now have their default behavior set to
  `omitnan`.  NaN and NA values will be ignored unless a moving window contains
  only NaN or NA values, in which case the function will return NaN for that
  element (bug #66156).

- `movsum`: When called with option `omitnan`, any windows containing only NaN
  and NA values will return 0 (bug #66156).

- `movprod`: When called with option `omitnan`, any windows containing only NaN
  and NA values will return 1 (bug #66156).
  
- `movmad`: The function now defaults to calculating median absolute deviation.
  Before Octave 10, the function calculated mean absolute deviation.  A new
  `mode` property has been provided that takes values of either "mean" or
  "median" to allow the user to select which option to use.  This property
  should not be expected to function in code used outside of Octave.

### Alphabetical list of new functions added in Octave 10

* `clim`
* `rticklabels`
* `tticklabels`

### Deprecated functions, properties, and operators

The following functions and properties have been deprecated in Octave 10
and will be removed from Octave 12 (or whatever version is the second
major release after 10):

- Functions

        Function               | Replacement
        -----------------------|------------------

- Core

        Function                              | Replacement
        --------------------------------------|------------------
        symbol_record_rep::mark_as_variable   | symbol_record_rep::mark_variable
        symbol_record_rep::unmark_as_variable | symbol_record_rep::unmark_variable
        symbol_record::mark_as_variable       | symbol_record::mark_variable
        symbol_record::unmark_as_variable     | symbol_record::unmark_variable
        interpreter::verbose                  | interpreter::init_trace
        cmdline_options::verbose              | cmdline_options::init_trace

  - A new method `rwdata ()` provides direct read/write access (a pointer) to
    the data in a liboctave `Array` object (or its derived classes such as
    `Matrix`).  Historically, the method `fortran_vec ()` provided this access,
    but the name is unclear, and it is recommended to use `rwdata ()` in any
    new code.  There are no plans to deprecate `fortran_vec`.

  - The `--verbose`,`-V` command-line option has been deprecated.  Use
    `--init-trace` to print the configuration files executed during
    initialization.

The following features were deprecated in Octave 8 and have been removed
from Octave 10.

- Functions

        Function               | Replacement
        -----------------------|------------------
        shift                  | circshift
        sparse_auto_mutate     | none (see below)

- `fminsearch` parameter passing: A legacy, undocumented, and only partially
  supported syntax for passing parameters to the minimized function `fcn`
  called by `fminsearch` by appending them to the input argument list was
  partly implemented since Octave 4.4.0.  Due to conflicts with other input
  methods, the documentation of this syntax was removed in Octave 5.1.0.  The
  remaining functionality has been completely removed in Octave 10.  Attempts
  to call `fminsearch` with that syntax will result in an error.  The preferred
  method of passing parameters to any of the minimization functions (including
  `fminsearch`, `fminbnd`, and `fminunc`) is through the use of anonymous
  functions.  Specific examples of this can be found in the "Minimizers"
  section of the Octave manual.

### Build system

- Octave now requires a C++ compiler that is compliant with C++17 (preferably
  with GNU extensions).


Summary of bugs fixed for version 10.1.0 (yyyy-mm-dd):
----------------------------------------------------

- bug #66488: qr (sparse, 0) isn't equal to qr (sparse, 'econ', 'vector')
- bug #66477: ASAN error with path(_) manipulations
- bug #66466: legend + bar causes ASAN to crash octave
- bug #66451: Ctrl-C kills octave 10.0.0
- bug #66448: File encoding conversion errors on macOS 14
- bug #66399: fseek and ftell functions don't always work correctly on Win11 with filesizes >2GB
- bug #66315: menu() throws an error when Qt dialogs are not available
- bug #66256: movmad uses 'mean absolute deviation', matlab uses 'median absolute deviation'
- bug #66156: Implement nanflag option for moving window functions
- bug #66025: movfun: implement SamplePoints option
- bug #66010: Function glpk produces incorrect output
- bug #65964: nthargout does not propagate error ID
- bug #65928: movfun dimension constraints: shouldn't error for dim > ndims(x), or wlen > size(x, dim), or wlen=1
- bug #65876: error retrieving data from struct values in containers.Maps
- bug #65768: segfault on default branch
- bug #65753: Documentation window: Function Index: Search box respects trailing spaces
- bug #65730: Some remaining cases of silent conversion of fractional inputs
- bug #65683: issorted: enable 'monotonic' and 'strict...' sort modes.
- bug #65674: axes 'colormap' property being set over figure 'colormap' property
- bug #65665: Input validation for system()
- bug #65645: Execute FIXME of perms.cc: Use constexpr instead of template specialisation
- bug #65641: 'view' produces incorrect viewpoint when given a vector aligned with the primary axes
- bug #65637: `short_disp` doesn't show ellipsis at end of long arrays
- bug #65617: <F9> clears editor setting 'Always show debug breakpoints and pointers ...'
- bug #65577: Feature request - editor right click to run test without needing to clear %! first
- bug #65538: xint_value does not work the way it is intended
- bug #65531: iqr handling of empty inputs is not compatible
- bug #65499: Use separate GUI settings for Octave releases
- bug #65495: nchoosek error 'gcd: all values must be integers'
- bug #65459: uifigure ('visible','off') temporarily creates a visible figure window
- bug #65447: jsonencode does not accept integer values larger than 999999
- bug #65441: nextpow2 incorrect for some inputs slightly larger than powers of two
- bug #65238: Improve nchoosek.m algorithm to prevent numerical issues
- bug #65221: movfun: create inputParser only once.
- bug #65176: unique.m - Enable third output with option 'stable'
- bug #65134: griddata: output size inconsistent for vector input interpolation points
- bug #65030: Column width in browser pane not retained between restarts of Octave
- bug #65010: compatibility: colormaps now default to 256 colors in matlab
- bug #64995: implement height and width as aliases for rows and columns for matlab compatibility
- bug #64991: polar doesn't populate rtick with center tick value
- bug #62928: Error sourcing file message when script with embedded '.' in filename has syntax error
- bug #61295: cross() dimensions inconsistent with Matlab when using mismatched input vector dimensions
- bug #60797: sqrtm: returns nan for matrix of ones with rows and columns >=4
- bug #55961: properties function does not preserve order
- bug #55198: rat() should support complex numbers

### Old release news

- [Octave 9.x](etc/NEWS.9.md)
- [Octave 8.x](etc/NEWS.8.md)
- [Octave 7.x](etc/NEWS.7.md)
- [Octave 6.x](etc/NEWS.6.md)
- [Octave 5.x](etc/NEWS.5.md)
- [Octave 4.x](etc/NEWS.4)
- [Octave 3.x](etc/NEWS.3)
- [Octave 2.x](etc/NEWS.2)
- [Octave 1.x](etc/NEWS.1)
