/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "protocols-combo-box.h"

#include "model/model-chain.h"
#include "model/roles.h"
#include "protocols/model/protocols-model-proxy.h"
#include "protocols/model/protocols-model.h"
#include "protocols/protocols-manager.h"

#include <QtWidgets/QAction>

ProtocolsComboBox::ProtocolsComboBox(QWidget *parent) :
		ActionsComboBox(parent)
{
}

ProtocolsComboBox::~ProtocolsComboBox()
{
}

void ProtocolsComboBox::setProtocolsManager(ProtocolsManager *protocolsManager)
{
	m_protocolsManager = protocolsManager;
}

void ProtocolsComboBox::init()
{
	addBeforeAction(make_owned<QAction>(tr(" - Select network - "), this), NotVisibleWithOneRowSourceModel);

	m_proxyModel = make_owned<ProtocolsModelProxy>(this);
	auto chain = make_owned<ModelChain>(this);
	chain->setBaseModel(make_owned<ProtocolsModel>(m_protocolsManager, chain));
	chain->addProxyModel(m_proxyModel);
	setUpModel(ProtocolRole, chain);
}

void ProtocolsComboBox::setCurrentProtocol(ProtocolFactory *protocol)
{
	setCurrentValue(QVariant::fromValue<ProtocolFactory *>(protocol));
}

ProtocolFactory * ProtocolsComboBox::currentProtocol()
{
	return currentValue().value<ProtocolFactory *>();
}

void ProtocolsComboBox::addFilter(AbstractProtocolFilter *filter)
{
	m_proxyModel->addFilter(filter);
}

void ProtocolsComboBox::removeFilter(AbstractProtocolFilter *filter)
{
	m_proxyModel->removeFilter(filter);
}

#include "moc_protocols-combo-box.cpp"
