/**
 * @addtogroup canMonitorModule.h
 * @{
 *******************************************************************************
 * @file	canMonitorModule.h
 * @author	Samuel Martel
 * @brief
 * Created on: Apr 17, 2021
 *******************************************************************************
 */

#ifndef CANMONITORMODULE_H_
#define CANMONITORMODULE_H_

/***********************************************/
/* Includes */
#include "shared/defines/module.hpp"

#include "shared/drivers/canModule.hpp"

/***********************************************/
/* Defines */
class CanMonitorModule : public cep::Module
{
public:
    CanMonitorModule(const std::string& label);
    virtual ~CanMonitorModule() override = default;

    virtual bool               DoPost() override;
    virtual void               Run() override;
    virtual const std::string& GetLabel() const { return m_label; }

private:
    std::string m_label = "";

private:
    void PrintFrame(const CEP_CAN::Frame& frame);
};



/***********************************************/
/* Function declarations */



/**
 * @}
 */
/* END OF FILE */
#endif /* CANMONITORMODULE_H_ */
