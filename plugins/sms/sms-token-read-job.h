/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SMS_TOKEN_READ_JOB_H
#define SMS_TOKEN_READ_JOB_H

#include <QtCore/QObject>
#include <QtScript/QScriptValue>

class QNetworkReply;

/**
 * @addtogroup SMS
 * @{
 */

/**
 * @class SmsTokenReadJob
 * @author Rafał 'Vogel' Malinowski
 * @short This job is responsible for fetchng token image from URL, presenting it to user and call a javascript function.
 *
 * This job fetches token image frm given URL, then it presents it to user using @link TokenWindow @endlink
 * and at last it calls a javascript function with entered token value.
 *
 * This object automatically destroys itself after javascript function is called.
 */
class SmsTokenReadJob : public QObject
{
	Q_OBJECT

	QNetworkReply *TokenNetworkReply;

	QScriptValue CallbackObject;
	QScriptValue CallbackMethod;

private slots:
	void tokenImageDownloaded();
	void tokenValueEntered(const QString &tokenValue);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new SmsTokenReadJob instance.
	 * @param callbackObject javascript object that will receive callback after token is read
	 * @param callbackMethod javascript method that will be called on callbackObject after token is read
	 * @param parent QObject parent of new object
	 */
	SmsTokenReadJob(QScriptValue callbackObject, QScriptValue callbackMethod, QObject *parent = 0);
	virtual ~SmsTokenReadJob();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Execute this job with given token URL.
	 * @param tokenImageUrl URL of token image to read
	 */
	virtual void exec(const QString &tokenImageUrl);

};

/**
 * @}
 */

#endif //SMS_TOKEN_READ_JOB_H
