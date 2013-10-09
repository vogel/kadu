/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef WORD_FIX_FORMATTED_STRING_VISITOR_H
#define WORD_FIX_FORMATTED_STRING_VISITOR_H

#include <QtCore/QMap>

#include "formatted-string/formatted-string-clone-visitor.h"

class WordFixFormattedStringVisitor : public FormattedStringCloneVisitor
{
	QMap<QString, QString> Words;

	QString fixWord(const QString &content, const QString &word, const QString &fix);
	QString fixWords(const QString &content);

public:
	explicit WordFixFormattedStringVisitor(QMap<QString, QString> words);
	virtual ~WordFixFormattedStringVisitor();

	virtual void visit(const FormattedStringTextBlock * const formattedStringTextBlock);
	virtual void visit(const FormattedStringImageBlock * const formattedStringImageBlock);

};

#endif // WORD_FIX_FORMATTED_STRING_VISITOR_H
