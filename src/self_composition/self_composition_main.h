#ifndef SELFCOMPOSITION_h
#define SELFCOMPOSITION_h

#include "base/main/main.h"
#include "base/abc/abc.h"
#include "base/io/ioAbc.h"

ABC_NAMESPACE_HEADER_START

extern Abc_Ntk_t *self_composeAbc(Abc_Frame_t *pAbc, int argc, char **argv);

#endif

ABC_NAMESPACE_HEADER_END
