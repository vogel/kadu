/*
  File name: kadu_encryption_factory.cpp
  Copyright: (C) 2008-2009 Tomasz Kazmierczak

  Creation date: 2008-08-14
  Last modification date: 2009-03-08

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

#include <QtCrypto>
#include "kadu_encryption_factory.h"
#include "kadu_encryption_rsa.h"
#include "kadu_encryption_simlite.h"

KaduEncryptionFactory *KaduEncryptionFactory::instance()
{
	static KaduEncryptionFactory inst;
	return &inst;
}

KaduEncryptionInterface *KaduEncryptionFactory::createEncryptionObject(KaduEncryptionMethod method, QString keysPath)
{
	QCA::Initializer init;
	KaduEncryptionInterface *ifc = 0;
	switch(method)
	{
		case RSA:
		{
			//we need support for the RSA algorithm
			if(!QCA::isSupported("pkey") ||
			   !QCA::PKey::supportedIOTypes().contains(QCA::PKey::RSA) ||
			   !QCA::isSupported("sha1"))
				ErrorInfo = "The QCA OSSL plugin for libqca2 is not present!";
			else
			{
				ifc = new KaduEncryptionRSA(keysPath);
				if(ifc == 0)
					ErrorInfo = "Out of memory!";
			}
		}
		break;
		case SIMLite:
		{
			//we need support for the RSA and Blowfish algorithms
			if(!QCA::isSupported("pkey") ||
			   !QCA::PKey::supportedIOTypes().contains(QCA::PKey::RSA) ||
			   !QCA::isSupported("blowfish-cbc-pkcs7") ||
			   !QCA::isSupported("sha1"))
				ErrorInfo = "The QCA OSSL plugin for libqca2 is not present!";
			else
			{
				ifc = new KaduEncryptionSIMLite(keysPath);
				if(ifc == 0)
					ErrorInfo = "Out of memory!";
			}
		}
		break;
	}
	return ifc;
}
