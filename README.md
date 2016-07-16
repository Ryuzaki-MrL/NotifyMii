# NotifyMii
Homebrew Notification Manager

Features:
- Create your own custom notifications.
  - Message: You can manually type a message, use a text file from the SD card as the message, or copy from another notification.
    - Cannot exceed 6016 bytes (~5.8kb).
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

TODO:
- Write a GUI.
- Notification editing.

Credits:
- Rinnegatamante, for the notification dumping code from Purgification and utf<->ascii functions from lpp-3ds.
- Kerouz, for the app's banner and icon.
- Smealum and many others, for ctrulib.
- Steveice10, for buildtools.
- xerpi, for sf2dlib, sfillib and sftdlib.
