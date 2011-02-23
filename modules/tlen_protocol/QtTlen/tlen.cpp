/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QTextCodec>
#include <QRegExp>
#include <QAction>
#include <QSettings>
#include <QXmlSimpleReader>
#include <QDebug>

#include <QHostAddress>
#include <QNetworkAccessManager>

#include <time.h>

#include "tlen.h"
#include "auth.h"
//#include "settings.h"
//#include "fileincoming.h"

#include "debug.h"

tlen* Tlen=0;

tlen::tlen( QObject* parent ): QObject( parent ) {
	state = tlen::Disconnected;

	hostname = "s1.tlen.pl";
	hostport = 443;
	Secure = false;
	Reconnect = false;

	Status= tlen::unavailable;
	Descr="";

	tmpDoc=new QDomDocument;

	socket=new QTcpSocket();
	ping=new QTimer();

	connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
	connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));

	connect(this, SIGNAL(tlenLoggedIn()), this, SLOT(writeStatus()));
	connect(this, SIGNAL(statusUpdate()), this, SLOT(writeStatus()));

	connect(this, SIGNAL(eventReceived(QDomNode)), this, SLOT(event(QDomNode)));

	connect(ping, SIGNAL(timeout()), this, SLOT(sendPing()));
	srand(time(NULL));
}

tlen::~tlen()
{
	if (ping)
	{
		delete ping;
		ping = 0;
	}

	if (socket)
	{
		socket->abort();
		delete socket;
		socket = 0;
	}

	if (tmpDoc)
	{
		delete tmpDoc;
		tmpDoc = 0;
	}
}

void tlen::openConn() {
	kdebugf();

	if (isConnecting() || User.isEmpty() || Password.isEmpty())
		return;

	state=tlen::ConnectingToHub;
	socket->connectToHost(hostname, hostport);
}

void tlen::closeConn() {
	kdebugf();
	socket->abort();
	state=tlen::Disconnected;
	// clear tcfg
	MiniMailBase = "";
	// clear token
	token = "";
}

bool tlen::isConnected() {
	return state == tlen::Connected;
}

bool tlen::isConnecting() {
	switch(state) {
	case tlen::ConnectingToHub:
	case tlen::Connecting:
		return true;
	break;

	default:
		return false;
	break;
	}
	return false;
}

bool tlen::isDisconnected() {
	return state == tlen::Disconnected;
}

void tlen::socketReadyRead() {
	kdebugf();
	stream+=socket->readAll();
	stream.prepend("<xmlroot>");
	stream.append("</xmlroot>");

	if( tmpDoc->setContent(stream) || stream.startsWith("<xmlroot><s ") ) {
		qDebug()<<"Read:"<<tmpDoc->toString();

		QDomDocument d;
		d.setContent(stream);
		QDomNode root=d.firstChild();

		if(root.hasChildNodes()) {
			QDomNodeList sl=root.childNodes();
			for(int i=0; i<sl.count(); i++)
				emit eventReceived(sl.item(i));
		}
		tmpDoc->clear();
		stream.clear();
	} else {
		stream.replace("<xmlroot>", "");
		stream.replace("</xmlroot>","");
	}
}

QString tlen::localAddress() {
	kdebugf();
	if( socket->isOpen() )
		return socket->localAddress().toString();

	return QString();
}

void tlen::socketConnected() {
	kdebugf();
	state = tlen::Connecting;
	socket->write( QByteArray(
	  isSecureConn()
	  ?"<s s=\"1\" v=\"15\" t=\"07000000\">"
	  :"<s v=\"15\" t=\"07000000\">") );
//	  ?"<s s=\"1\" v=\"9\" t=\"0600010C\">"
//	  :"<s v=\"9\" t=\"0600010C\">") );
}

void tlen::event(const QDomNode &n) {
	kdebugf();
	QString nodeName=n.nodeName();
	QDomElement element = n.toElement();
	if(nodeName=="s" && element.hasAttribute("i")) {
		ping->start( 50000 );
		sid = element.attribute("i");
		if(tlenLogin())
 			state = tlen::Connected;
		else
			socket->close(); // TODO: Dont close connection and repeat login, after n times disconnect, signal
	}
	else if(nodeName=="iq") {
		if(element.hasAttribute( "type" ) && element.attribute("type") == "result") {
			// tcfg
			if(element.hasAttribute("id") && element.attribute("id")==sid) {
				tcfgRequest();
				rosterRequest();
			}
			if(element.hasAttribute("from") && element.attribute("from")=="tuba" && element.hasAttribute("id")){
				// <iq from="tuba" type="result" to="jid" id="tr">
				// <query xmlns="jabber:iq:register">
				// <item></item></query></iq>
				if(element.attribute("id")=="tr") {
					QDomElement query = element.elementsByTagName("query").item(0).toElement();
					//if (query.hasAttribute("xmlns") && element.attribute("xmlns")=="jabber:iq:register")
					emit pubdirReceived(query.childNodes());
					return;
				}
				if(element.attribute("id")=="tw") {
					//if (query.hasAttribute("xmlns") && element.attribute("xmlns")=="jabber:iq:register")
					// TODO implement unsuccess if timeout
					emit pubdirUpdated(true);
					return;
				}
			}
			if(element.hasAttribute("id") && element.attribute("id")=="GetRoster") {
				emit clearRosterView();
			}
			if(element.hasAttribute("from") && element.attribute("from") == "tcfg") {
				tcfgReceived(element);
				return;
			}

			if(n.hasChildNodes()) {
				QDomNodeList el=n.childNodes();
				for(int i=0;i<el.count();++i)
					emit eventReceived(el.item(i));
			}

			if(0) {
				emit tlenLoggedIn();
			}

		}
		else if(element.hasAttribute("type") && element.attribute("type") == "set") {
			if(n.hasChildNodes()) {
				QDomNodeList el=n.childNodes();
				for(int i=0;i<el.count();++i)
					emit eventReceived(el.item(i));
			}
		}
	}
	else if(nodeName=="query") {
		QDomElement e=n.toElement();
		QDomNodeList nl=n.childNodes();
		for(int i=0;i<nl.count();++i)
			event(nl.item(i));
	}
	else if(nodeName=="item") {
		QDomElement e=n.toElement();
		QString jid=decode(e.attribute("jid"));
		QString subscription=e.attribute("subscription");
		QString name=NULL, group=NULL;

		if(subscription=="remove") {
			emit removeItem(jid);
			return;
		}

		if(e.hasAttribute("ask"))
			subscription=e.attribute("ask");

		if(e.hasAttribute("name"))
			name=e.attribute("name");
		else
			name=jid;

		if( n.hasChildNodes() ) {
			QDomNodeList nl=n.childNodes();
			for(int i=0;i<nl.count();++i) {
				group=nl.item(i).firstChild().toText().data();
			}
		}

		if(group.isEmpty())
			group=tr("Contacts");

		group=decode(group.toUtf8());
		name=decode(name.toUtf8());

		emit itemReceived(jid, name, subscription, group);

	}
	else if(nodeName=="presence") {
		QDomElement e=n.toElement();
		QString from=decode(e.attribute("from"));

		if(e.hasAttribute("type") && e.attribute("type")=="subscribe") {
			emit authorizationAsk(from);
		}
		else if( e.hasAttribute("type") && e.attribute("type")=="subscribed" )
			return;
		else if(e.hasAttribute("type") && (e.attribute("type")=="unsubscribe" || e.attribute("type")=="unsubscribed"))
			return;
		else {
			QString status="none";
			QString descr="";

			if(e.hasAttribute("type"))
				status=e.attribute("type");

			QDomNodeList l=n.childNodes();

			for(int i=0; i<l.count(); ++i) {
				if(l.item(i).nodeName()=="show" && status=="none")
					status=l.item(i).firstChild().toText().data();
				if(l.item(i).nodeName()=="status")
					descr=l.item(i).firstChild().toText().data();
				if(l.item(i).nodeName()=="avatar")
				{
					// TODO store jid,type,md5 on list/qmultimap, check md5 - avatar changed
					QDomElement avatar = l.item(i).toElement().elementsByTagName("a").item(0).toElement();
					if (avatar.hasAttribute("type") && avatar.hasAttribute("md5"))
					{
						emit avatarReceived(decode(from), avatar.attribute("type"), avatar.attribute("md5"));
						qDebug() << "Avatar " << from << "type:" <<avatar.attribute("type") << "md5:" << avatar.attribute("md5");
					}
				}
			}

			descr=decode(descr.toUtf8());

			emit presenceChanged(from,status,descr);
		}
	}
	else if(nodeName=="message") {
		QDomElement e=n.toElement();
		if(e.hasAttribute("type") && e.attribute("type")=="chat")
			emit chatMsgReceived(n);
	}
	else if(nodeName=="m") {
		QDomElement e=n.toElement();
		if(e.hasAttribute("tp")) {
			emit chatNotify(decode(e.attribute("f")), e.attribute("tp"));
		}
	}
	else if(nodeName=="n") {
		//<n f='Rainer+Wiesenfarth+%3CRainer.Wiesenfarth@inpho.de%3E' s='Re%3A+qt+and+mysql,+odbc'/> - new mail
	}
	else if(nodeName=="avatar") {
		// search for token
		QDomElement e=n.toElement();
		QDomNodeList l=n.childNodes();
		for(int i=0; i<l.count(); ++i)
			if(l.item(i).nodeName()=="token")
				token=l.item(i).firstChild().toText().data();
	}
	else if(nodeName=="f") {
		QDomElement e=n.toElement();
		if(e.attribute("e")=="1") {
			//fileIncomingDialog *dlg=new fileIncomingDialog(n);
			//connect(dlg, SIGNAL(receive(QString,QString,bool)), this, SLOT(receiveFile(QString,QString,bool)));
			//dlg->show();
		}
		else if(e.attribute("e")=="5") {
			//if( fileTransferDialog *dlg = fTransferMap.value( QString("%1-%2").arg( e.attribute("f") ).arg( e.attribute("i") ) ) )
			//	dlg->fileThread()->transferingAccepted();
		}
		else if(e.attribute("e")=="6") {
			//fileTransferDialog *dlg=new fileTransferDialog( e.attribute("i").toInt(), e.attribute("f"), e.attribute("a"), (quint16)e.attribute("p").toInt(), TRUE);
			//fTransferMap.insert(QString("%1-%2").arg( e.attribute("f") ).arg( e.attribute("i") ), dlg);
			//dlg->show();
		}
		else if(e.attribute("e")=="7") {
			//if( fileTransferDialog *dlg = fTransferMap.value( QString("%1-%2").arg( e.attribute("f") ).arg( e.attribute("i") ) ) )
			//	dlg->fileThread()->switchToSocketMode( e.attribute("a"), (quint16)e.attribute("p").toInt() );
		}
	}
}

void tlen::socketDisconnected()
{
	kdebugf();

	state=tlen::Disconnected;
	ping->stop();

	if (Reconnect)
	{
		openConn();
	}
	else
	{
		Status=tlen::unavailable;
		Descr="";
	}
	emit presenceDisconnected();
	emit statusChanged();
}
// every 60s
void tlen::sendPing() {
	kdebugf();
	//socket->write(QString("  \t  ").toUtf8());
	// 7.00 sends one space
	socket->write(QString(" ").toUtf8());
}
// TODO
// "<iq type='set' sid='GetRoster'><query xmlns="jabber:iq:auth"/>
// </iq>"
bool tlen::tlenLogin() {
	kdebugf();
	if(!isConnecting()/*!isConnected()*/)
		return false;

	QDomDocument doc;

	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "type", "set" );
	iq.setAttribute( "id", sid );
	doc.appendChild( iq );

	QDomElement query = doc.createElement( "query" );
	query.setAttribute( "xmlns", "jabber:iq:auth" );
	iq.appendChild( query );

	query.appendChild(textNode("username", User.split("@")[0]));
	query.appendChild(textNode("digest", tlen_hash( Password.toAscii().data(), sid.toAscii().data() )));
	query.appendChild(textNode("resource", "w")); // t
	// w iq jeszcze  : <host>tlen.pl</host>
	return write(doc);
}

bool tlen::write( const QDomDocument &d ) {
	kdebugf();
	if( !(isConnected() || isConnecting()) ) {
		return FALSE;
	}

	qDebug()<<"Write:"<<d.toByteArray();

	return (socket->write(d.toByteArray()) == (qint64)d.toByteArray().size());
}
// "<iq type='get' id='GetRoster'><query xmlns="jabber:iq:roster"/></iq>"
// t7 : <iq type="get" id="PJTCPW" ><query xmlns="jabber:iq:roster"/></iq>
// t7 reply: <iq id="PJTCPW" type='result'>...
void tlen::rosterRequest() {
	kdebugf();
	QDomDocument doc;
	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "type", "get" );
	iq.setAttribute( "id", "GetRoster" );
	doc.appendChild( iq );
	QDomElement query = doc.createElement( "query" );
	query.setAttribute( "xmlns", "jabber:iq:roster" );
	iq.appendChild( query );
	write(doc);
}
// "<iq to='tcfg' type='get' id='TcfgGetAfterLoggedIn'></iq>"
// t7 : <iq type="get" to="tcfg" id="VHF1OX" ><query><t/></query></iq>
// t7 reply : <iq from='tcfg' to='user' id='VHF1OX' type='result'><query> ...
void tlen::tcfgRequest() {
	kdebugf();

	QDomDocument doc;
	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "to", "tcfg" );
	iq.setAttribute( "type", "get" );
	iq.setAttribute( "id", "TcfgGetAfterLoggedIn" );

	doc.appendChild( iq );
	write(doc);
}
// "<iq type="get" id="tr" to="tuba"><query xmlns="jabber:iq:register"/></iq>"
void tlen::getPubDirInfoRequest() {
	kdebugf();

	QDomDocument doc;
	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "to", "tuba" );
	iq.setAttribute( "type", "get" );
	iq.setAttribute( "id", "tr" );

	QDomElement query = doc.createElement( "query" );
	query.setAttribute( "xmlns", "jabber:iq:register" );
	iq.appendChild( query );

	doc.appendChild( iq );
	write(doc);
}
//<iq type="set" id="tw" to="tuba"><query xmlns="jabber:iq:register">...<nick>nick</nick>...</query></iq>
void tlen::setPubDirInfo(const QString &first, const QString &last, const QString &nick, const QString &email,
			 const QString &city, int birth, int sex, int lookingFor, int job,
			 int todayPlans, bool visible, bool mic, bool cam)
{
	kdebugf();

	QDomDocument doc;
	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "to", "tuba" );
	iq.setAttribute( "type", "set" );
	iq.setAttribute( "id", "tw" );

	QDomElement query = doc.createElement( "query" );
	query.setAttribute( "xmlns", "jabber:iq:register" );
	iq.appendChild( query );

	if (!first.isEmpty())
		query.appendChild(textNode("first", first));

	if (!last.isEmpty())
		query.appendChild(textNode("last", last));

	if (!nick.isEmpty())
		query.appendChild(textNode("nick", nick));

	if (!email.isEmpty())
		query.appendChild(textNode("email", QString(encode(email))));

	if (!city.isEmpty())
		query.appendChild(textNode("city", city));

	if (birth > 0)
		query.appendChild(textNode("b", QString::number(birth)));

	if (sex > 0)
		query.appendChild(textNode("s", QString::number(sex)));

	if (lookingFor > 0)
		query.appendChild(textNode("r", QString::number(lookingFor)));

	if (job > 0)
		query.appendChild(textNode("j", QString::number(job)));

	if (todayPlans > 0)
		query.appendChild(textNode("p", QString::number(todayPlans)));

	if (visible)
		query.appendChild(textNode("v","1"));

	if (mic)
		query.appendChild(textNode("g","1"));

	if (cam)
		query.appendChild(textNode("k","1"));

	doc.appendChild( iq );
	write(doc);
}

QDomElement tlen::textNode(const QString &name, const QString &text)
{
	  QDomDocument doc;
	  QDomElement tmp=doc.createElement(name);
	  QDomText t=doc.createTextNode(text);
	  tmp.appendChild(t);

	  return tmp;
}

// TODO!!
QString tlen::getTextNode(const QDomElement &n, const QString &name)
{
	Q_UNUSED(n)
	Q_UNUSED(name)

	return QString();
}

void tlen::tcfgReceived(const QDomElement &n)
{
	kdebugf();
	// parse tlen config
	QDomElement query = n.elementsByTagName("query").item(0).toElement();
	QDomElement minimail = query.elementsByTagName("mini-mail").item(0).toElement();
	QDomNodeList minimailChailds = minimail.childNodes();
	for (int i=0;i<minimailChailds.count();++i)
	{
		QDomElement mm = minimailChailds.item(i).toElement();
		QString mmName = minimailChailds.item(i).nodeName();
		if (mmName == "base")
		{
			MiniMailBase = mm.text();
		}
		else if (mmName == "msg")
		{
			MiniMailMsg = mm.text();
			MiniMailMsgMethod = mm.attribute("method");
		}
		else if (mmName == "index")
		{
			MiniMailIndex = mm.text();
			MiniMailIndexMethod = mm.attribute("method");
		}
		else if (mmName == "login")
		{
			MiniMailLogin = mm.text();
			MiniMailLoginMethod = mm.attribute("method");
		}
		else if (mmName == "logout")
		{
			MiniMailLogout = mm.text();
			MiniMailLogoutMethod = mm.attribute("method");
		}
		else if (mmName == "compose")
		{
			MiniMailCompose = mm.text();
			MiniMailComposeMethod = mm.attribute("method");
		}
		else if (mmName == "avatar-get")
		{
			MiniMailAvatarGet = mm.text();
			MiniMailAvatarGetMethod = mm.attribute("method");
		}
		else if (mmName == "avatar-upload")
		{
			MiniMailAvatarUpload = mm.text();
			MiniMailAvatarUploadMethod = mm.attribute("method");
		}
		else if (mmName == "avatar-remove")
		{
			MiniMailAvatarRemove = mm.text();
			MiniMailAvatarRemoveMethod = mm.attribute("method");
		}
	}

	// tlen bug? - need to set status 2nd time, to recive real status of contacts, move this after roster rcv
	writeStatus();
}

QString tlen::decode( const QByteArray &in ) {
	kdebugf();
	QByteArray o;
	QString out;

	QRegExp ex("\\%[0-9A-Fa-f][0-9A-Fa-f]");

	for(int i = 0; i < in.length(); ++i) {
		if( in.mid(i,1) == "+" )
			o += " ";
		else if( ex.exactMatch(in.mid(i,3)) ) {
			bool ok;
			int code = QString(in.mid(i+1,2)).toInt( &ok, 16 );

			if(ok)
				o += (char)code;
			i+=2;
		}
		else
			o += in.mid(i,1);
	}

	QTextCodec *codec=QTextCodec::codecForName("ISO 8859-2");
	out = codec->toUnicode( o );

	return out;
}

QString tlen::decode(const QString &in)
{
	return decode(in.toAscii());
}

QByteArray tlen::encode( const QString &in ) {
	kdebugf();
	QTextCodec *codec = QTextCodec::codecForName("ISO 8859-2");
	QByteArray o = codec->fromUnicode( in );

	QByteArray out;

	QRegExp ex("[^A-Za-z0-9\\.\\-\\_]");

	for(int i = 0; i < o.length(); ++i) {
		if( o.mid(i,1) == " " )
			out += "+";
		else if( ex.exactMatch( o.mid(i,1) ) ) {
			uchar inCh = (uchar)o[ i ];

			out += "%";

			ushort c = inCh / 16;
			c += c > 9 ? 'A' - 10 : '0';
			out += c;

			c = inCh % 16;
			c += c > 9 ? 'A' - 10 : '0';
			out += c;
		}
		else
			out += o.mid(i,1);
	}

	return out;
}
//<presence type=\"invisible\" ><status>description</status></presence>
//<presence><show>status</show><status>description</status></presence>
// t7 po pobraniu roster-a <presence><show>available</show></presence><iq type="7" to="c" />
void tlen::writeStatus() {
	kdebugf();
	QDomDocument doc;
	QDomElement p = doc.createElement("presence");
	QDomElement d = doc.createElement("status");

	if(Status== tlen::unavailable || Status== tlen::invisible)
		p.setAttribute("type", statusName(Status));
	else
	{
		QDomElement s = doc.createElement("show");
		s.appendChild(doc.createTextNode(statusName(Status)));
		p.appendChild(s);
	}

	if(!Descr.isEmpty())
		d.appendChild(doc.createTextNode(QString(encode(Descr))));

	p.appendChild(d);
	doc.appendChild(p);

	if(write(doc))
		emit statusChanged();

	if(Status == tlen::unavailable)
	{
		Reconnect = false;
		closeConn();
	}
}

void tlen::setStatus(TlenStatus status) {
	kdebugf();
	if(Status==status && Descr == "")
		return;

	Status = status;

	emit statusUpdate();
}

void tlen::setStatusDescr(tlen::TlenStatus status, const QString &description) {
	kdebugf();
	if(Descr==description && Status==status)
		return;

	Descr = description;
	Status = status;

	if (isConnected())
	{
		emit statusUpdate();
	}
	else
	{
		openConn();
	}
}

void tlen::authorize(const QString &to, bool subscribe) {
	kdebugf();

	/*
	   encode() powodowalo wysylanie jid'a postaci user%20tlen.pl
	   przez co autoryzacja nie dochodzi�a nigdy do skutku. - Juzef
	*/
	QString encodedTo = to;//QString(encode(to));
	QDomDocument doc;
	QDomElement p=doc.createElement("presence");
	p.setAttribute("to", encodedTo);

	if(subscribe) {
		p.setAttribute("type", "subscribe");
		doc.appendChild(p);
		write(doc);
		doc.clear();
		p=doc.createElement("presence");
		p.setAttribute("to", encodedTo);
		p.setAttribute("type", "subscribed");
	}
	else {
		p.setAttribute("type", "unsubscribed");
	}

	doc.appendChild(p);
	write(doc);
}

//<presence type="subscribe" to="jid" />
//<iq type="set" id="Q2WLO5" ><query xmlns="jabber:iq:roster">
//<item name="name" jid="jid" ><group>Kontakty</group></item></query></iq>
void tlen::addItem(const QString &jid, const QString &name, const QString &group, bool subscribe) {
	kdebugf();
	QDomDocument doc;
	QDomElement iq=doc.createElement("iq");
	iq.setAttribute("type", "set");
	iq.setAttribute("id", sid);

	QDomElement query=doc.createElement("query");
	query.setAttribute("xmlns","jabber:iq:roster");

	QDomElement item=doc.createElement("item");

	int atPos=jid.indexOf("@");
	//if(atPos!=-1)
	//	jid.remove(atPos, jid.length()-atPos);
	//jid+="@tlen.pl";

	if (atPos==-1)
		jid+="@tlen.pl";

	item.setAttribute("jid", QString(encode(jid.toLower())));

	if(!name.isEmpty())
		item.setAttribute("name", QString( encode( name ) ) );

	if(!group.isEmpty())
		item.appendChild(textNode("group", group));

	query.appendChild(item);
	iq.appendChild(query);
	doc.appendChild(iq);
	write(doc);

	if(subscribe) {
		doc.clear();
		QDomElement p=doc.createElement("presence");
		p.setAttribute("type","subscribe");
		p.setAttribute("to", QString(encode(jid.toLower())));
		doc.appendChild(p);
		write(doc);
	}
}

//<iq type="set" id="G14KEU" ><query xmlns="jabber:iq:roster">
//<item subscription="remove" jid="jid" /></query></iq>
void tlen::remove(const QString &jid) {
	kdebugf();
	QDomDocument doc;
	QDomElement iq=doc.createElement("iq");
	iq.setAttribute("type", "set");
	iq.setAttribute("id", sid);

	QDomElement query=doc.createElement("query");
	query.setAttribute("xmlns", "jabber:iq:roster");

	QDomElement item=doc.createElement("item");
	item.setAttribute("subscription","remove");
	item.setAttribute("jid", QString(encode(jid)));

	query.appendChild(item);
	iq.appendChild(query);
	doc.appendChild(iq);
	write(doc);
}

void tlen::writeMsg(const QString &msg, const QString &to) {
	kdebugf();
	QDomDocument doc;
	QDomElement message=doc.createElement("message");
	message.setAttribute("type", "chat");
	message.setAttribute("to", to);
	message.appendChild(textNode("body",QString(encode(msg))));

	doc.appendChild(message);
	write(doc);
}

void tlen::chatNotify( const QString& to, bool t )
{
	kdebugf();
	QDomDocument doc;
	QDomElement m=doc.createElement("m");
	m.setAttribute("to", QString(encode(to)));

	if(t)
		m.setAttribute("tp", "t");
	else
		m.setAttribute("tp", "u");

	doc.appendChild(m);
	write(doc);
}

void tlen::sendAlarm(const QString &to)
{
	kdebugf();
	QDomDocument doc;
	QDomElement m=doc.createElement("m");
	m.setAttribute("to", QString(encode(to)));
	m.setAttribute("tp", "a");
	doc.appendChild(m);
	write(doc);
}

QString tlen::statusName(TlenStatus index)
{
	switch(index)
	{
		case tlen::available	: return "available";
		case tlen::chat		: return "chat";
		case tlen::xa		: return "xa";
		case tlen::away		: return "away";
		case tlen::dnd		: return "dnd";
		case tlen::invisible	: return "invisible";
		case tlen::unavailable	: return "unavailable";
		default			: return "unavailable";
	}
}

tlen::TlenStatus tlen::statusType(const QString &status)
{
	if (status == "available")	return tlen::available;
	if (status == "chat")		return tlen::chat;
	if (status == "xa")		return tlen::xa;
	if (status == "away")		return tlen::away;
	if (status == "dnd")		return tlen::dnd;
	if (status == "invisible")	return tlen::invisible;
	if (status == "unavailable")	return tlen::unavailable;

	return tlen::unavailable;
}

void tlen::receiveFile(const QString &rndid, const QString &sender, bool receive) {
	kdebugf();
	QDomDocument doc;
	QDomElement f=doc.createElement("f");

	if(receive) {
		f.setAttribute("e", "5");
		f.setAttribute("v", "1");
	} else {
		f.setAttribute("e", "4");
	}

	f.setAttribute("t", sender);
	f.setAttribute("i", rndid);
	doc.appendChild(f);
	write(doc);
}

