set (COMPILE_PLUGINS

# protocols
	# GaduGadu protocol suppot
	gadu_protocol
	# Jabber/XMPP protocol support
	jabber_protocol

# notifiers
	# Enables notifications about buddies presence and other in chat windows
	chat_notify
	# Notification by external commands module
	exec_notify
	# Hints near tray icon
	hints
	# Notifications by qt4_docking plugin
	qt4_docking_notify
	# Speech synthesis support
	speech
	# PC Speaker notification support
	pcspeaker

# history
	# General history plugin
	history
	# Chat history storing, browsing and searching support using sqlite
	sql_history
	# Migration from x <= 0.6.5 format
	history_migration

# encryption
	# Encrypted chat support
	encryption_ng
	# OpenSSL encrypted chat support
	encryption_ng_simlite

# docking
	# Tray icon support (common part of all docking modules)
	docking
	# Always on top window docking plugin
	desktop_docking
	# Qt tray docking techniques module (works everywhere)
	qt4_docking

# sound
	# General sound plugin
	sound
	# Phonon sound support
	phonon_sound
	# External sound player support
	ext_sound
	# QtSound sound support
	qt4_sound

# mediaplayer
	# Media players support plugin
	mediaplayer

# misc
	# Antistring
	antistring
	# Auto away module
	autoaway
	# Auto hide Kadu window
	auto_hide
	# Autoresponder plugin
	autoresponder
	# Autostatus
	autostatus
	# Cenzor
	cenzor
	# Configuration wizard
	config_wizard
	# Loads presence status messages from file
	filedesc
	# Protection against unwanted chats
	firewall
	# Idle time counter
	idle
	# Shows image links and youtube links as images and videos in chat
	imagelink
	# Last status infos
	last_seen
	# Imports profiles from old Kadu
	profiles_import
	# Simple view
	simpleview
	# Single window mode
	single_window
	# Sms gateway support
	sms
	# Spellchecking in chat window
	spellchecker
	# Tabbed chat dialog
	tabs
	# Automatic mispelled word replacement
	word_fix
)

# Platform-speficic plugins

if (UNIX)
	list (APPEND COMPILE_PLUGINS

	# mediaplayer
		# MPD mediaplayer support
#		mpd_mediaplayer
		# MPRIS Media Players support
		mprisplayer_mediaplayer
	)
endif (UNIX)

if (UNIX AND NOT APPLE)
	list (APPEND COMPILE_PLUGINS

	# notifiers
		# Freedesktop notification support
		freedesktop_notify

	# mediaplayer
		# Amarok 1.x mediaplayer support
		amarok1_mediaplayer
		# FALF mediaplayer support
		falf_mediaplayer

	# misc
		# Easily take screenshots and send as images
		screenshot
	)
endif (UNIX AND NOT APPLE)

if (APPLE)
	list (APPEND COMPILE_PLUGINS

	# notifiers
		# Growl notification support
		growl_notify

	# mediaplayer
		# Itunes Media Player support
		itunes_mediaplayer
	)
endif (APPLE)

if (WIN32)
	list (APPEND COMPILE_PLUGINS

	# mediaplayer
		# Winamp Media Player support
		winamp_mediaplayer
	)
endif (WIN32)

# Sort the list so plugins will be built in alphabetical order
list (SORT COMPILE_PLUGINS)
