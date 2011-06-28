/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "configuration/configuration-file.h"
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

#ifdef Q_WS_X11
	messageLabel->setText(QString(tr("A new version <b>%1</b> of Kadu Instant Messenger is available for download. "
							 "Please <a href='download'>download</a> an installer and upgrade or use "
							 "your package management system to update Kadu.")).arg(newestVersion));
#else
	messageLabel->setText(QString(tr("A new version <b>%1</b> of Kadu Instant Messenger is available for download. "
							 "Please <a href='download'>download</a> an installer and upgrade.")).arg(newestVersion));
#endif

	connect(messageLabel, SIGNAL(linkActivated(QString)), this, SLOT(downloadClicked()));

	CheckForUpdates = new QCheckBox(tr("Check for updates when Kadu is opened"));
	CheckForUpdates->setChecked(config_file.readBoolEntry("General", "CheckUpdates", true));

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
	if (config_file.readEntry("General", "Language") == "pl")
		UrlOpener::openUrl("http://www.kadu.net/w/Pobierz");
	else
		UrlOpener::openUrl("http://www.kadu.net/w/English:Download");
}

void UpdatesDialog::accepted()
{
	config_file.writeEntry("General", "CheckUpdates", CheckForUpdates->isChecked());

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
