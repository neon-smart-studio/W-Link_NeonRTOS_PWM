
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int ftoa(double flt, char* buf, int after_point)
{
    int int_part = (int) flt;
    double flt_part = fabs(flt-int_part);
    
    char* buf_ptr = buf;
    
    //printf("%d" , int_part);
    buf_ptr += sprintf(buf_ptr, "%d", int_part);
    
    //printf(".");
    *buf_ptr++ = '.';
    
    int flt_part_int;
    
    flt_part_int = (int)(flt_part*pow(10, after_point));
    
    //printf("%d" , flt_part_int);
    buf_ptr += sprintf(buf_ptr, "%d", flt_part_int);
    
    //printf("\n");
    
    return (buf_ptr-buf);
}
