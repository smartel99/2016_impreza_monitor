/**
 * @addtogroup canMonitorModule.cpp
 * @{
 *******************************************************************************
 * @file	canMonitorModule.cpp
 * @author	Samuel Martel
 * @brief
 * Created on: Apr 17, 2021
 *******************************************************************************
 */

#include "canMonitorModule.h"

#include "processes/MasterApplication.h"

#include <cstdio>

#if 0
#define LOG(msg, ...)                                                                              \
    do                                                                                             \
    {                                                                                              \
        LCD_UsrLog("[%s]: " msg "\n\r", m_label.c_str(), ##__VA_ARGS__);                           \
        LOG_INFO("[%s]: " msg, m_label.c_str(), ##__VA_ARGS__);                                    \
    } while (0)
#endif
#define LOG(msg, ...)                                                                              \
    do                                                                                             \
    {                                                                                              \
        LOG_INFO("[%s]: " msg, m_label.c_str(), ##__VA_ARGS__);                                    \
    } while (0)

CanMonitorModule::CanMonitorModule(const std::string& label) : m_label(label)
{
    LOG("Initialized");
}

bool CanMonitorModule::DoPost()
{
    LOG("POST OK");
    return true;
}


void CanMonitorModule::Run()
{
    static CanModule* can = CAN1_MODULE;
    //    static size_t     lastTime = 0;
    //    static size_t     cnt      = 0;
    //
    //    if (HAL_GetTick() >= lastTime + 1000)
    //    {
    //        lastTime = HAL_GetTick();
    //        cnt      = 0;
    //    }
    //
    //    LOG_DEBUG("%i", ++cnt);
    if (can->GetNumberOfAvailableFrames() > 0)
    {
        CEP_CAN::Frame frame = can->ReceiveFrame();

        PrintFrame(frame);
    }
}

void CanMonitorModule::PrintFrame(const CEP_CAN::Frame& frame)
{
    static char msg[256] = {0};

    size_t pos = sprintf(&msg[0], "Rcvd pkt: ");


    if (frame.frame.IDE == CAN_ID_STD)
    {
        pos += sprintf(&msg[pos], "STD: 0x%04X, ", (int)frame.frame.StdId);
    }
    else
    {
        pos += sprintf(&msg[pos], "EXT: 0x%08X, ", (int)frame.frame.ExtId);
    }

    if (frame.frame.RTR)
    {
        pos += sprintf(&msg[pos], "Remote ");
    }
    else
    {
        pos += sprintf(&msg[pos], "Data (%i)", (int)frame.frame.DLC);
        for (size_t i = 0; i < frame.frame.DLC; i++)
        {
            pos += sprintf(&msg[pos], "0x%02X, ", frame.data[i]);
        }
    }

    LOG("%s", msg);
}
