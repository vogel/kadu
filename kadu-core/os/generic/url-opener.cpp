/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QProcess>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QApplication>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/windows/message-dialog.h"

#include "url-opener.h"

bool UrlOpener::openUrl(const QByteArray &urlForDesktopServices, const QByteArray &urlForApplication, const QString &application)
{
	if (!application.isEmpty())
	{
		QProcess *process = new QProcess(qApp);

		QString launchLine = application;
		if (!launchLine.contains("%1"))
			launchLine.append(" \"" + QString::fromUtf8(urlForApplication) + '"');
		else
			launchLine.replace("%1", QString::fromUtf8(urlForApplication));

		process->start(launchLine);
		if (process->waitForStarted())
			return true;
	}

	return QDesktopServices::openUrl(QUrl::fromEncoded(urlForDesktopServices));
}

void UrlOpener::openUrl(const QByteArray &url)
{
	QString browser;
	bool useDefaultWebBrowser = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "UseDefaultWebBrowser", true);
	if (!useDefaultWebBrowser)
		browser = Application::instance()->configuration()->deprecatedApi()->readEntry("Chat", "WebBrowser");

	if (!openUrl(url, url, browser))
		MessageDialog::show(KaduIcon("dialog-error"), QCoreApplication::translate("@default", QT_TR_NOOP("Kadu")),
				QCoreApplication::translate("@default", QT_TR_NOOP("Could not spawn Web browser process. Check if the Web browser is functional")));
}

void UrlOpener::openEmail(const QByteArray &email)
{
	QString client;

	bool useDefaultEMailClient = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "UseDefaultEMailClient", true);
	if (!useDefaultEMailClient)
		client = Application::instance()->configuration()->deprecatedApi()->readEntry("Chat", "MailClient");

	QByteArray urlForDesktopServices;
	QByteArray urlForApplication;
	if (email.startsWith("mailto:"))
	{
		urlForDesktopServices = email;
		urlForApplication = email;
		urlForApplication.remove(0, 7);
	}
	else
	{
		urlForDesktopServices = "mailto:" + email;
		urlForApplication = email;
	}

	if (!openUrl(urlForDesktopServices, urlForApplication, client))
		MessageDialog::show(KaduIcon("dialog-error"), QCoreApplication::translate("@default", QT_TR_NOOP("Kadu")),
				QCoreApplication::translate("@default", QT_TR_NOOP("Could not spawn Mail client process. Check if the Mail client is functional")));
}
