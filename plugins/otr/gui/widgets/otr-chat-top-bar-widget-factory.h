/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_CHAT_TOP_BAR_WIDGET_FACTORY_H
#define OTR_CHAT_TOP_BAR_WIDGET_FACTORY_H

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

#include "gui/widgets/chat-top-bar-widget-factory.h"

class OtrAppOpsWrapper;
class OtrChatTopBarWidget;
class OtrPeerIdentityVerifier;

class OtrChatTopBarWidgetFactory : public QObject, public ChatTopBarWidgetFactory
{
	Q_OBJECT

	QWeakPointer<OtrAppOpsWrapper> AppOpsWrapper;
	QWeakPointer<OtrPeerIdentityVerifier> PeerIdentityVerifier;
	QList<OtrChatTopBarWidget *> Widgets;

private slots:
	void widgetDestroyed(QObject *widget);

public:
	virtual ~OtrChatTopBarWidgetFactory() {}

	void setOtrAppOpsWrapper(OtrAppOpsWrapper *otrAppOpsWrapper);
	void setPeerIdentityVerifier(OtrPeerIdentityVerifier *otrPeerIdentityVerifier);

	virtual QWidget * createWidget(const Chat &chat, QWidget *parent);

};

#endif // OTR_CHAT_TOP_BAR_WIDGET_FACTORY_H
