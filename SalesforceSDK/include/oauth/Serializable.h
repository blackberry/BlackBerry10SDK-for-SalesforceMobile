/*
 * Serializable.h
 *
 *  Created on: Oct 24, 2013
 *      Author: timshi
 */

#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include <QObject>

namespace sf {

class Serializable : public QObject{
	Q_OBJECT
public:
	Serializable();
	virtual ~Serializable();


protected:
	//serialize this object to String
	QString serialize();
	//deserialize this object's value from string
	void deserialize(QString serializedString);
	void archiveValueToFileInDir(QString key, QString fileDir);
	bool unArchiveValueFromFileInDir(QString key, QString fileDir);
	static bool removeArchiveInDir(QString key, QString fileDir);
};

} /* namespace sf */
#endif /* SERIALIZABLE_H_ */
