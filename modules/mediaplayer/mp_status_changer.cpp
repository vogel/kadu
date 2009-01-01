#include "mp_status_changer.h"
#include "mediaplayer.h"

MediaPlayerStatusChanger::MediaPlayerStatusChanger()
	: StatusChanger(900), disabled(true), mediaPlayerStatusPosition(DescriptionReplace) {}

MediaPlayerStatusChanger::~MediaPlayerStatusChanger() {}

void MediaPlayerStatusChanger::changeStatus(UserStatus &status)
{
	if (disabled)
		return;

	QString description = status.description();
	switch (mediaPlayerStatusPosition)
	{
		case DescriptionReplace:
			description = title;
			break;

		case DescriptionPrepend:
			description = title + description;
			break;

		case DescriptionAppend:
			description = description + title;
			break;

		case PlayerTagReplace:
			if (status.description().find("%player%") > -1)
				description.replace("%player%", title);
			break;
	}

	status.setDescription(description);
}

void MediaPlayerStatusChanger::setTitle(const QString &newTitle)
{
	disabled = false;

	if (newTitle != title)
	{
		title = newTitle;
		emit statusChanged();
	}
}

void MediaPlayerStatusChanger::setDisable(bool disable)
{
printf("MediaPlayerStatusChanger::setDisable(%d)\n", disable);
	disabled = disable;
	emit statusChanged();
}

void MediaPlayerStatusChanger::changePositionInStatus(ChangeDescriptionTo newSongInfoPlace)
{
	if (mediaPlayerStatusPosition != newSongInfoPlace)
	{
		mediaPlayerStatusPosition = newSongInfoPlace;
		if (!disabled)
			emit statusChanged();
	}
}
