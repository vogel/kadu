/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#include "debug.h"

#include "gui/windows/message-dialog.h"
#include "jabber-server-register-account.h"

JabberServerRegisterAccount::JabberServerRegisterAccount(const QString &server, const QString &username, const QString &password, bool legacySSLProbe, bool legacySSL, bool forceSSL, const QString &host, quint16 port)
	: QObject(), Result(0), Server(server), Username(username), Password(password)
{
	Client = new MiniClient;
	connect(Client, SIGNAL(handshaken()), SLOT(clientHandshaken()));
	connect(Client, SIGNAL(error()), SLOT(clientError()));
	Client->connectToServer(XMPP::Jid(Server), legacySSLProbe, legacySSL, forceSSL, host, port);
}

void JabberServerRegisterAccount::clientHandshaken()
{
	kdebugf();
	// try to register an account
	XMPP::JT_Register *reg = new XMPP::JT_Register(Client->client()->rootTask());
	connect(reg, SIGNAL(finished()), SLOT(sendRegistrationData()));
	reg->getForm(Server);
	reg->go(true);
	kdebugf();
}

void JabberServerRegisterAccount::clientError()
{
	kdebugf();
	Result = false;
	emit finished(this);
	kdebugf2();
}

void JabberServerRegisterAccount::sendRegistrationData()
{
	kdebugf();
	XMPP::JT_Register *reg = (XMPP::JT_Register *)sender();
	if (reg->success()) {
		XMPP::XData xdata;
	//TODO: upewnić się, że to to jest potrzebne tak jak jest
		if (reg->hasXData()) {
			isOld_ = false;
			xdata = reg->xdata();
		}
		else {
			isOld_ = true;
			xdata = convertToXData(reg->form());
		}
	//
		XMPP::JT_Register *reg = new XMPP::JT_Register(Client->client()->rootTask());
		connect(reg, SIGNAL(finished()), this, SLOT(actionFinished()));
		if (isOld_) {
			XMPP::Form form = convertFromXData(fields);
			form.setJid(Server);
			reg->setForm(form);
		}
		else {
			reg->setForm(Server,fields);
		}
		reg->go(true);

	}
	else {
		Result = false;
		emit finished(this);
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("This server does not support registration"));
	}
	kdebugf2();
}

XMPP::XData JabberServerRegisterAccount::convertToXData(const XMPP::Form& form)
{
	kdebugf();
	// Convert the fields
	XMPP::XData::FieldList fields;
	foreach (const XMPP::FormField &f, form) {
		XMPP::XData::Field field;
		field.setLabel(f.fieldName());
		field.setVar(f.realName());
		field.setRequired(true);
		if (f.isSecret()) {
			field.setType(XMPP::XData::Field::Field_TextPrivate);
		}
		else {
			field.setType(XMPP::XData::Field::Field_TextSingle);
		}
		fields.push_back(field);
	}

	// Create the form
	XMPP::XData xdata;
	xdata.setInstructions(form.instructions());
	xdata.setFields(fields);
	kdebugf2();
	return xdata;
}

XMPP::Form JabberServerRegisterAccount::convertFromXData(const XMPP::XData& xdata)
{
	kdebugf();
	XMPP::Form form;
	foreach(const XMPP::XData::Field &field, xdata.fields()) {
		if (!field.value().isEmpty()) {
			XMPP::FormField f;
			f.setType(field.var());
			f.setValue(field.value().at(0));
			form.push_back(f);
		}
	}
	kdebugf2();
	return form;
}

void JabberServerRegisterAccount::actionFinished()
{
	kdebugf();
	XMPP::JT_Register *reg = (XMPP::JT_Register *)sender();
	if (reg->success()) {
		Client->close();
		Result = true;
		emit finished(this);
	}
	else {
		Result = false;
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("There was an error registering the account.\nReason: %1").arg(reg->statusString()));
		emit finished(this);
	}
	kdebugf2();
}

void JabberServerRegisterAccount::performAction()
{
	kdebugf();
	XMPP::XData::FieldList fs;

	XMPP::XData::Field username;
	username.setLabel("Username");
	username.setVar("username");
	username.setValue(QStringList(Username));
	username.setRequired(true);
	username.setType(XMPP::XData::Field::Field_TextSingle);
	fs.push_back(username);

	Jid = XMPP::Jid(Username, Server).bare();

	XMPP::XData::Field pass;
	pass.setLabel("password");
	pass.setVar("password");
	pass.setValue(QStringList(Password));
	pass.setRequired(true);
	pass.setType(XMPP::XData::Field::Field_TextPrivate);
	fs.push_back(pass);

	fields.setFields(fs);
	kdebugf2();
}

