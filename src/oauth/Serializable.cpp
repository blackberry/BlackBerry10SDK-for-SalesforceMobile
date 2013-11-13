/*
 * Serializable.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: timshi
 */

#include "Serializable.h"
#include <bb/data/JsonDataAccess>
#include <QDir>
#include <QFile>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>

namespace sf {

Serializable::Serializable() {
}

Serializable::~Serializable() {
}

QString Serializable::serialize(){
	QVariantMap data;
	const QMetaObject *metaObj = metaObject();
	int count = metaObj->propertyCount();
	for(int i=0; i<count; i++) {
		QMetaProperty property = metaObj->property(i);
		if (!property.isReadable()) {
			continue;
		}
		data.insert(property.name(),property.read(this));
	}
	QString buffer;
	bb::data::JsonDataAccess jda;
	jda.saveToBuffer(data, &buffer);
	return buffer;
}

void Serializable::deserialize(QString serializedString){
	bb::data::JsonDataAccess jda;
	QVariant jsonData = jda.loadFromBuffer(serializedString.toUtf8());
	//we won't be able to serialize if it's not a map
	if (jsonData.type() != QVariant::Map){
		return;
	}
	QVariantMap data = jsonData.toMap();
	const QMetaObject *metaObj = metaObject();
	int count = metaObj->propertyCount();
	for(int i=0; i<count; i++) {
		QMetaProperty property = metaObj->property(i);
		if (!property.isWritable() || !data.contains(property.name())) {
			continue;
		}

		const QVariant & value = data[property.name()];
		if (!const_cast<QVariant&>(value).canConvert(property.type())) {
			continue;
		}
		property.write(this, value);
	}
}

void Serializable::archiveValueToFileInDir(QString key, QString fileDir){
	QDir home = QDir::home();
	QString absoluteDirPath = home.absoluteFilePath(fileDir);
	QDir dir = QDir(absoluteDirPath);
	if (!dir.exists()){
		home.mkdir(fileDir);
	}
	QString path = QString("%1/%2").arg(fileDir, key);
	QString absolutePath = home.absoluteFilePath(path);

	QFile credentialFile(absolutePath);
	credentialFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&credentialFile);
	QString serializedString = this->serialize();
	out<<serializedString;
	credentialFile.close();

}
bool Serializable::unArchiveValueFromFileInDir(QString key, QString fileDir){
	QDir home = QDir::home();
	QString absoluteDirPath = home.absoluteFilePath(fileDir);
	QDir dir = QDir(absoluteDirPath);
	if (!dir.exists()){
		return false;
	}

	QString path = QString("%1/%2").arg(fileDir, key);
	QString absolutePath = home.absoluteFilePath(path);

	QFile credentialFile(absolutePath);
	if (!credentialFile.exists()){
		return false;
	}
	credentialFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream out(&credentialFile);
	this->deserialize(out.readAll());
	credentialFile.close();

	return true;
}

bool Serializable::removeArchiveInDir(QString key, QString fileDir){
	QDir home = QDir::home();
	QString path = QString("%1/%2").arg(fileDir, key);
	QString absolutePath = home.absoluteFilePath(path);

	QFile credentialFile(absolutePath);
	return credentialFile.remove();
}
} /* namespace sf */
