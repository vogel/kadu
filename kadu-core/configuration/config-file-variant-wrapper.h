/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "exports.h"
#include "misc/variant-wrapper.h"

class KADUAPI ConfigFileVariantWrapper : public VariantWrapper
{
	QString Section;
	QString Name;

public:
	ConfigFileVariantWrapper(const QString &section, const QString &name);
	virtual ~ConfigFileVariantWrapper();

	virtual QVariant get(const QVariant &defaultValue = QVariant()) const;
	virtual void set(const QVariant &value);

};

#endif // CONFIG_FILE_VARIANT_WRAPPER_H
