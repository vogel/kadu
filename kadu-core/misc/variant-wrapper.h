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

#ifndef VARIANT_WRAPPER_H
#define VARIANT_WRAPPER_H

#include "exports.h"

class QVariant;

class KADUAPI VariantWrapper
{
public:
	virtual ~VariantWrapper() {}

	virtual QVariant get(const QVariant &defaultValue = QVariant()) const = 0;
	virtual void set(const QVariant &value) = 0;

};

#endif // VARIANT_WRAPPER_H
