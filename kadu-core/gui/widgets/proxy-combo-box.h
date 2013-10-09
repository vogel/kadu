/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef PROXY_COMBO_BOX_H
#define PROXY_COMBO_BOX_H

#include "network/proxy/network-proxy.h"
#include "exports.h"

#include "gui/widgets/actions-combo-box.h"

class NetworkProxyModel;

class KADUAPI ProxyComboBox : public ActionsComboBox
{
	Q_OBJECT

	NetworkProxyModel *Model;

	QAction *EditProxyAction;
	QAction *DefaultProxyAction;

private slots:
	void editProxy();

public:
	explicit ProxyComboBox(QWidget *parent = 0);
	virtual ~ProxyComboBox();

	void enableDefaultProxyAction();
	void selectDefaultProxy();
	bool isDefaultProxySelected();

	void setCurrentProxy(const NetworkProxy &networkProxy);
	NetworkProxy currentProxy();

};

#endif // PROXY_COMBO_BOX_H
