/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libdui.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef DUIDATAACCESS_STUB
#define DUIDATAACCESS_STUB

#include "duidataaccess.h"
#include <stubbase.h>


// 1. DECLARE STUB
// FIXME - stubgen is not yet finished
class DuiDataAccessStub : public StubBase {
  public:
  QVariant value ;
  bool setValue ;
  QStringList allKeys ;
  bool contains ;
  virtual void valueChanged(const QString &key, const QVariant &value);
}; 

// 2. IMPLEMENT STUB
void DuiDataAccessStub::valueChanged(const QString &key, const QVariant &value) {
  QList<ParameterBase*> params;
  params.append( new Parameter<const QString & >(key));
  params.append( new Parameter<const QVariant & >(value));
  stubMethodEntered("valueChanged",params);
}



// 3. CREATE A STUB INSTANCE
DuiDataAccessStub gDefaultDuiDataAccessStub;
DuiDataAccessStub* gDuiDataAccessStub = &gDefaultDuiDataAccessStub;


#endif
