#ifndef NOTIFICATION_H
#define NOTIFICATION_H

struct NewsList {
	u32 id;
	char title[64];
	bool hasImage;
    bool unread;
    bool isSpotPass;
	NewsList* next;
};

NewsList* createVoice(u32 id);
void addNews(const char *title_c, const char *text_c, u8 imgOption, NewsList* list);
void printNews(NewsList* list);
u32 selectNews(NewsList* news);
void readNews(u32 id, NewsList* list);
void dumpNews(u32 id, NewsList* news);
NewsList* deleteNews(u32 id, NewsList* news);
void clearNews(NewsList* list);

#endif /* NOTIFICATION_H */
