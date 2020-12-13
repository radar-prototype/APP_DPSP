
#include "Sim.h"
                      
#include "Dpc.h"
                      
int main()
{
        
Sim SIM;
SIM.init();
SIM.run();
                      
Dpc DPC;
DPC.init();
DPC.run();
                      
return 0;
}
        
