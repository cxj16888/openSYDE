//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for HALC use case configuration.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCONFIGTREEVIEW_H
#define C_SDNDEHALCCONFIGTREEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.h"
#include "C_SdNdeHalcConfigTreeModel.h"
#include "C_SdNdeHalcConfigTreeDelegate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcConfigTreeView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
public:
   C_SdNdeHalcConfigTreeView(QWidget * const opc_Parent = NULL);

private:
   stw_opensyde_gui_logic::C_SdNdeHalcConfigTreeDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
