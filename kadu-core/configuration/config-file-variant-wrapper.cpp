/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QVariant>

#include "configuration/configuration-file.h"

#include "config-file-variant-wrapper.h"

ConfigFileVariantWrapper::ConfigFileVariantWrapper(const QString &group, const QString &name) :
		Group(group), Name(name)
{
}

ConfigFileVariantWrapper::~ConfigFileVariantWrapper()
{
}

QVariant ConfigFileVariantWrapper::get(const QVariant &defaultValue) const
{
	return config_file.readEntry(Group, Name, defaultValue.toString());
}

void ConfigFileVariantWrapper::set(const QVariant &value)
{
	config_file.writeEntry(Group, Name, value.toString());
}
