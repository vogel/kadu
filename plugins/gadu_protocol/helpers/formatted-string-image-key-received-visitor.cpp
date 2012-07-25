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

#include "formatted-string/formatted-string-part.h"

#include "formatted-string-image-key-received-visitor.h"

FormattedStringImageKeyReceivedVisitor::FormattedStringImageKeyReceivedVisitor(const QString &id, QObject *parent) :
		QObject(parent), Id(id)
{
}

FormattedStringImageKeyReceivedVisitor::~FormattedStringImageKeyReceivedVisitor()
{
}

void FormattedStringImageKeyReceivedVisitor::visit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringImageKeyReceivedVisitor::visit(const FormattedStringPart * const formattedStringPart)
{
	if (formattedStringPart->isImage())
		emit chatImageKeyReceived(Id, formattedStringPart->imageKey());
}
