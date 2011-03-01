/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include "gui/widgets/identities-combo-box.h"
#include "gui/windows/message-dialog.h"
#include "gui/widgets/path-edit.h"

#include "modules/history_migration/history-importer.h"
#include "modules/history_migration/history-importer-manager.h"

#include "profile-importer.h"

#include "import-profile-window.h"

ImportProfileWindow::ImportProfileWindow(QWidget *parent) :
		QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowTitle(tr("Import external Kadu 0.6.5 profile"));

	createGui();
	validate();
}

ImportProfileWindow::~ImportProfileWindow()
{
}

void ImportProfileWindow::createGui()
{
	QFormLayout *layout = new QFormLayout(this);

	ProfilePathEdit = new PathEdit(tr("Select profile path"), this);
	layout->addRow(new QLabel(tr("Select profile path:")), ProfilePathEdit);
	connect(ProfilePathEdit, SIGNAL(pathChanged(QString)), this, SLOT(validate()));

	SelectIdentity = new IdentitiesComboBox(this);
	layout->addRow(new QLabel(tr("Select imported account identity:")), SelectIdentity);
	connect(SelectIdentity, SIGNAL(identityChanged(Identity)), this, SLOT(validate()));

	ImportHistory = new QCheckBox(tr("Import history"), this);
	ImportHistory->setChecked(true);
	layout->addRow(0, ImportHistory);

	ErrorLabel = new QLabel(this);
	layout->addRow(ErrorLabel);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	layout->addRow(buttons);

	ImportButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Import"), this);
	connect(ImportButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
	buttons->addButton(ImportButton, QDialogButtonBox::AcceptRole);

	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Close"), this);
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	setFixedHeight(layout->minimumSize().height());
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

void ImportProfileWindow::validate()
{
	bool valid = true;
	ErrorLabel->setText(QString());

	if (!SelectIdentity->currentIdentity())
	{
		ErrorLabel->setText(tr("<b>Identity not selected</b>"));
		valid = false;
	}

	QFileInfo kaduConfFile(ProfilePathEdit->path() + "/kadu.conf.xml");
	if (!kaduConfFile.exists())
	{
		ErrorLabel->setText(tr("<b>Selected directory does not contain kadu.conf.xml file</b>"));
		valid = false;
	}

	ImportButton->setEnabled(valid);
}

void ImportProfileWindow::accept()
{
	QFileInfo kaduConfFile(ProfilePathEdit->path() + "/kadu.conf.xml");

	if (!kaduConfFile.exists())
	{
		MessageDialog::exec("dialog-warning", tr("Import external profile..."), tr("This directory is not a Kadu profile directory.\nFile kadu.conf.xml not found"));
		return;
	}

	ProfileImporter importer(kaduConfFile.absoluteFilePath());
	if (importer.import(SelectIdentity->currentIdentity()))
	{
		MessageDialog::exec("dialog-information", tr("Import external profile..."), tr("Profile successfully imported!"));

		if (ImportHistory->isChecked() && !HistoryImporterManager::instance()->containsImporter(kaduConfFile.absoluteDir().absolutePath() + "/history/"))
		{
			HistoryImporter *hi = new HistoryImporter(importer.resultAccount(), kaduConfFile.absoluteDir().absolutePath() + "/history/");
			HistoryImporterManager::instance()->addImporter(hi);

			hi->run();
		}
	}
	else
		MessageDialog::exec("dialog-warning", tr("Import external profile..."), tr("Unable to import profile: %1").arg(importer.errorMessage()));

	QDialog::accept();
}
