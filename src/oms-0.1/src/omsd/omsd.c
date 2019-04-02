#include <stdio.h>
#include <unistd.h>
#include "omsd.h"

/*
void MyEventFunction(void *args)
{
  printf("MYEVENT %d CALLED\n", (int) args );
}

void KaltReset(void *args)
{
  puts("KaltReset");
}
void WarmReset(void *args)
{
  puts("WarmReset");
}*/

int main(void)
{
	tOmsDevice *dev_oms;

	dev_oms = oms_OpenDevice("/dev/input/oms_switch", OMS_MODE_ACTIVE);
	if(dev_oms == NULL)
	{
	  dev_oms = oms_OpenDevice("/dev/input/event0", OMS_MODE_ACTIVE);
	}

  if(!readConfigDir("/etc/oms.d"))
  {
    //oms_SM_PrintSwitches();
    if(dev_oms != NULL)
    {
      oms_Watch(dev_oms);
    }
  }

	return 0;
}

