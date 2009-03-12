#ifndef PLAYER_EXTERNAL_H
#define PLAYER_EXTERNAL_H

#include <QtCore/QObject>
#include <QtCore/QString>

/**
 * @defgroup phonon_sound Phonon sound
 * @{
 */
class PhononPlayer : public QObject
{
	Q_OBJECT

private slots:
	void playSound(const QString &s, bool volCntrl, double vol);

public:
	PhononPlayer();
	~PhononPlayer();
};

extern PhononPlayer *phonon_player;

/** @} */

#endif
