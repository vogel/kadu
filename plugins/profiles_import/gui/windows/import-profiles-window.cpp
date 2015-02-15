/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFileInfo>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>

#include "gui/windows/message-dialog.h"
#include "identities/identity-manager.h"
#include "misc/paths-provider.h"

#include "plugins/history_migration/history-importer-manager.h"
#include "plugins/history_migration/history-importer.h"

#include "profile-data-manager.h"
#include "profile-importer.h"
#include "profiles-import-actions.h"

#include "import-profiles-window.h"

ImportProfilesWindow::ImportProfilesWindow(QWidget *parent) :
		QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose, true);

	createGui();
}

ImportProfilesWindow::~ImportProfilesWindow()
{
}

void ImportProfilesWindow::createGui()
{
	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnMinimumWidth(0, 32);

	QLabel *descriptionLabel = new QLabel(
			tr("<p>Current version of Kadu does not support user profiles.<br />Instead, multiple account are supported in one instances of kadu.</p>"
			   "<p>Please select profiles that you would like to import as<br />account into this instance of Kadu.</p>"), this);
	// descriptionLabel->setWordWrap(true); // this work strange
	layout->addWidget(descriptionLabel, 0, 0, 1, 2);

	createProfileList(layout);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	layout->addWidget(buttons, layout->rowCount(), 0, 1, 2);

	QPushButton *importButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Import"), this);
	connect(importButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
	buttons->addButton(importButton, QDialogButtonBox::AcceptRole);

	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Close"), this);
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	setFixedHeight(layout->minimumSize().height());
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

void ImportProfilesWindow::createProfileList(QGridLayout *layout)
{
	QList<ProfileData> profiles = ProfileDataManager::readProfileData();
	foreach (const ProfileData &profile, profiles)
	{
		QCheckBox *profileCheckBox = new QCheckBox(profile.Name, this);
		profileCheckBox->setChecked(true);

		ProfileCheckBoxes[profileCheckBox] = profile;

		QCheckBox *historyCheckBox = new QCheckBox(tr("Import history"), this);
		historyCheckBox->setChecked(true);

		HistoryCheckBoxes[profileCheckBox] = historyCheckBox;

		layout->addWidget(profileCheckBox, layout->rowCount(), 0, 1, 2);
		layout->addWidget(historyCheckBox, layout->rowCount(), 1);
	}
}
void ImportProfilesWindow::accept()
{
	for (QMap<QCheckBox *, ProfileData>::const_iterator it = ProfileCheckBoxes.constBegin(), end = ProfileCheckBoxes.constEnd(); it != end; ++it)
	{
		if (!it.key()->isChecked())
			continue;

		const ProfileData &profile = it.value();
		bool importHistory = HistoryCheckBoxes.value(it.key())->isChecked();

		QString path = QFileInfo(profile.Path).isAbsolute()
				? profile.Path
				: PathsProvider::homePath() + '/' + profile.Path;

		ProfileImporter importer(path + "/kadu/kadu.conf.xml");
		if (importer.import(IdentityManager::instance()->byName(profile.Name, true)))
		{
			ProfileDataManager::markImported(profile.Name);
			MessageDialog::show(KaduIcon("dialog-information"), tr("Import external profile..."), tr("Profile %1 successfully imported!")
					.arg(profile.Name));

			if (importHistory && !HistoryImporterManager::instance()->containsImporter(path + "/kadu/history/"))
			{
				HistoryImporter *hi = new HistoryImporter(importer.resultAccount(), path + "/kadu/history/");
				HistoryImporterManager::instance()->addImporter(hi);

				hi->run();
			}
		}
		else
			MessageDialog::show(KaduIcon("dialog-warning"), tr("Import profile..."), tr("Unable to import profile: %1: %2")
					.arg(profile.Name)
					.arg(importer.errorMessage()));
	}

	ProfilesImportActions::instance()->updateActions();

	QDialog::accept();
}

#include "moc_import-profiles-window.cpp"
