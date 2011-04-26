/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or(at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GADU_IMPORTED_CONTACT_XML_RECEIVER_H
#define GADU_IMPORTED_CONTACT_XML_RECEIVER_H

#include <QtXmlPatterns/QAbstractXmlReceiver>
#include <QtXmlPatterns/QXmlNamePool>

#include "buddies/buddy.h"

class GaduImportedContactXmlReceiver : public QAbstractXmlReceiver
{
	const QXmlNamePool &XmlNamePool;

	QList<Buddy> ImportedBuddies;
	Buddy CurrentBuddy;

protected:
	virtual void atomicValue(const QVariant &value);
	virtual void attribute(const QXmlName &name, const QStringRef &value);
	virtual void characters(const QStringRef &value);
	virtual void comment(const QString &value);
	virtual void endDocument();
	virtual void endElement();
	virtual void endOfSequence();
	virtual void namespaceBinding(const QXmlName &name);
	virtual void processingInstruction(const QXmlName &target, const QString &value);
	virtual void startDocument();
	virtual void startElement(const QXmlName &name);
	virtual void startOfSequence();

public:
	GaduImportedContactXmlReceiver(const QXmlNamePool &xmlNamePool);
	~GaduImportedContactXmlReceiver();

	const QList<Buddy> & importedBuddies() { return ImportedBuddies; }

};

#endif // GADU_IMPORTED_CONTACT_XML_RECEIVER_H


