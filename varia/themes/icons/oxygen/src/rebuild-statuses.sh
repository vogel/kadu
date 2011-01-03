#!/bin/bash


FILTER="Blackman"


for size in "16x16" "22x22" "32x32" "48x48" "64x64"; do
	mkdir -p ../${size}
done
for size in "16x16" "22x22" "32x32" "48x48" "64x64"; do
	mkdir -p ../kadu_icons/${size}
done


mkdir -p ../kadu_icons/64x64
for kaduicon in "blocked" "blocking"; do
	xcf2png statuses.xcf ${kaduicon} > ../kadu_icons/64x64/${kaduicon}.png
	for size in "16x16" "22x22" "32x32" "48x48"; do
		mkdir -p ../kadu_icons/${size}
		convert ../kadu_icons/64x64/${kaduicon}.png -resize ${size} -filter ${FILTER} ../kadu_icons/${size}/${kaduicon}.png
	done
done

for protocol in "common" "gadu-gadu" "xmpp"; do
	mkdir -p ../protocols/${protocol}/64x64
	for status in "offline" "invisible" "do_not_disturb" "not_available" "away" "online" "free_for_chat"; do
		for desc in "" "_d"; do
			for mobile in "" "_m"; do
				xcf2png statuses.xcf ${status}_ ${mobile} ${desc} _${protocol} > ../protocols/${protocol}/64x64/${status}${desc}${mobile}.png
				for size in "16x16" "22x22" "32x32" "48x48"; do
					mkdir -p ../protocols/${protocol}/${size}
					convert ../protocols/${protocol}/64x64/${status}${desc}${mobile}.png -resize ${size} -filter ${FILTER} ../protocols/${protocol}/${size}/${status}${desc}${mobile}.png
				done
			done
		done
	done
done


for size in "16x16" "22x22" "32x32" "48x48" "64x64"; do
	cp ../protocols/common/${size}/online.png ../kadu_icons/${size}/section-status.png
done

for size in "16x16" "22x22" "32x32" "48x48" "64x64"; do
	cp ../protocols/common/${size}/online.png ../kadu_icons/${size}/change-status.png
done

for size in "16x16" "22x22" "32x32" "48x48" "64x64"; do
	cp ../protocols/common/${size}/offline.png ../kadu_icons/${size}/show-offline-buddies.png
done

for size in "16x16" "22x22" "32x32" "48x48" "64x64"; do
	cp ../kadu_icons/${size}/blocked.png ../kadu_icons/${size}/show-blocked-buddies.png
done

xcf2png statuses.xcf offline50_ _d ___desc > ../kadu_icons/64x64/only-show-with-description.png
for size in "16x16" "22x22" "32x32" "48x48"; do
	convert ../kadu_icons/64x64/only-show-with-description.png -resize ${size} -filter ${FILTER} ../kadu_icons/${size}/only-show-with-description.png
done

xcf2png statuses.xcf online_ _d ___status_desc > ../kadu_icons/64x64/only-show-online-and-with-description.png
for size in "16x16" "22x22" "32x32" "48x48"; do
	convert ../kadu_icons/64x64/only-show-online-and-with-description.png -resize ${size} -filter ${FILTER} ../kadu_icons/${size}/only-show-online-and-with-description.png
done


echo "Oxygen status icons rebuilt."
