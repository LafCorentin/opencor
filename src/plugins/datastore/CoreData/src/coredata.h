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
// A store for simulation data
//==============================================================================

#ifndef COREDATA_H
#define COREDATA_H

//==============================================================================

#include "coredataglobal.h"

//==============================================================================

#include <QString>
#include <QVector>

//==============================================================================

#include <QtGlobal>

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace CoreData {

//==============================================================================

typedef qulonglong SizeType ;   // Large array sizes and indexes
typedef long       IndexType ;  // Object counts and indexes (-1 ==> not inited)

//==============================================================================

class COREDATA_EXPORT DataVariable {

 public:
  DataVariable(const SizeType &pSize, const double *pValuePointer=0) ;
  virtual ~DataVariable() ;

  void setUri(const QString &pUri) ;
  void setUnits(const QString &pUnits) ;
  void setLabel(const QString &pLabel) ;

  QString getUri(void) const ;
  QString getLabel(void) const ;
  QString getUnits(void) const ;

  void savePoint(const SizeType &pPos) ;
  void savePoint(const SizeType &pPos, const double &pValue) ;

  double getPoint(const SizeType &pPos) const ;
  const double *getData(void) const ;
  SizeType getSize(void) const ;

 private:
  QString mUri ;
  QString mUnits ;
  QString mLabel ;
  const double *mValuePointer ;
  double *mBuffer ;
  SizeType mSize ;
  } ;

//==============================================================================

class COREDATA_EXPORT DataSet {

 public:
  DataSet(const SizeType &pSize) ;
  virtual ~DataSet() ;

  DataVariable *getVoi(void) const ;
  DataVariable *getVariable(long index) const ;
  const QVector<DataVariable *> &getVariables(void) const ;

  DataVariable *holdPoint(const double *pPoint=0, const bool &pVoi=false) ;
  QVector<DataVariable *> holdPoints(const IndexType &pCount, const double *pPoints) ;

  void savePoints(const SizeType &pPos) ;

  SizeType getSize(void) const ;
  IndexType length(void) const ;

 private:
  const SizeType mSize ;
  QVector<DataVariable *> mVariables ;
  DataVariable *mVoi ;
  } ;

//==============================================================================

}   // namespace CoreData
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
