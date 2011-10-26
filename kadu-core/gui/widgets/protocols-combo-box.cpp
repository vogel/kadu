/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "model/actions-proxy-model.h"
#include "model/roles.h"
#include "protocols/model/protocols-model.h"
#include "protocols/model/protocols-model-proxy.h"

#include "protocols-combo-box.h"

ProtocolsComboBox::ProtocolsComboBox(QWidget *parent) :
		ActionsComboBox(parent)
{
	setDataRole(ProtocolRole);

	ProxyModel = new ProtocolsModelProxy(this);
	setUpModel(new ProtocolsModel(this), ProxyModel);
	addBeforeAction(new QAction(tr(" - Select network - "), this), ActionsProxyModel::NotVisibleWithOneRowSourceModel);
}

ProtocolsComboBox::~ProtocolsComboBox()
{
}

void ProtocolsComboBox::setCurrentProtocol(ProtocolFactory *protocol)
{
	setCurrentValue(QVariant::fromValue<ProtocolFactory *>(protocol));
}

ProtocolFactory * ProtocolsComboBox::currentProtocol()
{
	return currentValue().value<ProtocolFactory *>();
}

void ProtocolsComboBox::valueChanged(QVariant value, QVariant previousValue)
{
	emit protocolChanged(value.value<ProtocolFactory *>(),
	                     previousValue.value<ProtocolFactory *>());
}

bool ProtocolsComboBox::compare(QVariant value, QVariant previousValue) const
{
	return value.value<ProtocolFactory *>() == previousValue.value<ProtocolFactory *>();
}

void ProtocolsComboBox::addFilter(AbstractProtocolFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void ProtocolsComboBox::removeFilter(AbstractProtocolFilter *filter)
{
	ProxyModel->removeFilter(filter);
}
