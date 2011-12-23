#ifndef MEDIA_PLAYER_STATUS_CHANGER_H
#define MEDIA_PLAYER_STATUS_CHANGER_H

#include "status/status-changer.h"

class MediaPlayerStatusChanger : public StatusChanger
{
	Q_OBJECT

public:
	enum ChangeDescriptionTo {
		DescriptionReplace,
		DescriptionPrepend,
		DescriptionAppend,
		PlayerTagReplace
	};

private:
	QString title;
	bool disabled;
	ChangeDescriptionTo mediaPlayerStatusPosition;

public:
	explicit MediaPlayerStatusChanger(QObject *parent = 0);
	virtual ~MediaPlayerStatusChanger();

	virtual void changeStatus(StatusContainer *container, Status &status);

	void setTitle(const QString &newTitle);
	void setDisable(bool disable);
	bool isDisabled() { return disabled; }
	void changePositionInStatus(ChangeDescriptionTo newPosition);

};

#endif
