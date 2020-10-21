#pragma once
#include <iostream>
#include <QtCore>
#include <qimage.h>

class User
{
public:
	User(QString username, QString password, QString nickname, int siteId);
	User(QString username, QString password, QString nickname, int siteId, QImage image);
	~User();
	QString getUsername();
	void setUsername(QString user);
	QString getPassword();
	void setPassword(QString password);
	QString getNickname();
	void setNickname(QString nickname);
	int getSiteId();
	void setSiteId(int siteId);
	QImage getImage();
	void setImage(QImage image);
	bool getHaveImage();
private:
	QString username;
	QString password;
	QString nickname;
	QImage image;
	bool haveImage;
	int siteId;
};	