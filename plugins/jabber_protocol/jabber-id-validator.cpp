/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-id-validator.h"

#include "jid.h"

#include <QtCore/QRegExp>

QValidator *JabberIdValidator::Instance = 0;

void JabberIdValidator::createInstance()
{
	if (!Instance)
		Instance = new JabberIdValidator();
}

void JabberIdValidator::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

QValidator * JabberIdValidator::instance()
{
	return Instance;
}

JabberIdValidator::JabberIdValidator(QObject *parent) :
		QValidator(parent)
{
}

JabberIdValidator::~JabberIdValidator()
{
}

QValidator::State JabberIdValidator::validate(QString &input, int &pos) const
{
	QString mid(input.mid(pos));
	if (mid.isEmpty())
		return QValidator::Intermediate;

	auto jid = Jid::parse(input);
	if (jid.isEmpty())
		return QValidator::Invalid;

	if (jid.node().isEmpty() || jid.domain().isEmpty() || jid.domain().contains('@'))
		return QValidator::Invalid;

	return QValidator::Acceptable;
}

#include "moc_jabber-id-validator.cpp"
