
#ifndef DIRECTFB_COMMON_H
#define DIRECTFB_COMMON_H

#include <directfb.h>

#define DFBCHECK(x...) \
     {                                                                \
          err = x;                                                    \
          if (err != DFB_OK) {                                        \
               fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
               DirectFBErrorFatal( #x, err );                         \
          }                                                           \
     }

extern IDirectFB* directfb_get(void);

#endif
