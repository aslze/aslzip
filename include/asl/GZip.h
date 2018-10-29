#ifndef ASLGZIP_H
#define ASLGZIP_H

#include "export.h"
#include <asl/String.h>

// This part is still very simple and experimental!

namespace asl
{

void ASLZIP_API gzip(const String& path);

void ASLZIP_API gunzip(const String& path);

}

#endif
