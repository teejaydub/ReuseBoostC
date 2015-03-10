/* shadowRegs.h
    Copyright (c) 2006-2008 by Timothy J. Weber, tw@timothyweber.org.

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

// Defines shadow registers for GPIO regs shared among multiple modules.
// This lets one module change GPIO bits without affecting bits it doesn't control,
// and without causing read-modify-write problems.

#ifndef _TJW_SHADOWREGS_H
#define _TJW_SHADOWREGS_H

#include "types-tjw.h"

#ifdef IN_SHADOWREGS
 #define SHADOW_REGS_EXTERN
#else
 #define SHADOW_REGS_EXTERN  extern
#endif

SHADOW_REGS_EXTERN byte porta_;
SHADOW_REGS_EXTERN byte portb_;
SHADOW_REGS_EXTERN byte portc_;

// Sets the specified shadowed register, changing only the masked bits.
#define SET_SHADOW(regName, shadowReg, newValue, mask)  { shadowReg = (shadowReg & (~(mask))) | (newValue & mask); regName = shadowReg; }
#define SET_SHADOW_A(newValue, mask)  { porta_ = (porta_ & (~(mask))) | (newValue & mask); porta = porta_; }
#define SET_SHADOW_B(newValue, mask)  { portb_ = (portb_ & (~(mask))) | (newValue & mask); portb = portb_; }
#define SET_SHADOW_C(newValue, mask)  { portc_ = (portc_ & (~(mask))) | (newValue & mask); portc = portc_; }
	
// Sets the given bit in a shadowed port.
// The bit number must be constant.
// NOTE: When newValue depends on shadowReg, BoostC seems to generate bad code!
#define SET_SHADOW_BIT(regName, shadowReg, bit, newValue)  { shadowReg.bit = newValue; regName = shadowReg; }
#define SET_SHADOW_A_BIT(bit, newValue)  { porta_.bit = newValue; porta = porta_; }
#define SET_SHADOW_B_BIT(bit, newValue)  { portb_.bit = newValue; portb = portb_; }
#define SET_SHADOW_C_BIT(bit, newValue)  { portc_.bit = newValue; portc = portc_; }
	
// Toggles the given bit in a shadowed port.
// The bit number must be constant.
#define TOGGLE_SHADOW_BIT(regName, shadowReg, bit)  { shadowReg ^= (1 << bit); regName = shadowReg; }

#endif
//_TJW_SHADOWREGS_H
