#ifndef MPRISCONTROLLER_H
#define MPRISCONTROLLER_H

#include <QtCore/QObject>
#include <QtCore/QVariantMap>

struct PlayerStatus
{
	int i1;
	int i2;
	int i3;
	int i4;
};
Q_DECLARE_METATYPE(PlayerStatus);

struct TrackInfo
{
	QString title;
	QString artist;
	QString album;
	QString track;
	QString file;
	uint    time;
};
Q_DECLARE_METATYPE(TrackInfo);

class MPRISController : public QObject
{
	Q_OBJECT

	PlayerStatus currentStatus_;
	TrackInfo    currentTrack_;
	bool         active_;
	QString      service;

public:
	explicit MPRISController(const QString &service, QObject *parent = 0);
	virtual ~MPRISController();

	const PlayerStatus & currentStatus() { return currentStatus_; };
	const TrackInfo & currentTrack() { return currentTrack_;  };
	bool active() { return active_; };
	void getStatus();

public slots:
	void statusChanged(const PlayerStatus &status);
	void trackChanged(const QVariantMap &map);

};

#endif
