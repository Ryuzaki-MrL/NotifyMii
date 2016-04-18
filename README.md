# NotifyMii
Homebrew Notification Manager

Features:
- Create custom notifications with or without image.
- Read and delete notifications.
- Dump a notification's message to a .txt file.
- Dump a notification's image to a .jpg file.
- Clear all notifications at once.

Issues:
- Deleting a notification with an image and then creating one without image would result on the created notification having the deleted notification's image.
- Default program's work directory (SD:/NotifyMii) is not being created automatically.
- The notification list won't refresh upon creating a new notification, you must exit and launch the app again. This is a limitation of Purgification's NewsList structure and I'll change this later.
- Deleting a notification that isn't the last one on the list will mess up some other notifications. Again, this can't be fixed until I switch off from Purgification's NewsLists.

TODO:
- Improve UI, maybe even make a GUI.
- Add "dump all notifications" function.
- Change the app's crappy banner and icon.
- Clean up this messy code.

Credits:
- Rinnegatamante, since part of my source code was based on Purgification's.
- Smealum, for ctrulib
- Steveice10, for bannertool
- 3dsguy, for makerom
- dnasdw, for 3dstool
