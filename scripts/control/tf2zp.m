# Copyright (C) 1996,1998 A. Scottedward Hodel 
#
# This file is part of Octave. 
#
# Octave is free software; you can redistribute it and/or modify it 
# under the terms of the GNU General Public License as published by the 
# Free Software Foundation; either version 2, or (at your option) any 
# later version. 
# 
# Octave is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
# for more details.
# 
# You should have received a copy of the GNU General Public License 
# along with Octave; see the file COPYING.  If not, write to the Free 
# Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 
function [zer,pol,k] = tf2zp(num,den)
# Converts transfer functions to poles / zeros.
#
# [zer,pol,k] = tf2zp(num,den) returns the zeros and poles of the SISO system
# defined by num/den.  K is a gain associated with the system zeros.

# Written by A. S. Hodel, etc.
# $Revision: 2.0.0.0 $

  if(nargin == 2)
    if(length(den) > 1)          pol = roots(den);
    else                         pol=[];                   endif
    if(length(num) > 1)         zer = roots(num);
    else                        zer=[];                    endif
  else                    error("Incorrect number of input arguments");
  endif

  [a,b,c,d] = tf2ss(num,den);
  [dum,k] = tzero(a,b,c,d);
endfunction
