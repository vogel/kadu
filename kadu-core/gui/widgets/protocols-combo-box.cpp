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

#include <QtCore/QVariant>
#include <QtGui/QAction>

#include "model/actions-proxy-model.h"
#include "model/roles.h"
#include "protocols/model/protocols-model.h"
#include "protocols/model/protocols-model-proxy.h"
#include "protocols/protocol-factory.h"

#include "protocols-combo-box.h"

ProtocolsComboBox::ProtocolsComboBox(QWidget *parent) :
		QComboBox(parent), CurrentProtocolFactory(0)
{
	Model = new ProtocolsModel(this);
	ProxyModel = new ProtocolsModelProxy(this);
	ProxyModel->setSourceModel(Model);

	ActionsModel = new ActionsProxyModel(this);
	ActionsModel->setSourceModel(ProxyModel);
	ActionsModel->addBeforeAction(new QAction(tr(" - Select network - "), this), ActionsProxyModel::NotVisibleWithOneRowSourceModel);

	setModel(ActionsModel);

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
}

ProtocolsComboBox::~ProtocolsComboBox()
{
}

void ProtocolsComboBox::setCurrentProtocol(ProtocolFactory *protocol)
{
	if (protocol == CurrentProtocolFactory)
		return;

	QModelIndex index = Model->indexForValue(QVariant::fromValue(protocol));
	index = ActionsModel->mapFromSource(index);

	if (index.row() < 0 || index.row() >= count())
		setCurrentIndex(0);
	else
		setCurrentIndex(index.row());

	ProtocolFactory *last = CurrentProtocolFactory;
	CurrentProtocolFactory = protocol;
	emit protocolChanged(CurrentProtocolFactory, last);
}

ProtocolFactory * ProtocolsComboBox::currentProtocol()
{
	CurrentProtocolFactory = qvariant_cast<ProtocolFactory *>(ActionsModel->index(currentIndex(), 0).data(ProtocolRole));
	return CurrentProtocolFactory;
}

void ProtocolsComboBox::currentIndexChangedSlot(int index)
{
	Q_UNUSED(index)

	ProtocolFactory *last = CurrentProtocolFactory;
	currentProtocol(); // sets CurrentProtocol variable
	if (last != CurrentProtocolFactory)
		emit protocolChanged(CurrentProtocolFactory, last);
}

void ProtocolsComboBox::addFilter(AbstractProtocolFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void ProtocolsComboBox::removeFilter(AbstractProtocolFilter *filter)
{
	ProxyModel->removeFilter(filter);
}
