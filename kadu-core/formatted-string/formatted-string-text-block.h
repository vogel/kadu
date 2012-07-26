/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef FORMATTED_STRING_TEXT_BLOCK_H
#define FORMATTED_STRING_TEXT_BLOCK_H

#include <QtCore/QString>
#include <QtGui/QColor>

#include "exports.h"

#include "formatted-string.h"

class QTextDocument;

class KADUAPI FormattedStringTextBlock : public FormattedString
{
	QString Content;
	bool Bold;
	bool Italic;
	bool Underline;
	QColor Color;

public:
	FormattedStringTextBlock(const QString &content, bool bold, bool italic, bool underline, QColor color);
	virtual ~FormattedStringTextBlock();

	virtual void accept(FormattedStringVisitor *visitor) const;

	virtual bool isEmpty() const;

	QString content() const;
	bool bold() const;
	bool italic() const;
	bool underline() const;
	QColor color() const;

};

#endif // FORMATTED_STRING_TEXT_BLOCK_H
