/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "os/generic/url-opener.h"

#include "updates-dialog.h"
#include "updates-dialog.moc"

UpdatesDialog::UpdatesDialog(const QString &newestVersion, QWidget *parent)
        : QDialog(parent), DesktopAwareObject(this), m_newestVersion{newestVersion}
{
    setWindowRole("kadu-updates");
    setWindowTitle(tr("New version is available. Please update"));

    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(QSize(450, 150));
}

UpdatesDialog::~UpdatesDialog()
{
}

void UpdatesDialog::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void UpdatesDialog::setUrlOpener(UrlOpener *urlOpener)
{
    m_urlOpener = urlOpener;
}

void UpdatesDialog::init()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *messageLabel = new QLabel(this);
    messageLabel->setWordWrap(true);

#if defined(Q_OS_UNIX)
    messageLabel->setText(QString(tr("A new version <b>%1</b> of Kadu Instant Messenger is available for download. "
                                     "Please <a href='download'>download</a> an installer and upgrade or use "
                                     "your package management system to update Kadu."))
                              .arg(m_newestVersion));
#else
    messageLabel->setText(QString(tr("A new version <b>%1</b> of Kadu Instant Messenger is available for download. "
                                     "Please <a href='download'>download</a> an installer and upgrade."))
                              .arg(m_newestVersion));
#endif

    connect(messageLabel, SIGNAL(linkActivated(QString)), this, SLOT(downloadClicked()));

    m_checkForUpdates = new QCheckBox(tr("Check for updates when Kadu is opened"));
    m_checkForUpdates->setChecked(m_configuration->deprecatedApi()->readBoolEntry("General", "CheckUpdates", true));

    QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

    QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), this);
    okButton->setDefault(true);
    buttons->addButton(okButton, QDialogButtonBox::AcceptRole);

    connect(okButton, SIGNAL(clicked(bool)), this, SLOT(accepted()));

    layout->addWidget(messageLabel);
    layout->addSpacing(5);
    layout->addWidget(m_checkForUpdates);
    layout->setAlignment(m_checkForUpdates, Qt::AlignRight);
    layout->addStretch(100);
    layout->addWidget(buttons);
}

void UpdatesDialog::downloadClicked()
{
    if (m_configuration->deprecatedApi()->readEntry("General", "Language") == "pl")
        m_urlOpener->openUrl("http://www.kadu.im/w/Pobierz");
    else
        m_urlOpener->openUrl("http://www.kadu.im/w/English:Download");
}

void UpdatesDialog::accepted()
{
    m_configuration->deprecatedApi()->writeEntry("General", "CheckUpdates", m_checkForUpdates->isChecked());

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
