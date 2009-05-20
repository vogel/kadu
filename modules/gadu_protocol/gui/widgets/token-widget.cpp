 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "server/token-fetcher.h"
#include "icons-manager.h"

#include "token-widget.h"

TokenWidget::TokenWidget(QWidget *parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	TokenImage = new QLabel(this);

	QPushButton *refreshButton = new QPushButton(IconsManager::instance()->loadIcon("ReloadPersonalInfoButton"), "" ,this);
	refreshButton->setIconSize(IconsManager::instance()->getIconsSize());
	connect(refreshButton, SIGNAL(clicked()), this, SLOT(refreshToken()));

	QFrame *tokenFrame = new QFrame(this);
	tokenFrame->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);

	QHBoxLayout *frameLayout = new QHBoxLayout(tokenFrame);
	frameLayout->setContentsMargins(0, 0, 0, 0);
	frameLayout->addWidget(TokenImage);
	frameLayout->addWidget(refreshButton);

	TokenCode = new QLineEdit(this);

	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	frameLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(tokenFrame);
	mainLayout->addWidget(TokenCode);

	Fetcher = new TokenFetcher(false, this);
	connect(Fetcher, SIGNAL(tokenFetched(const QString &, QPixmap)), this, SLOT(tokenFetched(const QString &, QPixmap)));
	refreshToken();
}

TokenWidget::~TokenWidget()
{
}

void TokenWidget::tokenFetched(const QString &tokenId, QPixmap tokenImage)
{
	TokenImage->setPixmap(tokenImage);
	TokenId = tokenId;
}

void TokenWidget::refreshToken()
{
	Fetcher->fetchToken();
}


QString TokenWidget::tokenId()
{
	return TokenId;
}

QString TokenWidget::tokenValue()
{
	return TokenCode->text();
}
