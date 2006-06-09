/* MSNP11 Challenge Handler v2
 * Based on the C++ implementation. 
 * This implementation works on big endian systems as well.
 * Compile: 'gcc chl.c -o chl -lcrypto -Wall'
 *
 * Licensed for distribution under the GPL version 2
 * Copyright 2005 Sanoi <sanoix@gmail.com>
 *
 * Slighly Modified for wxMsn Usage by Superna
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

#ifdef USE_LOCAL_MD5
	#ifdef USE_OPENSSL
		#undef USE_OPENSSL
	#endif
#endif

#ifdef USE_OPENSSL
	#warning Using OpenSSL MD5 function
	#include <openssl/md5.h>
#else
	#warning Using local MD5 by Aladdin Enterprises, see md5.c/md5.h header
	#include "md5.h"
#endif

#define _T(a) a
#include "wxmsn.h"

#define BUFSIZE 256

int isBigEndian(void)
{
  short int word = 0x0100;
  char *byte = (char *)&word;
  return(byte[0]);
}

unsigned int swapInt(unsigned int dw)
{
  unsigned int tmp;
  tmp =  (dw & 0x000000FF);
  tmp = ((dw & 0x0000FF00) >> 0x08) | (tmp << 0x08);
  tmp = ((dw & 0x00FF0000) >> 0x10) | (tmp << 0x08);
  tmp = ((dw & 0xFF000000) >> 0x18) | (tmp << 0x08);
  return(tmp);
}

void MSN_handle_chl(char *input, char *output)
{
 #ifndef USE_OPENSSL
 md5_state_t md5;
 #endif
	
 char *productKey = MSN_CHALLENGE_ID,//"CFHUR$52U_{VIX5T",
       *productID  = MSN_CHALLENGE_PROD,//"PROD0101{0RM?UBW",
       *hexChars   = "0123456789abcdef",
       buf[BUFSIZE];
  unsigned char md5Hash[16], *newHash;
  unsigned int *md5Parts, *chlStringParts, newHashParts[5];
  
  long long nHigh=0, nLow=0;
  
  int i, bigEndian;

  /* Determine our endianess */
  bigEndian = isBigEndian();

  /* Create the MD5 hash */
  snprintf(buf, BUFSIZE-1, "%s%s", input, productKey);
  
  //Change because OpenSSL is problematic under Win32 platform
  #ifdef USE_OPENSSL
  	MD5((unsigned char *)buf, strlen(buf), md5Hash);
  #else
	//Uses locally compiled md5
	md5_init(&md5);
	md5_append(&md5,(md5_byte_t *)buf, strlen(buf));
	md5_finish(&md5, md5Hash);
  #endif

  /* Split it into four integers */
  md5Parts = (unsigned int *)md5Hash;
  for(i=0; i<4; i++)
  {  
    /* check for endianess */
    if(bigEndian)
      md5Parts[i] = swapInt(md5Parts[i]);
    
    /* & each integer with 0x7FFFFFFF          */
    /* and save one unmodified array for later */
    newHashParts[i] = md5Parts[i];
    md5Parts[i] &= 0x7FFFFFFF;
  }
  
  /* make a new string and pad with '0' */
  snprintf(buf, BUFSIZE-5, "%s%s", input, productID);
  i = strlen(buf);
  memset(&buf[i], '0', 8 - (i % 8));
  buf[i + (8 - (i % 8))]='\0';
  
  /* split into integers */
  chlStringParts = (unsigned int *)buf;
  
  /* this is magic */
  for (i=0; i<(strlen(buf)/4)-1; i+=2)
  {
    long long temp;

    if(bigEndian)
    {
      chlStringParts[i]   = swapInt(chlStringParts[i]);
      chlStringParts[i+1] = swapInt(chlStringParts[i+1]);
    }

    temp=(md5Parts[0] * (((0x0E79A9C1 * (long long)chlStringParts[i]) % 0x7FFFFFFF)+nHigh) + md5Parts[1])%0x7FFFFFFF;
    nHigh=(md5Parts[2] * (((long long)chlStringParts[i+1]+temp) % 0x7FFFFFFF) + md5Parts[3]) % 0x7FFFFFFF;
    nLow=nLow + nHigh + temp;
  }
  nHigh=(nHigh+md5Parts[1]) % 0x7FFFFFFF;
  nLow=(nLow+md5Parts[3]) % 0x7FFFFFFF;
  
  newHashParts[0]^=nHigh;
  newHashParts[1]^=nLow;
  newHashParts[2]^=nHigh;
  newHashParts[3]^=nLow;
  
  /* swap more bytes if big endian */
  for(i=0; i<4 && bigEndian; i++)
    newHashParts[i] = swapInt(newHashParts[i]); 
  
  /* make a string of the parts */
  newHash = (unsigned char *)newHashParts;
  
  /* convert to hexadecimal */
  for (i=0; i<16; i++)
  {
    output[i*2]=hexChars[(newHash[i]>>4)&0xF];
    output[(i*2)+1]=hexChars[newHash[i]&0xF];
  }
  
  output[32]='\0';
}

/*int main()
{
  char chl_result[32];
  
  MSN_handle_chl("37819769320541083311", chl_result);
  
  / * chl_result should be '40575bf9740af7cad8671211e417d0cb' * /
  printf("Results: %s\n", chl_result);
  return 0;
}*/
