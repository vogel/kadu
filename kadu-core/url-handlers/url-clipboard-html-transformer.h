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

#ifndef URL_CLIPBOARD_HTML_TRANSFORMER_H
#define URL_CLIPBOARD_HTML_TRANSFORMER_H

#include <QtCore/QRegExp>

#include "gui/services/clipboard-html-transformer.h"

/**
 * @addtogroup Url
 * @{
 */

/**
 * @class UrlClipboardHtmlTransformer
 * @short Transformer for expanding URLS from short to full version in HTML code that is pasted into clipboard.
 * @author Rafał 'Vogel' Malinowski
 */
class UrlClipboardHtmlTransformer : public ClipboardHtmlTransformer
{
	QRegExp UrlRegExp;

public:
	UrlClipboardHtmlTransformer();
	virtual ~UrlClipboardHtmlTransformer();

	virtual QString transform(const QString &clipboardHtml);

};

/**
 * @}
 */

#endif // URL_CLIPBOARD_HTML_TRANSFORMER_H
