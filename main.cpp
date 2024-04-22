#include <QCoreApplication>
#include <server.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <cstring>
#include <string>

int main(int argc, char *argv[])
{
    //    QCoreApplication a(argc, argv);
    //    new Server(0);
    //    return a.exec();

    int sock = socket(PF_INET,SOCK_DGRAM, IPPROTO_IP);
//    std::string intName = {"eth0"};
    struct ifreq ifR;
    strncpy(ifR.ifr_ifrn.ifrn_name, "enp0s3", IFNAMSIZ);
    inet_pton(AF_INET, "192.168.0.120", ifR.ifr_ifru.ifru_addr.sa_data);
    ioctl(sock, SIOCSIFADDR, &ifR);

    inet_pton(AF_INET, "255.255.255.0", ifR.ifr_ifru.ifru_netmask.sa_data);
    ioctl(sock, SIOCSIFNETMASK, &ifR);

    qDebug() << ifR.ifr_ifrn.ifrn_name;
    qDebug() << ifR.ifr_ifru.ifru_addr.sa_data;
    qDebug() << sock;

    ioctl(sock, SIOCGIFFLAGS, &ifR);
    strncpy(ifR.ifr_name, "enp0s3", IFNAMSIZ);
    ifR.ifr_flags |= (IFF_UP | IFF_RUNNING);

    ioctl(sock, SIOCSIFFLAGS, &ifR);

    return 0;
}
