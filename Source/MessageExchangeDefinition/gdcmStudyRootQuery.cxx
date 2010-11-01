/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
/*
file name: gdcmStudyRootQuery.cxx
contains: the class which will produce a dataset for c-find and c-move with study root
name and date: 18 oct 2010 mmr

placeholder file to get the compiler/linker to play nice with this file
*/


#include "gdcmStudyRootQuery.h"

#include "gdcmAttribute.h"
#include "gdcmDataSet.h"

namespace gdcm{
namespace network {

StudyRootQuery::StudyRootQuery() {
  SetParameters();
}

StudyRootQuery::~StudyRootQuery(){};


void StudyRootQuery::SetParameters(){
  mRootType = eStudyRootType;
  mHelpDescription = "Study-level root query";
}


///have to be able to ensure that
///0x8,0x52 is set
///that the level is appropriate (ie, not setting PATIENT for a study query
///that the tags in the query match the right level (either required, unique, optional)
bool StudyRootQuery::ValidateQuery() const{
  //if it's empty, it's not useful
  DataSet ds = GetQueryDataSet();
  if (ds.Size() == 0) return false;

  //search for 0x8,0x52
  gdcm::Attribute<0x0008, 0x0052> level;
  level.SetFromDataElement( ds.GetDataElement( level.GetTag() ) );
  std::string theVal = level.GetValue();

  if (strcmp(theVal.c_str(), "PATIENT") == 0) return false;

  QueryBase* qb = NULL;

  if (strcmp(theVal.c_str(), "STUDY") == 0){
    //make sure remaining tags are somewhere in the list of required, unique, or optional tags
    qb = new QueryStudy();
  }
  if (strcmp(theVal.c_str(), "SERIES") == 0){
    //make sure remaining tags are somewhere in the list of required, unique, or optional tags
    qb = new QuerySeries();
  }
  if (strcmp(theVal.c_str(), "IMAGE") == 0 || strcmp(theVal.c_str(), "FRAME") == 0){
    //make sure remaining tags are somewhere in the list of required, unique, or optional tags
    qb = new QueryImage();
  }
  if (qb == NULL){
    return false;
  }
  bool theReturn = true;

  std::vector<gdcm::Tag> tags = qb->GetAllTags(eStudyRootType);
  //all the tags in the dataset should be in that tag list
  //otherwise, it's not valid
  gdcm::DataSet::ConstIterator itor;
  for (itor = ds.Begin(); itor != ds.End(); itor++){
    gdcm::Tag t = itor->GetTag();
    if (std::find(tags.begin(), tags.end(), t) == tags.end()){
      theReturn = false;
      break;
    }
  }

  delete qb;
  return theReturn;
}

}
}