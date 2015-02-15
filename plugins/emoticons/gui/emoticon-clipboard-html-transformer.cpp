/*
 * %kadu copyright begin%
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

#include "emoticon-clipboard-html-transformer.h"

// Assume we don't use apostrophes in HTML attributes.
// Expected string to replace is as follows (capitalics are captured):
// <img emoticon="EMOTICON_CODE"*>
EmoticonClipboardHtmlTransformer::EmoticonClipboardHtmlTransformer() :
		EmoticonRegularExpression("<img emoticon=\"([^\"]+)\"[^>]*>")
{
}

EmoticonClipboardHtmlTransformer::~EmoticonClipboardHtmlTransformer()
{
}

QString EmoticonClipboardHtmlTransformer::transform(const QString &clipboardHtml)
{
	return QString(clipboardHtml).replace(EmoticonRegularExpression, QLatin1String("\\1"));
}
