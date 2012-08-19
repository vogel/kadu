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

class DomTextCallback;

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class DomProcessor
 * @short Helper class for processing DOM documents.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class can be used to process DOM documents in easy way. To use this class set callbacks for different node types
 * using setXXXCallback methods (fro example use setDomTextCallback to set callback for handling text nodes) and call
 * processDomDocument().
 *
 * Callback objects are allowed to modify DOM tree in any way.
 */
class DomProcessor
{
	DomTextCallback *TextCallback;

public:
	/**
	 * @short Create new instance of DomProcessor.
	 * @author Rafał 'Vogel' Malinowski
	 */
	DomProcessor();

	/**
	 * @short Set callback class for handling text nodes.
	 * @author Rafał 'Vogel' Malinowski
	 * @short textCallback callback for handling text nodes
	 */
	void setDomTextCallback(DomTextCallback *textCallback);

	/**
	 * @short Process document using set callbacks.
	 * @author Rafał 'Vogel' Malinowski
	 * @param domDocument document to process
	 *
	 * Processing is not read-only, so domDocument can be changed after calling this method.
	 */
	void processDomDocument(QDomDocument domDocument);

};

/**
 * @}
 */

#endif // DOM_PROCESSOR_H
