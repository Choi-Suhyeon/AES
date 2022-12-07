#ifndef _CALTOY_H_
#define _CALTOY_H_


#ifdef __cplusplus
extern "C" {
#endif


#ifndef NOCRYPT
#define NOCRYPT
#endif


#if defined _MSC_VER
		//Visual Studio
	#ifdef _DEVELOPMENT
		#define DEV_DEFINE __declspec(dllexport)
	#else
		#define DEV_DEFINE __declspec(dllimport)
	#endif
#elif defined __GNUC__
		//GCC
	#ifdef _DEVELOPMENT
		#define DEV_DEFINE __attribute__ ((visibility("default")))
	#else
		//nothing to define
		#define DEV_DEFINE 
#endif
#endif

#include <stdint.h>
#include <stdio.h>
	typedef uint16_t pt_t; //plaintext type
	typedef uint16_t ct_t; //ciphertext type
	typedef uint16_t st_t; //intermedeate type
	typedef uint16_t rk_t; //roundkey type
	typedef uint8_t  wd_t; //word type

	DEV_DEFINE void caltoy_perm(st_t * out, st_t in);
	DEV_DEFINE void caltoy_inv_perm(st_t * out, st_t in);
	DEV_DEFINE void caltoy_sub(st_t * out, st_t in);
	DEV_DEFINE void caltoy_inv_sub(st_t * out, st_t in);
	DEV_DEFINE void caltoy_round(st_t * out, st_t in, rk_t rk);
	DEV_DEFINE void caltoy_inv_round(st_t * out, st_t in, rk_t rk);
	DEV_DEFINE void caltoy_enc(ct_t * out, pt_t in);
	DEV_DEFINE void caltoy_text_print(char * added_str, st_t text);
	DEV_DEFINE wd_t caltoy_sbox[16];
	DEV_DEFINE wd_t caltoy_inv_sbox[16];

#ifdef __cplusplus
}
#endif //extern "C"

#endif