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
#include <QtGui/QLabel>

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

void OtrChatTopBarWidget::createGui()
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(2);
	layout->setSpacing(0);

	TrustStatusLabel = new QLabel();
	layout->addWidget(TrustStatusLabel);

	updateTrustStatus();
}

void OtrChatTopBarWidget::updateTrustStatus()
{
	OtrTrustLevel::Level level = OtrTrustLevelContactStore::loadTrustLevelFromContact(MyContact);

	setStyleSheet(QString("color: white; background-color: %1; border: 1px solid #121212;")
			.arg(trustStatusColor(level).name()));
	TrustStatusLabel->setText(trustStatusString(level));
}

QColor OtrChatTopBarWidget::trustStatusColor(OtrTrustLevel::Level level)
{
	switch (level)
	{
		case OtrTrustLevel::TRUST_NOT_PRIVATE:
			return Qt::red;
		case OtrTrustLevel::TRUST_UNVERIFIED:
			return Qt::yellow;
		case OtrTrustLevel::TRUST_PRIVATE:
			return Qt::green;
	}
}

QString OtrChatTopBarWidget::trustStatusString(OtrTrustLevel::Level level)
{
	switch (level)
	{
		case OtrTrustLevel::TRUST_NOT_PRIVATE:
			return tr("Not Private");
		case OtrTrustLevel::TRUST_UNVERIFIED:
			return tr("Unverified");
		case OtrTrustLevel::TRUST_PRIVATE:
			return tr("Private");
	}
}

#include "moc_otr-chat-top-bar-widget.cpp"
