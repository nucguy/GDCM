#ifndef GDCMSURFACEWRITER_H
#define GDCMSURFACEWRITER_H

#include <gdcmSegmentWriter.h>
#include <gdcmSurface.h>

namespace gdcm
{

class GDCM_EXPORT SurfaceWriter : public SegmentWriter
{
public:
    SurfaceWriter();

    virtual ~SurfaceWriter();

//    const Surface & GetSurface() const { return *SurfaceData; }
//    Surface & GetSurface() { return *SurfaceData; }
//    void SetSurface(Surface const & segment);

    /// Write
    bool Write(); // Execute()

    unsigned long GetNumberOfSurfaces();
    void SetNumberOfSurfaces(const unsigned long nb);

protected:

    bool PrepareWrite();

    void ComputeNumberOfSurfaces();

    //0066 0001 UL 1 Number of Surfaces
    unsigned long NumberOfSurfaces;
};

}

#endif // GDCMSURFACEWRITER_H