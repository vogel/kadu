set (COMPILE_PLUGINS

# protocols
	# GaduGadu protocol suppot
	gadu_protocol
	# Jabber/XMPP protocol support
	jabber_protocol

# notifiers
	# Enables notifications about buddies presence and other in chat windows
	chat_notifier
	# Notification by external commands module
	exec_notify
	# Freedesktop notification support
	freedesktop_notify

# history
	# General history plugin
	history
	# Chat history storing, browsing and searching support using sqlite
	sql_history

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
	# ALSA sound support
	alsa_sound

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
	# Idle time counter
	idle
	# Shows image links and youtube links as images and videos in chat
#	imagelink
	# Sms gateway support
	sms
	# Tabbed chat dialog
	tabs

)
