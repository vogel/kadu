/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "custom-properties-variant-wrapper.h"

CustomPropertiesVariantWrapper::CustomPropertiesVariantWrapper(CustomProperties *customProperties, const QString &name, CustomProperties::Storability storability) :
		MyCustomProperties(customProperties), Name(name), Storability(storability)
{
}

CustomPropertiesVariantWrapper::~CustomPropertiesVariantWrapper()
{
}

QVariant CustomPropertiesVariantWrapper::get(const QVariant &defaultValue) const
{
	return MyCustomProperties.isNull()
			? defaultValue
			: MyCustomProperties->property(Name, defaultValue);
}

void CustomPropertiesVariantWrapper::set(const QVariant &value)
{
	if (!MyCustomProperties.isNull())
		MyCustomProperties->addProperty(Name, value, Storability);
}
