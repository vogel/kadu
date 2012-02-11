/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

/*
 * vcardfactory.cpp - class for caching vCards
 * Copyright (C) 2003  Michail Pishchagin
 */

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QMap>
#include <QObject>
#include <QTextStream>
#include <QtAlgorithms>

#include "misc/kadu-paths.h"

#include "jid-util.h"
#include "vcard-factory.h"

using namespace XMPP;

/**
 * \brief Factory for retrieving and changing VCards.
 */
VCardFactory::VCardFactory(QObject *parent)
	: QObject(parent), dictSize_(5)
{
}

/**
 * \brief Destroys all cached VCards.
 */
VCardFactory::~VCardFactory()
{
	qDeleteAll(vcardDict_);
	instance_ = 0;
}

void VCardFactory::createInstance()
{
	if (!instance_)
		instance_ = new VCardFactory();
}

void VCardFactory::destroyInstance()
{
	delete instance_;
	instance_ = 0;
}

/**
 * \brief Returns the VCardFactory instance.
 */
VCardFactory * VCardFactory::instance()
{
	return instance_;
}

/**
 * Adds a vcard to the cache (and removes other items if necessary)
 */
void VCardFactory::checkLimit(QString jid, VCard *vcard)
{
	if (vcardList_.contains(jid)) {
		vcardList_.removeAll(jid);
		delete vcardDict_.take(jid);
	}
	else if (vcardList_.size() > dictSize_) {
		QString j = vcardList_.takeLast();
		delete vcardDict_.take(j);
	}

	vcardDict_[jid] = vcard;
	vcardList_.push_front(jid);
}


void VCardFactory::taskFinished()
{
	JT_VCard *task = (JT_VCard *)sender();
	if ( task->success() ) {
		Jid j = task->jid();

		saveVCard(j, task->vcard());
	}
}

void VCardFactory::saveVCard(const Jid& j, const VCard& _vcard)
{
	VCard *vcard = new VCard;
	*vcard = _vcard;
	checkLimit(j.bare(), vcard);

	// save vCard to disk

	// ensure that there's a vcard directory to save into
	QDir vcardDir(KaduPaths::instance()->profilePath() + QLatin1String("vcard"));
	if (!vcardDir.exists())
		vcardDir.mkpath(KaduPaths::instance()->profilePath() + QLatin1String("vcard"));

	QFile file ( KaduPaths::instance()->profilePath() + QLatin1String("vcard/") + JIDUtil::encode(j.bare()).toLower() + QLatin1String(".xml") );
	file.open ( QIODevice::WriteOnly );
	QTextStream out ( &file );
	out.setCodec("UTF8");
	QDomDocument doc;
	doc.appendChild( vcard->toXml ( &doc ) );
	out << doc.toString(4);

	Jid jid = j;
	emit vcardChanged(jid);
}

/**
 * \brief Call this, when you need a cached vCard.
 */
const VCard* VCardFactory::vcard(const Jid &j)
{
	// first, try to get vCard from runtime cache
	if (vcardDict_.contains(j.bare())) {
		return vcardDict_[j.bare()];
	}

	// then try to load from cache on disk
	QFile file ( KaduPaths::instance()->profilePath() + QLatin1String("vcard/") + JIDUtil::encode(j.bare()).toLower() + QLatin1String(".xml") );
	file.open (QIODevice::ReadOnly);
	QDomDocument doc;
	VCard *vcard = new VCard;
	if ( doc.setContent(&file, false) ) {
		vcard->fromXml( doc.documentElement() );
		checkLimit(j.bare(), vcard);
		return vcard;
	}

	delete vcard;
	return 0;
}


/**
 * \brief Call this when you need to update vCard in cache.
 */
void VCardFactory::setVCard(const Jid &j, const VCard &v)
{
	saveVCard(j, v);
}

/**
 * \brief Updates vCard on specified \a account.
 */
void VCardFactory::setVCard(Task *rootTask, const Jid &jid, const VCard &v, QObject* obj, const char* slot)
{
	JT_VCard* jtVCard_ = new JT_VCard(rootTask);
	if (obj)
		connect(jtVCard_, SIGNAL(finished()), obj, slot);
	connect(jtVCard_, SIGNAL(finished()), SLOT(updateVCardFinished()));
	jtVCard_->set(jid, v);
	jtVCard_->go(true);
}

void VCardFactory::updateVCardFinished()
{
	JT_VCard* jtVCard = static_cast<JT_VCard*> (sender());
	if (jtVCard && jtVCard->success()) {
		setVCard(jtVCard->jid(), jtVCard->vcard());
	}
	if (jtVCard) {
		jtVCard->deleteLater();
	}
}

/**
 * \brief Call this when you need to retrieve fresh vCard from server (and store it in cache afterwards)
 */
JT_VCard* VCardFactory::getVCard(const Jid &jid, Task *rootTask, const QObject *obj, const char *slot, bool cacheVCard)
{
	JT_VCard *task = new JT_VCard( rootTask );
	if ( cacheVCard )
		task->connect(task, SIGNAL(finished()), this, SLOT(taskFinished()));
	task->connect(task, SIGNAL(finished()), obj, slot);
	task->get(Jid(jid.full()));
	task->go(true);
	return task;
}

VCardFactory* VCardFactory::instance_ = NULL;
