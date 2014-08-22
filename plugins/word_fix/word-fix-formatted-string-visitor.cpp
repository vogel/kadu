/*
 * %kadu copyright begin%
 * Copyright 2012, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "formatted-string/formatted-string-text-block.h"
#include "misc/memory.h"

#include "word-fix-formatted-string-visitor.h"

WordFixFormattedStringVisitor::WordFixFormattedStringVisitor(QMap<QString, QString> words) :
		Words(words)
{
}

WordFixFormattedStringVisitor::~WordFixFormattedStringVisitor()
{
}

QString WordFixFormattedStringVisitor::fixWord(const QString &content, const QString &word, const QString &fix)
{
	QString result = content;
	const int wordLength = word.length();
	const int fixLength = fix.length();

	int pos = 0;
	while ((pos = result.indexOf(word, pos)) != -1)
	{
		bool beginsWord = (pos == 0);
		if (!beginsWord)
		{
			const QChar ch(result.at(pos - 1));
			beginsWord = !ch.isLetterOrNumber() && !ch.isMark() && ch != QLatin1Char('_');

			if (!beginsWord)
			{
				pos += wordLength;
				continue;
			}
		}

		bool endsWord = (pos + wordLength == result.length());
		if (!endsWord)
		{
			const QChar ch(result.at(pos + wordLength));
			endsWord = !ch.isLetterOrNumber() && !ch.isMark() && ch != QLatin1Char('_');

			if (!endsWord)
			{
				pos += wordLength;
				continue;
			}
		}

		result.replace(pos, wordLength, fix);
		pos += fixLength;
	}

	return result;
}

QString WordFixFormattedStringVisitor::fixWords(const QString &content)
{
	QString result = content;

	for (QMap<QString, QString>::const_iterator i = Words.constBegin(); i != Words.constEnd(); ++i)
		result = fixWord(result, i.key(), i.value());

	return result;
}

void WordFixFormattedStringVisitor::visit(const FormattedStringTextBlock * const formattedStringTextBlock)
{
	cloned(make_unique<FormattedStringTextBlock>(
		fixWords(formattedStringTextBlock->content()),
		formattedStringTextBlock->bold(),
		formattedStringTextBlock->italic(),
		formattedStringTextBlock->underline(),
		formattedStringTextBlock->color()
	));
}

void WordFixFormattedStringVisitor::visit(const FormattedStringImageBlock * const formattedStringImageBlock)
{
	FormattedStringCloneVisitor::visit(formattedStringImageBlock);
}
