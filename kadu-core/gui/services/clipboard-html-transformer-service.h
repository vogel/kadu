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

#ifndef CLIPBOARD_HTML_TRANSFORMER_SERVICE_H
#define CLIPBOARD_HTML_TRANSFORMER_SERVICE_H

#include <QtCore/QObject>

#include "gui/services/clipboard-html-transformer.h"

#include "services/transformer-service.h"

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ClipboardHtmlTransformerService
 * @short Service for fixing HTML code that is pasted into clipboard.
 * @author Rafał 'Vogel' Malinowski
 *
 * Use this service to fix HTML code that is pasted into clipboard. Fixing can include: replacing emoticon images
 * with emoticon text triggers, expanding URLs, removing unneeded message parts.
 *
 * This should be done by DomProcessor, but unfortunately what is in chat view is not exactly valid XML (yet).
 */
class ClipboardHtmlTransformerService : public QObject, public TransformerService<ClipboardHtmlTransformer>
{
	Q_OBJECT

public:
	explicit ClipboardHtmlTransformerService(QObject *parent = 0);
	virtual ~ClipboardHtmlTransformerService();

};

/**
 * @}
 */

#endif // CLIPBOARD_HTML_TRANSFORMER_SERVICE_H
