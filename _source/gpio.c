#include "gpio.h"

XGpioPs Gpio;

int BrGpioInit(void)
{
    int i;

    for (i = 0; i < BR_COUNT; i++)
    {
        XGpioPs_SetDirectionPin(&Gpio,
                                br_pwr_enable_gpio[i],
                                1);

        XGpioPs_SetOutputEnablePin(&Gpio,
                                   br_pwr_enable_gpio[i],
                                   1);


    }

    for (i = 0; i < BR_COUNT; i++)
    {
        XGpioPs_SetDirectionPin(&Gpio,
                                br_pwr_mon_gpio[i],
                                0);

    }
    InitBr();
    return XST_SUCCESS;
}

int gpio_init(void)
{
    XGpioPs_Config *ConfigPtr;
    int Status;

    ConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
    if (ConfigPtr == NULL)
        return XST_FAILURE;

    Status = XGpioPs_CfgInitialize(&Gpio, ConfigPtr, ConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS)
        return XST_FAILURE;
#if 0
//// PLL
	XGpioPs_SetDirectionPin(&Gpio, MIO_PLL_B3_EN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_PLL_B3_EN, 1);


	XGpioPs_SetDirectionPin(&Gpio, MIO_PLL_DATA, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_PLL_DATA, 1);

	XGpioPs_SetDirectionPin(&Gpio, MIO_PLL_CLK, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_PLL_CLK, 1);


	XGpioPs_SetDirectionPin(&Gpio, MIO_PLL_B28_EN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_PLL_B28_EN, 1);


	XGpioPs_SetDirectionPin(&Gpio, MIO_PLL_B3_LD, 0);			////Input
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_PLL_B3_LD, 0);		//	Disable Output
	
	XGpioPs_SetDirectionPin(&Gpio, MIO_PLL_B28_LD, 0);			////Input
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_PLL_B28_LD, 0);		//	Disable Output

//// ATT
	XGpioPs_SetDirectionPin(&Gpio, MIO_ATT1_B3_EN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_ATT1_B3_EN, 1);

	XGpioPs_SetDirectionPin(&Gpio, MIO_ATT2_B3_EN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_ATT2_B3_EN, 1);


	XGpioPs_SetDirectionPin(&Gpio, MIO_ATT_DATA, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_ATT_DATA, 1);


	XGpioPs_SetDirectionPin(&Gpio, MIO_ATT_CLK, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_ATT_CLK, 1);

	XGpioPs_SetDirectionPin(&Gpio, MIO_ATT1_B28_EN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_ATT1_B28_EN, 1);

	XGpioPs_SetDirectionPin(&Gpio, MIO_ATT2_B28_EN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, MIO_ATT2_B28_EN, 1);
#endif
	BrGpioInit();
    return XST_SUCCESS;
}


void PLL_ClkWrite(unsigned char val)
{
	if (val == 1)
		XGpioPs_WritePin(&Gpio, MIO_PLL_CLK, 1); 	
	else if (val == 0)
		XGpioPs_WritePin(&Gpio, MIO_PLL_CLK, 0);	
}

void PLL_DataWrite(unsigned char val)
{
	if (val == 1)
		XGpioPs_WritePin(&Gpio, MIO_PLL_DATA, 1); 	
	else if (val == 0)
		XGpioPs_WritePin(&Gpio, MIO_PLL_DATA, 0);	
}

void PLL_EnWrite(unsigned char id, unsigned char val)
{
	if (id == 0) {
		if (val == 1)
			XGpioPs_WritePin(&Gpio, MIO_PLL_B28_EN, 1);		
		else if (val == 0)
			XGpioPs_WritePin(&Gpio, MIO_PLL_B28_EN, 0);		
	}
	else if (id == 1) {
		if (val == 1)
			XGpioPs_WritePin(&Gpio, MIO_PLL_B3_EN, 1);	
		else if (val == 0)
			XGpioPs_WritePin(&Gpio, MIO_PLL_B3_EN, 0);	
	}
}






void ATT_ClkWrite(unsigned char val)
{
	if (val == 1)
		XGpioPs_WritePin(&Gpio, MIO_ATT_CLK, 1); 		
	else if (val == 0)
		XGpioPs_WritePin(&Gpio, MIO_ATT_CLK, 0);		
}


////B28
void ATT1_EnWrite(unsigned char id, unsigned char val)
{
	if (id == 0) { ///DL
		if (val == 1)
			XGpioPs_WritePin(&Gpio, MIO_ATT1_B28_EN, 1);
		else if (val == 0)
			XGpioPs_WritePin(&Gpio, MIO_ATT1_B28_EN, 0);
	}
	else if (id == 1) { ///UL
		if (val == 1)
			XGpioPs_WritePin(&Gpio, MIO_ATT2_B28_EN, 1);
		else if (val == 0)
			XGpioPs_WritePin(&Gpio, MIO_ATT2_B28_EN, 0);
	}
}

////B3
void ATT2_EnWrite(unsigned char id, unsigned char val)
{
	if (id == 0) { ///DL
		if (val == 1)
			XGpioPs_WritePin(&Gpio, MIO_ATT1_B3_EN, 1);	
		else if (val == 0)
			XGpioPs_WritePin(&Gpio, MIO_ATT1_B3_EN, 0);	
	}
	else if (id == 1) { /// UL
		if (val == 1)
			XGpioPs_WritePin(&Gpio, MIO_ATT2_B3_EN, 1);	
		else if (val == 0)
			XGpioPs_WritePin(&Gpio, MIO_ATT2_B3_EN, 0);	
	}
}


void ATT_DataWrite(unsigned char val)
{
	if (val == 1)
		XGpioPs_WritePin(&Gpio, MIO_ATT_DATA, 1); 
	else if (val == 0)
		XGpioPs_WritePin(&Gpio, MIO_ATT_DATA, 0); 
}
///////////////////
int isPllLocked_Band(int band)
{	
	extern XGpioPs 		Gpio;
	
	if(band == 0)
	{
		return (XGpioPs_ReadPin(&Gpio, MIO_PLL_B28_LD));
	}
	else
	{
		return (XGpioPs_ReadPin(&Gpio, MIO_PLL_B3_LD));

	}
}

