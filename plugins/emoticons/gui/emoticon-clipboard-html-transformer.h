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

#pragma once

#include "gui/services/clipboard-html-transformer.h"

#include <QtCore/QObject>
#include <QtCore/QRegExp>

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonClipboardHtmlTransformer
 * @short Transformer for replacing emoticon images with emoticon text triggers in HTML code that is pasted into clipboard.
 * @author Rafał 'Vogel' Malinowski
 */
class EmoticonClipboardHtmlTransformer : public QObject, public ClipboardHtmlTransformer
{
	Q_OBJECT

	QRegExp EmoticonRegularExpression;

public:
	Q_INVOKABLE EmoticonClipboardHtmlTransformer(QObject *parent = nullptr);
	virtual ~EmoticonClipboardHtmlTransformer();

	virtual QString transform(const QString &clipboardHtml);

};

/**
 * @}
 */
