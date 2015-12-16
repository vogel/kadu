/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "docking-exports.h"
#include "status-notifier-item-configuration.h"

#include "misc/memory.h"

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtWidgets/QSystemTrayIcon>
#include <functional>

class StatusNotifierItemAttention;

class QMenu;

class DOCKINGAPI StatusNotifierItem final : public QObject
{
	Q_OBJECT

public:
	explicit StatusNotifierItem(QObject *parent = nullptr);
	virtual ~StatusNotifierItem();

	void setConfiguration(StatusNotifierItemConfiguration configuration);
	void setIconLoader(std::function<QIcon(const QString &)> iconLoader);
	void setNeedAttention(bool needAttention);
	void setTooltip(const QString &tooltip);

	void showMessage(QString title, QString message, QSystemTrayIcon::MessageIcon icon, int msecs);

	QPoint trayPosition();
	QMenu * contextMenu();

signals:
	void activateRequested();
	void messageClicked();

private:
	StatusNotifierItemConfiguration m_configuration;
	std::function<QIcon(const QString &)> m_iconLoader;
	bool m_needAttention;
	QPoint m_systemTrayLastPosition;
	owned_qptr<QSystemTrayIcon> m_systemTrayIcon;
	not_owned_qptr<StatusNotifierItemAttention> m_attention;

	void updateAttention();

private slots:
	void activated(QSystemTrayIcon::ActivationReason reason);

};
