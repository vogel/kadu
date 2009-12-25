/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SUBSCRIPTION_WINDOW_H
#define SUBSCRIPTION_WINDOW_H

#include <QtGui/QDialog>

class QCheckBox;
class QLineEdit;

class SubscriptionWindow : public QDialog
{
	Q_OBJECT

	QString Uid;
	QLineEdit *Subscription;
	QCheckBox *Store;

private slots:
	void accepted();

public:
	static void getSubscription(const QString& uid, QObject* receiver, const char* slot);
	
	explicit SubscriptionWindow(const QString& uid, QWidget* parent = 0);
	virtual ~SubscriptionWindow();

signals:
	void requestAccepted(QString &uid);

};

#endif // SUBSCRIPTION_WINDOW_H
