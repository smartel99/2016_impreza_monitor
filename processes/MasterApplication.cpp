/**
 ******************************************************************************
 * @addtogroup MasterApplication
 * @{
 * @file    MasterApplication
 * @author  Samuel Martel
 * @brief   Source for the MasterApplication module.
 *
 * @date 2021/04/09
 *
 ******************************************************************************
 */
#include "MasterApplication.h"

#include "defines/pin.h"

#include "Core/Inc/main.h"
#include "Core/Inc/can.h"
#include "Core/Inc/dma2d.h"
#include "Core/Inc/dsihost.h"
#include "Core/Inc/hdmi_cec.h"
#include "Core/Inc/ltdc.h"
#include "Core/Inc/sdmmc.h"
#include "Core/Inc/usart.h"
#include "Core/Inc/gpio.h"
#include "Core/Inc/fmc.h"
#include "FATFS/App/fatfs.h"

#include "shared/drivers/canModule.hpp"

#include "processes/canMonitorModule.h"

#include "Drivers/BSP/stm32f769i_discovery_ts.h"

MasterApplication* MasterApplication::s_instance = nullptr;

FATFS s_disk;
FIL   s_file;

MasterApplication::MasterApplication()
{
    CEP_ASSERT(s_instance == nullptr, "Cannot have multiple instances of Application!");
    s_instance = this;
}

void MasterApplication::Init()
{
    InitializeHAL();
    InitializeServices();
    InitializeModules();
}

bool MasterApplication::DoPost()
{
    m_logger->Log("\n\r----- Started POST...\n\r");

    bool allModulesPassedPost = true;

    for (auto& module : s_instance->m_modules)
    {
        if (module.second->DoPost() == false)
        {
            allModulesPassedPost = false;
        }
    }

    return allModulesPassedPost;
}

void MasterApplication::Run()
{
    static size_t lastSync = 0;

    if (HAL_GetTick() >= lastSync + 250)
    {
        lastSync = HAL_GetTick();
        LOG_INFO("Saving log to file...");
        f_sync(&s_file);
        LOG_INFO("Done! Took %i ms", HAL_GetTick() - lastSync);
    }
    while (true)
    {
        for (auto& module : s_instance->m_modules)
        {
            module.second->Run();
        }
    }
}

cep::Module* MasterApplication::GetModule(const std::string& moduleName)
{
    return s_instance->m_modules.at(moduleName);
}

/*****************************************************************************/
/* Private Method Definitions                                                */
/*****************************************************************************/
void MasterApplication::InitializeHAL()
{
    // HAL devices initialization
    MX_GPIO_Init();
    MX_DMA2D_Init();
    MX_DSIHOST_DSI_Init();
    MX_HDMI_CEC_Init();
    MX_USART1_UART_Init();
    MX_CAN1_Init();

    // Just initialize the SD peripheral if there's an SD card inserted.
    if (HAL_GPIO_ReadPin(uSD_Detect_GPIO_Port, uSD_Detect_Pin) == GPIO_PIN_RESET)
    {
        MX_SDMMC2_SD_Init();
        MX_FATFS_Init();
    }
    // Handled by the BSP.
    //    MX_FMC_Init();
    //    MX_LTDC_Init();
}

void MasterApplication::InitializeServices()
{
    BSP_LCD_Init();

    BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);

    BSP_LCD_SelectLayer(1);

    BSP_LCD_DisplayOn();

    LCD_LOG_Init();

    LCD_LOG_SetHeader((uint8_t*)"CAN Monitoring system");

    LCD_UsrLog("Initialized\n\r");
}

void MasterApplication::InitializeModules()
{
    // --- Drivers ---

    // Uart module for debug purposes.
    UartModule* uart1 = new UartModule(&huart1, "uart1");
    // Enable UART interrupts, low-ish priority.
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    AddModule(uart1);

    if (HAL_GPIO_ReadPin(uSD_Detect_GPIO_Port, uSD_Detect_Pin) == GPIO_PIN_RESET)
    {
        LCD_UsrLog("Mounting disk...");
        FRESULT r = f_mount(&s_disk, "", 1);
        if (r == FR_OK)
        {
            LCD_UsrLog("    Done!\n");
            char info[34] = {0};
            r             = f_getlabel("", info, nullptr);
            if (r != FR_OK)
            {
                LCD_UsrLog("Error reading disk information! %i\n", r);
            }
            else
            {
                LCD_UsrLog("Mounted disk '%s'\n", info);

                LCD_UsrLog("Opening log file...");


                r = f_open(&s_file, "log.txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);
                if (r != FR_OK)
                {
                    LCD_UsrLog("\nUnable to open log file! %i\n", r);
                }
                else
                {
                    LCD_UsrLog("    Done!\n");
                    m_logger = new Logger(uart1);
                    //                    m_logger = new Logger(uart1, [=](const char* msg, size_t
                    //                    len) {
                    //                        size_t written = 0;
                    //                        f_write(&s_file, reinterpret_cast<const void*>(msg),
                    //                        len, &written);
                    //                    });
                    m_logger->Log("Content of log.txt:\n\r");
                    char line[100];
                    while (f_gets(line, sizeof(line), &s_file))
                    {
                        m_logger->Log("%s", line);
                    }
                    m_logger->Log("\n\n\rDone dumping log.txt\n\r");
                    size_t written = 0;
                    f_write(&s_file, reinterpret_cast<const void*>("pog\n\r"), 5, &written);
                }
            }
        }
        else
        {
            LCD_UsrLog("\nUnable to mount disk! %i\n", r);
        }
    }
    else
    {
        m_logger = new Logger(uart1);
    }
    //    m_logger = new Logger(uart1, [](const char* msg, size_t len) { LCD_UsrLog(msg); });
    m_logger->Log("\n\n\r");
    m_logger->Log(
      "===============================================================================\n\r");
    m_logger->Log(
      "|                      Impreza On-board Computer Monitor                      |\n\r");
    m_logger->Log(
      "|                          (c) 2021 - Samuel Martel                           |\n\r");
    m_logger->Log(
      "|                         Developed by Samuel Martel                          |\n\r");
    m_logger->Log("|                        Built: %s\t%s                      |\n\r",
                  __DATE__,
                  __TIME__);
#if defined(DEBUG)
    m_logger->Log(
      "|                            !!! DEBUG VERSION !!!                            |\n\r");
#endif
    m_logger->Log(
      "===============================================================================\n\r");

    m_logger->Log("\n\r----- Initializing drivers...\n\r");


    auto can = new CanModule(&hcan1, "can1");
    AddModule(can);

    // CAN filter configuration
    // TODO Set CAN filters
    CEP_CAN::FilterConfiguration filter;
    can->ConfigureFilter(filter);

    can->EnableInterrupts({CEP_CAN::Irq::TxMailboxEmpty,
                           CEP_CAN::Irq::Fifo0MessagePending,
                           CEP_CAN::Irq::Fifo0Full,
                           CEP_CAN::Irq::Fifo0Overrun,
                           CEP_CAN::Irq::Fifo1MessagePending,
                           CEP_CAN::Irq::Fifo1Full,
                           CEP_CAN::Irq::Fifo1Overrun,
                           CEP_CAN::Irq::Wakeup,
                           CEP_CAN::Irq::SleepAck,
                           CEP_CAN::Irq::ErrorWarning,
                           CEP_CAN::Irq::ErrorPassive,
                           CEP_CAN::Irq::BusOffError,
                           CEP_CAN::Irq::LastErrorCode,
                           CEP_CAN::Irq::ErrorStatus});

    m_logger->Log("Done!\n\n\r----- Initializing services...\n\r");


    m_logger->Log("Done!\n\n\r----- Initializing processes...\n\r");
    AddModule(new CanMonitorModule("CAN MON"));

    m_logger->Log("Done!\n\n\r----- Initialization completed!\n\r");

    // IRQ enable after everything is initialized to prevent calling interrupts without handlers
    // ready.
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
}
