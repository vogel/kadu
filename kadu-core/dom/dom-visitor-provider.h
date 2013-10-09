/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef DOM_VISITOR_PROVIDER_H
#define DOM_VISITOR_PROVIDER_H

#include "provider/provider.h"

class DomVisitor;

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class DomVisitorProvider
 * @short Provider that returns DomVisitor instances.
 * @author Rafał 'Vogel' Malinowski
 *
 * Register DomVisitorProvider in DomProcessorService to make them accessible everywhere.
 */
class DomVisitorProvider : public Provider<DomVisitor *>
{
public:
	virtual ~DomVisitorProvider() {}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Provide DomVisitor.
	 *
	 * This method can return null value.
	 */
	virtual DomVisitor * provide() const = 0;

};

/**
 * @}
 */

#endif // DOM_VISITOR_PROVIDER_H
