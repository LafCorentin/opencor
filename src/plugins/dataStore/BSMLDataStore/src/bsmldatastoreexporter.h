/*******************************************************************************

Licensed to the OpenCOR team under one or more contributor license agreements.
See the NOTICE.txt file distributed with this work for additional information
regarding copyright ownership. The OpenCOR team licenses this file to you under
the Apache License, Version 2.0 (the "License"); you may not use this file
except in compliance with the License. You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.

*******************************************************************************/

//==============================================================================
// BioSignalML data store exporter class
//==============================================================================

#ifndef BSMLDATASTOREEXPORTER_H
#define BSMLDATASTOREEXPORTER_H

//==============================================================================

#include "bsmldatastoreglobal.h"
#include "datastoreexporter.h"
#include "bsmldatastoresavedialog.h"

//==============================================================================

#include <QMainWindow>

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace CoreDataStore {
    class CoreDataStore;
}   // namespace CoreDataStore

//==============================================================================

namespace BSMLDataStore {

//==============================================================================

class BSMLDATASTORE_EXPORT BioSignalMLExporter : public CoreDataStore::DataStoreExporter
{
public:
    BioSignalMLExporter(QMainWindow *pMainWindow, const QString &pId = QString());
    virtual void execute(CoreDataStore::CoreDataStore *pDataStore) const;

private:
    BioSignalMLSaveDialog *mSaveDialog;
};

//==============================================================================

}   // namespace BSMLDataStore
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
