/*=========================================================================

  Program: GDCM (Grass Root DICOM). A DICOM library
  Module:  $URL$

  Copyright (c) 2006-2008 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __gdcmCoder_h
#define __gdcmCoder_h

#include "gdcmTypes.h"
#include "gdcmDataElement.h" // FIXME

namespace gdcm
{
/** \class Coder
 */

class TransferSyntax;
class DataElement;
class GDCM_EXPORT Coder
{
public:
  virtual ~Coder() {}
  virtual bool CanCode(TransferSyntax const &) const = 0;
  virtual bool Code(DataElement const &in, DataElement &out) { (void)in; (void)out; return false; }
protected:
  virtual bool InternalCode(const char *bv, unsigned long len, std::ostream &os) { (void)bv;(void)os;return false; } 
};

} // end namespace gdcm

#endif //__gdcmCoder_h
