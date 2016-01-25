/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class IconsManager;
class Parser;
class StatusContainerManager;
class StatusTypeManager;
class TalkableConverter;

class ContactParserTags : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ContactParserTags(QObject *parent = nullptr);
	virtual ~ContactParserTags();

private:
	QPointer<IconsManager> m_iconsManager;
	QPointer<Parser> m_parser;
	QPointer<StatusContainerManager> m_statusContainerManager;
	QPointer<StatusTypeManager> m_statusTypeManager;
	QPointer<TalkableConverter> m_talkableConverter;

private slots:
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setParser(Parser *parser);
	INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
	INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
	INJEQT_SET void setTalkableConverter(TalkableConverter *talkableConverter);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};
