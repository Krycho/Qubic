#include "qcschemagenerator.h"

using std::list;

#include "database/databasedescriptor.h"
#include "database/databaseinfo.h"

#include "qcschemavalidator.h"

QcLogger* QcSchemaGenerator::logger;

QList<QcMetaTable> QcSchemaGenerator::getTables(Properties *properties) {
    QList<QcMetaTable> result;
    DatabaseDescriptor *descriptor = DatabaseInfo::getDescriptor(QcDatabase::getInstance()->getDatabase().driverName());

    for(QString tableName : descriptor->getTables()) {
        if(QcSchemaValidator::hasErrors()) {
            return result;
        }
        QcMetaTable table(tableName);
        logger->debug("Mapping table: " + tableName);
        table.setFields(getFields(tableName,properties));
        QcSchemaValidator::checkTable(table);
        result.append(table);
    }

    setRelations(result);

    return result;
}

QList<QcMetaField*> QcSchemaGenerator::getFields(const QString &tableName, Properties *properties) {
    qRegisterHelper<QcMetaField>();

    QList<QcMetaField*> result;
    DatabaseDescriptor *descriptor = DatabaseInfo::getDescriptor(QcDatabase::getInstance()->getDatabase().driverName());
    QMap<QString,QString> typeMapping = properties->getAttributesMap("map","fromType","toType");

    for(QString columnName : descriptor->getColumns(tableName)) {
        QcSchemaValidator::checkColumn(columnName);
        QString columnType = descriptor->getColumnType(tableName,columnName);
        QString columnTypeMap = typeMapping[columnType];
        int id = QMetaType::type(columnTypeMap.toStdString().c_str());
        logger->debug("Mapping field: '" + columnType + "' to type '" + columnTypeMap + "'");
        if(id == 0) {
            logger->error("Type: '" + columnType + "' is not supported");
            QcSchemaValidator::setError(ERROR_TYPE::UnsupportedType);
            break;
        }
        QcMetaField *field = static_cast<QcMetaField*>(QMetaType::create(id));
        logger->debug("Setting column name: " + columnName);
        field->setName(columnName);
        if(descriptor->isPrimaryKey(tableName,columnName)) {
            logger->debug("Column: " + columnName + " is primary key.");
            field->setPrimaryKey(true);
        }

        if(descriptor->isForeignKey(tableName,columnName)) {
            logger->debug("Column: " + columnName + " is foreign key.");
            field->setType(descriptor->getFKTable(tableName,columnName));
            field->setForeignKey(true);
        }

        result.append(field);
    }

    return result;
}

void QcSchemaGenerator::setRelations(QList<QcMetaTable> &tables) {
    logger->debug("Setting up relations");
    for(QcMetaTable &table : tables) {
        QList<QcMetaField*> fkFields = getFKFields(table);
        if(fkFields.size() == 1) {
            logger->debug("Found one to many relation");
            QcMetaTable &relationTable = findTable(tables, fkFields.at(0)->getName());
            logger->debug("Table: " + table.getName() + " got fk to: " + relationTable.getName() + " table");
            relationTable.setRelationType(RelationType::OneToMany);
            relationTable.setRelatedTable(&table);
        }

        if(fkFields.size() == 2) {
            logger->debug("Found many to many relation");
            QcMetaTable &leftSide = findTable(tables,fkFields.at(0)->getName());
            QcMetaTable &rightSide = findTable(tables,fkFields.at(1)->getName());

            logger->debug("Left: " + leftSide.getName() + " Center: " + table.getName() + " Right: " + rightSide.getName());
            leftSide.setRelationType(RelationType::ManyToMany);
            leftSide.setRelatedTable(&rightSide);
            leftSide.setJointTable(&table);

            rightSide.setRelationType(RelationType::ManyToMany);
            rightSide.setRelatedTable(&leftSide);
            rightSide.setJointTable(&table);
        }
    }
}

QcMetaTable& QcSchemaGenerator::findTable(QList<QcMetaTable> &tables, QString idName) {
    for(QcMetaTable &table : tables) {
        if(idName.contains(table.getName(),Qt::CaseInsensitive)) {
            return table;
        }
    }
}

QList<QcMetaField*> QcSchemaGenerator::getFKFields(QcMetaTable &table) {
    QList<QcMetaField*> result;
    for(QcMetaField* &field : table.getFields()) {
        if(field->isForeignKey()) {
            result.push_back(field);
        }
    }
    return result;
}

// --------------------- ------------- --------------------- //

QcSchema QcSchemaGenerator::getSchema(Properties *properties) {
    logger = QcLogger::getInstance();
    return QcSchema(getTables(properties));
}
