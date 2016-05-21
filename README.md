# NotifyMii
Homebrew Notification Manager

Features:
- Create your own custom notifications.
  - Message: You can use a text file from the SD card as the message, or copy from another notification.
  - Image (optional): You can select a JPG image from the SD card, or use an image from another notification.
    - Should be exactly 400x240 and cannot exceed 50kb.
  - Process ID (optional): You can select from which app the notification was received (no app = system notification).
- Read and delete notifications.
  - Notifications are automatically marked as read by NotifyMii upon reading.
  - You can also view a notification's image.
- Dump a notification's message and image to the SD card.
  - Messages are saved as HHMMSS_TITLE.txt.
  - Images are saved as HHMMSS_TITLE.jpg.
  - Both are saved to SD:/NotifyMii.
- Dump or delete all notifications at once.
- Launch the app from which the notification was received.

Issues:
- Deleting a notification that isn't the last in the list will mess up with other notifications.

TODO:
- Write a GUI.
- Fix flickering when selecting files/titles/notifications.
- Notification editing.
- Add flag selection to custom notifications.

Credits:
- Rinnegatamante, as some functions were stolen from Purgification and lpp-3ds.
- Kerouz, for the app's banner and icon.
- Smealum, for ctrulib.
- Steveice10, for bannertool.
- 3dsguy, for makerom.
- dnasdw, for 3dstool.
- xerpi, for sf2dlib and sfillib.
- jbr373, for hbkblib.