/*
  File name: kadu_encryption_factory.h
  Copyright: (C) 2008-2009 Tomasz Kazmierczak

  Creation date: 2008-08-14
  Last modification date: 2009-02-16

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

#ifndef KADU_ENCRYPTION_FACTORY_H
#define KADU_ENCRYPTION_FACTORY_H

#include "kadu_encryption_interface.h"

/**
 * An encryption object factory class, implemented as a singleton.
**/
class KaduEncryptionFactory
{
		QString ErrorInfo;
		KaduEncryptionFactory() : ErrorInfo("No error") {}

	public:
		/**
		 * Enumeration of available encryption methods.
		**/
		enum KaduEncryptionMethod {RSA, SIMLite};

		/**
		 * Returns an instance of the KaduEncryptionFactory.
		 *
		 * @return An instance of the factory object.
		**/
		static KaduEncryptionFactory *instance();

		/**
		 * The actual factory implementation that creates an encryption object
		 * that implements specified encryption method.
		 *
		 * @param method the encryption method for which to create the encryption object
		 * @param keysPath path to the directory for storing keys, if applicable
		 * @return Newely created encryption object, or 0 if an error occured.
		**/
		KaduEncryptionInterface *createEncryptionObject(KaduEncryptionMethod method, QString keysPath);

		/**
		 * Returns a description of the last error that occured.
		 *
		 * @return The description of the last error that occured.
		**/
		const QString & errorInfo() const { return ErrorInfo; }
};

#endif //KADU_ENCRYPTION_FACTORY_H
