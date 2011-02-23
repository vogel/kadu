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
		KaduComboBox<ProtocolFactory *>(parent)
{
	setUpModel(new ProtocolsModel(this), new ProtocolsModelProxy(this));

	connect(model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
			this, SLOT(updateValueBeforeChange()));
	connect(model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
			this, SLOT(rowsRemoved(const QModelIndex &, int, int)));
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
}

ProtocolsComboBox::~ProtocolsComboBox()
{
}

void ProtocolsComboBox::setCurrentProtocol(ProtocolFactory *protocol)
{
	setCurrentValue(protocol);
}

ProtocolFactory * ProtocolsComboBox::currentProtocol()
{
	return currentValue();
}

void ProtocolsComboBox::currentIndexChangedSlot(int index)
{
	if (KaduComboBox<ProtocolFactory *>::currentIndexChangedSlot(index))
		emit protocolChanged(CurrentValue, ValueBeforeChange);
}

void ProtocolsComboBox::updateValueBeforeChange()
{
	KaduComboBox<ProtocolFactory *>::updateValueBeforeChange();
}

void ProtocolsComboBox::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	KaduComboBox<ProtocolFactory *>::rowsRemoved(parent, start, end);
}

int ProtocolsComboBox::preferredDataRole() const
{
	return ProtocolRole;
}

QString ProtocolsComboBox::selectString() const
{
	return tr(" - Select network - ");
}

ActionsProxyModel::ActionVisibility ProtocolsComboBox::selectVisibility() const
{
	return ActionsProxyModel::NotVisibleWithOneRowSourceModel;
}

void ProtocolsComboBox::addFilter(AbstractProtocolFilter *filter)
{
	static_cast<ProtocolsModelProxy *>(SourceProxyModel)->addFilter(filter);
}

void ProtocolsComboBox::removeFilter(AbstractProtocolFilter *filter)
{
	static_cast<ProtocolsModelProxy *>(SourceProxyModel)->removeFilter(filter);
}
