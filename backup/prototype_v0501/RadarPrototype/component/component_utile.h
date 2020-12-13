#ifndef COMPONENT_UTILE_H
#define COMPONENT_UTILE_H
#include <QString>
#include <QDir>
#include <QList>

class Component_utile
{
public:

    Component_utile();
    static QList<QString> read_file_get_name_list();
};

#endif // COMPONENT_UTILE_H
