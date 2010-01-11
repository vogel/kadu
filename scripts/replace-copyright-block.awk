# *   %kadu copyright begin                                                 *
# *   %kadu copyright end                                                   *

BEGIN {
	copy = 1
}

/%kadu copyright end%/ {
	print AUTHORS
	copy = 1
}

{
	if (copy)
		print $0
}

/%kadu copyright begin%/ {
	copy = 0
}
