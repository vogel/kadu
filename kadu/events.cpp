/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qwidget.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qstring.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qobject.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>

#include "kadu.h"
#include "message_box.h"
#include "ignore.h"
#include "events.h"
#include "chat.h"
#include "history.h"
#include "pending_msgs.h"
#include "hints.h"
#include "debug.h"
#include "dcc.h"
#include "config_dialog.h"
#include "config_file.h"
#include "gadu.h"
#include "status.h"

AutoConnectionTimer *AutoConnectionTimer::autoconnection_object = NULL;
ConnectionTimeoutTimer *ConnectionTimeoutTimer::connectiontimeout_object = NULL;

AutoConnectionTimer::AutoConnectionTimer(QObject *parent, const char *name) : QTimer(parent, name) {
	connect(this, SIGNAL(timeout()), SLOT(doConnect()));
	start(1000, TRUE);
}

void AutoConnectionTimer::doConnect() {
	kadu->setStatus(loginparams.status & (~GG_STATUS_FRIENDS_MASK));
}

void AutoConnectionTimer::on() {
	if (!autoconnection_object)
		autoconnection_object = new AutoConnectionTimer(kadu, "auto_connection_timer_object");
}

void AutoConnectionTimer::off() {
	if (autoconnection_object) {
		delete autoconnection_object;
		autoconnection_object = NULL;
	}
}

ConnectionTimeoutTimer::ConnectionTimeoutTimer(QObject *parent, const char *name) : QTimer(parent, name) {
	start(5000, TRUE);
}

bool ConnectionTimeoutTimer::connectTimeoutRoutine(const QObject *receiver, const char *member) {
	return connect(connectiontimeout_object, SIGNAL(timeout()), receiver, member);
}

void ConnectionTimeoutTimer::on() {
	if (!connectiontimeout_object)
		connectiontimeout_object = new ConnectionTimeoutTimer(kadu, "connection_timeout_timer_object");
}

void ConnectionTimeoutTimer::off() {
	if (connectiontimeout_object) {
		delete connectiontimeout_object;
		connectiontimeout_object = NULL;
	}
}

EventConfigSlots::EventConfigSlots(QObject *parent, const char *name) : QObject(parent, name)
{
}

void EventConfigSlots::initModule()
{
	kdebugf();

	EventConfigSlots *eventconfigslots = new EventConfigSlots(gadu, "eventconfigslots");

// zakladka "powiadom"
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Notify"));
	ConfigDialog::addCheckBox("Notify", "Notify", QT_TRANSLATE_NOOP("@default", "Notify when users become available"), "NotifyStatusChange", false);
	ConfigDialog::addCheckBox("Notify", "Notify", QT_TRANSLATE_NOOP("@default", "Notify about all users"), "NotifyAboutAll", false);
	ConfigDialog::addGrid("Notify", "Notify" ,"listboxy",3);

	ConfigDialog::addGrid("Notify", "listboxy", "listbox1", 1);
	ConfigDialog::addLabel("Notify", "listbox1", QT_TRANSLATE_NOOP("@default", "Available"));
	ConfigDialog::addListBox("Notify", "listbox1","available");

	ConfigDialog::addGrid("Notify", "listboxy", "listbox2", 1);
	ConfigDialog::addPushButton("Notify", "listbox2", "", "AddToNotifyList","","forward");
	ConfigDialog::addPushButton("Notify", "listbox2", "", "RemoveFromNotifyList","","back");

	ConfigDialog::addGrid("Notify", "listboxy", "listbox3", 1);
	ConfigDialog::addLabel("Notify", "listbox3", QT_TRANSLATE_NOOP("@default", "Tracked"));
	ConfigDialog::addListBox("Notify", "listbox3", "track");

	ConfigDialog::addVGroupBox("Notify", "Notify", QT_TRANSLATE_NOOP("@default", "Notify options"));
	ConfigDialog::addCheckBox("Notify", "Notify options", QT_TRANSLATE_NOOP("@default", "Notify by dialog box"), "NotifyWithDialogBox", false);

//zakladka "siec"
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Network"));
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC enabled"), "AllowDCC", false);
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC IP autodetection"), "DccIpDetect", false);

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC IP"));
	ConfigDialog::addLineEdit("Network", "DCC IP", QT_TRANSLATE_NOOP("@default", "IP address:"),"DccIP");
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC forwarding enabled"), "DccForwarding", false);

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC forwarding properties"));
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "External IP address:"), "ExternalIP");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "External TCP port:"), "ExternalPort", "0");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "Local TCP port:"), "LocalPort", "1550");

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "Servers properties"));
	ConfigDialog::addGrid("Network", "Servers properties", "servergrid", 2);
	ConfigDialog::addCheckBox("Network", "servergrid", QT_TRANSLATE_NOOP("@default", "Use default servers"), "isDefServers", true);
#ifdef HAVE_OPENSSL
	ConfigDialog::addCheckBox("Network", "servergrid", QT_TRANSLATE_NOOP("@default", "Use TLSv1"), "UseTLS", false);
#endif
	ConfigDialog::addLineEdit("Network", "Servers properties", QT_TRANSLATE_NOOP("@default", "IP addresses:"), "Server","","","server");
#ifdef HAVE_OPENSSL
	ConfigDialog::addComboBox("Network", "Servers properties", QT_TRANSLATE_NOOP("@default", "Default port to connect to servers"));
#endif
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "Use proxy server"), "UseProxy", false);

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "Proxy server"));
	ConfigDialog::addGrid("Network", "Proxy server", "proxygrid", 2);
	ConfigDialog::addLineEdit("Network", "proxygrid", "IP address:", "ProxyHost", "","","proxyhost");
	ConfigDialog::addLineEdit("Network", "proxygrid", QT_TRANSLATE_NOOP("@default", "Port:"), "ProxyPort", "0");
	ConfigDialog::addLineEdit("Network", "proxygrid", QT_TRANSLATE_NOOP("@default", "Username:"), "ProxyUser");
	ConfigDialog::addLineEdit("Network", "proxygrid", QT_TRANSLATE_NOOP("@default", "Password:"), "ProxyPassword");

	ConfigDialog::registerSlotOnCreate(eventconfigslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(eventconfigslots, SLOT(onDestroyConfigDialog()));

	ConfigDialog::connectSlot("Network", "DCC enabled", SIGNAL(toggled(bool)), eventconfigslots, SLOT(ifDccEnabled(bool)));
	ConfigDialog::connectSlot("Network", "DCC IP autodetection", SIGNAL(toggled(bool)), eventconfigslots, SLOT(ifDccIpEnabled(bool)));
	ConfigDialog::connectSlot("Network", "Use default servers", SIGNAL(toggled(bool)), eventconfigslots, SLOT(ifDefServerEnabled(bool)));
#ifdef HAVE_OPENSSL
	ConfigDialog::connectSlot("Network", "Use TLSv1", SIGNAL(toggled(bool)), eventconfigslots, SLOT(useTlsEnabled(bool)));
#endif

	defaultdescriptions = QStringList::split(QRegExp("<-->"), config_file.readEntry("General","DefaultDescription", tr("I am busy.")), true);
	if (!config_file.readBoolEntry("Network","DccIpDetect"))
		if (!config_dccip.setAddress(config_file.readEntry("Network","DccIP", "")))
			config_dccip.setAddress((unsigned int)0);

	if (!config_extip.setAddress(config_file.readEntry("Network","ExternalIP", "")))
		config_extip.setAddress((unsigned int)0);

	QStringList servers;
	QHostAddress ip2;
	servers = QStringList::split(";", config_file.readEntry("Network","Server", ""));
	config_servers.clear();
	for (unsigned int i = 0; i < servers.count(); i++)
	{
		if (ip2.setAddress(servers[i]))
			config_servers.append(ip2);
	}
	server_nr = 0;


	ConfigDialog::connectSlot("Notify", "", SIGNAL(clicked()), eventconfigslots, SLOT(_Right()), "forward");
	ConfigDialog::connectSlot("Notify", "", SIGNAL(clicked()), eventconfigslots, SLOT(_Left()), "back");
	ConfigDialog::connectSlot("Notify", "available", SIGNAL(doubleClicked(QListBoxItem *)), eventconfigslots, SLOT(_Right2(QListBoxItem *)));
	ConfigDialog::connectSlot("Notify", "track", SIGNAL(doubleClicked(QListBoxItem *)), eventconfigslots, SLOT(_Left2(QListBoxItem *)));
	kdebugf2();
}

void EventConfigSlots::onCreateConfigDialog()
{
	kdebugf();

	QCheckBox *b_dccenabled = ConfigDialog::getCheckBox("Network", "DCC enabled");
	QCheckBox *b_dccip= ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QVGroupBox *g_proxy = ConfigDialog::getVGroupBox("Network", "Proxy server");
	QVGroupBox *g_fwdprop = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");
	QCheckBox *b_dccfwd = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");
	QCheckBox *b_useproxy= ConfigDialog::getCheckBox("Network", "Use proxy server");

#ifdef HAVE_OPENSSL
	QCheckBox *b_tls= ConfigDialog::getCheckBox("Network", "Use TLSv1");
	QComboBox *cb_portselect= ConfigDialog::getComboBox("Network", "Default port to connect to servers");
#endif

	QHBox *serverbox=(QHBox*)(ConfigDialog::getLineEdit("Network", "IP addresses:","server")->parent());
	QCheckBox* b_defaultserver= ConfigDialog::getCheckBox("Network", "Use default servers");

	b_dccip->setEnabled(b_dccenabled->isChecked());
	g_dccip->setEnabled(!b_dccip->isChecked()&& b_dccenabled->isChecked());
	b_dccfwd->setEnabled(b_dccenabled->isChecked());
	g_fwdprop->setEnabled(b_dccenabled->isChecked() && b_dccfwd->isChecked());
	g_proxy->setEnabled(b_useproxy->isChecked());

#ifdef HAVE_OPENSSL
	((QHBox*)cb_portselect->parent())->setEnabled(!b_tls->isChecked());
	cb_portselect->insertItem("8074");
	cb_portselect->insertItem("443");
	cb_portselect->setCurrentText(config_file.readEntry("Network", "DefaultPort", "8074"));
#endif
	serverbox->setEnabled(!b_defaultserver->isChecked());

	connect(b_dccfwd, SIGNAL(toggled(bool)), g_fwdprop, SLOT(setEnabled(bool)));
	connect(b_useproxy, SIGNAL(toggled(bool)), g_proxy, SLOT(setEnabled(bool)));

// notify

	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");
	for (UserList::ConstIterator i = userlist.begin(); i != userlist.end(); i++)
	{
		if ((*i).uin)
			if (!(*i).notify)
				e_availusers->insertItem((*i).altnick);
			else
				e_notifies->insertItem((*i).altnick);
	}

	e_availusers->sort();
	e_notifies->sort();
	e_availusers->setSelectionMode(QListBox::Extended);
	e_notifies->setSelectionMode(QListBox::Extended);

	QCheckBox *b_notifyglobal= ConfigDialog::getCheckBox("Notify", "Notify when users become available");
	QCheckBox *b_notifyall= ConfigDialog::getCheckBox("Notify", "Notify about all users");
	QVGroupBox *notifybox= ConfigDialog::getVGroupBox("Notify", "Notify options");
	QGrid *panebox = ConfigDialog::getGrid("Notify","listboxy");

	if (config_file.readBoolEntry("Notify", "NotifyAboutAll"))
		panebox->setEnabled(false);

	if (!config_file.readBoolEntry("Notify", "NotifyStatusChange"))
		{
		b_notifyall->setEnabled(false);
		panebox->setEnabled(false);
		notifybox->setEnabled(false);
		}

	QObject::connect(b_notifyall, SIGNAL(toggled(bool)), this, SLOT(ifNotifyAll(bool)));
	QObject::connect(b_notifyglobal, SIGNAL(toggled(bool)), this, SLOT(ifNotifyGlobal(bool)));

	kdebugf2();
}

void EventConfigSlots::onDestroyConfigDialog()
{
	kdebugf();

#ifdef HAVE_OPENSSL
	QComboBox *cb_portselect=ConfigDialog::getComboBox("Network", "Default port to connect to servers");
	config_file.writeEntry("Network","DefaultPort",cb_portselect->currentText());
#endif
	QLineEdit *e_servers=ConfigDialog::getLineEdit("Network", "IP addresses:", "server");

	QStringList tmpservers,server;
	QValueList<QHostAddress> servers;
	QHostAddress ip;
	bool ipok;
	unsigned int i;

	tmpservers = QStringList::split(";", e_servers->text());
	for (i = 0; i < tmpservers.count(); i++)
	{
		ipok = ip.setAddress(tmpservers[i]);
		if (!ipok)
			break;
		servers.append(ip);
		server.append(ip.toString());
	}
	config_file.writeEntry("Network","Server",server.join(";"));
	config_servers=servers;
	server_nr = 0;

	if (!config_dccip.setAddress(config_file.readEntry("Network","DccIP")))
	{
		config_file.writeEntry("Network","DccIP","0.0.0.0");
		config_dccip.setAddress((unsigned int)0);
	}

	if (!config_extip.setAddress(config_file.readEntry("Network","ExternalIP")))
	{
		config_file.writeEntry("Network","ExternalIP","0.0.0.0");
		config_extip.setAddress((unsigned int)0);
	}

	if (config_file.readNumEntry("Network","ExternalPort")<=1023)
		config_file.writeEntry("Network","ExternalPort",0);

	if (!ip.setAddress(config_file.readEntry("Network","ProxyHost")))
		config_file.writeEntry("Network","ProxyHost","0.0.0.0");

	if (config_file.readNumEntry("Network","ProxyPort")<=1023)
		config_file.writeEntry("Network","ProxyPort",0);


	//notify
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");

	QString tmp;
	for (i = 0; i < e_notifies->count(); i++) {
		tmp = e_notifies->text(i);
		userlist.byAltNick(tmp).notify = true;
		}
	for (i = 0; i < e_availusers->count(); i++) {
		tmp = e_availusers->text(i);
		userlist.byAltNick(tmp).notify = false;
		}

	/* and now, save it */
	userlist.writeToFile();
	//
	kdebugf2();
}

void EventConfigSlots::ifNotifyGlobal(bool toggled) {
	QCheckBox *b_notifyall= ConfigDialog::getCheckBox("Notify", "Notify about all users");
	QVGroupBox *notifybox= ConfigDialog::getVGroupBox("Notify", "Notify options");
	QGrid *panebox = ConfigDialog::getGrid("Notify","listboxy");

	b_notifyall->setEnabled(toggled);
	panebox->setEnabled(toggled && !b_notifyall->isChecked());
	notifybox->setEnabled(toggled);
}

void EventConfigSlots::ifNotifyAll(bool toggled) {
	QGrid *panebox = ConfigDialog::getGrid("Notify","listboxy");
	panebox->setEnabled(!toggled);
}

void EventConfigSlots::_Left2( QListBoxItem *item) {
	_Left();
}

void EventConfigSlots::_Right2( QListBoxItem *item) {
	_Right();
}


void EventConfigSlots::_Left(void) {
	kdebugf();
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");
	QStringList tomove;
	unsigned int i;

	for(i=0;i<e_notifies->count();i++){
		if (e_notifies->isSelected(i))
			tomove+=e_notifies->text(i);
	}

	for(i=0;i<tomove.size();i++){
		e_availusers->insertItem(tomove[i]);
		e_notifies->removeItem(e_notifies->index(e_notifies->findItem(tomove[i])));
	}

	e_availusers->sort();
	kdebugf2();
}

void EventConfigSlots::_Right(void) {
	kdebugf();
	QListBox *e_availusers= ConfigDialog::getListBox("Notify", "available");
	QListBox *e_notifies= ConfigDialog::getListBox("Notify", "track");
	QStringList tomove;
	unsigned int i;

	for(i=0;i<e_availusers->count();i++){
		if (e_availusers->isSelected(i))
			tomove+=e_availusers->text(i);
	}

	for(i=0;i<tomove.size();i++){
		e_notifies->insertItem(tomove[i]);
		e_availusers->removeItem(e_availusers->index(e_availusers->findItem(tomove[i])));
	}

	e_notifies->sort();
	kdebugf2();
}


void EventConfigSlots::ifDccEnabled(bool value)
{
	kdebugf();

	QCheckBox *b_dccip= ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QVGroupBox *g_fwdprop = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");
	QCheckBox *b_dccfwd = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");

	b_dccip->setEnabled(value);
	g_dccip->setEnabled(!b_dccip->isChecked()&& value);
	b_dccfwd->setEnabled(value);
	g_fwdprop->setEnabled(b_dccfwd->isChecked() &&value);
	kdebugf2();
}

void EventConfigSlots::ifDccIpEnabled(bool value)
{
	kdebugf();
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	g_dccip->setEnabled(!value);
	kdebugf2();
}

void EventConfigSlots::ifDefServerEnabled(bool value)
{
	kdebugf();
	QHBox *serverbox=(QHBox*)(ConfigDialog::getLineEdit("Network", "IP addresses:","server")->parent());
	serverbox->setEnabled(!value);
	kdebugf2();
}

void EventConfigSlots::useTlsEnabled(bool value)
{
#ifdef HAVE_OPENSSL
	kdebugf();
	QHBox *box_portselect=(QHBox*)(ConfigDialog::getComboBox("Network", "Default port to connect to servers")->parent());
	box_portselect->setEnabled(!value);
	kdebugf2();
#endif
}

