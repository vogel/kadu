/*
 *
 *    (C) Copyright 2002 Michal J. Kubski
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License Version
 *  2.1 as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __SIM_SIM_H__
#define __SIM_SIM_H__

#include <openssl/rsa.h>

#define PUBLIC 0
#define PRIVATE 1

typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;

/* Secure Internet Messaging */

#define SIM_SYMMETRIC_BLOCKSIZE 8	/* 64bit */
#define SIM_SYMMETRIC_KEYSIZE 16	/* 128bit */

#define SIM_RSA_KEYSIZE 128	/* 1024bit */

/* path to key files */

extern char SIM_Key_Path[255];

/* 'not encrypted' string */
extern char SIM_Not_Encrypted[255];

/* magicnumber */

#define SIM_MAGICNUMBER_V1 0x2391

/* flags */

/* NOT IMPLEMENTED YET! #define SIM_FLAG_SIGNED 0x01 */

/* message structure */

typedef struct SIM_Message_t {
    /* data encrypted with public key */

    unsigned char *key;		/* symmetric key (len = SIM_RSA_KEYSIZE) */

    /* data encrypted with symmetric key */

    unsigned char *init;	/* plays role of IV (len = SIM_SYMMETRIC_BLOCKSIZE) */

    uint16 magicnumber;
    uint8 flags;

    char *message;
} SIM_Message;

/*
if (flags & SIM_SIGNED) {
   message = signature(real_message) + real_message;
}
*/

/*
 * (KeyCache) cache item structure
 */

typedef struct SIM_KC_s {
    uint32 uin;
    unsigned char *key;		/* symmetric key (len = SIM_SYMMETRIC_KEYSIZE) */
    unsigned char *rkey;	/* rsa encrypted symmetric key (len = SIM_RSA_KEYSIZE) */
    RSA *pubkey;
    void *prev;
    void *next;
} SIM_KC;

/*
 * (Base64) Base64 Encoding
 */

int SIM_Base64_Encode(unsigned char *inbuf, unsigned char *outbuf,
		      int inlen);

/*
 * (Base64) Base64 Decoding
 */

int SIM_Base64_Decode(unsigned char *inbuf, unsigned char *outbuf,
		      int inlen);

/*
 * (RSA) Encryption with Public Key
 */

int SIM_RSA_Encrypt(unsigned char *inbuf, unsigned char *outbuf, int inlen,
		    RSA * key);

/*
 * (RSA) Decryption with Private Key
 */

int SIM_RSA_Decrypt(unsigned char *inbuf, unsigned char *outbuf, int inlen,
		    RSA * key);

/*
 * (RSA) Read Key from file
 */

RSA *SIM_RSA_ReadKey(const char *file, int type);

/*
 * (RSA) Write Key to file
 */

int SIM_RSA_WriteKey(RSA * rsa, const char *file, int type);

/*
 * (RSA) Key generation callback
 */

void SIM_RSA_GenKey_cb(int p, int c, unsigned char *f);

/*
 * (RSA) Key generation routine
 */

RSA *SIM_RSA_GenKey(int bits);

/*
 * (RSA) Signing with Private Key
 */

int SIM_RSA_Sign(unsigned char *inbuf, unsigned char *sigbuf, int inlen,
		 RSA * key);

/*
 * (RSA) Signature verifing with Public Key
 */

int RSA_Verify(unsigned char *inbuf, unsigned char *sigbuf, int inlen,
	       int siglen, RSA * key);

/*
 * (Symmetric) Encryption with a symmetric cipher
 */

int SIM_Symmetric_Encrypt(unsigned char *inbuf, unsigned char *outbuf,
			  int inlen, unsigned char *key,
			  unsigned char *iv);

/*
 * (Symmetric) Decryption with a symmetric cipher
 */

int SIM_Symmetric_Decrypt(unsigned char *inbuf, unsigned char *outbuf,
			  int inlen, unsigned char *key,
			  unsigned char *iv);

/*
 * (Message) Transform UIN to path & file name
 */

int SIM_Message_uin2keyfile(uint32 uin, char *file);

/*
 * (Message) Build path & file name of private key
 */

int SIM_Message_private2keyfile(char *file);

/*
 * (Message) Encrypt message
 */

int SIM_Message_Encrypt(unsigned char *in, unsigned char *out, int inlen,
			uint32 uin);

/*
 * (Message) Decrypt message
 */

int SIM_Message_Decrypt(unsigned char *in, unsigned char *out, int inlen,
                        uint32 uin);

/*
 * (KeyCache) create item
 */

SIM_KC *SIM_KC_New();

/*
 * (KeyCache) free item
 */

void SIM_KC_Free(SIM_KC * a);

/*
 * (KeyCache) free item and its children
 */

void SIM_KC_RecursiveFree(SIM_KC * a);

/*
 * (KeyCache) initialize
 */

int SIM_KC_Init();

/*
 * (KeyCache) finish
 */

void SIM_KC_Finish();

/*
 * (KeyCache) add item
 */

int SIM_KC_Add(SIM_KC * a);

/*
 * (KeyCache) find item by 'uin'
 */

SIM_KC *SIM_KC_Find(uint32 uin);

/*
 * (KeyCache) find item by 'uin' and delete it
 */

int SIM_KC_Delete(uint32 uin);


int SIM_Key_Fingerprint (unsigned char* fingerprint, uint32 uin);

#endif				/* #ifndef __SIM_SIM_H__ */
