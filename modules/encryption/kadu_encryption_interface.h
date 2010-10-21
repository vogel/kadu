/*
  File name: kadu_encryption_interface.h
  Copyright: (C) 2008-2009 Tomasz Kazmierczak

  Creation date: 2008-08-14
  Last modification date: 2009-03-07

  This file is part of Kadu encryption module

 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.  *
 *
 */

#ifndef KADU_ENCRYPTION_INTERFACE_H
#define KADU_ENCRYPTION_INTERFACE_H

#include <QString>
#include <stdint.h>

typedef enum {
	KEE_NO_ERROR,            //No error
	KEE_GENERATE_PUBKEY,     //Could not generate public key
	KEE_GENERATE_PRIVKEY,    //Could not generate private key
	KEE_WRITE_PUBKEY,        //Could not write public key
	KEE_WRITE_PRIVKEY,       //Could not write private key
	KEE_READ_PUBKEY,         //Could not read the public key
	KEE_READ_PRIVKEY,        //Could not read the private key
	KEE_ENCRYPTION,          //Error while encrypting
	KEE_DECRYPTION,          //Error while decrypting
	KEE_KEY_CANNOT_ENCRYPT,  //The (public) key cannot encrypt
	KEE_KEY_CANNOT_DECRYPT,  //The (private) key cannot decrypt
	KEE_READ_INVALID_MESSAGE //The received encrypted message is invalid
} KaduEncrytpionError;

/**
 * An interface for encryption objects in kadu.
**/
class KaduEncryptionInterface
{
	public:
		virtual ~KaduEncryptionInterface() {}

		/**
		 * This function generates the pair of private and public keys
		 * if such operation is supported by the chosen encryption method.
		 * 
		 * @param keyId identifier for the generated keys
		 * @return true if keys were generated successfuly, false otherwise.
		 */
		virtual bool generateKeys(QString keyId) = 0;

		/**
		 * Encrypts the given message.
		 * 
		 * @param message the message to encrypt
		 * @param keyId an identifier of a public key to use for encryption
		 * @return true if the encryption succeeded, false otherwise.
		 */
		virtual bool encrypt(QByteArray &message, QString keyId) = 0;
		/**
		 * Decrypts the given message.
		 * 
		 * @param message the message to decrypt
		 * @return true if the decryption succeeded, false otherwise.
		 */
		virtual bool decrypt(QByteArray &message) = 0;

		/**
		 * Calculates public key's fingerprint.
		 * 
		 * @param keyId an identifier of a public key of which to calculate the fingerprint
		 * @return The calculated fingerprint.
		 */
		virtual QString calculatePublicKeyFingerprint(QString keyId) = 0;

		/**
		 * Returns a code of the last error.
		 * 
		 * @return The error code of the last error.
		 */
		virtual KaduEncrytpionError error() = 0;
		/**
		 * Returns a description of the last error.
		 * 
		 * @return A pointer to a C-string describing the last error.
		 */
		virtual const char *errorDescription() = 0;

		/**
		 * Releases the encryption object (frees the allocated memory, etc.).
		 */
		virtual void release() = 0;
};

#endif //KADU_ENCRYPTION_INTERFACE_H
