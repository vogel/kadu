#ifndef PLAYER_EXTERNAL_H
#define PLAYER_EXTERNAL_H

#include <qobject.h>
#include <qstring.h>

/**
 * @defgroup ext_sound Ext sound
 * @{
 */
class ExternalPlayer : public QObject
{
	Q_OBJECT

	void createDefaultConfiguration();

private slots:
	void playSound(const QString &s, bool volCntrl, double vol);
	void play(const QString &s, bool volCntrl, double vol, QString player);

public:
	ExternalPlayer();
	~ExternalPlayer();
};

extern ExternalPlayer *external_player;

/** @} */

#endif
