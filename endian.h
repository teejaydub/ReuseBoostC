/* endian.h
    Copyright (c) 2007 by Timothy J. Weber, tw@timothyweber.org.

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
/*
	Helps work out endian issues with long ints.
	That is, when you access individual bytes, is the most-significant stored first, or last?
*/

#ifndef __ENDIAN_H
#define __ENDIAN_H

// BoostC is little-endian - that is, it stores the LSB first in multi-byte integers.
#define LITTLE_ENDIAN

#endif