/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qmessagebox.h>
#include <qdialog.h>

#include "remind_password.h"
#include "debug.h"
#include "config_file.h"
#include "misc.h"
#include "gadu.h"

RemindPassword::RemindPassword() {
	connect(gadu, SIGNAL(reminded(bool)), this, SLOT(reminded(bool)));
}

RemindPassword::~RemindPassword() {
}

void RemindPassword::start() {
	kdebugf();

	TokenDialog *tokendialog = new TokenDialog();
	if (tokendialog->exec() != QDialog::Accepted)
	{
		delete tokendialog;
		return;
	}

	QString Tokenid, Tokenval;
	tokendialog->getToken(Tokenid, Tokenval);
	delete tokendialog;

	gadu->doRemind(config_file.readNumEntry("General", "UIN"), Tokenid, Tokenval);
	kdebugf2();
}

void RemindPassword::reminded(bool ok)
{
	kdebugf();
	if (ok)
		QMessageBox::information(0, tr("Remind password"),
				tr("Password reminded"), tr("OK"), 0, 0, 1);
	else
		QMessageBox::information(0, tr("Remind password"),
				tr("Error during remind password"), tr("OK"), 0, 0, 1);

	deleteLater();
	kdebugf2();
}
