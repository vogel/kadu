/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "plugin-list-widget.h"

#include "configuration/configuration-manager.h"
#include "gui/widgets/categorized-list-view-painter.h"
#include "gui/widgets/categorized-list-view.h"
#include "gui/widgets/configuration/config-section.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/plugin-list/plugin-list-view-delegate.h"
#include "gui/widgets/plugin-list/plugin-list-widget-item-delegate.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/message-dialog.h"
#include "plugin/model/plugin-model.h"
#include "plugin/model/plugin-proxy-model.h"
#include "plugin/activation/plugin-activation-service.h"
#include "plugin/plugin-conflict-resolver.h"
#include "plugin/plugin-dependency-handler.h"
#include "plugin/state/plugin-state.h"
#include "plugin/state/plugin-state-manager.h"
#include "plugin/state/plugin-state-service.h"

#include <QtGui/QBoxLayout>

PluginListWidget::PluginListWidget(MainConfigurationWindow *window) :
		QWidget{window}, m_listView{0}, m_processingChange{false}
{
	auto layout = new QVBoxLayout;
	layout->setMargin(0);
	setLayout(layout);

	auto filterEdit = new FilterWidget{this};
	filterEdit->setAutoVisibility(false);
	m_listView = new CategorizedListView{this};
	m_listView->setVerticalScrollMode(QListView::ScrollPerPixel);
	m_listView->setAlternatingRowColors(true);
	auto painter = new CategorizedListViewPainter{m_listView};
	m_listView->setCategoryDrawer(painter);

	m_model = new PluginModel{this};
	connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(modelDataChanged(QModelIndex,QModelIndex)));

	m_proxyModel = new PluginProxyModel{this};
	m_proxyModel->setCategorizedModel(true);
	m_proxyModel->setSourceModel(m_model);
	m_listView->setModel(m_proxyModel);
	m_listView->setAlternatingRowColors(true);

	auto delegate = new PluginListWidgetItemDelegate{this, this};
	m_listView->setItemDelegate(delegate);

	m_listView->setMouseTracking(true);
	m_listView->viewport()->setAttribute(Qt::WA_Hover);

	filterEdit->setView(m_listView);

	connect(filterEdit, SIGNAL(textChanged(QString)), m_proxyModel, SLOT(setFilterText(QString)));
	connect(delegate, SIGNAL(changed(bool)), this, SIGNAL(changed(bool)));

	layout->addWidget(filterEdit);
	layout->addWidget(m_listView);

	auto pluginsSection = window->widget()->configSection("Plugins");
	if (pluginsSection)
		pluginsSection->addFullPageWidget("Plugins", this);

	connect(window, SIGNAL(configurationWindowApplied()), this, SLOT(configurationApplied()));
}

PluginListWidget::~PluginListWidget()
{
	delete m_listView->itemDelegate();
	delete m_listView;
}

void PluginListWidget::setPluginActivationService(PluginActivationService *pluginActivationService)
{
	m_pluginActivationService = pluginActivationService;

	if (m_pluginActivationService)
		m_model->setActivePlugins(m_pluginActivationService->activePlugins());
}

void PluginListWidget::setPluginConflictResolver(PluginConflictResolver *pluginConflictResolver)
{
	m_pluginConflictResolver = pluginConflictResolver;
}

void PluginListWidget::setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler)
{
	m_pluginDependencyHandler = pluginDependencyHandler;

	if (!m_pluginDependencyHandler)
		return;

	auto pluginEntries = QVector<PluginMetadata>{};
	for (auto pluginMetadata : m_pluginDependencyHandler)
		pluginEntries.append(pluginMetadata);
	m_model->setPluginEntries(pluginEntries);

	m_proxyModel->sort(0);
}

void PluginListWidget::setPluginStateManager(PluginStateManager *pluginStateManager)
{
	m_pluginStateManager = pluginStateManager;
}

void PluginListWidget::setPluginStateService(PluginStateService *pluginStateService)
{
	m_pluginStateService = pluginStateService;
}

int PluginListWidget::dependantLayoutValue(int value, int width, int totalWidth) const
{
	if (m_listView->layoutDirection() == Qt::LeftToRight)
		return value;

	return totalWidth - width - value;
}

void PluginListWidget::applyChanges()
{
	if (m_pluginActivationService)
	{
		auto deactivatedPlugins = QVector<QString>{};
		for (auto const &pluginName : pluginsWithNewActiveState(false))
			deactivatedPlugins += m_pluginActivationService->deactivatePluginWithDependents(pluginName);

		auto activatedPlugins = QVector<QString>{};
		for (auto const &pluginName : pluginsWithNewActiveState(true))
			activatedPlugins += m_pluginActivationService->activatePluginWithDependencies(pluginName);

		if (m_pluginStateService)
		{
			for (auto const &deactivatedPlugin : deactivatedPlugins)
				m_pluginStateService->setPluginState(deactivatedPlugin, PluginState::Disabled);
			for (auto const &activatedPlugin : activatedPlugins)
				m_pluginStateService->setPluginState(activatedPlugin, PluginState::Enabled);

			if (m_pluginStateManager && (!activatedPlugins.isEmpty() || !deactivatedPlugins.isEmpty()))
			{
				m_pluginStateManager->storePluginStates();
				ConfigurationManager::instance()->flush();
			}
		}
	}

	emit changed(false);
}

QVector<QString> PluginListWidget::pluginsWithNewActiveState(bool newActiveState) const
{
	auto result = QVector<QString>{};

	auto count = m_model->rowCount();
	for (auto i = 0; i < count; i++)
	{
		auto pluginName = m_model->index(i, 0).data(PluginModel::NameRole).toString();
		auto isActive = m_pluginActivationService->isActive(pluginName);
		auto isChecked = m_model->activePlugins().contains(pluginName);
		if ((isActive != isChecked) && (newActiveState == isChecked))
				result.append(pluginName);
	}

	return result;
}

void PluginListWidget::configurationApplied()
{
	applyChanges();
}

void PluginListWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	if (m_processingChange || !m_pluginActivationService || !m_pluginConflictResolver ||  !m_pluginDependencyHandler)
		return;

	// we do not know how to work with multiple rows!
	if (topLeft.row() != bottomRight.row())
		return;

	m_processingChange = true;

	auto pluginName = topLeft.data(PluginModel::NameRole).toString();
	auto checked = topLeft.data(Qt::CheckStateRole).toBool();

	if (checked)
	{
		auto activePlugins = std::set<QString>{};
		auto modelActivePlugins = m_model->activePlugins();
		modelActivePlugins.remove(pluginName);
		std::copy(std::begin(modelActivePlugins), std::end(modelActivePlugins), std::inserter(activePlugins, activePlugins.begin()));

		auto conflictingPlugins = m_pluginConflictResolver->conflictingPlugins(activePlugins, pluginName);

		if (!conflictingPlugins.empty())
		{
			auto list = QStringList{};
			std::copy(std::begin(conflictingPlugins), std::end(conflictingPlugins), std::back_inserter(list));
			auto dialog = MessageDialog::create(KaduIcon(), tr("Kadu"),
					tr("Following dependend plugins will be deactivated because of conflict: %1.").arg(list.join(", ")), this);
			dialog->addButton(QMessageBox::Yes, tr("Deactivate conflicting plugins"));
			dialog->addButton(QMessageBox::No, tr("Cancel"));

			if (dialog->ask())
				setAllChecked(list.toVector(), false);
			else
				setAllChecked(QVector<QString>{pluginName}, false);
		}
		else
			setAllChecked(m_pluginDependencyHandler->withDependencies(pluginName), true);
	}
	else
	{
		auto withDependents = m_pluginDependencyHandler->withDependents(pluginName);
		auto dependents = decltype(withDependents){};
		std::copy_if(std::begin(withDependents), std::end(withDependents), std::back_inserter(dependents),
				[=,&pluginName](QString const &dependentName)
				{
					return dependentName != pluginName && m_model->activePlugins().contains(dependentName);
				}
		);

		if (!dependents.isEmpty())
		{
			auto dialog = MessageDialog::create(KaduIcon(), tr("Kadu"),
					tr("Following dependend plugins will also be deactivated: %1.").arg(
						QStringList{dependents.toList()}.join(", ")), this);
			dialog->addButton(QMessageBox::Yes, tr("Deactivate dependend plugins"));
			dialog->addButton(QMessageBox::No, tr("Cancel"));

			if (dialog->ask())
				setAllChecked(withDependents, false);
			else
				setAllChecked(QVector<QString>{pluginName}, true);
		}
		else
			setAllChecked(withDependents, false);
	}

	m_processingChange = false;
}

template<template<class> class T>
void PluginListWidget::setAllChecked(const T<QString> &plugins, bool checked)
{
	auto count = m_model->rowCount();
	for (auto i = 0; i < count; i++)
	{
		auto index = m_model->index(i, 0);
		auto pluginName = index.data(PluginModel::NameRole).toString();
		if (contains(plugins, pluginName))
			m_model->setData(index, checked, Qt::CheckStateRole);
	}
}

#include "moc_plugin-list-widget.cpp"
