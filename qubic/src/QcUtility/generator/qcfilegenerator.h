#ifndef QCFILEGENERATOR_H
#define QCFILEGENERATOR_H

#include "schema/qcschema.h"
#include "schema/qcmetatable.h"
#include "qclogger.h"
#include "qcfunction.h"

#include <QString>
#include <QList>

#define PROJECT_FOLDER_NAME "Qubic-Generated"

class QcFileGenerator
{
private:
    static QcLogger *logger;

    static QcFunction getConstructor(QcMetaTable &table, ConstructorType type);
    static QList<QcFunction> getGetters(QcMetaTable &table);
    static QList<QcFunction> getSetters(QcMetaTable &table);
    static QList<QcVariable> getVariables(QcMetaTable &table);
    static QList<QcVariable> getQueryVariables(QcMetaTable &table);
    static QList<QcVariable> getCPPQueryVariables(QcMetaTable &table);
    static QList<QcFunction> getFKSetters(QcMetaTable &table);
    static QcFunction getPointerFunction(QcMetaTable &table);
    static QcFunction getRelationFunction(QcMetaTable &table);
    static void copyResources(QString srcPath, QString destPath);

    static void generateHeaders(QString dirPath, QcSchema schema);
    static void generateCPPs(QString dirPath, QcSchema schema);
    static void generatePRI(QString dirPath, QcSchema schema);
public:
    QcFileGenerator() = delete;
    static void generateProject(QString dirPath, QcSchema schema);
};

#endif // QCFILEGENERATOR_H
