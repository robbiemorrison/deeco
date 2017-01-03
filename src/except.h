
#ifndef _EXCEPT_              // header guard
#define _EXCEPT_


// This was excerpted from the SCO UnixWare /usr/include/math.h header,
// which takes a lot of trouble to make sure that this record type
// will NOT be available to C++ code.  Too bad we need it!


struct exception
{
    int     type;
    char    *name;
    double  arg1;
    double  arg2;
    double  retval;
};


#endif  // _EXCEPT_

