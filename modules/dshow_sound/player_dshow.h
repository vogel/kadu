#ifndef PLAYER_DSHOW_H
#define PLAYER_DSHOW_H

#include <QtCore/QString>
#include <QtGui/QWidget>

struct IGraphBuilder;
struct IMediaEventEx;
struct IMediaControl;

/**
 * @defgroup dshow_sound Ext sound
 * @{
 */
class DShowPlayer : public QWidget
{
	Q_OBJECT

	IGraphBuilder *dshow ;
	IMediaControl *control;
	IMediaEventEx *event;

	virtual bool winEvent(MSG * message, long * result);

private slots:
	void playSound(const QString &s, bool volCntrl, double vol);

public:
	DShowPlayer();
	~DShowPlayer();
};

extern DShowPlayer *dshow_player;

/** @} */

#endif
