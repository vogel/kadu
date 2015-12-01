/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QMap>
#include <QtWidgets/QLabel>

#include "configuration/configuration-aware-object.h"
#include "status/status-type.h"

#include "docking-exports.h"

class QAction;
class QMenu;

class Docker;
class KaduIcon;
class StatusContainer;
class StatusIcon;
class StatusMenu;

typedef QPair<QString,QList<StatusType> > StatusPair;
typedef QPair<QStringList,QString> DescriptionPair;

class DockingMenuActionRepository;
class DockingMenuHandler;
class StatusNotifierItem;

class DOCKINGAPI Docking : public QObject, ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Docking)

	static Docking *Instance;

	StatusNotifierItem *m_statusNotifierItem;
	DockingMenuHandler *m_dockingMenuHandler;
	DockingMenuActionRepository *m_dockingMenuActionRepository;
	StatusIcon *Icon;

	enum IconType {BlinkingEnvelope = 0, StaticEnvelope = 1, AnimatedEnvelope = 2} newMessageIcon;
	QTimer *icon_timer;
	bool blink;

	QList<StatusPair> getStatuses() const;
	QList<DescriptionPair> getDescriptions() const;
	QString prepareDescription(const QString &description) const;
	void defaultToolTip();

	void createDefaultConfiguration();

	Docking();
	void init();

	virtual ~Docking();

	void doUpdateContextMenu(QMenu *menu);
	void openUnreadMessages();
	
	void showKaduWindow();
	void hideKaduWindow();

private slots:
	void statusIconChanged(const KaduIcon &icon);
	void changeIcon();
	void needAttentionChanged(bool needAttention);
	void searchingForTrayPosition(QPoint &point);

protected:
	virtual void configurationUpdated();

public:
	static void createInstance();
	static void destroyInstance();
	static Docking * instance();
	
	DockingMenuActionRepository * dockingMenuActionRepository() const;

	void trayMousePressEvent(QMouseEvent * e);
	KaduIcon defaultIcon();

signals:
	void messageClicked();
	void mousePressMidButton();
	void mousePressLeftButton();
	void mousePressRightButton();

};
