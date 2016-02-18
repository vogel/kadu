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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact.h"
#include "core/injected-factory.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/tool-tip-class-manager.h"
#include "gui/tray/tray-service.h"
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

void HintManager::setTrayService(TrayService *trayService)
{
	m_trayService = trayService;
}

void HintManager::init()
{
	kdebugf();

	createDefaultConfiguration();

	frame = new QFrame(0, Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
	frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	layout = new QVBoxLayout(frame);
	layout->setSpacing(0);
	layout->setMargin(0);

	connect(hint_timer, SIGNAL(timeout()), this, SLOT(oneSecond()));

	const QString default_hints_syntax(QT_TRANSLATE_NOOP("HintManager", "<table>"
"<tr>"
"<td align=\"left\" valign=\"top\">"
"<img style=\"max-width:64px; max-height:64px;\" "
"src=\"{#{avatarPath} #{avatarPath}}{~#{avatarPath} @{kadu_icons/kadu:64x64}}\""
">"
"</td>"
"<td width=\"100%\">"
"<div>[<b>%a</b>][&nbsp;<b>(%g)</b>]</div>"
"[<div><img height=\"16\" width=\"16\" src=\"#{statusIconPath}\">&nbsp;&nbsp;%u</div>]"
"[<div><img height=\"16\" width=\"16\" src=\"@{phone:16x16}\">&nbsp;&nbsp;%m</div>]"
"[<div><img height=\"16\" width=\"16\" src=\"@{mail-message-new:16x16}\">&nbsp;&nbsp;%e</div>]"
"</td>"
"</tr>"
"</table>"
"[<hr><b>%s</b>][<b>:</b><br><small>%d</small>]"));
	if (m_configuration->deprecatedApi()->readEntry("Hints", "MouseOverUserSyntax").isEmpty())
		m_configuration->deprecatedApi()->writeEntry("Hints", "MouseOverUserSyntax", default_hints_syntax);

	m_notifierRepository->registerNotifier(this);
	m_toolTipClassManager->registerToolTipClass(QT_TRANSLATE_NOOP("@default", "Hints"), this);

	configurationUpdated();

	// remember to call it after setting `Style' member

	kdebugf2();
}

void HintManager::done()
{
	kdebugf();

	if (hint_timer)
		hint_timer->stop();

	m_toolTipClassManager->unregisterToolTipClass("Hints");
	m_notifierRepository->unregisterNotifier(this);

	disconnect();

	if (tipFrame)
		tipFrame->deleteLater();

	if (frame)
		frame->deleteLater();

	kdebugf2();
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
	kdebugf();

	if (m_hintRepository->isEmpty())
	{
		hint_timer->stop();
		frame->hide();
		return;
	}

	int minimumWidth = m_configuration->deprecatedApi()->readNumEntry("Hints", "MinimumWidth", 285);
	int maximumWidth = m_configuration->deprecatedApi()->readNumEntry("Hints", "MaximumWidth", 500);

	minimumWidth = minimumWidth >= 285 ? minimumWidth : 285;
	maximumWidth = maximumWidth >= 285 ? maximumWidth : 285;

	QPoint newPosition;
	auto trayPosition = m_trayService->trayPosition();

	frame->adjustSize();
	QSize preferredSize = frame->sizeHint();
	if (preferredSize.width() < minimumWidth)
		preferredSize.setWidth(minimumWidth);
	if (preferredSize.width() > maximumWidth)
		preferredSize.setWidth(maximumWidth);
	QSize desktopSize = QApplication::desktop()->screenGeometry(frame).size();

	if (m_configuration->deprecatedApi()->readBoolEntry("Hints", "UseUserPosition") || trayPosition.isNull())
	{
		newPosition = QPoint(m_configuration->deprecatedApi()->readNumEntry("Hints", "HintsPositionX"), m_configuration->deprecatedApi()->readNumEntry("Hints", "HintsPositionY"));

//		kdebugm(KDEBUG_INFO, "%d %d %d\n", m_configuration->deprecatedApi()->readNumEntry("Hints", "Corner"), preferredSize.width(), preferredSize.height());
		switch(m_configuration->deprecatedApi()->readNumEntry("Hints", "Corner"))
		{
			case 1: // "TopRight"
				newPosition -= QPoint(preferredSize.width(), 0);
				break;
			case 2: // "BottomLeft"
				newPosition -= QPoint(0, preferredSize.height());
				break;
			case 3: // "BottomRight"
				newPosition -= QPoint(preferredSize.width(), preferredSize.height());
				break;
			case 0: // "TopLeft"
				break;
		};

		if (newPosition.x() < 0) // when hints go out of the screen (on left)
			newPosition.setX(0);
		if (newPosition.y() < 0) //when hints go out of the screen (on top)
			newPosition.setY(0);

		if (newPosition.x() + preferredSize.width() >= desktopSize.width()) //when hints go out of the screen (on right)
			newPosition.setX(desktopSize.width() - preferredSize.width());
		if (newPosition.y() + preferredSize.height() >= desktopSize.height()) //when hints go out of the screen (on bottom)
			newPosition.setY(desktopSize.height() - preferredSize.height());
	}
	else
	{
		// those "strange" cases happens when "automatic panel hiding" is in use
		if (trayPosition.x() < 0)
			trayPosition.setX(0);
		else if (trayPosition.x() > desktopSize.width())
			trayPosition.setX(desktopSize.width() - 2);
		if (trayPosition.y() < 0)
			trayPosition.setY(0);
		else if (trayPosition.y() > desktopSize.height())
			trayPosition.setY(desktopSize.height() - 2);


		if (trayPosition.x() < desktopSize.width() / 2) // tray is on left
			newPosition.setX(trayPosition.x() + 32);
		else // tray is on right
			newPosition.setX(trayPosition.x() - preferredSize.width());

		if (trayPosition.y() < desktopSize.height() / 2) // tray is on top
			newPosition.setY(trayPosition.y() + 32);
		else // tray is on bottom
			newPosition.setY(trayPosition.y() - preferredSize.height());
	}

	// Only setFixedSize() and move() (in this order) guarantees correct
	// placement on all platforms (at least those I tested).
	frame->setFixedSize(preferredSize);
	frame->move(newPosition);

	if (frame->isVisible())
		frame->update();
	else
		frame->show();

	kdebugf2();
}

void HintManager::deleteHint(Hint *hint)
{
	kdebugf();

	m_hintRepository->removeHint(hint);

	layout->removeWidget(hint);

	hint->deleteLater();

	if (m_hintRepository->isEmpty())
	{
		hint_timer->stop();
		frame->hide();
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

	frame->hide();
}

Hint *HintManager::addHint(const Notification &notification)
{
	kdebugf();

	auto hint = m_injectedFactory->makeInjected<Hint>(frame, notification);
	m_hintRepository->addHint(hint);

	setLayoutDirection();
	layout->addWidget(hint);

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

void HintManager::setLayoutDirection()
{
	kdebugf();
	auto trayPosition = m_trayService->trayPosition();
	auto desktopSize = QApplication::desktop()->screenGeometry(frame).size();

	switch (m_configuration->deprecatedApi()->readNumEntry("Hints", "NewHintUnder"))
	{
		case 0:
			if (trayPosition.isNull() || m_configuration->deprecatedApi()->readBoolEntry("Hints","UseUserPosition"))
			{
				if (m_configuration->deprecatedApi()->readNumEntry("Hints","HintsPositionY") < desktopSize.height()/2)
					layout->setDirection(QBoxLayout::Down);
				else
					layout->setDirection(QBoxLayout::Up);
			}
			else
			{
				if (trayPosition.y() < desktopSize.height()/2)
					layout->setDirection(QBoxLayout::Down);
				else
					layout->setDirection(QBoxLayout::Up);
			}
			break;
		case 1:
			layout->setDirection(QBoxLayout::Up);
			break;
		case 2:
			layout->setDirection(QBoxLayout::Down);
			break;
	}
	kdebugf2();
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
	QSize desktopSize = QApplication::desktop()->screenGeometry(frame).size();
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
	kdebugf();

	addHint(notification);

	kdebugf2();
}

void HintManager::createDefaultConfiguration()
{
	// TODO: this should be more like: if (plugins.loaded(freedesktop_notify) && this_is_first_time_we_are_loaded_or_whatever)
#if !defined(Q_OS_UNIX)
	m_configuration->deprecatedApi()->addVariable("Notify", "ConnectionError_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "NewChat_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "NewMessage_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToFreeForChat_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToOnline_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToAway_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToNotAvailable_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToDoNotDisturb_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "StatusChanged/ToOffline_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer/IncomingFile_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "FileTransfer/Finished_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "multilogon_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "multilogon/sessionConnected_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "multilogon/sessionDisconnected_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ImportFailed_UseCustomSettings", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ImportFailed_Hints", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ExportFailed_UseCustomSettings", true);
	m_configuration->deprecatedApi()->addVariable("Notify", "Roster/ExportFailed_Hints", true);
#endif

	m_configuration->deprecatedApi()->addVariable("Hints", "CiteSign", 50);
	m_configuration->deprecatedApi()->addVariable("Hints", "Corner", 0);

	m_configuration->deprecatedApi()->addVariable("Hints", "HintsPositionX", 0);
	m_configuration->deprecatedApi()->addVariable("Hints", "HintsPositionY", 0);
	m_configuration->deprecatedApi()->addVariable("Hints", "MaximumWidth", 500);
	m_configuration->deprecatedApi()->addVariable("Hints", "MinimumWidth", 285);
	m_configuration->deprecatedApi()->addVariable("Hints", "MouseOverUserSyntax", QString());
	m_configuration->deprecatedApi()->addVariable("Hints", "NewHintUnder", 0);
	m_configuration->deprecatedApi()->addVariable("Hints", "ShowContentMessage", true);
	m_configuration->deprecatedApi()->addVariable("Hints", "UseUserPosition", false);

	m_configuration->deprecatedApi()->addVariable("Hints", "AllEvents_iconSize", 32);

	m_configuration->deprecatedApi()->addVariable("Hints", "HintOverUser_iconSize", 32);
	m_configuration->deprecatedApi()->addVariable("Hints", "HintOverUser_font", qApp->font());
	m_configuration->deprecatedApi()->addVariable("Hints", "HintOverUser_Geometry", "50, 50, 640, 610");
	m_configuration->deprecatedApi()->addVariable("Hints", "HintEventConfiguration_Geometry", "50, 50, 520, 345");
}

HintManager *hint_manager = NULL;

#include "moc_hint-manager.cpp"
