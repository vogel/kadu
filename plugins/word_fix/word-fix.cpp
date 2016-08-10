/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "configuration/gui/configuration-ui-handler-repository.h"
#include "formatted-string/formatted-string-factory.h"
#include "formatted-string/formatted-string-html-visitor.h"
#include "formatted-string/formatted-string.h"
#include "html/html-string.h"
#include "html/normalized-html-string.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"
#include "debug.h"

#include "word-fix-formatted-string-visitor.h"

#include "word-fix.h"


WordFix::WordFix(QObject *parent) :
		QObject{parent}
{
}

WordFix::~WordFix()
{
}

void WordFix::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void WordFix::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void WordFix::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	m_formattedStringFactory = formattedStringFactory;
}

void WordFix::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void WordFix::init()
{
	ExtractBody.setPattern("<body[^>]*>.*</body>");

	// Loading list
	QString data = m_configuration->deprecatedApi()->readEntry("word_fix", "WordFix_list");
	if (data.isEmpty())
	{
		QFile defList(m_pathsProvider->dataPath() + QStringLiteral("plugins/data/word_fix/wf_default_list.data"));
		if (defList.open(QIODevice::ReadOnly))
		{
			QTextStream s(&defList);
			QStringList pair;
			while (!s.atEnd())
			{
				pair = s.readLine().split('|');
				if (pair.isEmpty())
					continue;

				// TODO why we are not checking if there are actually at least 2 items?
				m_wordsList[pair.at(0)] = pair.at(1);
			}
			defList.close();
		}
		else
		{
			kdebug("Can't open file: %s\n", qPrintable((defList.fileName())));
		}
	}
	else
	{
		QStringList list = data.split("\t\t");
		for (int i = 0; i < list.count(); i++)
		{
			if (!list.at(i).isEmpty())
			{
				QStringList sp = list.at(i).split('\t');
				m_wordsList[sp.at(0)] = sp.at(1);
			}
		}
	}

	connect(m_chatWidgetRepository, SIGNAL(chatWidgetAdded(ChatWidget *)),
		this, SLOT(chatWidgetAdded(ChatWidget *)));
	connect(m_chatWidgetRepository, SIGNAL(chatWidgetRemoved(ChatWidget*)),
		this, SLOT(chatWidgetRemoved(ChatWidget *)));

	for (auto chatWidget : m_chatWidgetRepository)
		chatWidgetAdded(chatWidget);
}

void WordFix::chatWidgetAdded(ChatWidget *chatWidget)
{
	connect(chatWidget, SIGNAL(messageSendRequested(ChatWidget*)), this, SLOT(sendRequest(ChatWidget*)));
}

void WordFix::chatWidgetRemoved(ChatWidget *chatWidget)
{
	disconnect(chatWidget, 0, this, 0);
}

void WordFix::sendRequest(ChatWidget* chat)
{
	if (!m_configuration->deprecatedApi()->readBoolEntry("PowerKadu", "enable_word_fix", false))
		return;

	auto formattedString = m_formattedStringFactory->fromHtml(chat->edit()->htmlMessage());
	WordFixFormattedStringVisitor fixVisitor(m_wordsList);
	formattedString->accept(&fixVisitor);

	auto fixedString = fixVisitor.result();
	FormattedStringHtmlVisitor htmlVisitor;
	fixedString->accept(&htmlVisitor);

	chat->edit()->setHtml(htmlVisitor.result().string());
}

#include "moc_word-fix.cpp"
