/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
 * MPRIS standard implementation (used in Amarok2, Audacious, BMPx,
 * Dragon Player, VLC, XMMS2
 *
 * See http://mpris.org/ for more details about the standard
 */

#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

#include "plugins/mediaplayer/mediaplayer.h"

#include "mpris_controller.h"

MPRISController::MPRISController(const QString &service, QObject *parent) :
		QObject(parent), CurrentStatus(StatusStopped), Active(false), Service(service)
{
	QDBusConnection bus = QDBusConnection::sessionBus();

	bus.connect("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus",
	            QLatin1String("NameOwnerChanged"), this, SLOT(nameOwnerChanged(QString, QString, QString)));

	if (bus.interface()->registeredServiceNames().value().contains(Service))
		activate();
}

MPRISController::~MPRISController()
{
	QDBusConnection bus = QDBusConnection::sessionBus();

	bus.disconnect("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus",
	               QLatin1String("NameOwnerChanged"), this, SLOT(nameOwnerChanged(QString, QString, QString)));
}

void MPRISController::call(const QString &methodName)
{
	if (Service.isEmpty())
		return;

	QDBusInterface mprisApp(Service, "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player");
	mprisApp.call(methodName);
}

QString MPRISController::identity() const
{
	if (Service.isEmpty())
		return QString();

	QDBusInterface mprisApp(Service, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties");
	QDBusReply<QDBusVariant> reply = mprisApp.call("Get", "org.mpris.MediaPlayer2", "Identity");

	if (!reply.isValid())
		return QString();

	return reply.value().variant().toString();
}

MPRISController::PlayerStatus MPRISController::status() const
{
	return CurrentStatus;
}

const TrackInfo & MPRISController::track() const
{
	return CurrentTrack;
}

bool MPRISController::active() const
{
	return Active;
}

void MPRISController::activate()
{
	if (Active)
		return;

	Active = true;

	QDBusConnection::sessionBus().connect(Service, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties",
	                                      "PropertiesChanged", this, SLOT(propertiesChanged(QDBusMessage)));

	fetchStatus();
	fetchMetadata();

	MediaPlayer::instance()->statusChanged();
}

void MPRISController::deactivate()
{
	Active = false;

	QDBusConnection::sessionBus().disconnect(Service, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties",
	                                         "PropertiesChanged", this, SLOT(propertiesChanged(QDBusMessage)));

	updateStatus(StatusStopped);

	MediaPlayer::instance()->statusChanged();
}

void MPRISController::updateStatus(const PlayerStatus newStatus)
{
	if (newStatus == CurrentStatus)
		return;

	CurrentStatus = newStatus;
	MediaPlayer::instance()->statusChanged();
}

void MPRISController::updateStatus(const QString &newStatus)
{
	if (newStatus == "Playing")
		updateStatus(StatusPlaying);
	else if (newStatus == "Paused")
		updateStatus(StatusPaused);
	else
		updateStatus(StatusStopped);
}

TrackInfo MPRISController::toTrackInfo(const QVariantMap &metadata) const
{
	TrackInfo result;

	result.setTitle(metadata.value("xesam:title").toString());
	result.setArtist(metadata.value("xesam:artist").toString());
	result.setAlbum(metadata.value("xesam:album").toString());
	result.setTrackNumber(metadata.value("xesam:trackNumber").toUInt());
	result.setFile(metadata.value("mpris:url").toString());
	result.setLength(metadata.value("mpris:length").toUInt() / 1000);

	return result;
}

void MPRISController::updateMetadata(const QVariantMap &metadata)
{
	CurrentTrack = toTrackInfo(metadata);
	MediaPlayer::instance()->titleChanged();
}

void MPRISController::nameOwnerChanged(const QString &service, const QString &previousOwner, const QString &currentOwner)
{
	Q_UNUSED(previousOwner)

	if (service != Service)
		return;

	if (currentOwner.isEmpty())
		deactivate();
	else
		activate();
}

void MPRISController::propertiesChanged(const QDBusMessage &message)
{
	QList<QVariant> arguments = message.arguments();
	if (3 != arguments.count())
		return;

	QString interfaceName = message.arguments().at(0).toString();
	if (interfaceName != "org.mpris.MediaPlayer2.Player")
		return;

	QVariantMap changedProperties = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
	if (changedProperties.contains("PlaybackStatus"))
		updateStatus(changedProperties.value("PlaybackStatus").toString());
	if (changedProperties.contains("Metadata"))
		updateMetadata(qdbus_cast<QVariantMap>(changedProperties.value("Metadata")));
}

void MPRISController::fetchStatus()
{
	if (Service.isEmpty())
		return;

	QDBusInterface mprisApp(Service, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties");
	QDBusReply<QDBusVariant> reply = mprisApp.call("Get", "org.mpris.MediaPlayer2.Player", "PlaybackStatus");

	if (!reply.isValid())
		return;

	updateStatus(qdbus_cast<QString>(reply.value().variant()));
}

void MPRISController::fetchMetadata()
{
	if (Service.isEmpty())
		return;

	QDBusInterface mprisApp(Service, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties");
	QDBusReply<QDBusVariant> reply = mprisApp.call("Get", "org.mpris.MediaPlayer2.Player", "Metadata");

	if (!reply.isValid())
		return;

	updateMetadata(qdbus_cast<QVariantMap>(reply.value().variant()));
}

int MPRISController::getCurrentPosition() const
{
	if (Service.isEmpty())
		return 0;

	QDBusInterface mprisApp(Service, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties");
	QDBusReply<QDBusVariant> reply = mprisApp.call("Get", "org.mpris.MediaPlayer2.Player", "Position");

	if (!reply.isValid())
		return 0;

	return qdbus_cast<int>(reply.value().variant()) / 1000;
}

int MPRISController::getVolume() const
{
	if (Service.isEmpty())
		return 0;

	QDBusInterface mprisApp(Service, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties");
	QDBusReply<QDBusVariant> reply = mprisApp.call("Get", "org.mpris.MediaPlayer2.Player", "Volume");

	if (!reply.isValid())
		return 0;

	return 100 * reply.value().variant().toDouble();
}

void MPRISController::setVolume(int volume) const
{
	if (Service.isEmpty())
		return;

	QDBusVariant volumeArg;
	volumeArg.setVariant(QVariant::fromValue((double)volume / 100));

	QDBusInterface mprisApp(Service, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties");

	// Set method has signature ssv, so we have to put QDBusVariant as last argument
	// as call only accepts QVariant arguments, we must wrap QDBusVariant in QVariant
	// and QDBusVariant is just a wrapper for normal QVariant, so in result we have
	// 2 layers on variants wrapping real value
	// but it works
	mprisApp.call("Set", "org.mpris.MediaPlayer2.Player", "Volume", QVariant::fromValue(volumeArg));
}

QList<TrackInfo> MPRISController::getTrackList() const
{
	QList<TrackInfo> result;

	if (Service.isEmpty())
		return result;

	QDBusInterface mprisApp(Service, "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.TrackList");
	QDBusReply<QDBusVariant> reply = mprisApp.call("GetTracksMetadata");

	if (!reply.isValid())
		return result;

	QVariantList trackList = qdbus_cast<QVariantList>(reply.value().variant());
	foreach (const QVariant &track, trackList)
		result.append(toTrackInfo(qdbus_cast<QVariantMap>(track)));

	return result;
}

#include "moc_mpris_controller.cpp"
