#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <base64.h>

static char base64_code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * @Function: ������Base64������е�������ĳ���ַ�
 *            0-63 : A-Z(25) a-z(51), 0-9(61), +(62), /(63)
 * @Param:    unsigned char n
 * @Return:   �ַ�
 */
char  Base64::base2Chr( unsigned char n )
{
	n &= 0x3F;
	if ( n < 26 )
    	return ( char )( n + 'A' );
    else if ( n < 52 )
    	return ( char )( n - 26 + 'a' );
    else if ( n < 62 )
    	return ( char )( n - 52 + '0' );
    else if ( n == 62 )
    	return '+';
    else
    	return '/';
}

/**
 * @function ���ĳ���ַ���Base64������е����
 * @param    c   �ַ�
 * @return   ���ֵ
 */
unsigned char Base64::chr2Base( char c )
{
	if ( c >= 'A' && c <= 'Z' )
    	return ( unsigned char )( c - 'A' );
    else if ( c >= 'a' && c <= 'z' )
    	return ( unsigned char )( c - 'a' + 26 );
    else if ( c >= '0' && c <= '9' )
    	return ( unsigned char )( c - '0' + 52 );
    else if ( c == '+' )
    	return 62;
    else if ( c == '/' )
    	return 63;
    else
        return 64;  //  ��Ч�ַ�
}

/**
 * @function �����ݽ���Base64����
 * @param   data Ҫ���б��������
 * @param   size         Ҫ�����������
 * @param   str       ����Base64�������ַ���, ����ռ����ⲿ���䣬һ��Ҫ�㹻�󣬷�������쳣
 * @return  �ɹ���true
 * @details: ���㷨��Ҫ��ÿ��ȡ���������ݵ�6���أ�����ɴ�Сд��ĸ�����ֺ�+=��64����ĸ
 */
size_t Base64::encode(char *str, const void *data, int size)
{
	// ����������
	assert((0 != str) && (0 != data) && (0 <= size));

	char *s, *p;
	int c;
	const unsigned char *q;

	p = s = str;
	q = (const unsigned char*)data;

	// size��Ҫ�����������ݵĳ���
	for(long i = 0; i < size;)
	{
		// �����ʱ�򣬶��ǰ�24bit����һ����λ����Ϊ3*8=4*6
		c = q[i++];
		c *= 256;
		if(i < size)
			c += q[i];
		i++;
		c *= 256;
		if(i < size)
			c += q[i];
		i++;

		// ÿ��ȡ6bit����һ��8bit��char����p��
		p[0] = base64_code[(c&0x00fc0000) >> 18];
		p[1] = base64_code[(c&0x0003f000) >> 12];
		p[2] = base64_code[(c&0x00000fc0) >> 6];
		p[3] = base64_code[(c&0x0000003f) >> 0];

		// �����Ǵ����β���
		if(i > size)
			p[3] = '=';
		if(i > size+1)
			p[2] = '=';

		p += 4; // ����������ָ����Ӧ���ƶ�
	}
	*p = 0;   // ��Ұָ��

	return strlen(str);
}

/**
 * @Function:��Base64���������ݽ��н���
 * @Param:   unsigned char * const p_pszDest ����Base64����������
 *			 const char *p_pszSrc		     Base64��������
 * @Return:  ��������ݳ���
 */
size_t Base64::decode(unsigned char * const p_pszDest, const char *p_pszSrc)
{
	// ����������:ָ�벻��Ϊ���Ҳ�����ͬ
	assert((0 != p_pszDest) && (0 != p_pszSrc) && ((const char *)p_pszDest != p_pszSrc));

	unsigned char * p = p_pszDest;
    size_t          n = strlen( p_pszSrc );
    unsigned char   c, t = 0;

    for (size_t i = 0; i < n; i++ )
    {
    	if ( *p_pszSrc == '=' )
        	break;
        do {
        	if ( *p_pszSrc )
		       	c = chr2Base( *p_pszSrc++ );
            else
            	c = 65;  //  �ַ�������
        } while ( c == 64 );  //  ������Ч�ַ�����س���
        if ( c == 65 )
        	break;
        switch ( i % 4 )
        {
        case 0 :
            t = (unsigned char)(c << 2);
            break;
        case 1 :
            *p++ = ( unsigned char )( t | ( c >> 4 ) );
            t = ( unsigned char )( c << 4 );
            break;
        case 2 :
            *p++ = ( unsigned char )( t | ( c >> 2 ) );
            t = ( unsigned char )( c << 6 );
            break;
        case 3 :
            *p++ = ( unsigned char )( t | c );
            break;
		default:
			break;
        }
    }
	return ( p - p_pszDest );
}


//#define BASE64_MAIN
#ifdef BASE64_MAIN
main()
{
	Base64 oBase64;
	const char * pcBefore = "hello world";
	char *pcAfter = new char[strlen(pcBefore)*4/3+5];

	oBase64.encode(&pcAfter, (const void*)pcBefore, strlen(pcBefore));

}
#endif
