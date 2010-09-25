/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>

#include "configuration/configuration-file.h"
#include "gui/windows/message-dialog.h"

#include "url-opener.h"

bool UrlOpener::openUrl(const QString &urlForDesktopServices, const QString &urlForApplication, const QString &application)
{
	if (!application.isEmpty())
	{
		QProcess *process = new QProcess(qApp);

		QString launchLine = application;
		if (!launchLine.contains("%1"))
			launchLine.append(" \"" + urlForApplication + '"');
		else
			launchLine.replace("%1", urlForApplication);

		process->start(launchLine);
		if (process->waitForStarted())
			return true;
	}

	return QDesktopServices::openUrl(QUrl(urlForDesktopServices));
}

void UrlOpener::openUrl(const QString &url)
{
	QString browser;
	bool useDefaultWebBrowser = config_file.readBoolEntry("Chat", "UseDefaultWebBrowser", true);
	if (!useDefaultWebBrowser)
		browser = config_file.readEntry("Chat", "WebBrowser");

	if (!openUrl(url, url, browser))
		MessageDialog::msg(qApp->translate("@default", QT_TR_NOOP("Could not spawn Web browser process. Check if the Web browser is functional")), false, "dialog-error");
}

void UrlOpener::openEmail(const QString &email)
{
	QString client;
	bool useDefaultEMailClient = config_file.readBoolEntry("Chat", "UseDefaultEMailClient", true);
	if (useDefaultEMailClient)
		client = config_file.readEntry("Chat", "MailClient");

	QString urlForDesktopServices;
	QString urlForApplication;
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
		MessageDialog::msg(qApp->translate("@default", QT_TR_NOOP("Could not spawn Mail client process. Check if the Mail client is functional")), false, "dialog-error");
}
