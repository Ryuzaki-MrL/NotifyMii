#ifndef NOTIFICATION_H
#define NOTIFICATION_H

typedef struct {
    bool unread;
    bool isSpotPass;
    bool isOptedOut;
} news_flags;

//void addNews(char *title_c, char *message_c, u8 *image, u32 imgSize, u64 processID, news_flags flags = { 1, 0, 0 });
void readNews(u32 id);
void dumpNews(u32 id);
void deleteNews(u32 id);
void clearNews();

#endif /* NOTIFICATION_H */
