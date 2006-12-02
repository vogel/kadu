/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qregexp.h>

#include "config_file.h"
#include "debug.h"
#include "gadu_images_manager.h"
#include "gadu_rich_text.h"
#include "html_document.h"
#include "misc.h"
#include "ignore.h"
#include "userlist.h"

QString formatGGMessage(const QString &msg, unsigned int formats_length, void *formats, UinType sender)
{
	kdebugf();
	QString mesg, tmp;
	bool bold, italic, underline, color, inspan;
	char *cformats = (char *)formats;
	char *cformats_end = cformats + formats_length;
	struct gg_msg_richtext_format *actformat;
	struct gg_msg_richtext_color *actcolor;
	struct gg_msg_richtext_image* actimage;

	bold = italic = underline = color = inspan = false;
	unsigned int pos = 0;
	const int MAX_NUMBER_OF_IMAGES = 5;
	int number_of_images = 0;

	UinsList uins(sender);
	UserListElements users;
	UserListElement user = userlist->byID("Gadu", QString::number(sender));
	users.append(user);

	const UserStatus &curStat = gadu->currentStatus();

	/* gdy mamy sendera na li¶cie kontaktów, nie jest on ignorowany,
	   nie jest anononimowy i nasz status na to pozwala, to zezwalamy na obrazki */
	bool receiveImage =
		userlist->contains(user, FalseForAnonymous) &&
		!isIgnored(users) &&

		(curStat.isOnline() ||	curStat.isBusy() ||
		(curStat.isInvisible() && config_file.readBoolEntry("Chat", "ReceiveImagesDuringInvisibility")));
	kdebugm(KDEBUG_INFO, "msg: '%s'\n", msg.local8Bit().data());
	kdebugm(KDEBUG_INFO, "formats_length: %u\n", formats_length);
	for (unsigned int i = 0; i < formats_length; ++i)
		kdebugm(KDEBUG_INFO, ">>%d\n", cformats[i]);
	if (formats_length)
	{
		while (cformats < cformats_end)
		{
			actformat = (struct gg_msg_richtext_format *)cformats;
			cformats += sizeof(struct gg_msg_richtext_format);
			if (cformats > cformats_end)
			{
				kdebugm(KDEBUG_WARNING, "possible hacking attempt (1) - tryin' to exceed formats boundary!\n");
				continue;
			}
			uint16_t tmpposition = gg_fix16(actformat->position);
			kdebugm(KDEBUG_INFO, "position: %d, font: %d\n", tmpposition, actformat->font);
			if (tmpposition > pos)
			{
				tmp = msg.mid(pos, tmpposition - pos);
				HtmlDocument::escapeText(tmp);
				mesg.append(tmp);
				pos = tmpposition;
			}

			if (inspan)
				mesg.append("</span>");
			kdebugm(KDEBUG_INFO, "format: font:%d | bold:%d italic:%d underline:%d color:%d image:%d\n",
				actformat->font, (actformat->font & GG_FONT_BOLD) != 0, (actformat->font & GG_FONT_ITALIC) != 0,
				(actformat->font & GG_FONT_UNDERLINE) != 0, (actformat->font & GG_FONT_COLOR) != 0,
				(actformat->font & GG_FONT_IMAGE) != 0);

			if (actformat->font & (~GG_FONT_IMAGE))
			{
				inspan = true;
				mesg.append("<span style=\"");
				if (actformat->font & GG_FONT_BOLD)
					mesg.append("font-weight:600;");
				if (actformat->font & GG_FONT_ITALIC)
					mesg.append("font-style:italic;");
				if (actformat->font & GG_FONT_UNDERLINE)
					mesg.append("text-decoration:underline;");
				if (actformat->font & GG_FONT_COLOR)
				{
					mesg.append("color:");
					actcolor = (struct gg_msg_richtext_color *)cformats;
					cformats += sizeof(struct gg_msg_richtext_color);
					if (cformats > cformats_end)
					{
						kdebugm(KDEBUG_WARNING, "possible hacking attempt (2) - tryin' to exceed formats boundary!\n");
						continue;
					}
					mesg.append(QColor(actcolor->red, actcolor->green, actcolor->blue).name());
				}
				mesg.append("\">");
			}
			else
				inspan = false;
			if (actformat->font & GG_FONT_IMAGE)
			{
				kdebugmf(KDEBUG_INFO, "I got image probably\n");
				actimage = (struct gg_msg_richtext_image*)(cformats);
				cformats += sizeof(struct gg_msg_richtext_image);
				if (cformats > cformats_end)
				{
					kdebugm(KDEBUG_WARNING, "possible hacking attempt (3) - tryin' to exceed formats boundary!\n");
					continue;
				}
				uint32_t tmpsize = gg_fix32(actimage->size);
				uint32_t tmpcrc32 = gg_fix32(actimage->crc32);
				kdebugm(KDEBUG_INFO, "Image size: %d, crc32: %d, sender:%d\n", tmpsize, tmpcrc32, sender);

				if (++number_of_images > MAX_NUMBER_OF_IMAGES)
				{
					kdebugm(KDEBUG_INFO, "%d: number of images in message exceeded %d, possible hacking attempt!\n", sender, MAX_NUMBER_OF_IMAGES);
					if (number_of_images == MAX_NUMBER_OF_IMAGES + 1)
						mesg.append(qApp->translate("@default",
							QT_TR_NOOP("###TOO MANY IMAGES###")));
				}
				else if (tmpsize == 20 && (tmpcrc32 == 4567 || tmpcrc32==99))
				{
					// do not process spy and ekg2 special images
					kdebugm(KDEBUG_INFO, "%d: scanning for invisibility detected, preparing tactical nuclear missiles ;)\n", sender);
					if (receiveImage)
						gadu->sendImageRequest(user, tmpsize, tmpcrc32);
				}
				else if (sender!=0)
				{
					kdebugm(KDEBUG_INFO, "Someone sends us an image\n");
					QString file_name =
						gadu_images_manager.getSavedImageFileName(
							tmpsize,
							tmpcrc32);
					if (!file_name.isEmpty())
					{
						kdebugm(KDEBUG_INFO, "This image was already saved\n");
						mesg.append(GaduImagesManager::imageHtml(file_name));
					}
					else
					{
						if (tmpsize<(config_file.readUnsignedNumEntry("Chat", "MaxImageSize")*1024))
						{
							if (receiveImage)
							{
								kdebugm(KDEBUG_INFO, "sending request\n");
								gadu->sendImageRequest(user, tmpsize, tmpcrc32);
								mesg.append(GaduImagesManager::loadingImageHtml(
										sender,tmpsize,tmpcrc32));
							}
							else
								mesg.append(qApp->translate("@default", QT_TR_NOOP("###IMAGE BLOCKED###")));
						}
						else
							mesg.append(qApp->translate("@default", QT_TR_NOOP("###IMAGE TOO BIG###")));
					}
				}
				else
				{
					kdebugm(KDEBUG_INFO, "This is my message and my image\n");
					QString file_name =
						gadu_images_manager.getImageToSendFileName(
							tmpsize,
							tmpcrc32);
					mesg.append(GaduImagesManager::imageHtml(file_name));
				}
			}// if (actformat->font & GG_FONT_IMAGE)
		}//while (cformats < cformats_end)
		if (pos < msg.length())
		{
			tmp = msg.mid(pos, msg.length() - pos);
			HtmlDocument::escapeText(tmp);
			mesg.append(tmp);
		}
		if (inspan)
			mesg.append("</span>");
	}
	else
	{
		mesg = msg;
		HtmlDocument::escapeText(mesg);
	}
	kdebugf2();
	return mesg;
}

struct attrib_formant
{
	QString name;
	QString value;
	attrib_formant() : name(), value() {}
};

struct richtext_formant
{
	struct gg_msg_richtext_format format;
	struct gg_msg_richtext_color color;
	struct gg_msg_richtext_image image;
};

static QString stripHTMLFromGGMessage(const QString &msg)
{
	kdebugf();
	QRegExp regexp;
	QString mesg = msg;

//	mesg.remove(QRegExp("^<html><head><meta\\sname=\"qrichtext\"\\s*\\s/></head>"));
	mesg.remove(QRegExp("^<html><head>.*<body\\s.*\">\\r\\n"));
	mesg.remove(QRegExp("\\r\\n</body></html>\\r\\n$"));
	mesg.remove("<wsp>");
	mesg.remove("</wsp>");

	mesg.remove("<p>");
	mesg.remove("<p dir=\"ltr\">");
//	mesg.remove("<p dir=\"rtl\">");
	mesg.remove("</p>");
	regexp.setMinimal(true);
	regexp.setPattern("<font (face=\"(\\S)+\"\\s)?(size=\"\\d{1,2}\"(\\s)?)?(style=\"font-size:\\d{1,2}pt\"(\\s)?)?>");
	mesg.remove(regexp);
	mesg.remove("</font>");

	return mesg;
}

/**
 * Translates QValueList with formants into flat buffer on heap
 *
 * Precondition - formats_length must contain valid length of result buffer
 */
static void *allocFormantBuffer(const QValueList<struct richtext_formant> &formants, unsigned int &formats_length)
{
	kdebugf();
	struct gg_msg_richtext richtext_header;
	char *cformats, *tmpformats;

	richtext_header.flag = 2;
	richtext_header.length = gg_fix16(formats_length);
	formats_length += sizeof(struct gg_msg_richtext);
	cformats = new char[formats_length];
	tmpformats = cformats;
	memcpy(tmpformats, &richtext_header, sizeof(struct gg_msg_richtext));
	tmpformats += sizeof(struct gg_msg_richtext);
	CONST_FOREACH(it, formants)
	{
		struct richtext_formant actformant = (*it);
		actformant.format.position = gg_fix16(actformant.format.position);
		memcpy(tmpformats, &actformant, sizeof(gg_msg_richtext_format));
		tmpformats += sizeof(gg_msg_richtext_format);
		if (actformant.format.font & GG_FONT_COLOR)
		{
			memcpy(tmpformats, &actformant.color, sizeof(gg_msg_richtext_color));
			tmpformats += sizeof(gg_msg_richtext_color);
		}
		if (actformant.format.font & GG_FONT_IMAGE)
		{
			memcpy(tmpformats, &actformant.image, sizeof(gg_msg_richtext_image));
			tmpformats += sizeof(gg_msg_richtext_image);
		}
	}
	kdebugmf(KDEBUG_INFO, "formats_length=%u, tmpformats-cformats=%ld\n",
		formats_length, tmpformats - cformats);

	return (void *)cformats;
}

QString unformatGGMessage(const QString &msg, unsigned int &formats_length, void *&formats)
{
	kdebugf();
	QString mesg, tmp;
	QStringList attribs;
	struct attrib_formant actattrib;
	QValueList<attrib_formant> formantattribs;
	int pos, idx, inspan;
	struct richtext_formant actformant, lastformant;
	QValueList<struct richtext_formant> formants;
	bool endspan;

	mesg = stripHTMLFromGGMessage(msg);

	kdebugmf(KDEBUG_INFO, "\n%s\n", mesg.local8Bit().data());

	inspan = -1;
	pos = idx = formats_length = 0;
	endspan = false;
	lastformant.format.font = 0;

	while (uint(pos) < mesg.length())
	{
		// get indexes of unparsed tags
		int image_idx    = mesg.find("[IMAGE ", pos);
		int span_idx     = mesg.find("<span style=", pos);
		int span_end_idx = mesg.find("</span>", pos);

		// if image(s) was parsed recently, we possibly have to restore previous
		// active formatting (since image formant invalidates it)
		// the following code inserts formant saved in lastformant object
		if (lastformant.format.font != 0 &&
			pos != image_idx && pos != span_idx && pos != span_end_idx)
		{
			lastformant.format.position = pos;	// we need to update position
			formants.append(lastformant);
			formats_length += sizeof(struct gg_msg_richtext_format);
		}
		lastformant.format.font = 0; // don't insert this formant again

		// do we have an image preceding any <span> tags?
		if (image_idx != -1 &&
			(span_idx == -1 || image_idx < span_idx) &&
			(span_end_idx == -1 || image_idx < span_end_idx))
		{
			// we have to translate any unhandled </span> tags before image
			// by inserting empty formant 0
			// (fixes mantis bug 355)
			if (endspan && inspan == -1 && pos)
			{
				endspan = false;	// mark </span> as handled
				actformant.format.position = pos;
				actformant.format.font = 0;
				formants.append(actformant);
				formats_length += sizeof(struct gg_msg_richtext_format);
			}

			// parse [IMAGE] tag and remove it from message
			int idx_end = mesg.find("]", image_idx);
			if (idx_end == -1)
				idx_end = mesg.length() - 1;
			QString file_name = mesg.mid(image_idx+7, idx_end-image_idx-7);
			uint32_t size;
			uint32_t crc32;
			gadu_images_manager.addImageToSend(file_name, size, crc32);
			mesg.remove(image_idx, idx_end-image_idx+1);

			// search for last non-image formant before currently parsed image
			// we need to save it, and reinsert after image in next loop iteration
			// (this is required, since image formant removes any active formatting
			// options)
			QValueList<struct richtext_formant>::const_iterator it = formants.end();
			while (it != formants.begin())
			{
				--it;
				// check for non-image formants (formant 0 is ok)
				if (((*it).format.font & GG_FONT_IMAGE) == 0)
				{
					lastformant = *it;
					break;
				}
			}

			// insert the actual image formant into the list
			actformant.format.position = image_idx;
			actformant.format.font = GG_FONT_IMAGE;
			actformant.image.unknown1 = 0x0109;
			actformant.image.size = gg_fix32(size);
			actformant.image.crc32 = gg_fix32(crc32);
			formants.append(actformant);
			formats_length += sizeof(struct gg_msg_richtext_format)
				+ sizeof(struct gg_msg_richtext_image);
			pos = image_idx;
		}
		else if (inspan == -1)
		{
			// parsing <span> tag (NOTE: we actually handle </span> here too)
			idx = span_idx;
			if (idx != -1)
			{
				kdebugmf(KDEBUG_INFO, "idx=%d\n", idx);
				inspan = idx;

				// close any unhandled </span> tags (insert empty formant)
				if (pos && idx > pos)
				{
					endspan = false;	// mark </span> as handled
					actformant.format.position = pos;
					actformant.format.font = 0;
					formants.append(actformant);
					formats_length += sizeof(struct gg_msg_richtext_format);
				}

				// parse <span> attributes and initialize formant structure
				pos = idx;
				idx = mesg.find("\">", pos);
				tmp = mesg.mid(pos, idx - pos);
				idx += 2;
				mesg.remove(pos, idx - pos);
				tmp = tmp.section("\"", 1, 1);
				attribs = QStringList::split(";", tmp);
				formantattribs.clear();
				CONST_FOREACH(attrib, attribs)
				{
					actattrib.name = (*attrib).section(":", 0, 0);
					actattrib.value = (*attrib).section(":", 1, 1);
					formantattribs.append(actattrib);
				}
				actformant.format.position = pos;
				actformant.format.font = 0;
				CONST_FOREACH(actattrib, formantattribs)
				{
					if ((*actattrib).name == "font-style" && (*actattrib).value == "italic")
						actformant.format.font |= GG_FONT_ITALIC;
					if ((*actattrib).name == "text-decoration" && (*actattrib).value == "underline")
						actformant.format.font |= GG_FONT_UNDERLINE;
					if ((*actattrib).name == "font-weight" && (*actattrib).value == "600")
						actformant.format.font |= GG_FONT_BOLD;
					if ((*actattrib).name == "color")
					{
						actformant.format.font |= GG_FONT_COLOR;
						QColor color((*actattrib).value);
						actformant.color.red = color.red();
						actformant.color.green = color.green();
						actformant.color.blue = color.blue();
					}
				}

				// insert <span> formant into list
				formants.append(actformant);
				formats_length += sizeof(struct gg_msg_richtext_format)
					+ sizeof(struct gg_msg_richtext_color)
					* ((actformant.format.font & GG_FONT_COLOR) != 0);
			}
			else
				break;
		}
		else
		{
			// found a </span> tag
			idx = span_end_idx;
			if (idx != -1)
			{
				// we don't create the formant structure here
				// </span> tag is removed from string, empty formant
				// is inserted in next loop iteration in code above.
				kdebugmf(KDEBUG_INFO, "idx=%d\n", idx);
				pos = idx;
				mesg.remove(pos, 7);
				inspan = -1;
				endspan = true;	// we'll take care of this </span> later
			}
			else
				break;
		}
	}

	// if loop ended before we could insert </span> formant, insert it now
	if (pos && idx == -1)
	{
		actformant.format.position = pos;
		actformant.format.font = 0;
		formants.append(actformant);
		formats_length += sizeof(struct gg_msg_richtext_format);
	}

	// now convert QValueList into flat memory buffer
	if (formats_length)
		formats = allocFormantBuffer(formants, formats_length);
	else
		formats = NULL;

	HtmlDocument::unescapeText(mesg);
	kdebugmf(KDEBUG_INFO|KDEBUG_FUNCTION_END, "\n%s\n", unicode2latin(mesg).data());
	return mesg;
}
