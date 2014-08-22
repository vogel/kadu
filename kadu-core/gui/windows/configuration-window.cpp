/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtXml/QDomElement>

#include "configuration/config-file-variant-wrapper.h"
#include "configuration/configuration-manager.h"
#include "core/core.h"
#include "gui/widgets/configuration/config-action-button.h"
#include "gui/widgets/configuration/config-check-box.h"
#include "gui/widgets/configuration/config-color-button.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-gg-password-edit.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-hot-key-edit.h"
#include "gui/widgets/configuration/config-label.h"
#include "gui/widgets/configuration/config-line-edit.h"
#include "gui/widgets/configuration/config-line-separator.h"
#include "gui/widgets/configuration/config-list-widget.h"
#include "gui/widgets/configuration/config-path-list-edit.h"
#include "gui/widgets/configuration/config-preview.h"
#include "gui/widgets/configuration/config-section.h"
#include "gui/widgets/configuration/config-select-file.h"
#include "gui/widgets/configuration/config-select-font.h"
#include "gui/widgets/configuration/config-slider.h"
#include "gui/widgets/configuration/config-spin-box.h"
#include "gui/widgets/configuration/config-syntax-editor.h"
#include "gui/widgets/configuration/config-widget.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/configuration-window.h"
#include "gui/windows/kadu-window.h"
#include "os/generic/window-geometry-manager.h"

#include "activate.h"

ConfigurationWindow::ConfigurationWindow(const QString &name, const QString &caption, const QString &section, ConfigurationWindowDataManager *dataManager)
	: QDialog(Core::instance()->kaduWindow(), Qt::Window), DesktopAwareObject(this),  Name(name), Section(section)
{
	setWindowRole("kadu-configuration");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(caption);

	QVBoxLayout *main_layout = new QVBoxLayout(this);

	configurationWidget = new ConfigurationWidget(dataManager, this);

	QDialogButtonBox *buttons_layout = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), this);
	buttons_layout->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *applyButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Apply"), this);
	buttons_layout->addButton(applyButton, QDialogButtonBox::ApplyRole);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	buttons_layout->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(updateAndCloseConfig()));
	connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(updateConfig()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	main_layout->addWidget(configurationWidget);
	main_layout->addSpacing(16);
	main_layout->addWidget(buttons_layout);

	new WindowGeometryManager(new ConfigFileVariantWrapper(section, name + "_Geometry"), QRect(0, 50, 790, 580), this);
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
		_activateWindow(this);
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

	emit configurationSaved();
	ConfigurationAwareObject::notifyAll();

	ConfigurationManager::instance()->flush();
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

#include "moc_configuration-window.cpp"
