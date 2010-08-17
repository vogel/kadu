/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PROTOCOLS_COMBO_BOX_H
#define PROTOCOLS_COMBO_BOX_H

#include <QtGui/QComboBox>

class AbstractProtocolFilter;
class ActionsProxyModel;
class ProtocolFactory;
class ProtocolsModel;
class ProtocolsModelProxy;

class ProtocolsComboBox : public QComboBox
{
	Q_OBJECT

	ProtocolsModel *Model;
	ProtocolsModelProxy *ProxyModel;
	ActionsProxyModel *ActionsModel;

	ProtocolFactory *CurrentProtocolFactory;

private slots:
	void currentIndexChangedSlot(int index);

public:
	explicit ProtocolsComboBox(QWidget *parent = 0);
	virtual ~ProtocolsComboBox();

	void setCurrentProtocol(ProtocolFactory *protocol);
	ProtocolFactory * currentProtocol();
	
	void addFilter(AbstractProtocolFilter *filter);
	void removeFilter(AbstractProtocolFilter *filter);

signals:
	void protocolChanged(ProtocolFactory *protocol, ProtocolFactory *lastProtocol);

};

#endif // PROTOCOLS_COMBO_BOX_H
