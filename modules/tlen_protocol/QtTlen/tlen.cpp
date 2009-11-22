/***************************************************************************
 *   Copyright (C) 2004-2005 by Naresh [Kamil Klimek]                      *
 *   naresh@tlen.pl                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
	sort=TRUE;
	hostname = "s1.tlen.pl";
	hostport = 443;
	Secure = false;
	Reconnect = false;

	Status="unavailable";
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

void tlen::openConn() {
	kdebugf();
	state=tlen::Connecting;
	socket->connectToHost(hostname, hostport);
}

void tlen::closeConn() {
	kdebugf();
	socket->close();
	state=tlen::Disconnected;
	// clear tcfg
	MiniMailBase = "";
	// clear token
	token = "";
}

bool tlen::isConnected() {
	switch(state) {
	case tlen::Connected:
	case tlen::Connecting:
		return true;
	break;

	default:
		return false;
	break;
	}
	return false;
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

void tlen::event(QDomNode n) {
	kdebugf();
	QString nodeName=n.nodeName();
	QDomElement element = n.toElement();
	if(nodeName=="s" && element.hasAttribute("i")) {
		ping->start( 50000 );
		sid = element.attribute("i");
		if(tlenLogin())
 			state = tlen::Connected;
		else
			socket->close();
	}
	else if(nodeName=="iq") {
		if(element.hasAttribute( "type" ) && element.attribute("type") == "result") {
			// tcfg
			if(element.hasAttribute("id") && element.attribute("id")==sid) {
				tcfgRequest();
				rosterRequest();
			}
			// <iq from="tuba" type="result" to="jid" id="tr">
			// <query xmlns="jabber:iq:register">
			// <item></item></query></iq>
			if(element.hasAttribute("from") && element.attribute("from")=="tuba"
				&& element.hasAttribute("id") && element.attribute("id")=="tr") {
					QDomElement query = element.elementsByTagName("query").item(0).toElement();
					//if (query.hasAttribute("xmlns") && element.attribute("xmlns")=="jabber:iq:register")
						emit pubdirReceived(query.childNodes());
					return;
			}
			if(element.hasAttribute("id") && element.attribute("id")=="GetRoster") {
				emit clearRosterView();
				sort=FALSE;
			}
			if(element.hasAttribute("from") && element.attribute("from") == "tcfg") {
				// parse tlen config
				kdebugf();
				QDomElement query = element.elementsByTagName("query").item(0).toElement();
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
			}

			if(n.hasChildNodes()) {
				QDomNodeList el=n.childNodes();
				for(int i=0;i<el.count();++i)
					emit eventReceived(el.item(i));
			}

			if(!sort) {
				emit sortRoster();
				emit tlenLoggedIn();
				sort=TRUE;
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
		QString jid=e.attribute("jid");
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

		emit itemReceived(jid, name, subscription, group, sort);

	}
	else if(nodeName=="presence") {
		QDomElement e=n.toElement();
		QString from=e.attribute("from");

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
						emit avatarReceived(from, avatar.attribute("type"), avatar.attribute("md5"));
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
			emit chatNotify(e.attribute("f"), e.attribute("tp"));
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
	state=tlen::Disconnected;
	ping->stop();

	if (Reconnect)
	{
		openConn();
	}
	else
	{
		Status="unavailable";
		Descr="";
		emit presenceDisconnected();
	}
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
	if( !isConnected() )
		return false;

	QDomDocument doc;

	QDomElement iq = doc.createElement( "iq" );
	iq.setAttribute( "type", "set" );
	iq.setAttribute( "id", sid );
	doc.appendChild( iq );

	QDomElement query = doc.createElement( "query" );
	query.setAttribute( "xmlns", "jabber:iq:auth" );
	iq.appendChild( query );

	QDomElement username_node = doc.createElement( "username" );
	query.appendChild( username_node );

	QDomText text = doc.createTextNode( u );
	username_node.appendChild( text );

	QDomElement digest = doc.createElement( "digest" );
	query.appendChild( digest );

	text = doc.createTextNode( tlen_hash( p.toAscii().data(), sid.toAscii().data() ) );
	digest.appendChild( text );

	QDomElement resource = doc.createElement( "resource" );
	query.appendChild( resource );
	text = doc.createTextNode( "w" ); // t
	resource.appendChild( text );
	return write(doc);
}

bool tlen::write( const QDomDocument &d ) {
	if( !isConnected() ) {
		openConn();
		return FALSE;
	}

	qDebug()<<"Write:"<<d.toByteArray();

	return (socket->write(d.toByteArray()) == (qint64)d.toByteArray().size());
}
// "<iq type='get' id='GetRoster'><query xmlns="jabber:iq:roster"/></iq>"
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
void tlen::writeStatus() {
	kdebugf();
	QDomDocument doc;
	QDomElement p = doc.createElement("presence");
	QDomElement d = doc.createElement("status");

	if(Status=="unavailable" || Status=="invisible")
		p.setAttribute("type", Status);
	else
	{
		QDomElement s = doc.createElement("show");
		s.appendChild(doc.createTextNode(Status));
		p.appendChild(s);
	}

	if(!Descr.isEmpty())
		d.appendChild(doc.createTextNode(QString(encode(Descr))));

	p.appendChild(d);
	doc.appendChild(p);

	if(write(doc))
		emit statusChanged();
}

void tlen::setStatus(QString status) {
	kdebugf();
	if(Status==status && Descr == "")
		return;

	Status = status;
	
	emit statusUpdate();
}

void tlen::setStatusDescr(QString status,QString description) {
	kdebugf();
	if(Descr==description && Status==status)
		return;

	Descr = description;
	Status = status;
	emit statusUpdate();
}

void tlen::authorize( QString to, bool subscribe ) {
	kdebugf();
	QDomDocument doc;
	QDomElement p=doc.createElement("presence");
	p.setAttribute("to", to);

	if(subscribe) {
		p.setAttribute("type", "subscribe");
		doc.appendChild(p);
		write(doc);
		doc.clear();
		p=doc.createElement("presence");
		p.setAttribute("to", to);
		p.setAttribute("type", "subscribed");
		doc.appendChild(p);
		write(doc);
	}
	else {
		p.setAttribute("type", "unsubscribed");
		doc.appendChild(p);
		write(doc);
	}
}

void tlen::addItem( QString jid, QString name, QString g, bool subscribe ) {
	kdebugf();
	QDomDocument doc;
	QDomElement iq=doc.createElement("iq");
	iq.setAttribute("type", "set");
	iq.setAttribute("id", sid);

	QDomElement query=doc.createElement("query");
	query.setAttribute("xmlns","jabber:iq:roster");

	QDomElement item=doc.createElement("item");

	int atPos=jid.indexOf("@");
	if(atPos!=-1)
		jid.remove(atPos, jid.length()-atPos);

	jid+="@tlen.pl";

	item.setAttribute("jid", jid.toLower());

	if(!name.isEmpty())
		item.setAttribute("name", QString( encode( name ) ) );

	if(!g.isEmpty()) {
		QDomElement group=doc.createElement("group");
		QDomText t=doc.createTextNode(g);
		group.appendChild(t);
		item.appendChild(group);
	}

	query.appendChild(item);
	iq.appendChild(query);
	doc.appendChild(iq);
	write(doc);

	if(subscribe) {
		doc.clear();
		QDomElement p=doc.createElement("presence");
		p.setAttribute("type","subscribe");
		p.setAttribute("to", jid.toLower());
		doc.appendChild(p);
		write(doc);
	}
}

void tlen::remove(QString jid) {
	kdebugf();
	QDomDocument doc;
	QDomElement iq=doc.createElement("iq");
	iq.setAttribute("type", "set");
	iq.setAttribute("id", sid);

	QDomElement query=doc.createElement("query");
	query.setAttribute("xmlns", "jabber:iq:roster");

	QDomElement item=doc.createElement("item");
	item.setAttribute("subscription","remove");
	item.setAttribute("jid", jid);

	query.appendChild(item);
	iq.appendChild(query);
	doc.appendChild(iq);
	write(doc);
}

void tlen::writeMsg( QString msg, QString to ) {
	kdebugf();
	QDomDocument doc;
	QDomElement message=doc.createElement("message");
	message.setAttribute("type", "chat");
	message.setAttribute("to", to);

	QDomElement body=doc.createElement("body");
	QDomText text=doc.createTextNode(QString(encode(msg)));
	body.appendChild(text);
	message.appendChild(body);
	doc.appendChild(message);
	write(doc);
}

void tlen::chatNotify( QString to, bool t )
{
	kdebugf();
	QDomDocument doc;
	QDomElement m=doc.createElement("m");
	m.setAttribute("to", to);

	if(t)
		m.setAttribute("tp", "t");
	else
		m.setAttribute("tp", "u");

	doc.appendChild(m);
	write(doc);
}

void tlen::sendAlarm(QString to)
{
	kdebugf();
	QDomDocument doc;
	QDomElement m=doc.createElement("m");
	m.setAttribute("to", to);
	m.setAttribute("tp", "a");
	doc.appendChild(m);
	write(doc);
}

void tlen::receiveFile(QString rndid, QString sender, bool receive) {
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

