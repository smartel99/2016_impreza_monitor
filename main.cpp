/**
 * @addtogroup  main
 * @{
 * @file        main.cpp
 * @author      Pascal-Emmanuel Lachance
 */
/*************************************************************************************************/
/* File includes ------------------------------------------------------------------------------- */

#include "processes/MasterApplication.h"

#include "shared/services/logger.hpp"

#include "Core/Inc/main.h"

/*************************************************************************************************/
/* Private functions declarations -------------------------------------------------------------- */
void SystemClock_Config();

/*************************************************************************************************/
/* Global variables ---------------------------------------------------------------------------- */

/*************************************************************************************************/
/* main ---------------------------------------------------------------------------------------- */
int main()
{
    /* Enable I-Cache---------------------------------------------------------*/
    SCB_EnableICache();

    /* Enable D-Cache---------------------------------------------------------*/
    SCB_EnableDCache();
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    // Make sure that SYSCLK isn't set to PLL before configuring it.
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_SW);

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize program */
    MasterApplication app;

    app.Init();

    if (app.DoPost() == true)
    {
        app.Run();
    }

    LOG_ERROR("Application exited, please restart the device.");
    /* We should never get here as control is now taken by the scheduler */
    /* Infinite loop */
    while (true)
    {
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
void                   SystemClock_Config(void)
{
    RCC_OscInitTypeDef       RCC_OscInitStruct   = {0};
    RCC_ClkInitTypeDef       RCC_ClkInitStruct   = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure LSE Drive Capability
     */
    HAL_PWR_EnableBkUpAccess();
    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM            = 25;
    RCC_OscInitStruct.PLL.PLLN            = 432;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ            = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Activate the Over-Drive mode
     */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC | RCC_PERIPHCLK_USART1 |
                                               RCC_PERIPHCLK_SDMMC2 | RCC_PERIPHCLK_CLK48 |
                                               RCC_PERIPHCLK_CEC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN       = 192;
    PeriphClkInitStruct.PLLSAI.PLLSAIR       = 2;
    PeriphClkInitStruct.PLLSAI.PLLSAIQ       = 3;
    PeriphClkInitStruct.PLLSAI.PLLSAIP       = RCC_PLLSAIP_DIV4;
    PeriphClkInitStruct.PLLSAIDivQ           = 1;
    PeriphClkInitStruct.PLLSAIDivR           = RCC_PLLSAIDIVR_2;
    PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInitStruct.CecClockSelection    = RCC_CECCLKSOURCE_HSI;
    PeriphClkInitStruct.Clk48ClockSelection  = RCC_CLK48SOURCE_PLLSAIP;
    PeriphClkInitStruct.Sdmmc2ClockSelection = RCC_SDMMC2CLKSOURCE_CLK48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_1);
}

#pragma GCC diagnostic pop

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler()
{
    while (true)
    {
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    UNUSED(file);
    UNUSED(line);
    while (true)
    {
    }
}
#endif /* USE_FULL_ASSERT */

/*************************************************************************************************/
/**
 * @}
 */
/* ----- END OF FILE ----- */
