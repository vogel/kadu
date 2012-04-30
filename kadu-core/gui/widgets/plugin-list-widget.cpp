/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * %kadu copyright end%
 *
 * This file is derived from part of the KDE project
 * Copyright (C) 2007, 2006 Rafael Fernández López <ereslibre@kde.org>
 * Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>
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

#define MARGIN 5

#include <QtGui/QApplication>
#include <QtGui/QBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QStyleOptionViewItemV4>

#include "configuration/configuration-manager.h"
#include "gui/widgets/categorized-list-view.h"
#include "gui/widgets/categorized-list-view-painter.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/plugin-list-view-delegate.h"
#include "gui/widgets/plugin-list-widget-item-delegate.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "model/categorized-sort-filter-proxy-model.h"
#include "plugins/model/plugin-model.h"
#include "plugins/model/plugin-proxy-model.h"
#include "plugins/plugin.h"
#include "plugins/plugin-info.h"
#include "plugins/plugins-manager.h"

#include "plugin-list-widget.h"


PluginListWidget::PluginListWidget(QWidget *parent) :
		QWidget(parent), listView(0), showIcons(false)
{
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setMargin(0);
	setLayout(layout);

	lineEdit = new FilterWidget(this);
	lineEdit->setAutoVisibility(false);
	listView = new CategorizedListView(this);
	listView->setVerticalScrollMode(QListView::ScrollPerPixel);
	listView->setAlternatingRowColors(true);
	categoryDrawer = new CategorizedListViewPainter(listView);
	listView->setCategoryDrawer(categoryDrawer);

	pluginModel = new PluginModel(this, this);
	proxyModel = new ProxyModel(this, this);
	proxyModel->setCategorizedModel(true);
	proxyModel->setSourceModel(pluginModel);
	pluginModel->loadPluginData();
	listView->setModel(proxyModel);
	listView->setAlternatingRowColors(true);

	pluginDelegate = new PluginListWidgetItemDelegate(this, this);
	listView->setItemDelegate(pluginDelegate);

	listView->setMouseTracking(true);
	listView->viewport()->setAttribute(Qt::WA_Hover);

	lineEdit->setView(listView);

	connect(lineEdit, SIGNAL(textChanged(QString)), proxyModel, SLOT(invalidate()));
	connect(pluginDelegate, SIGNAL(changed(bool)), this, SIGNAL(changed(bool)));
	connect(pluginDelegate, SIGNAL(configCommitted(QByteArray)), this, SIGNAL(configCommitted(QByteArray)));

	layout->addWidget(lineEdit);
	layout->addWidget(listView);
}

PluginListWidget::~PluginListWidget()
{
	delete listView->itemDelegate();
	delete listView;
	delete categoryDrawer;
}

int PluginListWidget::dependantLayoutValue(int value, int width, int totalWidth) const
{
	if (listView->layoutDirection() == Qt::LeftToRight)
		return value;

	return totalWidth - width - value;
}

void PluginListWidget::applyChanges()
{
	bool changeOccured = false;

	for (int i = 0; i < pluginModel->rowCount(); i++)
	{
		const QModelIndex index = pluginModel->index(i, 0);
		PluginEntry *pluginEntry = static_cast<PluginEntry*>(index.internalPointer());

		Plugin *plugin = PluginsManager::instance()->plugins().value(pluginEntry->pluginName);

		if (plugin && plugin->isActive() != pluginEntry->checked)
		{
			if (pluginEntry->checked)
				PluginsManager::instance()->activatePlugin(plugin, PluginActivationReasonUserRequest);
			else
				PluginsManager::instance()->deactivatePlugin(plugin, PluginDeactivationReasonUserRequest);

			changeOccured = true;
		}
	}

	pluginModel->loadPluginData();

	if (changeOccured)
		ConfigurationManager::instance()->flush();

	emit changed(false);
}
