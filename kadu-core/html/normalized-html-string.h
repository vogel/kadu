/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "exports.h"

#include <QtCore/QString>

class HtmlString;

class KADUAPI NormalizedHtmlString
{
public:
	NormalizedHtmlString() = default;

	const QString & string() const;

	template<typename ... T>
	NormalizedHtmlString arg(const T & ... args)
	{
		return NormalizedHtmlString{m_string.arg(args.string()...)};
	}

	friend bool operator == (const NormalizedHtmlString &x, const NormalizedHtmlString &y)
	{
		return x.m_string == y.m_string;
	}

	friend bool operator != (const NormalizedHtmlString &x, const NormalizedHtmlString &y)
	{
		return !(x == y);
	}

	friend bool operator < (const NormalizedHtmlString &x, const NormalizedHtmlString &y)
	{
		return x.m_string < y.m_string;
	}

private:
	friend NormalizedHtmlString normalizeHtml(const HtmlString &html);

	explicit NormalizedHtmlString(QString string);
	QString m_string;

};
