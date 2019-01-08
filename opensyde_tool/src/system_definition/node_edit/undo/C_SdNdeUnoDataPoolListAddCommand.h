//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list add undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEUNODATAPOOLLISTADDCOMMAND_H
#define C_SDNDEUNODATAPOOLLISTADDCOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "C_SdNdeUnoDataPoolListAddDeleteBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeUnoDataPoolListAddCommand :
   public C_SdNdeUnoDataPoolListAddDeleteBaseCommand
{
public:
   C_SdNdeUnoDataPoolListAddCommand(const stw_types::uint32 & oru32_NodeIndex,
                                    const stw_types::uint32 & oru32_DataPoolIndex,
                                    stw_opensyde_gui::C_SdNdeDataPoolListsTreeWidget * const opc_DataPoolListsTreeWidget,
                                    const std::vector<stw_types::uint32> & orc_Indices,
                                    QUndoCommand * const opc_Parent = NULL);
   virtual void redo(void) override;
   virtual void undo(void) override;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif