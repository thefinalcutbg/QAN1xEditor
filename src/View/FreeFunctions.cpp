#include "FreeFunctions.h"

QIcon FreeFn::getTypeIcon(int type) {

    if (type < 0 || type > 22) return QIcon();

    QString qrcPath = ":/icon/resources/icon";
    qrcPath += QString::number(type);
    qrcPath += ".png";

    return QIcon(qrcPath);
}