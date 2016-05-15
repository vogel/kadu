/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "windows-thumbnail-toolbar.h"

#include "status/status-actions.h"
#include "status/status-container.h"
#include "status/status-setter.h"
#include "status/status-type-manager.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QWidget>
#include <QtWinExtras/QtWinExtras>

WindowsThumbnailToolbar::WindowsThumbnailToolbar(not_owned_qptr<StatusActions> statusActions, QWidget *parent) :
		QObject{parent},
		m_statusActions{std::move(statusActions)}
{
	parent->window()->winId(); // force windowHandle() to be valid

	m_toolbar = make_owned<QWinThumbnailToolBar>(parent->window());
	m_toolbar->setWindow(parent->window()->windowHandle());

	connect(m_statusActions, &StatusActions::statusActionsRecreated, this, &WindowsThumbnailToolbar::statusActionsRecreated);
	connect(m_statusActions, &StatusActions::statusActionTriggered, this, &WindowsThumbnailToolbar::changeStatus);
	statusActionsRecreated();
}

WindowsThumbnailToolbar::~WindowsThumbnailToolbar()
{
}

void WindowsThumbnailToolbar::setStatusSetter(StatusSetter *statusSetter)
{
	m_statusSetter = statusSetter;
}

void WindowsThumbnailToolbar::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
	m_statusTypeManager = statusTypeManager;
}

void WindowsThumbnailToolbar::statusActionsRecreated()
{
	m_toolbar->clear();

	for (auto action : m_statusActions->actions())
	{
		auto button = make_owned<QWinThumbnailToolButton>(m_toolbar.get());
		button->setToolTip(action->text());
		button->setIcon(action->icon());
		button->setDismissOnClick(true);
		connect(button.get(), &QWinThumbnailToolButton::clicked, action, &QAction::trigger);
		m_toolbar->addButton(button.get());
	}
}

void WindowsThumbnailToolbar::changeStatus(QAction *action)
{
	auto statusType = action->data().value<StatusType>();

	for (auto &&container : m_statusActions->statusContainer()->subStatusContainers())
	{
		auto status = Status{m_statusSetter->manuallySetStatus(container)};
		status.setType(m_statusTypeManager, statusType);

		m_statusSetter->setStatusManually(container, status);
		container->storeStatus(status);
	}
}

#include "windows-thumbnail-toolbar.moc"
