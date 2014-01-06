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

#ifndef CUSTOM_PROPERTIES_VARIANT_WRAPPER_H
#define CUSTOM_PROPERTIES_VARIANT_WRAPPER_H

#include <QtCore/QString>
#include <QtCore/QPointer>

#include "misc/variant-wrapper.h"
#include "storage/custom-properties.h"
#include "exports.h"

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @class CustomPropertiesVariantWrapper
 * @short Class implementing a single CustomProperties entry access with VariantWrapper interface.
 * @author Bartosz 'beevvy' Brachaczek
 *
 * This class uses CustomProperties::property() as VariantWrapper::get() implementation
 * and CustomProperties::addProperty() as VariantWrapper::set() implementation.
 */
class KADUAPI CustomPropertiesVariantWrapper : public VariantWrapper
{
	QPointer<CustomProperties> MyCustomProperties;
	QString Name;
	CustomProperties::Storability Storability;

public:
	/**
	 * @short Constructs object wrapping a single CustomProperties entry.
	 * @author Bartosz 'beevvy' Brachaczek
	 * @param customProperties pointer to a CustomProperties object
	 * @param name name of CustomProperties property which will be wrapped
	 * @param storability CustomProperties::Storability parameter which will be passed to CustomProperties::addProperty() method by set() calls
	 */
	CustomPropertiesVariantWrapper(CustomProperties *customProperties, const QString &name, CustomProperties::Storability storability);
	virtual ~CustomPropertiesVariantWrapper();

	virtual QVariant get(const QVariant &defaultValue = QVariant()) const;
	virtual void set(const QVariant &value);

};

/**
 * @}
 */

#endif // CUSTOM_PROPERTIES_VARIANT_WRAPPER_H
