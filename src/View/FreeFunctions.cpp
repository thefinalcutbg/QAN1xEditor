#include "FreeFunctions.h"

std::vector<QIcon> s_icons;
bool icon_arr_init = false;

QIcon FreeFn::getTypeIcon(int type) {

    if (!icon_arr_init) {

        for (int i = 0; i < 23; i++) {

            QString qrcPath = ":/icon/resources/icon";
            qrcPath += QString::number(i);
            qrcPath += ".png";

            s_icons.push_back(QIcon(qrcPath));
        }

        icon_arr_init = true;
    }

    if (type < 0 || type > 22) type = 0;

    return s_icons[type];
}