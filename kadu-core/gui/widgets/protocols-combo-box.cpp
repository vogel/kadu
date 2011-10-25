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
		KaduComboBox(parent)
{
	setUpModel(new ProtocolsModel(this), new ProtocolsModelProxy(this));

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
}

ProtocolsComboBox::~ProtocolsComboBox()
{
}

void ProtocolsComboBox::setCurrentProtocol(ProtocolFactory *protocol)
{
	setCurrentValue(qVariantFromValue((void *)protocol));
}

ProtocolFactory * ProtocolsComboBox::currentProtocol()
{
	return (ProtocolFactory *)(currentValue().value<void *>());
}

void ProtocolsComboBox::currentIndexChangedSlot(int index)
{
	if (KaduComboBox::currentIndexChangedSlot(index))
		emit protocolChanged((ProtocolFactory *)(CurrentValue.value<void *>()),
		                     (ProtocolFactory *)(ValueBeforeChange.value<void *>()));
}

bool ProtocolsComboBox::compare(QVariant value, QVariant previousValue) const
{
	return (ProtocolFactory *)(value.value<void *>()) ==
	        (ProtocolFactory *)(previousValue.value<void *>());
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
