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

#include "otr-app-ops-wrapper.h"
#include "otr-policy.h"
#include "otr-policy-account-store.h"
#include "otr-trust-level-contact-store.h"

#include "otr-chat-top-bar-widget.h"

OtrChatTopBarWidget::OtrChatTopBarWidget(const Contact &contact, QWidget *parent) :
		QWidget(parent), MyContact(contact)
{
	createGui();
}

OtrChatTopBarWidget::~OtrChatTopBarWidget()
{
}

void OtrChatTopBarWidget::setAppOpsWrapper(OtrAppOpsWrapper *appOpsWrapper)
{
	CurrentAppOpsWrapper = appOpsWrapper;
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

	OtrStatusButton->setMenu(otrMenu);

	updateTrustStatus();
}

void OtrChatTopBarWidget::updateTrustStatus()
{
	OtrTrustLevel::Level level = trustLevel();

	OtrStatusButton->setText(trustStatusString(level));
}

OtrTrustLevel::Level OtrChatTopBarWidget::trustLevel() const
{
	return OtrTrustLevelContactStore::loadTrustLevelFromContact(MyContact);
}

QString OtrChatTopBarWidget::trustStatusString(OtrTrustLevel::Level level) const
{
	switch (level)
	{
		case OtrTrustLevel::TRUST_NOT_PRIVATE:
			return tr("Not Private");
		case OtrTrustLevel::TRUST_UNVERIFIED:
			return tr("Unverified");
		case OtrTrustLevel::TRUST_PRIVATE:
			return tr("Private");
		default:
			return tr("Not Private");
	}
}

void OtrChatTopBarWidget::startPrivateConversation()
{
	if (CurrentAppOpsWrapper)
		CurrentAppOpsWrapper.data()->startPrivateConversation(MyContact);
}

void OtrChatTopBarWidget::endPrivateConversation()
{
	if (CurrentAppOpsWrapper)
		CurrentAppOpsWrapper.data()->endPrivateConversation(MyContact);
}

#include "moc_otr-chat-top-bar-widget.cpp"
