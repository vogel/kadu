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

#include "docking-tooltip-handler.h"

#include "docking-configuration.h"
#include "docking-configuration-provider.h"
#include "status-notifier-item.h"

#include "status/status-container-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"

#include <QtCore/QSet>
#include <QtGui/QPalette>
#include <QtWidgets/QApplication>

DockingTooltipHandler::DockingTooltipHandler(StatusNotifierItem *statusNotifierItem, QObject *parent) :
		QObject{parent},
		m_statusContainerManager{nullptr},
		m_statusTypeManager{nullptr},
		m_statusNotifierItem{statusNotifierItem}
{
}

DockingTooltipHandler::~DockingTooltipHandler()
{
}

void DockingTooltipHandler::setDockingConfigurationProvider(DockingConfigurationProvider *dockingConfigurationProvider)
{
	m_dockingConfigurationProvider = dockingConfigurationProvider;
	connect(m_dockingConfigurationProvider, SIGNAL(updated()), this, SLOT(updateTooltip()));
	updateTooltip();
}

void DockingTooltipHandler::setStatusContainerManager(StatusContainerManager *statusContainerManager)
{
	m_statusContainerManager = statusContainerManager;
}

void DockingTooltipHandler::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
	m_statusTypeManager = statusTypeManager;
}

QString DockingTooltipHandler::tooltip() const
{
	if (!m_dockingConfigurationProvider->configuration().ShowTooltipInTray)
		return {};

#ifdef Q_OS_WIN
	return plainTooltip();
#else
	return htmlTooltip();
#endif
}

QString DockingTooltipHandler::plainTooltip() const
{
	auto status = m_statusContainerManager->status();
	auto tiptext = QString{"%1: %2"}.arg(tr("Status"), status.displayName());
	if (!status.description().isEmpty())
		tiptext += QString{"\n\n%1:\n%2"}.arg(tr("Description"), status.description());

	return tiptext;
}

QString DockingTooltipHandler::htmlTooltip() const
{
	return QString{
		"<table>"
		"	<tr><td align='center' style='white-space:nowrap; font-weight:bold;'><big>Kadu</big></td></tr>"
		"%1"
		"%2"
		"</table>"
	}.arg(htmlStatus()).arg(htmlDescription());
}

QString DockingTooltipHandler::htmlStatus() const
{
	auto statuses = getStatuses();
	if (statuses.count() == 1)
		return htmlSingleStatus(statuses.first());
	else if (statuses.count() > 1)
		return htmlMultiStatus(statuses);
	else
		return {};
}

QString DockingTooltipHandler::htmlSingleStatus(const StatusPair &status) const
{
	auto displays = QStringList{};
	for (auto type : status.second)
		displays.append(m_statusTypeManager->statusTypeData(type).displayName());

	return QString{
		"<tr><td style='white-space:nowrap; font-weight:bold;'>%1:</td></tr>"
		"<tr><td style='white-space:nowrap; padding:0 1.5em;'>%2</td></tr>"
	}.arg(tr("Status")).arg(displays.join(", "));
}

QString DockingTooltipHandler::htmlMultiStatus(const QList<StatusPair> &statuses) const
{
	auto table = QString{};
	for (auto status : statuses)
	{
		auto id = status.first;
		auto list = status.second;
		auto displays = QStringList{};
		for (auto type : list)
			displays.append(m_statusTypeManager->statusTypeData(type).displayName());
		table += QString{
			"<tr><td align='right' style='white-space:nowrap;'>%1:</td><td style='white-space:nowrap; padding-left:0.4em; font-style:italic;'>%2</td></tr>"
		}.arg(id, displays.join(", "));
	}

	return QString{
		"<tr><td style='white-space:nowrap; font-weight:bold;'>%1:</td></tr>"
		"<tr><td style='padding:0 1.5em;'>"
		"<table>%2</table>"
		"</td></tr>"
	}.arg(tr("Statuses")).arg(table);
}

QString DockingTooltipHandler::htmlDescription() const
{
	auto descriptions = getDescriptions();
	if (descriptions.count() == 1)
		return htmlSingleDescription(descriptions.first());
	else if (descriptions.count() > 1)
		return htmlMultiDescription(descriptions);
	else
		return {};
}

QString DockingTooltipHandler::htmlSingleDescription(const DescriptionPair &description) const
{
	return QString{
		"<tr><td style='white-space:nowrap; font-weight:bold;'>%1:</td></tr>"
		"<tr><td style='padding:0 1.5em;'>%2</td></tr>"
	}.arg(tr("Description").arg(prepareDescription(description.second)));
}

QString DockingTooltipHandler::htmlMultiDescription(const QList<DescriptionPair> &descriptions) const
{
 	auto table = QString{};
	for (auto description : descriptions)
	{
		auto ids = description.first;
		auto prepared = prepareDescription(description.second);
		auto k = 0;
		for (const auto &id : ids) // Qt has problems with calculating width of table cell containing <br>s
		{
			auto separator = QString{k < ids.count() - 1 ? "," : ":"};
			if (k == 0)
				table += QString{"<tr><td align='right' style='white-space:nowrap;'>%1%2</td><td rowspan='%3' style='padding-left:0.4em; font-style:italic;'>%4</td></tr>"}
					.arg(id, separator, QString::number(ids.count()), prepared);
			else
				table += QString{"<tr><td align='right' style='white-space:nowrap;'>%1%2</td></tr>"}
					.arg(id, separator);
			k++;
		}
	}

	return QString{
		"<tr><td style='white-space:nowrap; font-weight:bold;'>%1:</td></tr>"
		"<table>"
		"<tr><td style='padding:0 1.5em;'>%2</td></tr>"
		"</table>"
	}.arg(tr("Descriptions").arg(table));
}

void DockingTooltipHandler::updateTooltip()
{
	m_statusNotifierItem->setTooltip(tooltip());
}

QList<StatusPair> DockingTooltipHandler::getStatuses() const
{
	auto statusesList = QList<StatusPair>{};
	auto alltypes = QSet<StatusType>{};

	for (auto container : m_statusContainerManager->subStatusContainers())
	{
		auto statuses = QList<StatusType>{};
		for (auto innerContainer : container->subStatusContainers())
		{
			auto type = innerContainer->status().type();
			if (!statuses.contains(type))
				statuses.append(type);
			alltypes.insert(type);
		}
		statusesList.append(qMakePair(container->statusContainerName(), statuses));
	}

	if (alltypes.count() == 1)
	{
		statusesList.clear();
		statusesList.append(qMakePair(QString{}, QList<StatusType>() << *alltypes.begin()));
	}

	return statusesList;
}

QList<DescriptionPair> DockingTooltipHandler::getDescriptions() const
{
	auto descriptionsList = QList<DescriptionPair>{};
	auto descriptionsContainers = QHash<QString,QStringList>{};
	auto orderedDescriptions = QList<QString>{};

	for (auto container : m_statusContainerManager->subStatusContainers())
	{
		auto description = container->status().description();
		descriptionsContainers[description].append(container->statusContainerName());
		if (!orderedDescriptions.contains(description))
			orderedDescriptions.append(description);
	}

	for (auto description : orderedDescriptions)
		descriptionsList.append(qMakePair(descriptionsContainers[description], description));

	return descriptionsList;
}

QString DockingTooltipHandler::prepareDescription(const QString &description) const
{
	auto color = qApp->palette().windowText().color();
	color.setAlpha(128);
	auto colorString = QString{"rgba(%1,%2,%3,%4)"}.arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());

	auto html = Qt::escape(description);
	html.replace('\n', QString(QLatin1String("<span style='color:%1;'> ") + QChar(0x21B5) + QLatin1String("</span><br />")).arg(colorString));

	return html;
}

#include "moc_docking-tooltip-handler.cpp"
