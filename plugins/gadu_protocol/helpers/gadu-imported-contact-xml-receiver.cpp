/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "gadu-imported-contact-xml-receiver.h"

GaduImportedContactXmlReceiver::GaduImportedContactXmlReceiver(const QXmlNamePool &xmlNamePool) :
		XmlNamePool(xmlNamePool)
{

}

GaduImportedContactXmlReceiver::~GaduImportedContactXmlReceiver()
{

}

void GaduImportedContactXmlReceiver::atomicValue(const QVariant &value)
{
	Q_UNUSED(value)
}

void GaduImportedContactXmlReceiver::attribute(const QXmlName &name, const QStringRef &value)
{
	if (CurrentBuddy)
		CurrentBuddy.setCustomData(name.localName(XmlNamePool), value.toString());
}

void GaduImportedContactXmlReceiver::characters(const QStringRef &value)
{
	Q_UNUSED(value)
}

void GaduImportedContactXmlReceiver::comment(const QString &value)
{
	Q_UNUSED(value)
}

void GaduImportedContactXmlReceiver::endDocument()
{
}

void GaduImportedContactXmlReceiver::endElement()
{
	if (CurrentBuddy)
	{
		ImportedBuddies.append(CurrentBuddy);
		CurrentBuddy = Buddy::null;
	}
}

void GaduImportedContactXmlReceiver::endOfSequence()
{
}

void GaduImportedContactXmlReceiver::namespaceBinding(const QXmlName &name)
{
	Q_UNUSED(name);
}

void GaduImportedContactXmlReceiver::processingInstruction(const QXmlName &target, const QString &value)
{
	Q_UNUSED(target);
	Q_UNUSED(value);
}

void GaduImportedContactXmlReceiver::startDocument()
{
}

void GaduImportedContactXmlReceiver::startElement(const QXmlName &name)
{
	if (name.localName(XmlNamePool) == "Contact")
		CurrentBuddy = Buddy::create();
}

void GaduImportedContactXmlReceiver::startOfSequence()
{
}
