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
    Extra_UtilGetoptReset();
    
    if((Extra_UtilGetopt(argc, argv, "h"))!=EOF)
       goto usage;
    
    if(argc!=(globalUtilOptind+2))
    {
       printf("self_compose: Input and variable list file names should be given on the command line.\n");
       return 0;
    } 

    Abc_Ntk_t * pNtk;
    pNtk = self_composeAbc(pAbc, argc, argv);
    
    if(pNtk == NULL)
       return 1;
    
    Abc_FrameReplaceCurrentNetwork(pAbc, pNtk);
    Abc_FrameClearVerifStatus(pAbc);
    return 0;
    
    usage:
    Abc_Print(-2, "usage: self_compose [-h] <verilog_file_name> <variable_list_file_name>\n");
    Abc_Print(-2, "\t         builds a self-composed AIG from the verilog file\n");
    Abc_Print(-2, "\t-h     : print the command usage\n");
    return 1;
}

ABC_NAMESPACE_IMPL_END
