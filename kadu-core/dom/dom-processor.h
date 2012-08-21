/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef DOM_PROCESSOR_H
#define DOM_PROCESSOR_H

class QDomDocument;

class DomVisitor;

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class DomProcessor
 * @short Helper class for processing DOM documents.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class can be used to process DOM documents using visitor pattern.
 * Visitor object is allowed to modify DOM tree in any way.
 */
class DomProcessor
{
	QDomDocument &DomDocument;

public:
	/**
	 * @short Create new instance of DomProcessor.
	 * @author Rafał 'Vogel' Malinowski
	 * @param domDocument DOM document to process
	 */
	DomProcessor(QDomDocument &domDocument);

	/**
	 * @short Accept a visitor.
	 * @author Rafał 'Vogel' Malinowski
	 * @param visitor to accept
	 *
	 * Processing is not read-only, so domDocument can be changed after calling this method.
	 */
	void accept(DomVisitor *visitor);

};

/**
 * @}
 */

#endif // DOM_PROCESSOR_H
