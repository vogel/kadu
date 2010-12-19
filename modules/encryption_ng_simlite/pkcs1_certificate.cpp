/*
  File name: pkcs1_certificate.cpp
  Copyright: (C) 2009 Tomasz Kazmierczak

  Creation date: 2009-02-12
  Last modification date: 2009-06-12

  This file is part of Kadu encryption module

  PKCS#1 certificate reader/writer (implemented according to the PKCS#1 v2.1 Specification).
  Note that this implementation was created only for the purpose of reading/writing RSA
  private and public keys from/into the PKCS#1 certificates, encoded in DER format (defined
  in the ITU X.690 Recommendation). This means that this implementation of the PKCS#1
  certificate most likely isn't full.

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

#include <limits.h>
#include <stdint.h>
#include "pkcs1_certificate.h"

PKCS1Certificate::~PKCS1Certificate()
{
	if(Certificate != 0)
		delete Certificate;
}

void PKCS1Certificate::reset()
{
	if(Certificate != 0)
		delete Certificate;
	CertificateReadOffset = 0;
}

bool PKCS1Certificate::storePublicKey(QCA::SecureArray &output, const QCA::BigInteger &modulus,
                                      const QCA::BigInteger &exponent)
{
	reset();
	Certificate = new QCA::SecureArray;

	//we don't know yet what will be the total size of the certificate, so
	//first we will write it's contents, and at the end we will prepend
	//the SEQUENCE information
	//INTEGER - the modulus
	Certificate->append(QCA::SecureArray(1, Integer));
	QCA::SecureArray array = modulus.toArray();
	if(!writeDefiniteLength(array.size()))
		return false;
	Certificate->append(array);

	//INTEGER - the exponent
	Certificate->append(QCA::SecureArray(1, Integer));
	array.clear();
	array = exponent.toArray();
	if(!writeDefiniteLength(array.size()))
		return false;
	Certificate->append(array);

	//now construct the final certificate, first copy what's been written so far
	//into a temporary array
	QCA::SecureArray certContent(*Certificate);
	Certificate->clear();
	//add the SEQUENCE tag, the length and the contents
	Certificate->append(QCA::SecureArray(1, Sequence));
	if(!writeDefiniteLength(certContent.size()))
		return false;
	Certificate->append(certContent);

	//put the certificate into the output array
	output.clear();
	output.append(*Certificate);

	delete Certificate;
	Certificate = 0;
	return true;
}

bool PKCS1Certificate::extractPublicKey(const QCA::SecureArray &certificate,
                                        QCA::BigInteger &modulus, QCA::BigInteger &exponent)
{
	reset();
	Certificate = new QCA::SecureArray(certificate);

	if(Certificate->size() <= 0)
	{
		Status = InvalidCertificate;
		return false;
	}

	uint8_t octet = readNextOctet();
	//we expect the SEQUENCE tag at the beginning
	if(octet != Sequence)
	{
		Status = UnknownCertificate;
		return false;
	}

	//read the length of the rest of the certificate
	uint64_t length = readDefiniteLength();
	if(Status != OK)
		return false;
	//check if the certificate has the needed length
	if(length + CertificateReadOffset > (uint64_t)Certificate->size())
	{
		Status = BrokenCertificate;
		return false;
	}

	octet = readNextOctet();
	//we expect an integer (the modulus)
	if(octet != Integer)
	{
		Status = UnknownCertificate;
		return false;
	}
	//the length of the modulus
	length = readDefiniteLength();
	if(Status != OK)
		return false;
	//check if the certificate has the needed length
	if(length + CertificateReadOffset > (uint64_t)Certificate->size())
	{
		Status = BrokenCertificate;
		return false;
	}
	//read the modulus
	QCA::SecureArray modulusData(length);
	for(uint64_t i = 0; i < length; i++)
		modulusData[i] = readNextOctet();
	modulus.fromArray(modulusData);

	octet = readNextOctet();
	//we expect an integer (the exponent)
	if(octet != Integer)
	{
		Status = UnknownCertificate;
		return false;
	}
	//the length of the exponent
	length = readDefiniteLength();
	if(Status != OK)
		return false;
	//check if the certificate has the needed length
	if(length + CertificateReadOffset > (uint64_t)Certificate->size())
	{
		Status = BrokenCertificate;
		return false;
	}
	//read the exponent
	QCA::SecureArray exponentData(length);
	for(uint64_t i = 0; i < length; i++)
		exponentData[i] = readNextOctet();
	exponent.fromArray(exponentData);

	delete Certificate;
	Certificate = 0;
	return true;
}

bool PKCS1Certificate::storePrivateKey(QCA::SecureArray &output, const QCA::BigInteger &modulus,
                                       const QCA::BigInteger &e, const QCA::BigInteger &p,
                                       const QCA::BigInteger &q, const QCA::BigInteger &d)
{
	reset();
	Certificate = new QCA::SecureArray;

	//we don't know yet what will be the total size of the certificate, so
	//first we will write it's contents, and at the end we will prepend
	//the SEQUENCE information
	//INTEGER - the version (0)
	Certificate->append(QCA::SecureArray(1, Integer));
	QCA::SecureArray array(1, 0);
	if(!writeDefiniteLength(array.size()))
		return false;
	Certificate->append(array);

	//INTEGER - the modulus
	Certificate->append(QCA::SecureArray(1, Integer));
	array = modulus.toArray();
	if(!writeDefiniteLength(array.size()))
		return false;
	Certificate->append(array);

	//INTEGER - the public exponent (e)
	Certificate->append(QCA::SecureArray(1, Integer));
	array.clear();
	array = e.toArray();
	if(!writeDefiniteLength(array.size()))
		return false;
	Certificate->append(array);

	//INTEGER - the private exponent (d)
	Certificate->append(QCA::SecureArray(1, Integer));
	array.clear();
	array = d.toArray();
	if(!writeDefiniteLength(array.size()))
		return false;
	Certificate->append(array);

	//INTEGER - the first prime (p)
	Certificate->append(QCA::SecureArray(1, Integer));
	array.clear();
	array = p.toArray();
	if(!writeDefiniteLength(array.size()))
		return false;
	Certificate->append(array);

	//INTEGER - the second prime (q)
	Certificate->append(QCA::SecureArray(1, Integer));
	array.clear();
	array = q.toArray();
	if(!writeDefiniteLength(array.size()))
		return false;
	Certificate->append(array);

	//INTEGER - exponent1 (d mod (p-1))
	Certificate->append(QCA::SecureArray(1, Integer));
	array.clear();
	QCA::BigInteger temp = p;
	temp -= QCA::BigInteger(1);
	QCA::BigInteger exp = d;
	exp %= temp;
	array = exp.toArray();
	if(!writeDefiniteLength(array.size()))
		return false;
	Certificate->append(array);

	//INTEGER - exponent2 (d mod (q-1))
	Certificate->append(QCA::SecureArray(1, Integer));
	array.clear();
	temp = q;
	temp -= QCA::BigInteger(1);
	exp = d;
	exp %= temp;
	array = exp.toArray();
	if(!writeDefiniteLength(array.size()))
		return false;
	Certificate->append(array);

	//INTEGER - coefficient ((1/q) mod p)
	Certificate->append(QCA::SecureArray(1, Integer));
	array.clear();
	exp = QCA::BigInteger(1);
	exp /= q;
	exp %= p;
	array = exp.toArray();
	if(!writeDefiniteLength(array.size()))
		return false;
	Certificate->append(array);

	//now construct the final certificate, first copy what's been written so far
	//into a temporary array
	QCA::SecureArray certContent(*Certificate);
	Certificate->clear();
	//add the SEQUENCE tag, the length and the contents
	Certificate->append(QCA::SecureArray(1, Sequence));
	if(!writeDefiniteLength(certContent.size()))
		return false;
	Certificate->append(certContent);

	//put the certificate into the output array
	output.clear();
	output.append(*Certificate);

	delete Certificate;
	Certificate = 0;
	return true;
}

bool PKCS1Certificate::extractPrivateKey(const QCA::SecureArray &certificate, QCA::BigInteger &modulus,
                                         QCA::BigInteger &e, QCA::BigInteger &p,
                                         QCA::BigInteger &q, QCA::BigInteger &d)
{
	reset();
	Certificate = new QCA::SecureArray(certificate);

	if(Certificate->size() <= 0)
	{
		Status = InvalidCertificate;
		return false;
	}

	uint8_t octet = readNextOctet();
	//we expect the SEQUENCE tag at the beginning
	if(octet != Sequence)
	{
		Status = UnknownCertificate;
		return false;
	}

	//read the length of the rest of the certificate
	uint64_t length = readDefiniteLength();
	if(Status != OK)
		return false;
	//check if the certificate has the needed length
	if(length + CertificateReadOffset > (uint64_t)Certificate->size())
	{
		Status = BrokenCertificate;
		return false;
	}

	octet = readNextOctet();
	//we expect an integer (version information)
	if(octet != Integer)
	{
		Status = UnknownCertificate;
		return false;
	}
	//the length of the version information
	length = readDefiniteLength();
	if(Status != OK)
		return false;
	//check if the certificate has the needed length
	if(length + CertificateReadOffset > (uint64_t)Certificate->size())
	{
		Status = BrokenCertificate;
		return false;
	}
	//read the version
	QCA::SecureArray versionData(length);
	for(uint64_t i = 0; i < length; i++)
		versionData[i] = readNextOctet();
	QCA::BigInteger version(versionData);
	//only version 0 supported
	if(version > QCA::BigInteger(0))
	{
		Status = UnsuppportedCertVersion;
		return false;
	}

	octet = readNextOctet();
	//we expect an integer (the modulus)
	if(octet != Integer)
	{
		Status = UnknownCertificate;
		return false;
	}
	//the length of the modulus
	length = readDefiniteLength();
	if(Status != OK)
		return false;
	//check if the certificate has the needed length
	if(length + CertificateReadOffset > (uint64_t)Certificate->size())
	{
		Status = BrokenCertificate;
		return false;
	}
	//read the modulus
	QCA::SecureArray modulusData(length);
	for(uint64_t i = 0; i < length; i++)
		modulusData[i] = readNextOctet();
	modulus.fromArray(modulusData);

	octet = readNextOctet();
	//we expect an integer (the public exponent)
	if(octet != Integer)
	{
		Status = UnknownCertificate;
		return false;
	}
	//the length of the public exponent
	length = readDefiniteLength();
	if(Status != OK)
		return false;
	//check if the certificate has the needed length
	if(length + CertificateReadOffset > (uint64_t)Certificate->size())
	{
		Status = BrokenCertificate;
		return false;
	}
	//read the public exponent
	QCA::SecureArray eData(length);
	for(uint64_t i = 0; i < length; i++)
		eData[i] = readNextOctet();
	e.fromArray(eData);

	octet = readNextOctet();
	//we expect an integer (the private exponent)
	if(octet != Integer)
	{
		Status = UnknownCertificate;
		return false;
	}
	//the length of the private exponent
	length = readDefiniteLength();
	if(Status != OK)
		return false;
	//check if the certificate has the needed length
	if(length + CertificateReadOffset > (uint64_t)Certificate->size())
	{
		Status = BrokenCertificate;
		return false;
	}
	//read the private exponent
	QCA::SecureArray dData(length);
	for(uint64_t i = 0; i < length; i++)
		dData[i] = readNextOctet();
	d.fromArray(dData);

	octet = readNextOctet();
	//we expect an integer (the first prime, p)
	if(octet != Integer)
	{
		Status = UnknownCertificate;
		return false;
	}
	//the length of the p
	length = readDefiniteLength();
	if(Status != OK)
		return false;
	//check if the certificate has the needed length
	if(length + CertificateReadOffset > (uint64_t)Certificate->size())
	{
		Status = BrokenCertificate;
		return false;
	}
	//read the p
	QCA::SecureArray pData(length);
	for(uint64_t i = 0; i < length; i++)
		pData[i] = readNextOctet();
	p.fromArray(pData);

	octet = readNextOctet();
	//we expect an integer (the second prime, q)
	if(octet != Integer)
	{
		Status = UnknownCertificate;
		return false;
	}
	//the length of the q
	length = readDefiniteLength();
	if(Status != OK)
		return false;
	//check if the certificate has the needed length
	if(length + CertificateReadOffset > (uint64_t)Certificate->size())
	{
		Status = BrokenCertificate;
		return false;
	}
	//read the q
	QCA::SecureArray qData(length);
	for(uint64_t i = 0; i < length; i++)
		qData[i] = readNextOctet();
	q.fromArray(qData);

	return true;
}

PKCS1Certificate::ConversionStatus PKCS1Certificate::publicKeyToDER(const QCA::RSAPublicKey &key,
                                                                    QCA::SecureArray &certificate)
{
	if(!storePublicKey(certificate, key.n(), key.e()))
		return Status;
	return OK;
}

QCA::RSAPublicKey PKCS1Certificate::publicKeyFromDER(const QCA::SecureArray &certificate,
                                                     ConversionStatus &status)
{
	QCA::BigInteger modulus, exponent;
	if(extractPublicKey(certificate, modulus, exponent))
	{
		status = OK;
		return QCA::RSAPublicKey(modulus, exponent);
	}
	else
	{
		status = Status;
		return QCA::RSAPublicKey();
	}
}

PKCS1Certificate::ConversionStatus PKCS1Certificate::privateKeyToDER(const QCA::RSAPrivateKey &key,
                                                                     QCA::SecureArray &certificate)
{
	if(!storePrivateKey(certificate, key.n(), key.e(), key.p(), key.q(), key.d()))
		return Status;
	return OK;
}

QCA::RSAPrivateKey PKCS1Certificate::privateKeyFromDER(const QCA::SecureArray &certificate,
                                                       ConversionStatus &status)
{
	QCA::BigInteger modulus, e, p, q, d;
	if(extractPrivateKey(certificate, modulus, e, p, q, d))
	{
		status = OK;
		return QCA::RSAPrivateKey(modulus, e, p, q, d);
	}
	else
	{
		status = Status;
		return QCA::RSAPrivateKey();
	}
}

uint8_t PKCS1Certificate::readNextOctet()
{
	if(CertificateReadOffset >= Certificate->size())
		Status = BrokenCertificate;

	uint8_t next = Certificate->at(CertificateReadOffset);
	CertificateReadOffset++;
	return next;
}

uint64_t PKCS1Certificate::readDefiniteLength()
{
	uint8_t octet = readNextOctet();
	//if the first length octet is 255 (all bits are set to 1), then the size is not
	//supported by this implementation
	if(octet == 255) {
		Status = UnsupportedCertSize;
		return 0;
	}

	//if the most significant bit (bit 8) is set, then this is the long form,
	//where bits 7 to 1 denote the number of subsequent length field octets
	if(octet & 128)
	{
		uint8_t lengthOctetsCount = octet & 127;

		//check if the length octets count doesn't exceed 8 (the size of a 64 bit integer)
		if(lengthOctetsCount > 8)
		{
			Status = UnsupportedCertSize;
			return 0;
		}
		//check if reading the Length octets won't take us beyond the certificate data
		if(CertificateReadOffset + lengthOctetsCount > Certificate->size())
		{
			Status = BrokenCertificate;
			return 0;
		}

		//construct the length value
		quint64 length = 0;
                for (quint8 i = lengthOctetsCount; i > 0; i--)
                        length |= ((quint64)readNextOctet()) << ((i-1)*8);

		//the SecureArray stores it's size in an int variable, so we must check
		//if the read length is not greater than the maximum value that int can
		//store
		if(length > INT_MAX)
		{
			Status = UnsupportedCertSize;
			return 0;
		}
		return length;
	}
	//if the short form
	else return octet;
}

bool PKCS1Certificate::writeDefiniteLength(uint64_t length)
{
	if(length <= 127)
		Certificate->append(QCA::SecureArray(1, (uint8_t)length));
	else
	{
		//write the length octets into a temporary array
		QCA::SecureArray array;
		uint8_t lengthOctetsCount = 0;
		//check all the bytes, starting from the most significant byte,
		//but left-side trim all bytes that are equal to zero
		bool begunWriting = false;
		for(uint8_t i = sizeof(length); i > 0; i--) {
			//extract the value of the (i-1)th byte
			uint8_t shiftSize = (i*8)-8;
			uint8_t octet = (uint8_t)((length & ((uint64_t)0xFF << shiftSize)) >> shiftSize);
			if(octet > 0)
				begunWriting = true;
			if(!begunWriting)
				continue;
			array.append(QCA::SecureArray(1, octet));
			lengthOctetsCount++;
		}
		//the maximum supported length-field length is 126 octets (that's a 1008 bit unsigned integer,
		//which means that the maximum length of the value-field is 2^1008 - that's much:D)
		if(lengthOctetsCount > 126) {
			Status = UnsupportedCertSize;
			return false;
		}
		//128 (bit number 8 set) means that this is the long form of the length field
		uint8_t octet = 128 | lengthOctetsCount;
		Certificate->append(QCA::SecureArray(1, octet));
		Certificate->append(array);
	}
	return true;
}
