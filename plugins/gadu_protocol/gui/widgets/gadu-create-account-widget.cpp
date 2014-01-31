/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gadu-create-account-widget.h"

#include "gui/widgets/simple-configuration-value-state-notifier.h"
#include "os/generic/url-opener.h"

#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

GaduCreateAccountWidget::GaduCreateAccountWidget(QWidget *parent) :
		AccountCreateWidget{parent}
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	simpleStateNotifier()->setState(StateChangedDataValid); // always OK

	createGui();
}

GaduCreateAccountWidget::~GaduCreateAccountWidget()
{
}

void GaduCreateAccountWidget::createGui()
{
	auto mainLayout = new QVBoxLayout{this};

	auto registerAccountLabel = new QLabel(QString("<a href='register'>%1</a>").arg(tr("Register Account")));
	registerAccountLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	mainLayout->addWidget(registerAccountLabel);
	connect(registerAccountLabel, SIGNAL(linkActivated(QString)), this, SLOT(registerAccount()));

	mainLayout->addStretch(100);
}

void GaduCreateAccountWidget::registerAccount()
{
	UrlOpener::openUrl("https://login.gg.pl/createGG/step1/?id=frame_1");
}

void GaduCreateAccountWidget::apply()
{
	emit accountCreated(true, {});
}

void GaduCreateAccountWidget::cancel()
{
}

#include "moc_gadu-create-account-widget.cpp"
