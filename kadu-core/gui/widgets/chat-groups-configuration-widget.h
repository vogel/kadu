/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class GroupList;
class InjectedFactory;

class KADUAPI ChatGroupsConfigurationWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ChatGroupsConfigurationWidget(const Chat &chat, QWidget *parent = nullptr);
	virtual ~ChatGroupsConfigurationWidget();

	void save();

private:
	QPointer<InjectedFactory> m_injectedFactory;

	Chat m_chat;
	GroupList *m_groupList;

	void createGui();

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void init();

};
