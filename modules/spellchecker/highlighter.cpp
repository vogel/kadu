/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "spellchecker.h"

#include "highlighter.h"

QList<Highlighter *> Highlighter::Highlighters;
QTextCharFormat Highlighter::HighlightFormat;

void Highlighter::rehighlightAll()
{
	foreach (Highlighter *highlighter, Highlighters)
		highlighter->rehighlight();
}

Highlighter::Highlighter(QTextDocument *document) :
		QSyntaxHighlighter(document)
{
	Highlighters.append(this);
}

Highlighter::~Highlighter()
{
	Highlighters.removeAll(this);
}

void Highlighter::highlightBlock(const QString& text)
{
	QRegExp word("\\b\\w+\\b");

	int index = 0;
	while ((index = word.indexIn(text, index)) != -1)
	{
		if (!spellcheck->checkWord(word.cap()))
			setFormat(index, word.matchedLength(), HighlightFormat);
		index += word.matchedLength();
	}
}

void Highlighter::setHighlightFormat(const QTextCharFormat &format)
{
	HighlightFormat = format;
}

void Highlighter::removeAll()
{
	qDeleteAll(Highlighters);
	Highlighters.clear();
}
