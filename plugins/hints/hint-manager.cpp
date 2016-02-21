/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "hint-manager.h"

#include "hint-repository.h"
#include "hints-widget-positioner.h"
#include "hints-widget.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact.h"
#include "core/injected-factory.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/tool-tip-class-manager.h"
#include "icons/icons-manager.h"
#include "message/message-manager.h"
#include "message/sorted-messages.h"
#include "misc/misc.h"
#include "notification/notification.h"
#include "notification/notifier-repository.h"
#include "parser/parser.h"
#include "activate.h"
#include "debug.h"

#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

#include "chat/chat-manager.h"

/**
 * @ingroup hints
 * @{
 */
#define FRAME_WIDTH 1
#define BORDER_RADIUS 0

HintManager::HintManager(QObject *parent) :
		QObject{parent},
		Notifier("Hints", "Hints", KaduIcon("kadu_icons/notify-hints")), AbstractToolTip(),
		hint_timer(new QTimer(this)),
		tipFrame(0)
{
}

HintManager::~HintManager()
{
}

void HintManager::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void HintManager::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void HintManager::setHintRepository(HintRepository *hintRepository)
{
	m_hintRepository = hintRepository;
}

void HintManager::setHintsWidget(HintsWidget *hintsWidget)
{
	m_hintsWidget = hintsWidget;
}

void HintManager::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void HintManager::setNotifierRepository(NotifierRepository *notifierRepository)
{
	m_notifierRepository = notifierRepository;
}

void HintManager::setParser(Parser *parser)
{
	m_parser = parser;
}

void HintManager::setToolTipClassManager(ToolTipClassManager *toolTipClassManager)
{
	m_toolTipClassManager = toolTipClassManager;
}

void HintManager::init()
{
	kdebugf();

	connect(hint_timer, SIGNAL(timeout()), this, SLOT(oneSecond()));

	m_notifierRepository->registerNotifier(this);
	m_toolTipClassManager->registerToolTipClass(QT_TRANSLATE_NOOP("@default", "Hints"), this);

	configurationUpdated();

	// remember to call it after setting `Style' member

	kdebugf2();
}

void HintManager::done()
{
	if (hint_timer)
		hint_timer->stop();

	m_toolTipClassManager->unregisterToolTipClass("Hints");
	m_notifierRepository->unregisterNotifier(this);

	disconnect();

	if (tipFrame)
		tipFrame->deleteLater();
}

void HintManager::hintUpdated()
{
	setHint();
}

void HintManager::configurationUpdated()
{
	setHint();
}

void HintManager::setHint()
{
	if (m_hintRepository->isEmpty())
	{
		hint_timer->stop();
		m_hintsWidget->hide();
		return;
	}
	else
		m_hintsWidget->show();
}

void HintManager::deleteHint(Hint *hint)
{
	kdebugf();

	m_hintRepository->removeHint(hint);

	m_hintsWidget->removeHint(hint);

	hint->deleteLater();

	if (m_hintRepository->isEmpty())
	{
		hint_timer->stop();
		m_hintsWidget->hide();
	}

	kdebugf2();
}

void HintManager::deleteHintAndUpdate(Hint *hint)
{
	deleteHint(hint);
	setHint();
}

void HintManager::oneSecond(void)
{
	for (auto hint : m_hintRepository)
		hint->nextSecond();

	std::vector<Hint *> deprecated;
	std::copy_if(begin(m_hintRepository), end(m_hintRepository), std::back_inserter(deprecated), [](Hint *hint) { return hint->isDeprecated(); });
	for (auto hint : deprecated)
	{
		deleteHint(hint);
		m_hintRepository->removeHint(hint);
	}

	if (!deprecated.empty())
		setHint();
}

NotifierConfigurationWidget * HintManager::createConfigurationWidget(QWidget *parent)
{
	Q_UNUSED(parent);

	return nullptr;
}

void HintManager::leftButtonSlot(Hint *hint)
{
	hint->acceptNotification();
}

void HintManager::rightButtonSlot(Hint *hint)
{
	hint->discardNotification();
	deleteHintAndUpdate(hint);
}

void HintManager::midButtonSlot(Hint *hint)
{
	Q_UNUSED(hint);

	deleteAllHints();
	setHint();
}

void HintManager::deleteAllHints()
{
	hint_timer->stop();

	while (begin(m_hintRepository) != end(m_hintRepository))
	{
		(*begin(m_hintRepository))->discardNotification();
		m_hintRepository->removeHint(*begin(m_hintRepository));
	}

	m_hintsWidget->hide();
}

Hint *HintManager::addHint(const Notification &notification)
{
	kdebugf();

	auto hint = m_injectedFactory->makeInjected<Hint>(m_hintsWidget, notification);
	m_hintRepository->addHint(hint);
	m_hintsWidget->addHint(hint);

	connect(hint, SIGNAL(leftButtonClicked(Hint *)), this, SLOT(leftButtonSlot(Hint *)));
	connect(hint, SIGNAL(rightButtonClicked(Hint *)), this, SLOT(rightButtonSlot(Hint *)));
	connect(hint, SIGNAL(midButtonClicked(Hint *)), this, SLOT(midButtonSlot(Hint *)));
	connect(hint, SIGNAL(closing(Hint *)), this, SLOT(deleteHintAndUpdate(Hint *)));
	connect(hint, SIGNAL(updated(Hint *)), this, SLOT(hintUpdated()));
	setHint();

	if (!hint_timer->isActive())
		hint_timer->start(1000);

	return hint;
}

void HintManager::prepareOverUserHint(QFrame *tipFrame, QLabel *tipLabel, Talkable talkable)
{
	auto syntax = m_configuration->deprecatedApi()->readEntry("Hints", "MouseOverUserSyntax");
	// file:/// is added by parser where required
	syntax = syntax.remove("file:///");

	auto text = m_parser->parse(syntax, talkable, ParserEscape::HtmlEscape);
	while (text.endsWith(QStringLiteral("<br/>")))
		text.resize(text.length() - 5 /* 5 == QString("<br/>").length()*/);
	while (text.startsWith(QStringLiteral("<br/>")))
		text = text.right(text.length() - 5 /* 5 == QString("<br/>").length()*/);

#ifdef Q_OS_UNIX
	text = text.remove("file://");
#endif

	tipLabel->setFont(m_configuration->deprecatedApi()->readFontEntry("Hints", "HintOverUser_font"));
	tipLabel->setText(text);

	tipFrame->setObjectName("tip_frame");
	tipFrame->setFixedSize(tipLabel->sizeHint() + QSize(2 * FRAME_WIDTH, 2 * FRAME_WIDTH));
}

void HintManager::showToolTip(const QPoint &point, Talkable talkable)
{
	kdebugf();

	delete tipFrame;

	tipFrame = new QFrame(0, Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);

	QHBoxLayout *lay = new QHBoxLayout(tipFrame);
	lay->setMargin(10);
	lay->setSizeConstraint(QLayout::SetFixedSize);

	QLabel *tipLabel = new QLabel(tipFrame);
	tipLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	tipLabel->setContentsMargins(10, 10, 10, 10);
	tipLabel->setTextFormat(Qt::RichText);
	tipLabel->setWordWrap(true);

	lay->addWidget(tipLabel);

	prepareOverUserHint(tipFrame, tipLabel, talkable);

	QPoint pos(point + QPoint(5, 5));

	QSize preferredSize = tipFrame->sizeHint();
	QSize desktopSize = QApplication::desktop()->screenGeometry(m_hintsWidget).size();
	if (pos.x() + preferredSize.width() > desktopSize.width())
		pos.setX(pos.x() - preferredSize.width() - 10);
	if (pos.y() + preferredSize.height() > desktopSize.height())
		pos.setY(pos.y() - preferredSize.height() - 10);

	tipFrame->move(pos);
	tipFrame->show();

	kdebugf2();
}

void HintManager::hideToolTip()
{
	if (tipFrame)
	{
		tipFrame->hide();
		tipFrame->deleteLater();
		tipFrame = 0;
	}
}

void HintManager::notify(const Notification &notification)
{
	addHint(notification);
}

HintManager *hint_manager = NULL;

#include "moc_hint-manager.cpp"
