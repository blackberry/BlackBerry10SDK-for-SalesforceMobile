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

/*!
 * @class Serializable
 * @headerfile Serializable.h <oauth/Serializable.h>
 *
 * @brief @c Serializable class provides the functions to serialize an QObject.
 * The serialized object can then be saved to the file system.
 */
class Serializable : public QObject{
	Q_OBJECT
public:
	Serializable();
	virtual ~Serializable();


protected:
	/*!
	 * @return the serialized string
	 */
	QString serialize();
	/*!
	 * @param serializedString populate this object's fields using the serialized string.
	 */
	void deserialize(QString serializedString);
	/*!
	 * @param key to save the file that contains the serialized object
	 * @param fileDir the directory to save the file in
	 * Serialize this object and save it to the file
	 */
	void archiveValueToFileInDir(QString key, QString fileDir);
	/*!
	 * @param key to look up the file that contains the serialized object
	 * @param fileDir the directory to look in
	 * @return true if the file exist and this object's member variables are populated with the deserialized data
	 */
	bool unArchiveValueFromFileInDir(QString key, QString fileDir);
	/*!
	 * @param key to look up the file that contains the serialized object
	 * @param fileDir the directory to look in
	 * @return true if the file that stores the serialized data is removed.
	 */
	static bool removeArchiveInDir(QString key, QString fileDir);
};

} /* namespace sf */
#endif /* SERIALIZABLE_H_ */
