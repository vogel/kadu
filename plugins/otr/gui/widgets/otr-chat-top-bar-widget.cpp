/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QHBoxLayout>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>

#include "otr-policy.h"
#include "otr-policy-account-store.h"

#include "otr-chat-top-bar-widget.h"

OtrChatTopBarWidget::OtrChatTopBarWidget(const Contact &contact, QWidget *parent) :
		QWidget(parent), MyContact(contact)
{
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
	layout->addWidget(OtrStatusButton);
	layout->addStretch(1);

	QMenu *otrMenu = new QMenu(OtrStatusButton);
	QAction *startAction = otrMenu->addAction(tr("Start Private Conversation"));
	connect(startAction, SIGNAL(triggered(bool)), this, SLOT(startPrivateConversation()));
	QAction *endAction = otrMenu->addAction(tr("End Private Conversation"));
	connect(endAction, SIGNAL(triggered(bool)), this, SLOT(endPrivateConversation()));
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
}

void OtrChatTopBarWidget::trustLevelUpdated()
{
	OtrTrustLevelService::TrustLevel level = trustLevel();

	OtrStatusButton->setText(trustStatusString(level));
	VerifyAction->setEnabled(level >= OtrTrustLevelService::TrustLevelUnverified);
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
		case OtrTrustLevelService::TrustLevelUnknown:
			return tr("Unknown");
		case OtrTrustLevelService::TrustLevelNotPrivate:
			return tr("Not Private");
		case OtrTrustLevelService::TrustLevelUnverified:
			return tr("Unverified");
		case OtrTrustLevelService::TrustLevelPrivate:
			return tr("Private");
		default:
			return tr("Not Private");
	}
}

void OtrChatTopBarWidget::startPrivateConversation()
{
	emit startPrivateConversation(MyContact);
}

void OtrChatTopBarWidget::endPrivateConversation()
{
	emit endPrivateConversation(MyContact);
}

void OtrChatTopBarWidget::verifyPeerIdentity()
{
	emit verifyPeerIdentity(MyContact);
}

#include "moc_otr-chat-top-bar-widget.cpp"
