/* The functions defined here should be simple complement functions
 * to manipulate, format and convert data types
 */

/* BCD format to byte */
void bcd_to_byte (unsigned char *datum);
/* byte to BCD format */
void byte_to_bcd (unsigned char *datum);
/* Byte concatenation */
unsigned int concatenate (unsigned char *upper, unsigned char *lower);