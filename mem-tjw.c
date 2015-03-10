/* mem-tjw.c
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

void copyBytes(char* dst, char* src, unsigned char len)
{
	while (len--)
		*dst++ = *src++;
}

unsigned char bytesEqual(char* a, char* b, unsigned char len)
{
	while (len--)
		if (*a++ != *b++)
			return 0;
			
	return 1;
}
