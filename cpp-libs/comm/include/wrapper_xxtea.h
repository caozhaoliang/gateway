/***********************************************************************

    Copyright 2006-2009 Ma Bingyao
    Copyright 2013 Gao Chunhui, Liu Tao

    These sources is free software. Redistributions of source code must
    retain the above copyright notice. Redistributions in binary form
    must reproduce the above copyright notice. You can redistribute it
    freely. You can use it with any free or commercial software.

    These sources is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY. Without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

        github: https://github.com/liut/pecl-xxtea

*************************************************************************/

#ifndef PHP_XXTEA_H
#define PHP_XXTEA_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "xxtea.h"

unsigned char *xxtea_decrypt(unsigned char *data , xxtea_long data_len , unsigned char *key , xxtea_long key_len , xxtea_long* ret_length);
unsigned char *xxtea_encrypt(unsigned char *data , xxtea_long data_len , unsigned char *key , xxtea_long key_len , xxtea_long* ret_length);

#ifdef __cplusplus
}
#endif

#endif /* ifndef PHP_XXTEA_H */
