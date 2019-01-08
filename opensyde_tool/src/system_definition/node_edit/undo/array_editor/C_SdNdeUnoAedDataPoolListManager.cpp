//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data pool list array edit command stack (implementation)

   Data pool list array edit command stack

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_SdNdeUnoAedDataPoolListManager.h"
#include "C_SdNdeUnoAedDataPoolListDataChangeCommand.h"
#include "C_SdNdeDataPoolListTableView.h"
#include "C_SdNdeDataPoolListTableModel.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_tgl;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     25.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeUnoAedDataPoolListManager::C_SdNdeUnoAedDataPoolListManager(void) :
   mpc_UndoCommand(NULL)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do data change

   \param[in]     oru32_NodeIndex         Node index
   \param[in]     oru32_DataPoolIndex     Node data pool index
   \param[in]     oru32_ListIndex         Node data pool list index
   \param[in]     oru32_ElementIndex      Node data pool list element index
   \param[in]     ore_ArrayEditType       Enum for node data pool list element variable
   \param[in]     oru32_DataSetIndex      If min or max use 0
                                          Else use data set index
   \param[in]     oru32_ArrayElementIndex Array index
   \param[in]     orc_NewData             New data

   \created     09.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoAedDataPoolListManager::DoDataChangeElements(const uint32 & oru32_NodeIndex,
                                                            const uint32 & oru32_DataPoolIndex,
                                                            const uint32 & oru32_ListIndex,
                                                            const uint32 & oru32_ElementIndex,
                                                            const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType, const uint32 & oru32_DataSetIndex, const uint32 & oru32_ArrayElementIndex, const QVariant & orc_NewData,
                                                            C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager)
{
   QUndoCommand * pc_Command;

   m_InitUndoCommand();
   pc_Command = new C_SdNdeUnoAedDataPoolListDataChangeCommand(
      oru32_NodeIndex,
      oru32_DataPoolIndex, oru32_ListIndex,
      oru32_ElementIndex,
      ore_ArrayEditType, oru32_DataSetIndex, oru32_ArrayElementIndex, orc_NewData, opc_DataPoolListModelViewManager,
      this->mpc_UndoCommand);
   //Do action once
   pc_Command->redo();
   //lint -e{429}  no memory leak because of the parent of pc_Command and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   GetUndocommand and take ownership

   Internal undo command is reseted

   \return
   NULL No changes
   Else Undocommand accumulating all changes

   \created     16.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QUndoCommand * C_SdNdeUnoAedDataPoolListManager::TakeUndoCommand(void)
{
   QUndoCommand * const pc_Retval = this->mpc_UndoCommand;

   this->mpc_UndoCommand = NULL;
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Init data set undo command

   \created     09.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeUnoAedDataPoolListManager::m_InitUndoCommand(void)
{
   if (this->mpc_UndoCommand == NULL)
   {
      this->mpc_UndoCommand = new QUndoCommand("Change array element(s)");
   }
}