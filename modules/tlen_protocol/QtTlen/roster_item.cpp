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

#include <QStringList>

#include "roster_item.h"

rosterItem::rosterItem( QString j, QString dn, int type, rosterItem *parent, QString s ): QObject( (QObject*)parent ) {
	parentItem = parent;
	expanded=FALSE;
	jid=j;
	subscription=s;
	status="unavailable";
	displayName=dn;
	t=type;
}

rosterItem::~rosterItem() {
    qDeleteAll(childItems);
}

void rosterItem::appendChild( rosterItem* item ) {
    childItems.append(item);
}

void rosterItem::takeChild( rosterItem* item) {
	childItems.removeAll(item);
}

rosterItem *rosterItem::child( int row ) {
    return childItems.value(row);
}

int rosterItem::childCount() const {
    return childItems.count();
}

int rosterItem::columnCount() const {
	return 1;
}

QString rosterItem::data(int column) const {
	switch(column) {
	case 0:
		return displayName;
	case 1:
		return descr;
	case 2:
		return jid;
	case 3:
		return subscription;
	case 4:
		return status;
	default:
		return QString();
	}
}

rosterItem *rosterItem::parent() {
    return parentItem;
}

int rosterItem::row() const {
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<rosterItem*>(this));

    return 0;
}

void rosterItem::setExpanded( bool state ) {
	expanded=state;
}

rosterItem* rosterItem::child( QString name, int col ) {
	QListIterator<rosterItem*> i(childItems);

	while(i.hasNext()) {
		rosterItem* item=i.next();
		if(item->data(col)==name)
			return item;
	}
	return 0;
}

QImage rosterItem::icon() {
	switch(t) {
	case rosterItem::Contact:
		if(status=="available")
			return descr.isEmpty()?QImage(":online.png"):QImage(":onlinei.png");
		else if(status=="chat")
			return descr.isEmpty()?QImage(":chatty.png"):QImage(":chattyi.png");
		else if(status=="away")
			return descr.isEmpty()?QImage(":away.png"):QImage(":awayi.png");
		else if(status=="xa")
			return descr.isEmpty()?QImage(":unavailable.png"):QImage(":unavailablei.png");
		else if(status=="dnd")
			return descr.isEmpty()?QImage(":dnd.png"):QImage(":dndi.png");
		else if(status=="unavailable")
			return descr.isEmpty()?QImage(":offline.png"):QImage(":offlinei.png");
		return QImage();

	case rosterItem::Group:
		return expanded?QImage(":expanded.png"):QImage(":collapsed.png");

	default:
		return QImage();
	}
}

void rosterItem::setStatus( QString s ) {
	status=s;
}

void rosterItem::setDescr( QString d ) {
	descr=d;
}

void rosterItem::setName( QString n ) {
	displayName=n;
}

int rosterItem::availCount() const {
	int count=0;
	QListIterator<rosterItem*> it(childItems);
	while(it.hasNext())
		if(it.next()->data(4)!="unavailable")
			++count;

	return count;
}

void rosterItem::moveItem( int from, int to ) {
	childItems.move(from,to);
}

void rosterItem::sort( bool subgroups ) {
	removeSubgroups();

	QMap<QString,QString> sort;
	QMap<QString,rosterItem*> childMap;
	int available=availCount();
	QListIterator<rosterItem*> it(childItems);
	while(it.hasNext()) {
		rosterItem *item=it.next();
		QString name=item->data(0);
		QString jid=item->data(2);
		sort.insert(name.toLower(),jid);
		childMap.insert(jid,item);
	}

	if(sort.count()>1) {
		QStringList list=sort.values();

		QListIterator<QString> si(list);
		int to=0;

		while(si.hasNext()) {
			int from=childMap[si.next()]->row();
			if(from!=to)
				moveItem(from,to);
			++to;
		}

		if(!parent()) {
			int f=-1;
			if(rosterItem *general=child(tr("Contacts")))
				f=general->row();
			if(f!=-1 && f!=0)
				moveItem(f,0);
		}
		else {
			it=childItems;
			if(available<childCount()-available) {
				to=0;
				while(it.hasNext()) {
					if(to==available)
						break;
					rosterItem *item=it.next();
					if(item->data(4)!="unavailable") {
						moveItem(item->row(), to);
						++to;
					}
				}
			}
			else {
				to=childCount()-1;
				it.toBack();
				while(it.hasPrevious()) {
					if(to==available-1)
						break;
					rosterItem *item=it.previous();
					if(item->data(4)=="unavailable") {
						moveItem(item->row(), to);
						--to;
					}
				}
			}
		}
	}

	if(subgroups) {
		int ask=0;
		int none=0;
		int lastPos=childCount()-1;
		it=childItems;
		it.toBack();
		while(it.hasPrevious()) {
			rosterItem *item=it.previous();
			if(item->data(3)=="none") {
				moveItem(item->row(), lastPos-none);
				++none;
			}
			else if(item->data(3)=="subscribe") {
				moveItem(item->row(), lastPos-(none+ask));
				++ask;
			}
		}

		if(none) {
			rosterItem *item=new rosterItem(QString(), tr("No authorization"), rosterItem::SubGroup,static_cast<rosterItem*>(this));
			appendChild(item);
			moveItem(item->row(), childCount()-(none+1));
		}
		if(ask) {
			rosterItem *item=new rosterItem(QString(), tr("Awaiting authorization"), rosterItem::SubGroup,static_cast<rosterItem*>(this));
			appendChild(item);
			moveItem(item->row(), childCount()-(none+ask+(none?2:1)));
		}
		if(contactCount()-(ask+none+available)) {
			rosterItem *item=new rosterItem(QString(), tr("Unavailable"), rosterItem::SubGroup,static_cast<rosterItem*>(this));
			appendChild(item);
			moveItem(item->row(),available);
		}
		if(available) {
			rosterItem *item=new rosterItem(QString(), tr("Currently available"), rosterItem::SubGroup,static_cast<rosterItem*>(this));
			appendChild(item);
			moveItem(item->row(),0);
		}
	}
}

void rosterItem::removeSubgroups() {
	QListIterator<rosterItem*> it(childItems);
	while(it.hasNext()) {
		rosterItem *item=it.next();
		if(item->type()==rosterItem::SubGroup)
			takeChild(item);
	}
}

int rosterItem::contactCount() const {
	int count=0;
	QListIterator<rosterItem*> it(childItems);
	while(it.hasNext()) {
		rosterItem *item=it.next();
		if(item->type()==rosterItem::Contact)
			++count;
	}
	return count;
}

void rosterItem::setSubscription( QString s ) {
	subscription=s;
}
