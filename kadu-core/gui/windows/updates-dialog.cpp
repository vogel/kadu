/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QLocale>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QVBoxLayout>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "os/generic/url-opener.h"

#include "updates-dialog.h"

UpdatesDialog::UpdatesDialog(const QString &newestVersion, QWidget *parent) :
		QDialog(parent), DesktopAwareObject(this)
{
	setWindowRole("kadu-updates");
	setWindowTitle(tr("New version is available. Please update"));

	setAttribute(Qt::WA_DeleteOnClose);
	setMinimumSize(QSize(450, 150));

	QVBoxLayout *layout = new QVBoxLayout(this);

	QLabel *messageLabel = new QLabel(this);
	messageLabel->setWordWrap(true);

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	messageLabel->setText(QString(tr("A new version <b>%1</b> of Kadu Instant Messenger is available for download. "
							 "Please <a href='download'>download</a> an installer and upgrade or use "
							 "your package management system to update Kadu.")).arg(newestVersion));
#else
	messageLabel->setText(QString(tr("A new version <b>%1</b> of Kadu Instant Messenger is available for download. "
							 "Please <a href='download'>download</a> an installer and upgrade.")).arg(newestVersion));
#endif

	connect(messageLabel, SIGNAL(linkActivated(QString)), this, SLOT(downloadClicked()));

	CheckForUpdates = new QCheckBox(tr("Check for updates when Kadu is opened"));
	CheckForUpdates->setChecked(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "CheckUpdates", true));

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), this);
	okButton->setDefault(true);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(accepted()));

	layout->addWidget(messageLabel);
	layout->addSpacing(5);
	layout->addWidget(CheckForUpdates);
	layout->setAlignment(CheckForUpdates, Qt::AlignRight);
	layout->addStretch(100);
	layout->addWidget(buttons);
}

UpdatesDialog::~UpdatesDialog()
{
}

void UpdatesDialog::downloadClicked()
{
	if (Application::instance()->configuration()->deprecatedApi()->readEntry("General", "Language") == "pl")
		UrlOpener::openUrl("http://www.kadu.im/w/Pobierz");
	else
		UrlOpener::openUrl("http://www.kadu.im/w/English:Download");
}

void UpdatesDialog::accepted()
{
	Application::instance()->configuration()->deprecatedApi()->writeEntry("General", "CheckUpdates", CheckForUpdates->isChecked());

	close();
}

void UpdatesDialog::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}

#include "moc_updates-dialog.cpp"
