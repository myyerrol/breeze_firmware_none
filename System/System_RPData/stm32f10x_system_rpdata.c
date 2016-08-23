#include "stm32f10x_system_rpdata.h"
#include "stm32f10x_system_nrf24l01.h"

NRF_GetData NRF_Data;
uint8_t  appCmdFlag = 0;
uint8_t  flyLogApp = 0;
uint8_t  armState = DISARMED;
uint16_t rcData[4] = {1500, 1500, 1500, 1500};

void ReceiveDataFromNRF(void)
{
    if ((NRF24L01_RXDATA[0] == '$') && (NRF24L01_RXDATA[1] == 'M') && (NRF24L01_RXDATA[2] == '<'))
    {
        switch (NRF24L01_RXDATA[4])
        {
            case MSP_SET_4CON:
                rcData[THROTTLE] = NRF24L01_RXDATA[5] + (NRF24L01_RXDATA[6] << 8);
                rcData[YAW] = NRF24L01_RXDATA[7] + (NRF24L01_RXDATA[8] << 8);
                rcData[PITCH] = NRF24L01_RXDATA[9] + (NRF24L01_RXDATA[10] << 8);
                rcData[ROLL] = NRF24L01_RXDATA[11] + (NRF24L01_RXDATA[12] << 8);
            break;
            
            case MSP_ARM_IT:
                armState = REQ_ARM;
            break;
            
            case MSP_DISARM_IT:
                armState = REQ_DISARM;
            break;
            
            case MSP_ACC_CALI:
                //imuCaliFlag = 1;
            break;
        }
    }
}

void PrcessDataFromNRF(void)
{
    CONSTRAIN(rcData[PITCH], 1000, 2000);
	CONSTRAIN(rcData[YAW], 1000, 2000);
	CONSTRAIN(rcData[ROLL], 1000, 2000);
    CONSTRAIN(rcData[THROTTLE], 1000, 2000);
    
    NRF_Data.throttle = rcData[THROTTLE] - 1000;
	NRF_Data.yaw = YAW_RATE_MAX * dbScaleLinear((rcData[YAW] - 1500),500,APP_YAW_DB);
	NRF_Data.pitch = Angle_Max * dbScaleLinear((rcData[PITCH] - 1500),500,APP_PR_DB);
	NRF_Data.roll= Angle_Max * dbScaleLinear((rcData[ROLL] - 1500),500,APP_PR_DB);
    
//    switch (armState)
//    {
//        case REQ_ARM:
//            if (IMUCheck() && !Battery.alarm)
//            {
//                armState = ARMED;
//                FLY_ENABLE = 0xA5;
//			}
//            else
//            {
//                armState = DISARMED;
//                FLY_ENABLE = 0;				
//			}
//        break;

//        case REQ_DISARM:
//            armState = DISARMED;
//            FLY_ENABLE = 0;
//            altCtrlMode = MANUAL;		//上锁后加的处理
//            zIntReset = 1;
//            thrustZSp = 0;	
//            thrustZInt = estimateHoverThru();
//            offLandFlag = 0;            
//        break;
//        
//        default:
//            break;
//	}
}

//cut deadband, move linear
float dbScaleLinear(float x, float x_end, float deadband)
{
	if (x > deadband)
    {
        return (x - deadband) / (x_end - deadband);
    }
    else if (x < -deadband)
    {
        return (x + deadband) / (x_end - deadband);
	}
    else
    {
		return 0.0f;
	}
}