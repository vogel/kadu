/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <QtXml/QDomElement>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDialogButtonBox>

#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-widget.h"
#include "gui/widgets/configuration/config-section.h"
#include "gui/widgets/configuration/config-line-edit.h"
#include "gui/widgets/configuration/config-gg-password-edit.h"
#include "gui/widgets/configuration/config-check-box.h"
#include "gui/widgets/configuration/config-spin-box.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-hot-key-edit.h"
#include "gui/widgets/configuration/config-path-list-edit.h"
#include "gui/widgets/configuration/config-color-button.h"
#include "gui/widgets/configuration/config-select-font.h"
#include "gui/widgets/configuration/config-syntax-editor.h"
#include "gui/widgets/configuration/config-action-button.h"
#include "gui/widgets/configuration/config-select-file.h"
#include "gui/widgets/configuration/config-preview.h"
#include "gui/widgets/configuration/config-slider.h"
#include "gui/widgets/configuration/config-label.h"
#include "gui/widgets/configuration/config-list-widget.h"
#include "gui/widgets/configuration/config-manage-accounts.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/configuration-window.h"

#include "kadu.h"

ConfigurationWindow::ConfigurationWindow(const QString &name, const QString &caption, ConfigurationWindowDataManager *dataManager)
	: QDialog(kadu, Qt::Window), Name(name)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(caption);

	QVBoxLayout *main_layout = new QVBoxLayout(this);

	configurationWidget = new ConfigurationWidget(dataManager, this);

	QDialogButtonBox *buttons_layout = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Ok"), this);
	buttons_layout->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *applyButton = new QPushButton(icons_manager->loadIcon("ApplyWindowButton"), tr("Apply"), this);
	buttons_layout->addButton(applyButton, QDialogButtonBox::ApplyRole);
	QPushButton *cancelButton = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	buttons_layout->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked()), this, SLOT(updateAndCloseConfig()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(updateConfig()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	main_layout->addWidget(configurationWidget);
	main_layout->addWidget(buttons_layout);
}

ConfigurationWindow::~ConfigurationWindow()
{
}

void ConfigurationWindow::show()
{
	if (!isVisible())
  	{
		widget()->init();
		widget()->loadConfiguration();
		QWidget::show();
	}
	else
	{
		activateWindow();
		raise();
	}
}

void ConfigurationWindow::updateAndCloseConfig()
{
	updateConfig();

	accept();
	close();
}

void ConfigurationWindow::updateConfig()
{
	emit configurationWindowApplied();
	configurationWidget->saveConfiguration();

	ConfigurationAwareObject::notifyAll();
}

void ConfigurationWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QDialog::keyPressEvent(e);
}
