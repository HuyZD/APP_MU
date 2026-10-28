#include "xparameters.h"
#include "xiic.h"
#include "xstatus.h"
#include "i2c_temp.h"
#include "repeater.h"
#include "FreeRTOS.h"
#include "task.h"

#define IIC_DEVICE_ID  XPAR_AXI_IIC_0_DEVICE_ID

#define TMP75B_ADDR    0x48
#define REG_TEMP       0x00

static XIic IicInstance;

static void TempTask(void *arg)
{
    struct repeater_struct *pdev;
    short temp;

    pdev = (struct repeater_struct *)arg;

    while (1)
    {
         if (get_temp(0, &temp) == 0)
         {
             //pdev->temperature = temp;
        	 pdev->temperature = temp * 0.0625;
         }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int i2c_init(struct repeater_struct *pdev)
{
    int Status;

    Status = XIic_Initialize(&IicInstance, IIC_DEVICE_ID);

    if (Status != XST_SUCCESS)
        return -1;

    xTaskCreate(
        TempTask,
        "TempTask",
        1024,
        pdev,
        2,
        NULL
    );

    return 0;
}

int get_temp(int sensor, short *temp)
{
    u8 tx[1];
    u8 rx[2];
    int raw;
    int Status;

    (void)sensor;


    if (temp == NULL)
        return -1;

    tx[0] = REG_TEMP;

    Status = XIic_Send(
        IicInstance.BaseAddress,
        TMP75B_ADDR,
        tx,
        1,
        XIIC_STOP
    );

    if (Status != 1)
        return -2;

    Status = XIic_Recv(
        IicInstance.BaseAddress,
        TMP75B_ADDR,
        rx,
        2,
        XIIC_STOP
    );

    if (Status != 2)
        return -3;

    raw = ((int)rx[0] << 8) | rx[1];

    raw >>= 4;

    if (raw & 0x800)
        raw |= 0xFFFFF000;

    *temp = (short)raw;

    return 0;
}

char *TempHelp[] = {
    "Temperature",
    0
};

int Temp(int argc, char *argv[])
{
    short temp;
    int ret;

    (void)argc;
    (void)argv;

    ret = get_temp(0, &temp);

    if (ret != 0)
    {
        printf("Get temperature failed (%d)\n", ret);
        return ret;
    }

    printf("TEMP = %d.%d C\n",
           temp / 16,
           ((temp & 0x0F) * 10) / 16);

    return 0;
}
