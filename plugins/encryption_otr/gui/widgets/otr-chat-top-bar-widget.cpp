/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>

#include "icons/kadu-icon.h"

#include "otr-policy.h"

#include "otr-chat-top-bar-widget.h"

OtrChatTopBarWidget::OtrChatTopBarWidget(const Contact &contact, QWidget *parent) :
		QWidget(parent), MyContact(contact)
{
	setFocusPolicy(Qt::NoFocus);

	createGui();
}

OtrChatTopBarWidget::~OtrChatTopBarWidget()
{
}

void OtrChatTopBarWidget::createGui()
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(2);
	layout->setSpacing(0);

	OtrStatusButton = new QPushButton();
	OtrStatusButton->setFlat(true);
	layout->addWidget(OtrStatusButton);
	layout->addStretch(1);

	QMenu *otrMenu = new QMenu(OtrStatusButton);
	StartAction = otrMenu->addAction(tr("Start Private Conversation"));
	connect(StartAction, SIGNAL(triggered(bool)), this, SLOT(startSession()));
	EndAction = otrMenu->addAction(tr("End Private Conversation"));
	connect(EndAction, SIGNAL(triggered(bool)), this, SLOT(endSession()));
	otrMenu->addSeparator();
	VerifyAction = otrMenu->addAction(tr("Verify Peer Identity"));
	connect(VerifyAction, SIGNAL(triggered(bool)), this, SLOT(verifyPeerIdentity()));

	OtrStatusButton->setMenu(otrMenu);

	trustLevelUpdated();
}

void OtrChatTopBarWidget::setTrustLevelService(OtrTrustLevelService *trustLevelService)
{
	if (TrustLevelService)
		disconnect(TrustLevelService.data(), 0, this, 0);
	TrustLevelService = trustLevelService;
	if (TrustLevelService)
		connect(TrustLevelService.data(), SIGNAL(trustLevelsUpdated()), this, SLOT(trustLevelUpdated()));

	trustLevelUpdated();
}

void OtrChatTopBarWidget::trustLevelUpdated()
{
	OtrTrustLevelService::TrustLevel level = trustLevel();

#if defined(Q_OS_WIN)
	OtrStatusButton->setText(trustStatusString(level) + "    "); // see #2835
#else
	OtrStatusButton->setText(trustStatusString(level));
#endif

	bool isPrivate = level >= OtrTrustLevelService::TrustLevelUnverified;

	if (isPrivate)
	{
		OtrStatusButton->setIcon(KaduIcon("security-high").icon());
		StartAction->setText(tr("Refresh Private Conversation"));
	}
	else
	{
		OtrStatusButton->setIcon(KaduIcon("security-low").icon());
		StartAction->setText(tr("Start Private Conversation"));
	}

	EndAction->setEnabled(isPrivate);
	VerifyAction->setEnabled(isPrivate);
}

OtrTrustLevelService::TrustLevel OtrChatTopBarWidget::trustLevel() const
{
	if (TrustLevelService)
		return TrustLevelService.data()->loadTrustLevelFromContact(MyContact);
	else
		return OtrTrustLevelService::TrustLevelUnknown;
}

QString OtrChatTopBarWidget::trustStatusString(OtrTrustLevelService::TrustLevel level) const
{
	switch (level)
	{
		case OtrTrustLevelService::TrustLevelUnverified:
			return tr("Unverified");
		case OtrTrustLevelService::TrustLevelPrivate:
			return tr("Private");
		case OtrTrustLevelService::TrustLevelNotPrivate:
		case OtrTrustLevelService::TrustLevelUnknown:
		default:
			return tr("Not Private");
	}
}

void OtrChatTopBarWidget::startSession()
{
	emit startSession(MyContact);
}

void OtrChatTopBarWidget::endSession()
{
	emit endSession(MyContact);
}

void OtrChatTopBarWidget::verifyPeerIdentity()
{
	emit verifyPeerIdentity(MyContact);
}

#include "moc_otr-chat-top-bar-widget.cpp"
