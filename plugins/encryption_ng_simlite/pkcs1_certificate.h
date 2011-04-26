/*
  File name: pkcs1_certificate.h
  Copyright: (C) 2009 Tomasz Kazmierczak

  Creation date: 2009-02-12
  Last modification date: 2009-02-25

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

#ifndef PKCS1_CERTIFICATE_H
#define PKCS1_CERTIFICATE_H

#include <QtCrypto>
#include <stdint.h>

/**
 * PKCS#1 certificate reader/writer (implemented according to the PKCS#1 v2.1 Specification).
 * Note that this implementation was created only for the purpose of reading/writing RSA
 * private and public keys from/into PKCS#1 certificates, encoded in DER format (defined
 * in the ITU X.690 Recommendation). This means that this implementation of the PKCS#1
 * certificate most likely isn't full.
**/
class PKCS1Certificate
{
	public:
		/**
		 * An enumeration of possible conversion statuses.
		**/
		enum ConversionStatus {OK, UnknownCertificate, InvalidCertificate, BrokenCertificate, UnsupportedCertSize,
		                       UnsuppportedCertVersion, CannotCreatePublicKey, CannotCreatePrivateKey};

		/**
		 * The only constructor of the class. Initializes the class.
		**/
		PKCS1Certificate() : Certificate(0), CertificateReadOffset(0), Status(OK) {}
		/**
		 * The destructor of the class.
		**/
		~PKCS1Certificate();

		/**
		 * DER-encodes given RSA public key.
		 * 
		 * @param key [in] a reference to a QCA::RSAPublicKey to store in a certificate
		 * @param certificate [out] a reference to a QCA::SecureArray that will contain the certificate
		 * @return A status result of the conversion.
		**/
		ConversionStatus publicKeyToDER(const QCA::RSAPublicKey &key, QCA::SecureArray &certificate);
		/**
		 * Reads DER-encoded RSA public key from a given certificate.
		 * 
		 * @param certificate [in] a reference to a QCA::SecureArray containing the certificate
		 * @param status [out] a reference to a variable that will store the result of the conversion
		 * @return An RSA public key read from the certificate, or an empty key when failed.
		**/
		QCA::RSAPublicKey publicKeyFromDER(const QCA::SecureArray &certificate, ConversionStatus &status);

		/**
		 * DER-encodes given RSA private key.
		 * 
		 * @param key [in] a reference to a QCA::RSAPrivateKey to store in a certificate
		 * @param certificate [out] a reference to a QCA::SecureArray that will contain the certificate
		 * @return A status result of the conversion.
		**/
		ConversionStatus privateKeyToDER(const QCA::RSAPrivateKey &key, QCA::SecureArray &certificate);
		/**
		 * Reads DER-encoded RSA private key from a given certificate.
		 * 
		 * @param certificate [in] a reference to a QCA::SecureArray containing the certificate
		 * @param status [out] a reference to a variable that will store the result of the conversion
		 * @return An RSA public key read from the certificate, or an empty key when failed.
		**/
		QCA::RSAPrivateKey privateKeyFromDER(const QCA::SecureArray &certificate, ConversionStatus &status);

	private:
		enum DERTag {Integer = 0x02, Sequence = 0x30};

		QCA::SecureArray *Certificate;
		int               CertificateReadOffset;
		ConversionStatus  Status;

		bool storePublicKey(QCA::SecureArray &output, const QCA::BigInteger &modulus,
		                    const QCA::BigInteger &exponent);
		bool extractPublicKey(const QCA::SecureArray &certificate,
		                      QCA::BigInteger &modulus, QCA::BigInteger &exponent);

		bool storePrivateKey(QCA::SecureArray &output, const QCA::BigInteger &modulus,
		                     const QCA::BigInteger &e, const QCA::BigInteger &p,
		                     const QCA::BigInteger &q, const QCA::BigInteger &d);
		bool extractPrivateKey(const QCA::SecureArray &certificate, QCA::BigInteger &modulus,
		                       QCA::BigInteger &e, QCA::BigInteger &p,
		                       QCA::BigInteger &q, QCA::BigInteger &d);

		uint8_t readNextOctet();
		uint64_t readDefiniteLength();
		bool writeDefiniteLength(uint64_t length);
		void reset();
};

#endif //PKCS1_CERTIFICATE_H
