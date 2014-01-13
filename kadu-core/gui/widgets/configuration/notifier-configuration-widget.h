/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef NOTIFIER_CONFIGURATION_WIDGET_H
#define NOTIFIER_CONFIGURATION_WIDGET_H

#include <QtWidgets/QWidget>

#include "exports.h"

/**
	@class NotifierConfigurationWidget
	@author Vogel
	@short Widget konfiguracyjny dla Notifiera.

	Widget jest tworzony i dodawany w odpowiednim miejscu w oknie konfiguracyjnym.
	Zawarto�� element�w zmienia si� po wyborze innej notyfikacji w oknie konfiguracyjnym.
	Wiget zapamietuje wszystkie zmiany dla wszystkich typ�w notyfikacji i w odpowienim
	momencie je zapisuje.
**/
class KADUAPI NotifierConfigurationWidget : public QWidget
{
	Q_OBJECT

public:
	explicit NotifierConfigurationWidget(QWidget *parent = 0);

	/**
		W tej metodzie widget mo�e wczyta� konfigruacje wszystkich zdarze�.
	 **/
	virtual void loadNotifyConfigurations() = 0;
	/**
		W tej metodzie widget musi zapisa� wszystkie zmienione konfiguracje
		wszystkich zmienionych zdarze�.
	 **/
	virtual void saveNotifyConfigurations() = 0;

public slots:
	/**
		Slot wywo�ywany, gdy widget ma prze��czy� si� na konfigruacj� innego zdarzenia.
		Zmiany w aktualnym zdarzeniu powinny zosta� zapisane.

		@arg event - nazwa nowego zdarzenia
	 **/
	virtual void switchToEvent(const QString &event) = 0;

};

#endif // NOTIFIER_CONFIGURATION_WIDGET_H
