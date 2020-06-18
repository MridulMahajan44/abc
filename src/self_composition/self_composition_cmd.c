#include "base/main/main.h"
#include "base/abc/abc.h"
#include "base/io/ioAbc.h"
#include "self_composition_main.h"

ABC_NAMESPACE_IMPL_START

static int self_compose_cmd(Abc_Frame_t *pAbc, int argc, char **argv);

void self_compose_Init(Abc_Frame_t *pAbc)
{
    Cmd_CommandAdd(pAbc, "HyperLTL", "self_compose", self_compose_cmd, 1);
}

int self_compose_cmd(Abc_Frame_t *pAbc, int argc, char **argv)
{
    Abc_Ntk_t * pNtk;
    pNtk = self_composeAbc(pAbc, argc, argv);
    
    if(pNtk == NULL)
       return 1;
    
    Abc_FrameReplaceCurrentNetwork(pAbc, pNtk);
    Abc_FrameClearVerifStatus(pAbc);
    return 0;
}

ABC_NAMESPACE_IMPL_END
