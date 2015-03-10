/* log.h
    Copyright (c) 2009 by Timothy J. Weber, tw@timothyweber.org.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
    
// Provides various ways to compute logarithms.

#ifndef _LOG_H_
#define _LOG_H_

#include "fixed16.h"

// Returns the log base 2 of x.
// If x is 0, returns 0.
fixed16 log2_us(unsigned short x);

// Returns the log base 2 of x.
// If X is nonpositive, returns 0.
fixed16 log2_f(fixed16 x);


#endif //_LOG_H_
