/*
 * %kadu copyright begin%
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

#include <QtGui/QAction>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QVBoxLayout>
#ifdef Q_WS_MAEMO_5
#include <QtGui/QScrollArea>
#endif

#include "accounts/account-manager.h"
#include "accounts/model/accounts-model.h"
#include "gui/widgets/account-add-widget.h"
#include "gui/widgets/account-create-widget.h"
#include "gui/widgets/account-edit-widget.h"
#include "gui/widgets/modal-configuration-widget.h"
#include "gui/widgets/protocols-combo-box.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "protocols/filter/can-register-protocol-filter.h"

#include "activate.h"
#include "icons-manager.h"

#include "multilogon-window.h"

MultilogonWindow *MultilogonWindow::Instance = 0;

MultilogonWindow::MultilogonWindow(QWidget *parent) :
		QWidget(parent)
{
	setWindowRole("kadu-multilogon");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Multilogon window"));

	createGui();

	loadWindowGeometry(this, "General", "MultilogonWindowGeometry", 0, 50, 700, 500);
}

MultilogonWindow::~MultilogonWindow()
{
	saveWindowGeometry(this, "General", "MultilogonWindowGeometry");

	Instance = 0;
}

MultilogonWindow * MultilogonWindow::instance()
{
	if (!Instance)
		Instance = new MultilogonWindow();

	return Instance;
}

void MultilogonWindow::show()
{
	QWidget::show();

	_activateWindow(this);
}

void MultilogonWindow::createGui()
{
}

void MultilogonWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}
