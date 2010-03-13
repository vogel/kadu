/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SMS_DIALOG_H
#define SMS_DIALOG_H

#include <QtCore/QBuffer>
#include <QtCore/QMap>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtNetwork/QHttp>

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "http_client.h"
#include "gui/actions/action.h"
#include "gui/windows/main-configuration-window.h"

#include "sms_exports.h"
#include "sms-sender.h"
#include "sms-gateway-manager.h"
#include "mobile-number-manager.h"

class QCheckBox;
class QComboBox;
class QListWidget;
class QProcess;
class QTextEdit;

class SmsDialog : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

	QTextEdit *body;
	QLineEdit *recipient;
	QComboBox *list;
	QLabel *smslen;
	QLineEdit *e_contact;
	QLineEdit *e_signature;
	QPushButton *b_send;
	QCheckBox *c_saveInHistory;
	QProcess *smsProcess;
	QComboBox *ProvidersList;

	void createGui();

private slots:
	void updateRecipient(const QString &);
	void updateList(const QString &newnumber);
	void editReturnPressed();
	void sendSms();
	void updateCounter();
	void smsSigHandler();
	void onSmsSenderFinished(bool success);

protected:
	virtual void configurationUpdated();
	virtual void keyPressEvent(QKeyEvent *e);

public:
	explicit SmsDialog(const QString &altnick, QWidget *parent = 0);
	virtual ~SmsDialog();
	void setRecipient(const QString &phone);

};

/** @} */

#endif // SMS_DIALOG_H
