#ifndef PLAYER_DSHOW_H
#define PLAYER_DSHOW_H

#include <QtCore/QObject>
#include <QtCore/QString>

/**
 * @defgroup dshow_sound Ext sound
 * @{
 */
class DShowPlayer : public QObject
{
	Q_OBJECT

private slots:
	void playSound(const QString &s, bool volCntrl, double vol);

public:
	DShowPlayer();
	~DShowPlayer();
};

extern DShowPlayer *dshow_player;

/** @} */

#endif
