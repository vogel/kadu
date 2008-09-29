#ifndef PLAYER_EXTERNAL_H
#define PLAYER_EXTERNAL_H

#include <QtCore/QObject>
#include <QtCore/QString>

/**
 * @defgroup ext_sound Ext sound
 * @{
 */
class Qt4Player : public QObject
{
	Q_OBJECT

private slots:
	void playSound(const QString &s, bool volCntrl, double vol);

public:
	Qt4Player();
	~Qt4Player();
};

extern Qt4Player *qt4_player;

/** @} */

#endif
