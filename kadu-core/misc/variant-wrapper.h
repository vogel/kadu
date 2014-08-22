/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef VARIANT_WRAPPER_H
#define VARIANT_WRAPPER_H

#include "exports.h"

class QVariant;

/**
 * @addtogroup Misc
 * @{
 */

/**
 * @class VariantWrapper
 * @short Interface class for getting and setting a single QVariant value.
 * @author Bartosz 'beevvy' Brachaczek
 */
class KADUAPI VariantWrapper
{
public:
	virtual ~VariantWrapper() {}

	/**
	 * @short Returns value stored by the underlying resource.
	 * @author Bartosz 'beevvy' Brachaczek
	 * @param defaultValue value which will returned if no value has been ever set on the underlying resource
	 * @return value stored by the underlying resource or @p defaultValue
	 *
	 * Returns value stored by the underlying resource. If no value has been
	 * ever set on the underlying resource, @p defaultValue is returned.
	 */
	virtual QVariant get(const QVariant &defaultValue = QVariant()) const = 0;

	/**
	 * @short Sets passed value on the underlying resource.
	 * @author Bartosz 'beevvy' Brachaczek
	 * @param value value which will be set on the underlying resource
	 */
	virtual void set(const QVariant &value) = 0;

};

/**
 * @}
 */

#endif // VARIANT_WRAPPER_H
