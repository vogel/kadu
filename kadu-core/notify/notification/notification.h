/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QtCore/QPair>

#include "buddies/buddy-set.h"
#include "icons/kadu-icon.h"
#include "parser/parser-data.h"
#include "exports.h"

class QTimer;

class Notifier;

class KADUAPI Notification : public QObject, public ParserData
{
	Q_OBJECT

public:
	struct Callback
	{
		QString Caption;
		const char *Slot;
		const char *Signature;

		Callback() : Slot(0), Signature(0) {}

		Callback(const Callback &copy)
		{
			Caption = copy.Caption;
			Slot = copy.Slot;
			Signature = copy.Signature;
		}

		Callback & operator = (const Callback &copy)
		{
			Caption = copy.Caption;
			Slot = copy.Slot;
			Signature = copy.Signature;

			return *this;
		}
	};

private:

	QString Type;

	QString Title;
	QString Text;
	QStringList Details;
	KaduIcon Icon;

	QList<Callback> Callbacks;
	QTimer *DefaultCallbackTimer;

protected:
	QSet<Notifier *> Notifiers;
	bool Closing;

public:
	static void registerParserTags();
	static void unregisterParserTags();

	Notification(const QString &type, const KaduIcon &icon);
	virtual ~Notification();

	virtual void acquire(Notifier *notifier);
	virtual void release(Notifier *notifier);
	virtual void close();

	void clearCallbacks();
	void addCallback(const QString &caption, const char *slot, const char *signature);
	void setDefaultCallback(int timeout, const char *slot);

	virtual bool requireCallback() { return false; }

	virtual const QString & type() const { return Type; }

	virtual QString key() const;

	virtual QString groupKey() const { return Title; }

	virtual QString identifier() { return Type + "_" + groupKey(); }

	void setTitle(const QString &title);

	virtual const QString title() const { return Title; }
	virtual void setText(const QString &text);
	virtual const QString text() const { return Text; }
	virtual void setDetails(const QStringList &details);
	virtual void setDetails(const QString &details);
	virtual const QStringList details() const { return Details; }

	virtual bool isPeriodic() { return false; }
	virtual int period() { return 0; }
	virtual void setIcon(const KaduIcon &icon);
	virtual const KaduIcon & icon() const { return Icon; }
	virtual const QList<Callback> & getCallbacks() { return Callbacks; }

public slots:
	virtual void callbackAccept();
	virtual void callbackDiscard();

	/// @todo API users can easily forget to call it. We should probably clear default callback automatically.
	virtual void clearDefaultCallback();

signals:
	void updated(Notification *);
	void closed(Notification *);


};

#endif // NOTIFICATION_H
