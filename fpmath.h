// Header for SourceBoost-provided fpmath.c.

#ifndef __FPMATH_H
#define __FPMATH_H

typedef long single;

single int2float(long ii);
long float2int(single xx, char round);
single addfloat(single xx,single yy);
single subfloat(single xx,single yy);
single mulfloat(single xx,single yy);
single divfloat(single xx,single yy);
single absfloat(single xx);
char xGTyfloat(single xx, single yy);

#endif