//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list data set move undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEUNODASDATAPOOLLISTMOVECOMMAND_H
#define C_SDNDEUNODASDATAPOOLLISTMOVECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "stwtypes.h"
#include "C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeUnoDasDataPoolListMoveCommand :
   public C_SdNdeUnoDasDataPoolListAddDeleteBaseCommand
{
public:
   C_SdNdeUnoDasDataPoolListMoveCommand(const stw_types::uint32 & oru32_NodeIndex,
                                        const stw_types::uint32 & oru32_DataPoolIndex,
                                        const stw_types::uint32 & oru32_DataPoolListIndex,
                                        C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager,
                                        const std::vector<stw_types::uint32> & orc_SourceCol,
                                        const std::vector<stw_types::uint32> & orc_TargetCol,
                                        const bool & orq_AdaptIndices,
                                        QUndoCommand * const opc_Parent = NULL);
   virtual void redo(void) override;
   virtual void undo(void) override;

private:
   std::vector<stw_types::uint32> mc_SourceCol;
   std::vector<stw_types::uint32> mc_TargetCol;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif