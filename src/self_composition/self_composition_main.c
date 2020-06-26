#include "base/main/main.h"
#include "base/abc/abc.h"
#include "base/io/ioAbc.h"

ABC_NAMESPACE_IMPL_START

Abc_Ntk_t *self_compose(Abc_Ntk_t *pNtk1, Abc_Ntk_t *pNtk2, char *var_list);

Abc_Ntk_t *self_composeAbc(Abc_Frame_t *pAbc, int argc, char **argv)
{
    char * pFileName;
    Abc_Ntk_t *pNtk1, *pNtk2, *result;
    Extra_UtilGetoptReset();
    pFileName1 = argv[globalUtilOptind+1];
    pFileName2 = argv[globalUtilOptind+2];
    pNtk1 = Io_Read(pFileName1, IO_FILE_VERILOG, 0, 0); 
    pNtk2 = Io_Read(pFileName2, IO_FILE_VERILOG, 0, 0);  

    if(pNtk1 == NULL || pNtk2 == NULL){
        Abc_Print(-1, "self_compose: Getting the target network has failed.\n");
        return NULL;
    }  

    result = self_compose(pNtk1, pNtk2, pFileName2);

    return result;
} 

Abc_Ntk_t *self_compose(Abc_Ntk_t *pNtk1, Abc_Ntk_t *pNtk2, char *var_list)
{
    int fd = dup(fileno(stdin)), n1, n2, i;
    
    if(freopen(var_list, "r", stdin)==NULL)
    {
      printf("self_composition: Opening the variable list file has failed.\n");
      dup2(fd, fileno(stdin));
      close(fd);
      return NULL;
    }

    if(scanf("%d %d", &n1, &n2)<2)
    {
      printf("self_composition: The variable list is not valid.\n");
      dup2(fd, fileno(stdin));
      close(fd);
      return NULL;
    }

    int assump_vars[n1], assert_vars[n2];

    for(i=0; i<n1; i++)
      if(scanf("%d", &assump_vars[i])<1)
      {
         printf("self_composition: The variable list isn't valid.\n");
         dup2(fd, fileno(stdin));
         close(fd);
         return NULL;
       }
    for(i=0; i<n2; i++)
       if(scanf("%d", &assert_vars[i])<1)
       {
         printf("self_composition: The variable list isn't valid.\n");
         dup2(fd, fileno(stdin));
         close(fd);
         return NULL;
       }

    dup2(fd, fileno(stdin));
    close(fd);
    
    char Buffer[1000];
    Abc_Ntk_t * pNtkSelfComp;

    pNtk1 = Abc_NtkStrash(pNtk1, 0, 0, 0);
    pNtk2 = Abc_NtkStrash(pNtk2, 0, 0, 0);

    pNtkSelfComp = Abc_NtkAlloc(ABC_NTK_STRASH, ABC_FUNC_AIG, 1);
    sprintf(Buffer, "Self-Composed_%s", pNtk1->pName);
    pNtkSelfComp->pName = Extra_UtilStrsav(Buffer);

    Abc_Obj_t *pObj, *pObjNew;
    int i;

    Abc_AigConst1(pNtk1)->pCopy = Abc_AigConst1(pNtkSelfComp);
    Abc_AigConst1(pNtk2)->pCopy = Abc_AigConst1(pNtkSelfComp);

    Abc_NtkForEachPi(pNtk1, pObj, i)
    {
       pObjNew = Abc_NtkCreatePi(pNtkSelfComp);
       pObj->pCopy = pObjNew;
       Abc_ObjAssignName(pObjNew, Abc_ObjName(pObj), "_1");
       pObj = Abc_NtkCi(pNtk2, i);  
       pObjNew = Abc_NtkCreatePi(pNtkSelfComp);
       pObj->pCopy = pObjNew;
       Abc_ObjAssignName(pObjNew, Abc_ObjName(pObj), "_2");
    }

    Abc_NtkForEachLatch(pNtk1, pObj, i)
    {
       pObjNew = Abc_NtkDupBox(pNtkSelfComp, pObj, 0);
       Abc_ObjAssignName(pObjNew, Abc_ObjName(pObj), "_1");
       Abc_ObjAssignName(Abc_ObjFanin0(pObjNew),  Abc_ObjName(Abc_ObjFanin0(pObj)), "_1");
       Abc_ObjAssignName(Abc_ObjFanout0(pObjNew), Abc_ObjName(Abc_ObjFanout0(pObj)), "_1");
    }
    Abc_NtkForEachLatch(pNtk2, pObj, i)
    {
       pObjNew = Abc_NtkDupBox(pNtkSelfComp, pObj, 0);
       Abc_ObjAssignName(pObjNew, Abc_ObjName(pObj), "_2");
       Abc_ObjAssignName(Abc_ObjFanin0(pObjNew),  Abc_ObjName(Abc_ObjFanin0(pObj)), "_2");
       Abc_ObjAssignName(Abc_ObjFanout0(pObjNew), Abc_ObjName(Abc_ObjFanout0(pObj)), "_2");
     }

     Abc_Obj_t *pNode;

     assert(Abc_NtkIsDfsOrdered(pNtk1));
     Abc_AigForEachAnd(pNtk1, pNode, i)
     pNode->pCopy = Abc_AigAnd((Abc_Aig_t *)pNtkSelfComp->pManFunc, Abc_ObjChild0Copy(pNode), Abc_ObjChild1Copy(pNode));

     assert(Abc_NtkIsDfsOrdered(pNtk2));
     Abc_AigForEachAnd(pNtk2, pNode, i)
     pNode->pCopy = Abc_AigAnd((Abc_Aig_t *)pNtkSelfComp->pManFunc, Abc_ObjChild0Copy(pNode), Abc_ObjChild1Copy(pNode));  
        
     Abc_NtkForEachLatch(pNtk1, pNode, i )
     Abc_ObjAddFanin(Abc_ObjFanin0(pNode)->pCopy, Abc_ObjChild0Copy(Abc_ObjFanin0(pNode)));
     Abc_NtkForEachLatch(pNtk2, pNode, i )
     Abc_ObjAddFanin(Abc_ObjFanin0(pNode)->pCopy, Abc_ObjChild0Copy(Abc_ObjFanin0(pNode)));
        
     Abc_Obj_t *o1, *o2, *oA;
     
     for(i=0; i<n1; i++)
     {
       o1 = Abc_NtkPo(pNtk1, assump_vars[i]);
       o2 = Abc_NtkPo(pNtk2, assump_vars[i]);
       oA = Abc_ObjNot(Abc_AigXor((Abc_Aig_t *)pNtkSelfComp->pManFunc, Abc_ObjChild0Copy(o1), Abc_ObjChild0Copy(o2)));
       pObjNew = Abc_NtkCreatePo(pNtkSelfComp);
       Abc_ObjAddFanin(pObjNew, oA);
       Abc_ObjAssignName(pObjNew, Abc_ObjName(o1), "_comp");
     }
     for (i=0; i<n2; i++)
     {
       o1 = Abc_NtkPo(pNtk1, assert_vars[i]);
       o2 = Abc_NtkPo(pNtk2, assert_vars[i]);
       oA = Abc_ObjNot(Abc_AigXor((Abc_Aig_t *)pNtkSelfComp->pManFunc, Abc_ObjChild0Copy(o1), Abc_ObjChild0Copy(o2)));
       pObjNew = Abc_NtkCreatePo(pNtkSelfComp);
       Abc_ObjAddFanin(pObjNew, oA);
       Abc_ObjAssignName(pObjNew, Abc_ObjName(o1), "_comp");
      }

    Abc_AigCleanup((Abc_Aig_t *)pNtkSelfComp->pManFunc);

    if (!Abc_NtkCheck(pNtkSelfComp))
    {
        printf("self_composition: The network check has failed.\n");
        Abc_NtkDelete(pNtkSelfComp);
        return NULL;
    }
    return pNtkSelfComp;
}

ABC_NAMESPACE_IMPL_END
