#include "qcdatabase.h"

#include <QDir>

#define QC_PROPERTIES_FILE "qcproperties.xml"

QSharedPointer<QcDatabase> QcDatabase::instance;

QcDatabase *QcDatabase::getInstance() {
    if (instance.isNull()) instance = QSharedPointer<QcDatabase>(new QcDatabase());
    return instance.data();
}

QcDatabase *QcDatabase::getInstance(QString propertiesPath) {
    if (instance.isNull()) instance = QSharedPointer<QcDatabase>(new QcDatabase(propertiesPath));
    return instance.data();
}

QcDatabase::QcDatabase(QString propertiesPath) {
    logger->info("Current path is: " + propertiesPath);

}