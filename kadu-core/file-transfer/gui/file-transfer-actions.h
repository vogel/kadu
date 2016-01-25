/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/memory.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Actions;
class InjectedFactory;
class MenuInventory;
class SendFileAction;
class ShowFileTransferWindowAction;

class FileTransferActions : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit FileTransferActions(QObject *parent = nullptr);
	virtual ~FileTransferActions();

private:
	QPointer<Actions> m_actions;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<MenuInventory> m_menuInventory;

	owned_qptr<SendFileAction> m_sendFileAction;
	owned_qptr<ShowFileTransferWindowAction> m_showFileTransferWindow;

private slots:
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};
