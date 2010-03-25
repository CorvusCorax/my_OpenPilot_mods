/*
 * mapgadgetfactory.h
 *
 *  Created on: Mar 6, 2010
 *      Author: peter
 */

#ifndef MAPGADGETFACTORY_H_
#define MAPGADGETFACTORY_H_

#include <coreplugin/iuavgadgetfactory.h>

namespace Core {
class IUAVGadget;
class IUAVGadgetFactory;
}

using namespace Core;

class MapGadgetFactory : public Core::IUAVGadgetFactory
{
    Q_OBJECT
public:
    MapGadgetFactory(QObject *parent = 0);
    ~MapGadgetFactory();

    Core::IUAVGadget *createGadget(QList<IUAVGadgetConfiguration*> *configurations, QWidget *parent);
    IUAVGadgetConfiguration *createConfiguration(bool locked,
                                                 const QString configName,
                                                 const QByteArray &state);
    IOptionsPage *createOptionsPage(IUAVGadgetConfiguration *config);
};

#endif // MAPGADGETFACTORY_H_
