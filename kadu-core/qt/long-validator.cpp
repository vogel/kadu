/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "long-validator.h"

LongValidator::LongValidator(qlonglong bottom, qlonglong top, QObject* parent) :
		QValidator(parent), Bottom(bottom), Top(top)
{
}

LongValidator::~LongValidator()
{
}

QValidator::State LongValidator::validate(QString &input, int &) const
{
	if (input.isEmpty())
		return Intermediate;

	bool ok;
	qlonglong result = input.toLongLong(&ok);

	if (!ok)
		return Invalid;

	if (result < Bottom || result > Top)
		return Invalid;

	return Acceptable;
}

void LongValidator::fixup(QString &input) const
{
	QString result;

	int length = input.length();
	for (int i = 0; i < length; i++)
	{
		QChar c = input.at(i);
		if (c.isDigit())
			result += c;
	}

	input = result;
}
