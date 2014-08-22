/*
 * %kadu copyright begin%
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#ifndef EMOTICON_CLIPBOARD_HTML_TRANSFORMER_H
#define EMOTICON_CLIPBOARD_HTML_TRANSFORMER_H

#include <QtCore/QRegExp>

#include "gui/services/clipboard-html-transformer.h"

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonClipboardHtmlTransformer
 * @short Transformer for replacing emoticon images with emoticon text triggers in HTML code that is pasted into clipboard.
 * @author Rafał 'Vogel' Malinowski
 */
class EmoticonClipboardHtmlTransformer : public ClipboardHtmlTransformer
{
	QRegExp EmoticonRegularExpression;

public:
	EmoticonClipboardHtmlTransformer();
	virtual ~EmoticonClipboardHtmlTransformer();

	virtual QString transform(const QString &clipboardHtml);

};

/**
 * @}
 */

#endif // EMOTICON_CLIPBOARD_HTML_TRANSFORMER_H
