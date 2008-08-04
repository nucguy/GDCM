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
/*
 * gdcmraw - ACR/NEMA DICOM PS3 ... DICOM PS3 - DICOM image to raw file
 * Synopsis:
 * gdcmraw [ -t | --tag Tag# (default: 07fe,0010) ] -i inputfile
 * Description:
 * gdcmraw
 * reads the named dicom or acr-nema input file and copies the raw image
 * pixel data to a raw binary file without a header of any kind.
 * The byte order, packing or encapsulation of the raw result is dependent
 * only on the encoding of the input file and cannot be changed.
*/

#include "gdcmReader.h"
#include "gdcmFileMetaInformation.h"
#include "gdcmDataSet.h"
#include "gdcmTag.h"
#include "gdcmByteValue.h"
#include "gdcmSequenceOfFragments.h"
#include "gdcmFragment.h"
#include "gdcmFilename.h"

#include <string>
#include <iostream>

#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <string.h>


int main(int argc, char *argv[])
{
  int c;
  //int digit_optind = 0;

  gdcm::Tag rawTag(0x7fe0, 0x0010); // Default to Pixel Data
  std::string filename;
  std::string outfilename;
  std::string pattern;
  int splitfrags = 0;
  while (1) {
    //int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
        {"input", 1, 0, 0},                 // i
        {"output", 1, 0, 0},                // o
        {"tag", 1, 0, 0},                   // t
        {"split-frags", 0, &splitfrags, 1}, // f
        {"pattern", 1, 0, 0},               // p
        {0, 0, 0, 0}
    };

    c = getopt_long (argc, argv, "i:o:t:p:f",
      long_options, &option_index);
    if (c == -1)
      {
      break;
      }

    switch (c)
      {
    case 0:
        {
        const char *s = long_options[option_index].name;
        printf ("option %s", s);
        if (optarg)
          {
          if( option_index == 0 ) /* input */
            {
            assert( strcmp(s, "input") == 0 );
            assert( filename.empty() );
            filename = optarg;
            }
          else if( option_index == 4 ) /* input */
            {
            assert( strcmp(s, "pattern") == 0 );
            assert( pattern.empty() );
            pattern = optarg;
            }
          printf (" with arg %s", optarg);
          }
        printf ("\n");
        }
      break;

    case 'i':
      //printf ("option i with value '%s'\n", optarg);
      assert( filename.empty() );
      filename = optarg;
      break;

    case 'o':
      //printf ("option o with value '%s'\n", optarg);
      assert( outfilename.empty() );
      outfilename = optarg;
      break;

    case 't':
      printf ("option t with value '%s'\n", optarg);
      rawTag.ReadFromCommaSeparatedString(optarg);
      //std::cerr << rawTag << std::endl;
      break;

    case '?':
      break;

    default:
      printf ("?? getopt returned character code 0%o ??\n", c);
      }
  }

  if (optind < argc)
    {
    printf ("non-option ARGV-elements: ");
    while (optind < argc)
      {
      printf ("%s ", argv[optind++]);
      }
    printf ("\n");
    }

  if( filename.empty() )
    {
    std::cerr << "Need input file (-i)\n";
    return 1;
    }
  // else
  //std::cout << "Filename: " << filename << std::endl;
  gdcm::Reader reader;
  reader.SetFileName( filename.c_str() );
  if( !reader.Read() )
    {
    std::cerr << "Failed to read: " << filename << std::endl;
    return 1;
    }

  const gdcm::FileMetaInformation &h = reader.GetFile().GetHeader();
  //std::cout << h << std::endl;

  const gdcm::DataSet &ds = reader.GetFile().GetDataSet();
  //std::cout << ds << std::endl;

  if( !ds.FindDataElement( rawTag ) )
    {
    std::cerr << "Cannot find Tag: " << rawTag << std::endl;
    return 1;
    }

  if( outfilename.empty() )
    {
    std::cerr << "Need output file (-o)\n";
    return 1;
    }
  gdcm::Filename fn1(filename.c_str()), fn2(outfilename.c_str());
  if( fn1.IsIdentical(fn2) )
    {
    std::cerr << "Ouput is Input\n";
    return 1;
    }

  const gdcm::DataElement& pdde = ds.GetDataElement( rawTag );
  const gdcm::ByteValue *bv = pdde.GetByteValue();
  const gdcm::SequenceOfFragments *sf = pdde.GetSequenceOfFragments();
  if( bv )
    {
    std::ofstream output(outfilename.c_str(), std::ios::binary);
    bv->WriteBuffer(output);
    }
  else if( sf )
    {
    if( splitfrags )
      {
      unsigned int nfrags = sf->GetNumberOfFragments();
      for(unsigned int i = 0; i < nfrags; ++i)
        {
        const gdcm::Fragment& frag = sf->GetFragment(i);
        const gdcm::ByteValue *fragbv = frag.GetByteValue();
        std::ostringstream os;
        os << outfilename;
        if( pattern.empty() )
          {
          os << i;
          }
        else
          {
          char buffer[256]; // hope that's enough...
          sprintf(buffer, pattern.c_str(), i);
          os << buffer;
          }
        std::string outfilenamei = os.str();
        std::ofstream outputi(outfilenamei.c_str(), std::ios::binary);
        fragbv->WriteBuffer(outputi);
        }
      }
    else
      {
      std::ofstream output(outfilename.c_str(), std::ios::binary);
      sf->WriteBuffer(output);
      }
    }
  else
    {
    std::cerr << "Unhandled" << std::endl;
    return 1;
    }

  return 0;
}

