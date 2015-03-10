/* mem-tjw.h
    Copyright (c) 2006 by Timothy J. Weber, tw@timothyweber.org.

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

// Similar to std C lib mem*, but more efficient if you don't need the exact behavior.

#ifndef __MEMTJW_H
#define __MEMTJW_H

// Like memcpy, but doesn't return anything.
void copyBytes(char* dst, char* src, unsigned char len);

// Like memcmp, but only returns true or false.
unsigned char bytesEqual(char* a, char* b, unsigned char len);

#endif
// __MEMTJW_H
