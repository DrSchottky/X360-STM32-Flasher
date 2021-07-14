#include "XPower.h"
#include "XSPI.h"

void PowerUp(void)
{
	XSPI_LeaveFlashmode();
	XSPI_Powerup();
}

void Shutdown(void)
{
	XSPI_LeaveFlashmode();
	XSPI_Shutdown();
}