/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONFIG_FILE_VARIANT_WRAPPER_H
#define CONFIG_FILE_VARIANT_WRAPPER_H

#include <QtCore/QString>

#include "misc/variant-wrapper.h"
#include "exports.h"

/**
 * @addtogroup Configuration
 * @{
 */

/**
 * @class ConfigFileVariantWrapper
 * @short Class implementing a single ConfigFile entry access with VariantWrapper interface.
 * @author Bartosz 'beevvy' Brachaczek
 *
 * This class uses ConfigFile::readEntry() as VariantWrapper::get() implementation
 * and ConfigFile::writeEntry() as VariantWrapper::set() implementation.
 */
class KADUAPI ConfigFileVariantWrapper : public VariantWrapper
{
	QString Group;
	QString Name;

public:
	/**
	 * @short Constructs object wrapping a single ConfigFile entry.
	 * @author Bartosz 'beevvy' Brachaczek
	 * @param group name of ConfigFile group
	 * @param name name of ConfigFile field in @p group
	 */
	ConfigFileVariantWrapper(const QString &group, const QString &name);
	virtual ~ConfigFileVariantWrapper();

	virtual QVariant get(const QVariant &defaultValue = QVariant()) const;
	virtual void set(const QVariant &value);

};

/**
 * @}
 */

#endif // CONFIG_FILE_VARIANT_WRAPPER_H
