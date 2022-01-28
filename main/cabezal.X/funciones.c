/* The functions defined here should be simple complement functions
 * to manipulate, format and convert data types
 */

#include "consts.h"
#include "funciones.h"

/* BCD format to byte */
void bcd_to_byte(unsigned char *datum) {
    unsigned char aux;
    
    aux = (*datum) & 0xF0;  //tens
    aux = aux >> 4;
    aux *= 10;  //tens converted to byte
    aux += (*datum) & 0x0F;  //unit
    (*datum) = aux;

}

/* byte to BCD format */
void byte_to_bcd(unsigned char *datum) {
    unsigned char aux;
    
    aux = (*datum) % 10;    //this gets the unit
    (*datum) -= aux;
    (*datum) /= 10; //this sets the tens in the lower nibble
    (*datum) = (*datum) << 4;   //tens shifted to upper nibble
    (*datum) += aux;    //unit
}

/* concatenate 2 bytes as upper and lower */
unsigned int concatenate (unsigned char *upper, unsigned char *lower) {
    unsigned int aux;
    
    aux = (*upper);
    aux = aux << 8;
    aux += (*lower);
    
    return aux;
}