/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/windows/proxy-edit-window.h"
#include "model/roles.h"
#include "network/proxy/model/network-proxy-model.h"
#include "network/proxy/model/network-proxy-proxy-model.h"

#include "proxy-combo-box.h"

#define DEFAULT_PROXY_INDEX 1

ProxyComboBox::ProxyComboBox(QWidget *parent) :
		KaduComboBox<NetworkProxy>(parent), InActivatedSlot(false), DefaultProxyAction(0)
{
	Model = new NetworkProxyModel(this);
	setUpModel(Model, new NetworkProxyProxyModel(this));

	EditProxyAction = new QAction(tr("Edit proxy configuration..."), this);
	QFont editProxyActionFont = EditProxyAction->font();
	editProxyActionFont.setItalic(true);
	EditProxyAction->setFont(editProxyActionFont);
	EditProxyAction->setData("editProxyConfiguration");
	ActionsModel->addAfterAction(EditProxyAction);

	connect(model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
			this, SLOT(updateValueBeforeChange()));
	connect(model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
			this, SLOT(rowsRemoved(const QModelIndex &, int, int)));
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
}

ProxyComboBox::~ProxyComboBox()
{
}

void ProxyComboBox::enableDefaultProxyAction()
{
	DefaultProxyAction = new QAction(tr(" - Use Default Proxy - "), this);
	DefaultProxyAction->setData("defaultProxy");
	DefaultProxyAction->setFont(QFont());
	ActionsModel->addBeforeAction(DefaultProxyAction);
}

void ProxyComboBox::selectDefaultProxy()
{
	if (DefaultProxyAction)
		setCurrentIndex(DEFAULT_PROXY_INDEX);
}

bool ProxyComboBox::isDefaultProxySelected()
{
	if (DefaultProxyAction)
		return DEFAULT_PROXY_INDEX == currentIndex();
	else
		return false;
}

void ProxyComboBox::setCurrentProxy(const NetworkProxy &networkProxy)
{
	setCurrentValue(networkProxy);
}

NetworkProxy ProxyComboBox::currentProxy()
{
	return currentValue();
}

void ProxyComboBox::currentIndexChangedSlot(int index)
{
	QModelIndex modelIndex = this->model()->index(index, modelColumn(), rootModelIndex());
	QAction *action = modelIndex.data(ActionRole).value<QAction *>();

	if (action == EditProxyAction)
	{
		ProxyEditWindow::show();
		setCurrentValue(CurrentValue);
		return;
	}

	if (KaduComboBox<NetworkProxy>::currentIndexChangedSlot(index))
		emit proxyChanged(CurrentValue, ValueBeforeChange);
}

void ProxyComboBox::updateValueBeforeChange()
{
	KaduComboBox<NetworkProxy>::updateValueBeforeChange();
}

void ProxyComboBox::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	KaduComboBox<NetworkProxy>::rowsRemoved(parent, start, end);
}

int ProxyComboBox::preferredDataRole() const
{
	return NetworkProxyRole;
}

QString ProxyComboBox::selectString() const
{
	return tr(" - No proxy - ");
}

ActionsProxyModel::ActionVisibility ProxyComboBox::selectVisibility() const
{
	return ActionsProxyModel::AlwaysVisible;
}
