#ifndef MEDIA_PLAYER_STATUS_CHANGER_H
#define MEDIA_PLAYER_STATUS_CHANGER_H

#include "status_changer.h"

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

	MediaPlayerStatusChanger();
	virtual ~MediaPlayerStatusChanger();

	virtual void changeStatus(UserStatus &status);

	void setTitle(const QString &newTitle);
	void setDisable(bool disable);
	void changePositionInStatus(ChangeDescriptionTo newPosition);

};

#endif
