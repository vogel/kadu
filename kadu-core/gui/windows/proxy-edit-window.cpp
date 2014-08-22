/*
 * %kadu copyright begin%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QPointer>
#include <QtCore/QUrl>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "configuration/config-file-variant-wrapper.h"
#include "model/action-filter-proxy-model.h"
#include "model/action-list-model.h"
#include "model/merged-proxy-model-factory.h"
#include "model/roles.h"
#include "network/proxy/model/network-proxy-model.h"
#include "network/proxy/model/network-proxy-proxy-model.h"
#include "network/proxy/network-proxy-manager.h"
#include "os/generic/window-geometry-manager.h"
#include "activate.h"

#include "proxy-edit-window.h"

ProxyEditWindow * ProxyEditWindow::Instance = 0;

void ProxyEditWindow::show()
{
	if (!Instance)
		Instance = new ProxyEditWindow();

	Instance->setVisible(true);
	_activateWindow(Instance);
}

ProxyEditWindow::ProxyEditWindow(QWidget *parent) :
		QWidget(parent), ForceProxyChange(false)
{
	setWindowRole("kadu-proxy-configuration");
	setAttribute(Qt::WA_DeleteOnClose);

	setWindowTitle(tr("Proxy Configuration"));

	createGui();
	ProxyView->selectionModel()->select(ProxyView->model()->index(0, 0), QItemSelectionModel::ClearAndSelect);

	new WindowGeometryManager(new ConfigFileVariantWrapper("General", "ProxyEditWindowGeometry"), QRect(200, 200, 750, 500), this);
}

ProxyEditWindow::~ProxyEditWindow()
{
	Instance = 0;
}

void ProxyEditWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QHBoxLayout *contentLayout = new QHBoxLayout();
	mainLayout->addItem(contentLayout);

	ProxyView = new QListView(this);
	ProxyView->setMinimumWidth(150);
	contentLayout->addWidget(ProxyView);

	ProxyModel = new NetworkProxyModel(ProxyView);
	ProxyProxyModel = new NetworkProxyProxyModel(ProxyView);
	ProxyProxyModel->setSourceModel(ProxyModel);

	QAction *separator = new QAction(this);
	separator->setSeparator(true);

	AddProxyAction = new QAction(tr("Add new proxy"), this);

	ActionListModel *actionsModel = new ActionListModel(this);
	actionsModel->appendAction(separator);
	actionsModel->appendAction(AddProxyAction);

	QList<QAbstractItemModel *> models;
	models.append(ProxyProxyModel);
	models.append(actionsModel);

	ActionFilterProxyModel *actionProxyModel = new ActionFilterProxyModel(this);
	actionProxyModel->setSourceModel(MergedProxyModelFactory::createInstance(models, this));
	actionProxyModel->setModel(ProxyProxyModel);
	actionProxyModel->addHideWhenModelEmpty(separator);

	ProxyView->setModel(actionProxyModel);
	ProxyView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	ProxyView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ProxyView->setIconSize(QSize(32, 32));
	connect(ProxyView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(proxySelectionChanged(const QItemSelection &, const QItemSelection &)));

	QWidget *editPanel = new QWidget(this);
	contentLayout->addWidget(editPanel);

	QFormLayout *editLayout = new QFormLayout(editPanel);

	Type = new QComboBox(editPanel);
	Type->addItem(tr("HTTP CONNECT method"), "http");
	Type->addItem(tr("SOCKS Version 5"), "socks");
	Type->addItem(tr("HTTP Polling"), "poll");
	connect(Type, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
	editLayout->addRow(tr("Type (for Jabber)"), Type);

	PollingUrl = new QLineEdit(editPanel);
	connect(PollingUrl, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	editLayout->addRow(tr("Polling URL"), PollingUrl);

	Host = new QLineEdit(editPanel);
	connect(Host, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	editLayout->addRow(tr("Host"), Host);

	Port = new QLineEdit(editPanel);
	connect(Port, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	Port->setValidator(new QIntValidator(1, 65535, Port));
	editLayout->addRow(tr("Port"), Port);

	User = new QLineEdit(editPanel);
	connect(User, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	editLayout->addRow(tr("User"), User);

	Password = new QLineEdit(editPanel);
	connect(Password, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	Password->setEchoMode(QLineEdit::Password);
	editLayout->addRow(tr("Password"), Password);

	QDialogButtonBox *editButtons = new QDialogButtonBox(Qt::Horizontal, editPanel);
	editLayout->addRow(editButtons);

	SaveButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Add"), editButtons);
	connect(SaveButton, SIGNAL(clicked()), this, SLOT(saveProxyButtonClicked()));
	CancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), editButtons);
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
	RemoveButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Remove"), editButtons);
	connect(RemoveButton, SIGNAL(clicked(bool)), this, SLOT(removeButtonClicked()));

	editButtons->addButton(SaveButton, QDialogButtonBox::ApplyRole);
	editButtons->addButton(CancelButton, QDialogButtonBox::RejectRole);
	editButtons->addButton(RemoveButton, QDialogButtonBox::DestructiveRole);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	QPushButton *closeButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Close"), buttons);

	connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));
	buttons->addButton(closeButton, QDialogButtonBox::RejectRole);
}

void ProxyEditWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}

void ProxyEditWindow::updateProxyView()
{
	QModelIndexList selection = ProxyView->selectionModel()->selectedIndexes();
	if (1 != selection.size())
		return;

	Host->clear();
	Port->clear();
	User->clear();
	Password->clear();

	NetworkProxy proxy = selection.at(0).data(NetworkProxyRole).value<NetworkProxy>();
	if (proxy)
	{
	  	int type = Type->findData(proxy.type());
		Type->setCurrentIndex(type == -1 ? 0 : type);
		Host->setText(proxy.address());
		Port->setText(QString::number(proxy.port()));
		User->setText(proxy.user());
		Password->setText(proxy.password());
		PollingUrl->setText(proxy.pollingUrl());

		SaveButton->setText(tr("Save"));
		RemoveButton->show();
	}
	else
	{
		SaveButton->setText(tr("Add"));
		RemoveButton->hide();
	}

	dataChanged();
}

void ProxyEditWindow::saveProxyButtonClicked()
{
	QModelIndexList selection = ProxyView->selectionModel()->selectedIndexes();
	if (1 != selection.size())
		return;

	saveProxy(selection.at(0).data(NetworkProxyRole).value<NetworkProxy>());
}

void ProxyEditWindow::cancelButtonClicked()
{
	updateProxyView();
}

void ProxyEditWindow::removeButtonClicked()
{
	QModelIndexList selection = ProxyView->selectionModel()->selectedIndexes();
	if (1 != selection.size())
		return;

	NetworkProxy proxy = selection.at(0).data(NetworkProxyRole).value<NetworkProxy>();

	QPointer<QMessageBox> messageBox = new QMessageBox(this);
	messageBox.data()->setWindowTitle(tr("Confirm proxy removal"));
	messageBox.data()->setText(tr("Are you sure do you want to remove this proxy?"));

	QPushButton *removeButton = messageBox.data()->addButton(tr("Remove proxy"), QMessageBox::AcceptRole);
	messageBox.data()->addButton(QMessageBox::Cancel);
	messageBox.data()->setDefaultButton(QMessageBox::Cancel);
	messageBox.data()->exec();

	if (messageBox.isNull())
		return;

	if (messageBox.data()->clickedButton() == removeButton)
		NetworkProxyManager::instance()->removeItem(proxy);

	delete messageBox.data();
}

void ProxyEditWindow::saveProxy(NetworkProxy proxy)
{
	if (proxy)
	{
		proxy.setType(Type->itemData(Type->currentIndex()).toString());
		proxy.setAddress(Host->text());
		proxy.setPort(Port->text().toInt());
		proxy.setUser(User->text());
		proxy.setPassword(Password->text());
		proxy.setPollingUrl(PollingUrl->text());
	}
	else
		proxy = NetworkProxyManager::instance()->byConfiguration(Host->text(), Port->text().toInt(), User->text(), Password->text(),
		                                                 ActionCreateAndAdd);

	ForceProxyChange = true;
	selectProxy(proxy);
	ForceProxyChange = false;
}

ConfigurationValueState ProxyEditWindow::state(NetworkProxy proxy)
{
	bool valid = !Host->text().isEmpty()
			&& !Port->text().isEmpty();
	bool changed = proxy.type() != Type->itemData(Type->currentIndex()).toString()
			|| proxy.address() != Host->text()
			|| ((QString::number(proxy.port()) != Port->text()) && (proxy.port() != 0 || !Port->text().isEmpty()))
			|| proxy.user() != User->text()
			|| proxy.password() !=  Password->text()
			|| proxy.pollingUrl() !=  PollingUrl->text();

	if (!changed)
		return StateNotChanged;

	return valid
			? StateChangedDataValid
			: StateChangedDataInvalid;
}

ConfigurationValueState ProxyEditWindow::state()
{
	QModelIndexList selection = ProxyView->selectionModel()->selectedIndexes();
	if (selection.size() != 1)
		return StateChangedDataInvalid;

	NetworkProxy proxy = selection.at(0).data(NetworkProxyRole).value<NetworkProxy>();
	return state(proxy);
}

bool ProxyEditWindow::canChangeProxy(const QItemSelection &selection)
{
	if (selection.size() == 0)
		return true;

	if (selection.size() != 1)
		return false;

	QModelIndex selectedItem = selection.at(0).topLeft();
	NetworkProxy proxy = selectedItem.data(NetworkProxyRole).value<NetworkProxy>();

	if (ForceProxyChange)
		return true;

	ConfigurationValueState currenState = state(proxy);
	if (StateNotChanged == currenState)
		return true;

	if (StateChangedDataValid == currenState)
	{
		QMessageBox::StandardButton result = QMessageBox::question(this, tr("Proxy"),
				tr("You have unsaved changes in current proxy.<br />Do you want to save them?"),
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

		switch (result)
		{
			case QMessageBox::Yes:
				saveProxy(proxy);
				return true;

			case QMessageBox::No:
				return true;

			default:
				return false;
		}
	}

	if (StateChangedDataInvalid == currenState)
	{
		QMessageBox::StandardButton result = QMessageBox::question(this, tr("Proxy"),
				tr("You have unsaved changes in current proxy.<br />This data is invalid, so you will loose all changes.<br />Do you want to go back to edit them?"),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		switch (result)
		{
			case QMessageBox::No:
				return true;

			default:
				return false;
		}
	}

	return true;
}

void ProxyEditWindow::selectProxy(NetworkProxy proxy)
{
	ProxyView->selectionModel()->clearSelection();

	const QModelIndexList &proxyIndexes = ProxyModel->indexListForValue(proxy);
	Q_ASSERT(proxyIndexes.size() == 1);

	const QModelIndex &index = ProxyProxyModel->mapFromSource(proxyIndexes.first());
	ProxyView->selectionModel()->select(index, QItemSelectionModel::Select);

	SaveButton->setText(tr("Save"));
}

void ProxyEditWindow::dataChanged()
{
	PollingUrl->setEnabled(Type->itemData(Type->currentIndex()).toString() == QLatin1String("poll"));

	ConfigurationValueState changeState = state();
	SaveButton->setEnabled(StateChangedDataValid == changeState);
	CancelButton->setEnabled(StateNotChanged != changeState);
}

void ProxyEditWindow::proxySelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	Q_UNUSED(selected)

	if (ForceProxyChange)
		return;

	if (canChangeProxy(deselected))
	{
		// in case canChangeProxy changed this
		ForceProxyChange = true;
		ProxyView->selectionModel()->select(selected, QItemSelectionModel::ClearAndSelect);
		ForceProxyChange = false;

		updateProxyView();
		return;
	}

	ForceProxyChange = true;
	ProxyView->selectionModel()->select(deselected, QItemSelectionModel::ClearAndSelect);
	ForceProxyChange = false;
}

#include "moc_proxy-edit-window.cpp"
