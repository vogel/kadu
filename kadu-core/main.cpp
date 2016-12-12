/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2014 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2007, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-module.h"
#include "actions/actions-module.h"
#include "avatars/avatar-module.h"
#include "buddies/buddy-module.h"
#include "chat-style/chat-style-module.h"
#include "chat/chat-module.h"
#include "configuration/configuration-module.h"
#include "configuration/configuration-path-provider.h"
#include "configuration/configuration-unusable-exception.h"
#include "contacts/contact-module.h"
#include "core/application.h"
#include "core/core-module.h"
#include "core/core.h"
#include "dom/dom-module.h"
#include "execution-arguments/execution-arguments-parser.h"
#include "execution-arguments/execution-arguments.h"
#include "file-transfer/file-transfer-module.h"
#include "formatted-string/formatted-string-module.h"
#include "gui/gui-module.h"
#include "icons/icons-module.h"
#include "identities/identity-module.h"
#include "message/message-module.h"
#include "message/message.h"
#include "multilogon/multilogon-module.h"
#include "network/network-module.h"
#include "notification/notification-module.h"
#include "os/os-module.h"
#include "os/win/wsa-exception.h"
#include "os/win/wsa-handler.h"
#include "parser/parser-module.h"
#include "plugin/plugin-module.h"
#include "roster/roster-module.h"
#include "ssl/ssl-module.h"
#include "status/status-module.h"
#include "talkable/talkable-module.h"
#include "themes/themes-module.h"
#include "widgets/chat-widget/chat-widget-module.h"
#include "windows/chat-window/chat-window-module.h"
#include "kadu-config.h"

#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <injeqt/injector.h>
#include <memory>

#ifndef Q_OS_WIN
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#if HAVE_EXECINFO
#include <execinfo.h>
#endif

static void printBacktrace(const QString &header)
{
	if (header.isEmpty())
		fprintf(stderr, "\nbacktrace:\n");
	else
		fprintf(stderr, "\nbacktrace: ('%s')\n", qPrintable(header));
#if HAVE_EXECINFO
	void *bt_array[100];
	char **bt_strings;
	int num_entries;
	if ((num_entries = backtrace(bt_array, 100)) < 0) {
		fprintf(stderr, "could not generate backtrace\n");
		return;
	}
	if ((bt_strings = backtrace_symbols(bt_array, num_entries)) == NULL) {
		fprintf(stderr, "could not get symbol names for backtrace\n");
		return;
	}
	fprintf(stderr, "======= BEGIN OF BACKTRACE =====\n");
	for (int i = 0; i < num_entries; ++i)
		fprintf(stderr, "[%d] %s\n", i, bt_strings[i]);
	fprintf(stderr, "======= END OF BACKTRACE  ======\n");
	free(bt_strings);
#else
	fprintf(stderr, "backtrace not available\n");
#endif
	fflush(stderr);
}

static void kaduQtMessageHandler(QtMsgType type, const char *msg)
{
	switch (type)
	{
		case QtDebugMsg:
			fprintf(stderr, "Debug: %s\n", msg);
			fflush(stderr);
			break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
		case QtInfoMsg:
			fprintf(stderr, "Info: %s\n", msg);
			fflush(stderr);
			break;
#endif
		case QtWarningMsg:
			fprintf(stderr, "\033[34mWarning: %s\033[0m\n", msg);
			fflush(stderr);
			if (strstr(msg, "no mimesource for") == 0)
				printBacktrace("warning from Qt (above)");
			break;
		case QtCriticalMsg:
			fprintf(stderr, "\033[31;1mCritical: %s\033[0m\n", msg);
			fflush(stderr);
			printBacktrace("critical error from Qt (above)");
			break;
		case QtFatalMsg:
			fprintf(stderr, "\033[31;1mFatal: %s\033[0m\n", msg);
			fflush(stderr);
			printBacktrace("fatal error from Qt (above)");
			abort();
			break;
	}
}
#endif // !Q_OS_WIN

#ifdef DEBUG_OUTPUT_ENABLED
extern KADUAPI bool showTimesInDebug;
#endif

static void printVersion()
{
	printf(
		"Kadu %s Copyright (c) 2001-2014 Kadu Team\n"
		"Compiled with Qt %s\nRunning on Qt %s\n",
		qPrintable(KADU_VERSION), QT_VERSION_STR, qVersion());
}

static void printUsage()
{
	printf(
		"Usage: kadu [General Options] [Options]\n\n"
		"Kadu Instant Messenger\n"
		"\nGeneral Options:\n"
		"  --help                     Print Kadu options\n"
		"  --version                  Print Kadu and Qt version\n"
		"\nOptions:\n"
		"  --debug <mask>             Set debugging mask\n"
		"  --config-dir <path>        Set configuration directory\n"
		"                             (overwrites CONFIG_DIR variable)\n");
}

int main(int argc, char *argv[]) try
{
	WSAHandler wsaHandler;

	QApplication application{argc, argv};
	application.setApplicationName("Kadu");
	application.setQuitOnLastWindowClosed(false);

	auto executionArgumentsParser = ExecutionArgumentsParser{};
	// do not parse program name
	auto executionArguments = executionArgumentsParser.parse(QCoreApplication::arguments().mid(1));

	if (executionArguments.queryVersion())
	{
		printVersion();
		return 0;
	}

	if (executionArguments.queryUsage())
	{
		printUsage();
		return 0;
	}

	if (!executionArguments.debugMask().isEmpty())
	{
		bool ok;
		executionArguments.debugMask().toInt(&ok);
		if (ok)
			qputenv("DEBUG_MASK", executionArguments.debugMask().toUtf8());
		else
			fprintf(stderr, "Ignoring invalid debug mask '%s'\n", executionArguments.debugMask().toUtf8().constData());
	}

	qRegisterMetaType<Message>();

	auto profileDirectory = executionArguments.profileDirectory().isEmpty()
			? QString::fromUtf8(qgetenv("CONFIG_DIR"))
			: executionArguments.profileDirectory();

	auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
	modules.emplace_back(std::make_unique<AccountModule>());
	modules.emplace_back(std::make_unique<ActionsModule>());
	modules.emplace_back(std::make_unique<AvatarModule>());
	modules.emplace_back(std::make_unique<BuddyModule>());
	modules.emplace_back(std::make_unique<ChatModule>());
	modules.emplace_back(std::make_unique<ChatStyleModule>());
	modules.emplace_back(std::make_unique<ChatWidgetModule>());
	modules.emplace_back(std::make_unique<ChatWindowModule>());
	modules.emplace_back(std::make_unique<CoreModule>(std::move(profileDirectory)));
	modules.emplace_back(std::make_unique<ConfigurationModule>());
	modules.emplace_back(std::make_unique<ContactModule>());
	modules.emplace_back(std::make_unique<DomModule>());
	modules.emplace_back(std::make_unique<FileTransferModule>());
	modules.emplace_back(std::make_unique<FormattedStringModule>());
	modules.emplace_back(std::make_unique<GuiModule>());
	modules.emplace_back(std::make_unique<IconsModule>());
	modules.emplace_back(std::make_unique<IdentityModule>());
	modules.emplace_back(std::make_unique<MessageModule>());
	modules.emplace_back(std::make_unique<MultilogonModule>());
	modules.emplace_back(std::make_unique<NetworkModule>());
	modules.emplace_back(std::make_unique<NotificationModule>());
	modules.emplace_back(std::make_unique<OsModule>());
	modules.emplace_back(std::make_unique<ParserModule>());
	modules.emplace_back(std::make_unique<PluginModule>());
	modules.emplace_back(std::make_unique<RosterModule>());
	modules.emplace_back(std::make_unique<SslModule>());
	modules.emplace_back(std::make_unique<StatusModule>());
	modules.emplace_back(std::make_unique<TalkableModule>());
	modules.emplace_back(std::make_unique<ThemesModule>());

	auto injector = injeqt::injector{std::move(modules)};

	try
	{
		injector.instantiate<Application>(); // force creation of Application object

#ifndef Q_OS_WIN
		// Qt version is better on win32
		qInstallMsgHandler(kaduQtMessageHandler);
#endif

#ifdef DEBUG_OUTPUT_ENABLED
		showTimesInDebug = (0 != qgetenv("SHOW_TIMES").toInt());
#endif

		Core core{std::move(injector)};
		return core.executeSingle(executionArguments);
	}
	catch (ConfigurationUnusableException &)
	{
		auto profilePath = injector.get<ConfigurationPathProvider>()->configurationDirectoryPath();
		auto errorMessage = QCoreApplication::translate("@default", "We're sorry, but Kadu cannot be loaded. "
				"Profile is inaccessible. Please check permissions in the '%1' directory.")
				.arg(profilePath.left(profilePath.length() - 1));
		QMessageBox::critical(0, QCoreApplication::translate("@default", "Profile Inaccessible"), errorMessage, QMessageBox::Abort);

		throw;
	}
}
#if defined(Q_OS_WIN)
catch (WSAException &)
{
	return 2;
}
#endif
catch (ConfigurationUnusableException &)
{
	// already handled
}
catch (...)
{
	throw;
}
