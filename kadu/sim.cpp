/*
 *
 *    (C) Copyright 2002 Michal J. Kubski,
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

#include <stdio.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

#include "sim.h"



/* path to key files */
char SIM_Key_Path[255];

/* 'not encrypted' string */
char SIM_Not_Encrypted[255];

/*
 * 
 * (Base64) Base64 Encoding
 *
 */

int SIM_Base64_Encode(unsigned char *inbuf, unsigned char *outbuf,
		      int inlen)
{
    BIO *bio, *mbio, *bbio;

    unsigned char *dst;

    int outlen;

    mbio = BIO_new(BIO_s_mem());
    bbio = BIO_new(BIO_f_base64());
    BIO_set_flags(bbio, BIO_FLAGS_BASE64_NO_NL);

    bio = BIO_push(bbio, mbio);

    BIO_write(bio, inbuf, inlen);
    BIO_flush(bio);

    outlen = BIO_get_mem_data(mbio, (unsigned char *) &dst);
    memcpy(outbuf, dst, outlen);
    BIO_free_all(bio);

    return outlen;
}

/*
 * 
 * (Base64) Base64 Decoding
 *
 */

int SIM_Base64_Decode(unsigned char *inbuf, unsigned char *outbuf,
		      int inlen)
{
    BIO *bio, *mbio, *bbio;

    int outlen;

    mbio = BIO_new(BIO_s_mem());
    bbio = BIO_new(BIO_f_base64());
    BIO_set_flags(bbio, BIO_FLAGS_BASE64_NO_NL);

    bio = BIO_push(bbio, mbio);

    BIO_write(mbio, inbuf, inlen);
    BIO_flush(mbio);

    outlen = BIO_read(bio, outbuf, inlen);
    BIO_free_all(bio);

    return outlen;
}

/*
 * 
 * (RSA) Encryption with Public Key
 *
 */

int SIM_RSA_Encrypt(unsigned char *inbuf, unsigned char *outbuf, int inlen,
		    RSA * key)
{
    unsigned char *buf;
    int outlen = -1;

    buf = (unsigned char *) OPENSSL_malloc(RSA_size(key));
    outlen =
	RSA_public_encrypt(inlen, inbuf, buf, key, RSA_PKCS1_OAEP_PADDING);
    if (outlen != -1)
	memcpy(outbuf, buf, outlen);

    OPENSSL_free(buf);

    return outlen;
}

/*
 * 
 * (RSA) Decryption with Private Key
 *
 */

int SIM_RSA_Decrypt(unsigned char *inbuf, unsigned char *outbuf, int inlen,
		    RSA * key)
{
    unsigned char *buf;
    int outlen;

    buf = (unsigned char *) OPENSSL_malloc(RSA_size(key));
    outlen =
	RSA_private_decrypt(inlen, inbuf, buf, key,
			    RSA_PKCS1_OAEP_PADDING);
    if (outlen != -1)
	memcpy(outbuf, buf, outlen);

    OPENSSL_free(buf);

    return outlen;
}

/*
 * 
 * (RSA) Read Key from file
 *
 */

RSA *SIM_RSA_ReadKey(const char *file, int type)
{
    FILE *fp = fopen(file, "r");
    RSA *rsa;

    if (!fp)
	return NULL;

    switch (type) {
    case PUBLIC:
	{
	    rsa = PEM_read_RSAPublicKey(fp, NULL, NULL, NULL);
	    break;
	}
    case PRIVATE:
	{
	    rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
	    break;
	}
    }

    fclose(fp);

    return rsa;
}

/*
 * 
 * (RSA) Write Key to file
 *
 */

int SIM_RSA_WriteKey(RSA * rsa, const char *file, int type)
{
    FILE *fp = fopen(file, "w");
    int ret;

    if (!fp)
	return 1;

    switch (type) {
    case PUBLIC:
	{
	    ret = PEM_write_RSAPublicKey(fp, rsa);
	    break;
	}
    case PRIVATE:
	{
	    ret =
		PEM_write_RSAPrivateKey(fp, rsa, NULL, NULL, 0, NULL,
					NULL);
	    break;
	}
    }

    fclose(fp);

    return ret;
}


/*
 * 
 * (RSA) Key generation routine
 *
 */

RSA *SIM_RSA_GenKey(int bits)
{
    RSA *rsa;

    RAND_load_file("/dev/urandom", (bits * 2) / 8);

    rsa = RSA_generate_key(bits, RSA_F4, NULL, NULL);

    return rsa;
}

/*
 * 
 * (RSA) Signing with Private Key
 *
 */

int SIM_RSA_Sign(unsigned char *inbuf, unsigned char *sigbuf, int inlen,
		 RSA * key)
{
    unsigned char *buf;
    unsigned char *buf2;
    unsigned int outlen, ret;

    buf = (unsigned char *) OPENSSL_malloc(SHA_DIGEST_LENGTH);
    buf2 = (unsigned char *) OPENSSL_malloc(RSA_size(key));

    SHA1(inbuf, inlen, buf);
    ret = RSA_sign(NID_sha1, buf, SHA_DIGEST_LENGTH, buf2, &outlen, key);
    if (ret == 1)
	memcpy(sigbuf, buf2, outlen);

    OPENSSL_free(buf);
    OPENSSL_free(buf2);

    return outlen;
}

/*
 * 
 * (RSA) Signature verifing with Public Key
 *
 */

int RSA_Verify(unsigned char *inbuf, unsigned char *sigbuf, int inlen,
	       int siglen, RSA * key)
{
    unsigned char *buf;
    int verify;

    buf = (unsigned char *) OPENSSL_malloc(SHA_DIGEST_LENGTH);

    SHA1(inbuf, inlen, buf);

    verify =
	RSA_verify(NID_sha1, buf, SHA_DIGEST_LENGTH, sigbuf, siglen, key);

    OPENSSL_free(buf);

    return verify;
}

/*
 * 
 * (Symmetric) Encryption with a symmetric cipher
 *
 */

int SIM_Symmetric_Encrypt(unsigned char *inbuf, unsigned char *outbuf,
			  int inlen, unsigned char *key, unsigned char *iv)
{
    BIO *bio, *mbio, *cbio;

    unsigned char *dst;

    int outlen;

    mbio = BIO_new(BIO_s_mem());
    cbio = BIO_new(BIO_f_cipher());
    BIO_set_cipher(cbio, EVP_bf_cbc(), key, iv, 1);

    bio = BIO_push(cbio, mbio);

    BIO_write(bio, inbuf, inlen);
    BIO_flush(bio);

    outlen = BIO_get_mem_data(mbio, (unsigned char *) &dst);
    memcpy(outbuf, dst, outlen);
    BIO_free_all(bio);

    return outlen;
}

/*
 * 
 * (Symmetric) Decryption with a symmetric cipher
 *
 */

int SIM_Symmetric_Decrypt(unsigned char *inbuf, unsigned char *outbuf,
			  int inlen, unsigned char *key, unsigned char *iv)
{
    BIO *bio, *mbio, *cbio;

    unsigned char *dst;

    int outlen;

    mbio = BIO_new(BIO_s_mem());
    cbio = BIO_new(BIO_f_cipher());
    BIO_set_cipher(cbio, EVP_bf_cbc(), key, iv, 0);

    bio = BIO_push(cbio, mbio);

    BIO_write(bio, inbuf, inlen);
    BIO_flush(bio);

    outlen = BIO_get_mem_data(mbio, (unsigned char *) &dst);
    memcpy(outbuf, dst, outlen);
    BIO_free_all(bio);

    return outlen;
}

/*
 *
 * (Message) Pointer to Private Key
 *
 */

RSA *SIM_Message_PrivateKey;

/*
 * 
 * (Message) Transform UIN to path & file name
 *
 */

int SIM_Message_uin2keyfile(uint32 uin, char *file)
{
        // jesli uin==1 zwraca sciezke do wlasnego (pub.pem)
        if (uin==1)
                return sprintf(file, "%spub.pem", SIM_Key_Path);
        else
                return sprintf(file, "%s%d.pem", SIM_Key_Path, uin);
}


/*
 * 
 * (Message) Build path & file name of private key
 *
 */

int SIM_Message_private2keyfile(char *file)
{
    return sprintf(file, "%s%s", SIM_Key_Path, "private.pem");
}

/*
 * 
 * (Message) Encrypt message
 *
 */

int SIM_Message_Encrypt(unsigned char *in, unsigned char *out, int inlen,
			uint32 uin)
{
    SIM_Message msg;
    char *fname;
    unsigned char *iv;
    int len, klen, a;

    unsigned char *buf;
    unsigned char *buf2;

    SIM_KC *item;

    item = SIM_KC_Find(uin);

    if (item == NULL) {
		item = SIM_KC_New();
		item->uin = uin;
		fname = (char *) OPENSSL_malloc(256);
		SIM_Message_uin2keyfile(uin, fname);
		item->pubkey = SIM_RSA_ReadKey(fname, PUBLIC);
		if (!item->pubkey) {
#if 0
			fprintf(stderr, "Error: can't load public key from: %s\n",
				fname);
#endif
			return -1;
		}
		OPENSSL_free(fname);

		item->key =
			(unsigned char *) OPENSSL_malloc(SIM_SYMMETRIC_KEYSIZE);
		RAND_bytes(item->key, SIM_SYMMETRIC_KEYSIZE);

		item->rkey = (unsigned char *) OPENSSL_malloc(SIM_RSA_KEYSIZE);

		SIM_KC_Add(item);
    }

    SIM_RSA_Encrypt(item->key, item->rkey, SIM_SYMMETRIC_KEYSIZE,
		    item->pubkey);

    msg.magicnumber = SIM_MAGICNUMBER_V1;
    msg.flags = 0;

    /* should be seeded somewhere!!! */
    msg.init = (unsigned char *) OPENSSL_malloc(SIM_SYMMETRIC_BLOCKSIZE);
    RAND_bytes(msg.init, SIM_SYMMETRIC_BLOCKSIZE);

    /* create message buffer */

    buf = (unsigned char *) OPENSSL_malloc(8192);

    klen = 0;
    memcpy(buf + klen, msg.init, (a = SIM_SYMMETRIC_BLOCKSIZE));
    klen += a;
    OPENSSL_free(msg.init);

    memcpy(buf + klen, &msg.magicnumber, (a = sizeof(msg.magicnumber)));
    klen += a;

    memcpy(buf + klen, &msg.flags, (a = sizeof(msg.flags)));
    klen += a;

    memcpy(buf + klen, in, (a = inlen));
    klen += a;

    /* encrypt message */

    buf2 =
	(unsigned char *) OPENSSL_malloc(klen + SIM_SYMMETRIC_BLOCKSIZE);

    iv = (unsigned char *) OPENSSL_malloc(SIM_SYMMETRIC_BLOCKSIZE);
    memset(iv, 0, SIM_SYMMETRIC_BLOCKSIZE);

    len = SIM_Symmetric_Encrypt(buf, buf2, klen, item->key, iv);
    OPENSSL_free(iv);

    memcpy(buf, item->rkey, SIM_RSA_KEYSIZE);
    memcpy(buf + SIM_RSA_KEYSIZE, buf2, len);

    /* encode it in base64 */

    len = SIM_Base64_Encode(buf, out, len + SIM_RSA_KEYSIZE);

    OPENSSL_free(buf);
    OPENSSL_free(buf2);

    return len;
}

/*
 * 
 * (Message) Decrypt message
 *
 */

int SIM_Message_Decrypt(unsigned char *in, unsigned char *out, int inlen,
			uint32 uin)
{
    SIM_Message msg;
    SIM_KC *item;
    int SIM_Not_Encrypted_len;

    char *fname;
    unsigned char *iv;
    int klen, a, len, x, ylen;

    unsigned char *buf;
    unsigned char *buf2;
    unsigned char *key = NULL;

    SIM_Not_Encrypted_len = strlen(SIM_Not_Encrypted);
   
    if (SIM_Message_PrivateKey == NULL) {
		fname = (char *) OPENSSL_malloc(256);
		SIM_Message_private2keyfile(fname);
		SIM_Message_PrivateKey = SIM_RSA_ReadKey(fname, PRIVATE);

		OPENSSL_free(fname);
    }

	// Gdy nie mamy klucza pryw. lub wiadomosc jest krotsza od minimalnej 
	// dlugosci jaka moze miec zaszyfrowana to zwracamy tekst bez zmian
	if (!SIM_Message_PrivateKey || (inlen<190)) {  //chyba 192
		memcpy(out, SIM_Not_Encrypted, SIM_Not_Encrypted_len);
		memcpy(out + SIM_Not_Encrypted_len, in, inlen);
		return inlen + SIM_Not_Encrypted_len;
	}

    buf = (unsigned char *) OPENSSL_malloc(8192);

    len = SIM_Base64_Decode(in, buf, inlen);

    item = SIM_KC_Find(uin);

    ylen = 0;

	// najpierw próba deszyfrowania na skórty, kluczem symetrycznym
    if ((item != NULL)) {
		key = (unsigned char *) OPENSSL_malloc(SIM_SYMMETRIC_KEYSIZE);
		memcpy(key, item->key, SIM_SYMMETRIC_KEYSIZE);
		x = SIM_SYMMETRIC_KEYSIZE;

		buf2 = (unsigned char *) OPENSSL_malloc(8192);
		iv = (unsigned char *) OPENSSL_malloc(SIM_SYMMETRIC_BLOCKSIZE);
		memset(iv, 0, SIM_SYMMETRIC_BLOCKSIZE);

		ylen = SIM_Symmetric_Decrypt(buf + SIM_RSA_KEYSIZE, buf2,
					  len - SIM_RSA_KEYSIZE, key, iv);
		OPENSSL_free(iv);

		//	memcpy(&msg.magicnumber, &buf2[SIM_SYMMETRIC_BLOCKSIZE],
		//	       sizeof(msg.magicnumber));

		klen = SIM_SYMMETRIC_BLOCKSIZE;
		memcpy(&msg.magicnumber, &buf2[klen], (a = sizeof(msg.magicnumber)));
		klen += a;

        // jesli klucz symetrczyny pasuje to zwracamy odszyfrowany tekst
		if (msg.magicnumber == SIM_MAGICNUMBER_V1) {
			
			memcpy(&msg.flags, &buf2[klen], (a = sizeof(msg.flags)));
			klen += a;

			/* no flags yet ;) */
			len = ylen - klen;
			memcpy(out, &buf2[klen], len);

			OPENSSL_free(buf2);
			buf2=NULL;
			out[len++]='\0';

			return len;
		}
    } else {	// dodac item do keychacha gdy == NULL
	
        item = SIM_KC_New();
        item->uin = uin;
        fname = (char *) OPENSSL_malloc(256);
        SIM_Message_uin2keyfile(uin, fname);
        item->pubkey = SIM_RSA_ReadKey(fname, PUBLIC);
        if (!item->pubkey) {
            //return -1;
        }
        OPENSSL_free(fname);

        item->key =
            (unsigned char *) OPENSSL_malloc(SIM_SYMMETRIC_KEYSIZE);
        RAND_bytes(item->key, SIM_SYMMETRIC_KEYSIZE);

        item->rkey = (unsigned char *) OPENSSL_malloc(SIM_RSA_KEYSIZE);

        SIM_KC_Add(item);
    }
    

	
    // tu dojdzemy gdy nie udalo sie odszyfrowac kluczem symetrycznym
    
    msg.magicnumber = 0; //to chyba zbêdne

    // Próbujemy odszyfrowaæ RSA klucz symetryczny
    key = (unsigned char *) OPENSSL_malloc(SIM_RSA_KEYSIZE);
    x = SIM_RSA_Decrypt(buf, key, SIM_RSA_KEYSIZE, SIM_Message_PrivateKey);

    
	// i zwracamy tekst bez deszyfrowania jesli nie udalo 
    if (x != SIM_SYMMETRIC_KEYSIZE) {
		memcpy(out, SIM_Not_Encrypted, SIM_Not_Encrypted_len);
		memcpy(out + SIM_Not_Encrypted_len, in, inlen);

		return inlen + SIM_Not_Encrypted_len;
    }

	// Deszyfrowanie kluczem symetrycznym
    buf2 = (unsigned char *) OPENSSL_malloc(8192);
    iv = (unsigned char *) OPENSSL_malloc(SIM_SYMMETRIC_BLOCKSIZE);
    memset(iv, 0, SIM_SYMMETRIC_BLOCKSIZE);

    len = SIM_Symmetric_Decrypt(buf + SIM_RSA_KEYSIZE, buf2, len - SIM_RSA_KEYSIZE, key, iv);
    OPENSSL_free(iv);

    klen = SIM_SYMMETRIC_BLOCKSIZE;
    memcpy(&msg.magicnumber, &buf2[klen], (a = sizeof(msg.magicnumber)));
    klen += a;

	// gdy magicnumber sie nie zgadza, zwracamy oryginalny tekst
    if (msg.magicnumber != SIM_MAGICNUMBER_V1) {
		memcpy(out, SIM_Not_Encrypted, SIM_Not_Encrypted_len);
		memcpy(out + SIM_Not_Encrypted_len, in, inlen);
		
		return inlen + SIM_Not_Encrypted_len;
    }
    

    // w tym miejscu magicnumber sie zgadza wiec updatujemy sym klucz w cachu
    item->key = (unsigned char *) OPENSSL_malloc(SIM_RSA_KEYSIZE);
    memcpy(item->key, key, SIM_SYMMETRIC_KEYSIZE); 
    OPENSSL_free(key);
    
            
    memcpy(&msg.flags, &buf2[klen], (a = sizeof(msg.flags)));
    klen += a;

    /* no flags yet ;) */
    len = len - klen;
    memcpy(out, &buf2[klen], len);

    OPENSSL_free(buf2);
    buf2=NULL;
    out[len++]='\0';

    return len;
}

/*
 * 
 * (KeyCache) internal pointers
 *
 */

SIM_KC *SIM_KC_Cache;
SIM_KC *SIM_KC_Cache_LastItem;

/*
 * 
 *
 */

void SIM_KC_Free(SIM_KC * a)
{
    SIM_KC *p, *n;
    if (a == NULL)
	return;
    if (a->key != NULL)
	OPENSSL_free(a->key);
    if (a->rkey != NULL)
	OPENSSL_free(a->rkey);
    if (a->pubkey != NULL)
	RSA_free(a->pubkey);

    p = (SIM_KC *) a->prev;
    n = (SIM_KC *) a->next;
    if (p != NULL)
	p->next = (void *) n;
    if (n != NULL)
	n->prev = (void *) p;

    if (a == SIM_KC_Cache_LastItem) {
	SIM_KC_Cache_LastItem = (SIM_KC *) p;
    }

    OPENSSL_free(a);
}

/*
 * 
 * (KeyCache) free item and its children
 *
 */

void SIM_KC_RecursiveFree(SIM_KC * a)
{
    SIM_KC *b;
    do {
	b = (SIM_KC *) a->next;
	SIM_KC_Free(a);
	a = b;
    } while (a != NULL);
}

/*
 * 
 * (KeyCache) create item
 *
 */

SIM_KC *SIM_KC_New()
{
    SIM_KC *a;

    a = (SIM_KC *) OPENSSL_malloc(sizeof(SIM_KC));

    if (a == NULL)
	return NULL;

    a->uin = 0;
    a->key = NULL;
    a->rkey = NULL;
    a->pubkey = NULL;
    a->prev = NULL;
    a->next = NULL;

    return a;
}

/*
 * 
 * (KeyCache) initialize
 *
 */

int SIM_KC_Init()
{
    SIM_KC_Cache = SIM_KC_New();
    if (SIM_KC_Cache == NULL)
	return -1;

    SIM_KC_Cache_LastItem = SIM_KC_Cache;

    return 0;
}

/*
 * 
 * (KeyCache) finish
 *
 */

void SIM_KC_Finish()
{
    SIM_KC_RecursiveFree(SIM_KC_Cache);
}


/*
 * 
 * (KeyCache) add item
 *
 */

int SIM_KC_Add(SIM_KC * a)
{
    SIM_KC *prev;
    prev = SIM_KC_Cache_LastItem;

    if (prev == NULL)
	return -1;

    a->prev = prev;
    prev->next = a;

    SIM_KC_Cache_LastItem = a;

    return 0;
}

/*
 * 
 * (KeyCache) find item by 'uin'
 *
 */

SIM_KC *SIM_KC_Find(uint32 uin)
{
    SIM_KC *cur;

    cur = SIM_KC_Cache_LastItem;

    do {
	//      printf("find: %d\n", cur->uin);
	if (cur->uin == uin)
	    return cur;
	cur = (SIM_KC *) cur->prev;
    } while (cur != NULL);

    return NULL;
}

/*
 * 
 * (KeyCache) find item by 'uin' and delete it
 *
 */

int SIM_KC_Delete(uint32 uin)
{
    SIM_KC *cur;
    SIM_KC *prev, *next;

    cur = SIM_KC_Cache_LastItem;

    do {
	if (cur->uin == uin) {
	    prev = (SIM_KC *) cur->prev;
	    next = (SIM_KC *) cur->next;
	    if (cur == SIM_KC_Cache_LastItem) {
		SIM_KC_Cache_LastItem = prev;
	    }
	    SIM_KC_Free(cur);
	    return 0;
	}
	cur = (SIM_KC *) cur->prev;
    } while (cur->uin != 0);

    return -1;
}


/*
 *
 *  SHA1 sum (Fingerprint) of uin's RSA key
 *
 */
int SIM_Key_Fingerprint (unsigned char* fingerprint, uint32 uin)
{
	RSA *pubkey;	
    EVP_MD_CTX md_ctx;
    unsigned char md_value[EVP_MAX_MD_SIZE],*der,*keybuf;
	char *fname;
	unsigned int dersize,md_len,i;
    
	fname = (char *) OPENSSL_malloc(256);
	SIM_Message_uin2keyfile(uin, fname);
	pubkey = (RSA *)SIM_RSA_ReadKey(fname, PUBLIC);
	OPENSSL_free(fname);
    
    if (!pubkey) {
		return -1;
    };

    dersize   =  i2d_RSAPublicKey(pubkey, NULL);
    der		  =  (unsigned char *) OPENSSL_malloc (dersize*sizeof(unsigned char));

    keybuf=der;
    dersize = i2d_RSAPublicKey(pubkey, &keybuf);

    EVP_DigestInit  (&md_ctx, EVP_sha1());
    EVP_DigestUpdate(&md_ctx, der, dersize);
    EVP_DigestFinal (&md_ctx, md_value, &md_len);

	// zwroci w formie czytelnej dla ludzi - HEX za spacjami
	for(i = 0; i < md_len; i++) sprintf((char *)fingerprint+(i*3),"%02x ", md_value[i]);
    

    return md_len;
}

/*
 * 
 * [EOF]
 *
 */
