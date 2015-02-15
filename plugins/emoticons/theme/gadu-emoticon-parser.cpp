/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QStringList>

#include "misc/misc.h"

#include "emoticon.h"

#include "gadu-emoticon-parser.h"

GaduEmoticonParser::GaduEmoticonParser(const QString &themePath, const QString &emoticonLine) :
		ThemePath(themePath), EmoticonLine(emoticonLine), Index(0)
{
	parse();
}

QChar GaduEmoticonParser::peek()
{
	if (Index >= EmoticonLine.length())
		return QChar();
	return EmoticonLine.at(Index);
}

QChar GaduEmoticonParser::get()
{
	QChar result = peek();
	Index++;
	return result;
}

void GaduEmoticonParser::eat()
{
	Index++;
}

void GaduEmoticonParser::parse()
{
	QChar c = peek();
	if (c.isNull())
		return;

	if (c == '*') // ignore first *, I don't know why
		eat();

	QStringList aliases = parseAliases();
	eat(); // ,
	QString animatedPath = parseQuoted();
	eat(); // ,
	QString staticPath = parseQuoted();

	if (aliases.isEmpty() || animatedPath.isEmpty())
		return;

	animatedPath = ThemePath + fixFileName(ThemePath, animatedPath);
	if (staticPath.isEmpty())
		staticPath = animatedPath;
	else
		staticPath = ThemePath + fixFileName(ThemePath, staticPath);

	Result = Emoticon(aliases.at(0), staticPath, animatedPath);
	foreach (const QString &alias, aliases)
		Aliases.append(Emoticon(alias, staticPath, animatedPath));
}

QStringList GaduEmoticonParser::parseAliases()
{
	QStringList result;

	QChar c = peek();
	if (c.isNull())
		return result;

	bool multiple = false;
	if (c == '(')
	{
		multiple = true;
		eat();
	}

	while (true)
	{
		QString alias = parseQuoted();
		if (!alias.isEmpty())
			result.append(alias);

		if (!multiple)
			return result;

		c = get();
		if (c.isNull() || c == ')')
			return result;

		if (c != ',') // some kind of error
			return result;
	}

	Q_ASSERT(false);
}

QString GaduEmoticonParser::parseQuoted()
{
	QChar c = peek();
	if (c == '"')
	{
		eat();
		int quoteIndex = EmoticonLine.indexOf('"', Index);
		if (quoteIndex < 0)
			return getToIndex(EmoticonLine.length());
		else
			return getToIndex(quoteIndex);
	}

	int bracketIndex = EmoticonLine.indexOf(')', Index);
	int comaIndex = EmoticonLine.indexOf(',', Index);
	int endIndex = bracketIndex < 0
			? comaIndex
			: comaIndex < 0
					? bracketIndex
					: qMin(bracketIndex, comaIndex);

	return getToIndex(endIndex);
}

QString GaduEmoticonParser::getToIndex(int endIndexExclusive)
{
	QString result = EmoticonLine.mid(Index, endIndexExclusive - Index);
	Index = endIndexExclusive + 1;
	return result;
}
Emoticon GaduEmoticonParser::emoticon() const
{
	return Result;
}

QVector<Emoticon> GaduEmoticonParser::aliases() const
{
	return Aliases;
}
