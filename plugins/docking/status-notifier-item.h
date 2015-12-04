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

#include "misc/memory.h"

#include <QtCore/QObject>
#include <QtCore/QPoint>

enum class StatusNotifierItemAttentionMode;
class KaduIcon;
class StatusNotifierItemAttentionBlinker;

class QMenu;
class QMovie;
class KStatusNotifierItem;

class DOCKINGAPI StatusNotifierItem final : public QObject
{
	Q_OBJECT

public:
	explicit StatusNotifierItem(QObject *parent = nullptr);
	virtual ~StatusNotifierItem();

	void setAttentionMode(StatusNotifierItemAttentionMode attentionMode);
	void setNeedAttention(bool needAttention);

	void setAttentionIcon(const QString &attentionIconPath);
	void setAttentionMovie(const QString &attentionMoviePath);
	void setIcon(const QString &iconPath);
	void setTooltip(const QString &tooltip);

	QPoint trayPosition();

	QMenu * contextMenu();

signals:
	void activateRequested();
	void messageClicked();

private:
	StatusNotifierItemAttentionMode m_attentionMode;
	bool m_needAttention;
	QString m_attentionIconPath;
	QString m_attentionMoviePath;
	QString m_iconPath;

	owned_qptr<KStatusNotifierItem> m_statusNotifierItem;
	not_owned_qptr<StatusNotifierItemAttentionBlinker> m_blinker;

	void updateAttention();
	bool shouldBlink();
	void startBlinking();
	void stopBlinking();

private slots:
	//void trayActivated(QSystemTrayIcon::ActivationReason reason);

};
