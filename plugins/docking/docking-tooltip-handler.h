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

#include "status/status-type.h"

#include <QtCore/QObject>

class DockingConfigurationProvider;
class StatusContainerManager;
class StatusNotifierItem;
class StatusTypeManager;

typedef QPair<QString,QList<StatusType> > StatusPair;
typedef QPair<QStringList,QString> DescriptionPair;

class DockingTooltipHandler final : public QObject
{
	Q_OBJECT

public:
	explicit DockingTooltipHandler(StatusNotifierItem *statusNotifierItem, QObject *parent = nullptr);
	virtual ~DockingTooltipHandler();

	void setDockingConfigurationProvider(DockingConfigurationProvider *dockingConfigurationProvider);
	void setStatusContainerManager(StatusContainerManager *statusContainerManager);
	void setStatusTypeManager(StatusTypeManager *statusTypeManager);

public slots:
	void updateTooltip();

private:
	DockingConfigurationProvider *m_dockingConfigurationProvider;
	StatusContainerManager *m_statusContainerManager;
	StatusTypeManager *m_statusTypeManager;

	StatusNotifierItem *m_statusNotifierItem;

	QString tooltip() const;
	QString plainTooltip() const;
	QString htmlTooltip() const;
	QString htmlStatus() const;
	QString htmlSingleStatus(const StatusPair &status) const;
	QString htmlMultiStatus(const QList<StatusPair> &statuses) const;
	QString htmlDescription() const;
	QString htmlSingleDescription(const DescriptionPair &description) const;
	QString htmlMultiDescription(const QList<DescriptionPair> &descriptions) const;

	QList<StatusPair> getStatuses() const;
	QList<DescriptionPair> getDescriptions() const;
	QString prepareDescription(const QString &description) const;

};
