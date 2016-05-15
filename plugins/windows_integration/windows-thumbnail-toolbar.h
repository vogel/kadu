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

#pragma once

#include "misc/memory.h"
#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class StatusActions;
class StatusSetter;
class StatusTypeManager;

class QAction;
class QWidget;
class QWinThumbnailToolBar;

class WindowsThumbnailToolbar : public QObject
{
	Q_OBJECT

public:
	explicit WindowsThumbnailToolbar(not_owned_qptr<StatusActions> statusActions, QWidget *parent = nullptr);
	virtual ~WindowsThumbnailToolbar();

private:
	QPointer<StatusSetter> m_statusSetter;
	QPointer<StatusTypeManager> m_statusTypeManager;

	not_owned_qptr<StatusActions> m_statusActions;
	owned_qptr<QWinThumbnailToolBar> m_toolbar;

	void statusActionsRecreated();
	void changeStatus(QAction *action);

private slots:
	INJEQT_SET void setStatusSetter(StatusSetter *statusSetter);
	INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);

};
