#ifndef REMOTEDATAACCESS_H
#define REMOTEDATAACCESS_H

#include "appmodels.h"

class RemoteDataAccess
{
public:
    static SharedResourcesInfo querySharedResourcesInfo(const QString& url);
};

#endif // REMOTEDATAACCESS_H
