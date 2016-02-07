/*
 * %kadu copyright begin%
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

#pragma once

#include "gui/windows/main-configuration-window.h"

#include "notification/notifier.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
#include <QtX11Extras/QX11Info>
#endif

class Configuration;
class InjectedFactory;

class PCSpeakerNotifier : public QObject, public Notifier
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit PCSpeakerNotifier(QObject *parent = nullptr);
	~PCSpeakerNotifier();

	virtual void notify(const Notification &notification) override;
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = nullptr) override;

	void parseAndPlay(QString linia);

private:
	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	Display *xdisplay;
#endif
	int volume;

	void createDefaultConfiguration();
	void parseStringToSound(QString linia, int tablica[21], int tablica2[20]);
	void beep(int pitch, int duration);
	void play(int sound[21], int soundlength[20]);

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);

};
