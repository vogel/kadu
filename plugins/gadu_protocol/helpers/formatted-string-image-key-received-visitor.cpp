/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "formatted-string/formatted-string-image-block.h"

#include "formatted-string-image-key-received-visitor.h"

FormattedStringImageKeyReceivedVisitor::FormattedStringImageKeyReceivedVisitor(const QString &id, QObject *parent) :
		QObject(parent), Id(id)
{
}

FormattedStringImageKeyReceivedVisitor::~FormattedStringImageKeyReceivedVisitor()
{
}

void FormattedStringImageKeyReceivedVisitor::beginVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringImageKeyReceivedVisitor::endVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringImageKeyReceivedVisitor::visit(const FormattedStringImageBlock * const formattedStringImageBlock)
{
	if (formattedStringImageBlock->imagePath().length() != 16)
		return;

	emit chatImageKeyReceived(Id, formattedStringImageBlock->image());
}

void FormattedStringImageKeyReceivedVisitor::visit(const FormattedStringTextBlock * const formattedStringTextBlock)
{
	Q_UNUSED(formattedStringTextBlock);
}

#include "moc_formatted-string-image-key-received-visitor.cpp"
