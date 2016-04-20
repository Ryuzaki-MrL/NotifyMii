#ifndef NOTIFICATION_H
#define NOTIFICATION_H

//void addNews(const char *title, const char *message, u8 *image);
void printNews(u32 selected, u32 scroll, bool info);
void readNews(u32 id);
void dumpNews(u32 id);
void deleteNews(u32 id);

#endif /* NOTIFICATION_H */
