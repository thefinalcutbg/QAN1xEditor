#include "FreeFunctions.h"
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>
#include "GlobalWidgets.h"
#include <QDesktopServices>

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





bool FreeFn::getUpdate()
{

    QNetworkAccessManager mngr;


    QNetworkRequest request(QUrl{ "https://raw.githubusercontent.com/thefinalcutbg/QAN1xEditor/master/latest" });

    auto reply = mngr.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    bool ok;

    int latest_ver = reply->readAll().toInt(&ok);

    reply->deleteLater();

    //Current version
    constexpr int current_ver = 8;

    if (!ok || current_ver >= latest_ver) return false;

    if(!GlobalWidgets::askQuestion("A new version is available. Do you want to go to download page?")) return false;

    QDesktopServices::openUrl(QUrl("https://github.com/thefinalcutbg/QAN1xEditor/releases/latest", QUrl::TolerantMode));

    return true;
}
