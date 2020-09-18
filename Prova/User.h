#pragma once
#include <iostream>
#include <QtCore>
class User
{
public:
	User(QString username, QString password, QString nickname, int siteId);
	~User();
	QString getUsername();
	void setUsername(QString user);
	QString getPassword();
	void setPassword(QString password);
	QString getNickname();
	void setNickname(QString nickname);
	int getSiteId();
	void setSiteId(int siteId);
private:
	QString username;
	QString password;
	QString nickname;
	int siteId;
};	