/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exec-configuration-widget.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/windows/main-configuration-window.h"

#include <QtCore/QCoreApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>

ExecConfigurationWidget::ExecConfigurationWidget(QWidget *parent) :
		NotifierConfigurationWidget(parent)
{
	m_commandLineEdit = new QLineEdit(this);
	m_commandLineEdit->setToolTip(QCoreApplication::translate("@default", MainConfigurationWindow::SyntaxTextNotify));

	auto layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(m_commandLineEdit);

	static_cast<NotifyGroupBox *>(parent)->addWidget(this);
}

ExecConfigurationWidget::~ExecConfigurationWidget()
{
}

void ExecConfigurationWidget::saveNotifyConfigurations()
{
	if (!m_currentNotificationEvent.isEmpty())
		m_commands[m_currentNotificationEvent] = m_commandLineEdit->text();

	for (QMap<QString, QString>::const_iterator it = m_commands.constBegin(), end = m_commands.constEnd(); it != end; ++it)
		Application::instance()->configuration()->deprecatedApi()->writeEntry("Exec Notify", it.key() + "Cmd", it.value());
}

void ExecConfigurationWidget::switchToEvent(const QString &event)
{
	if (!m_currentNotificationEvent.isEmpty())
		m_commands[m_currentNotificationEvent] = m_commandLineEdit->text();
	m_currentNotificationEvent = event;

	if (m_commands.contains(event))
		m_commandLineEdit->setText(m_commands[event]);
	else
		m_commandLineEdit->setText(Application::instance()->configuration()->deprecatedApi()->readEntry("Exec Notify", event + "Cmd"));
}

#include "moc_exec-configuration-widget.cpp"
