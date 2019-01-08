//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for data pool entry with delete functionality (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.10.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDBDATAPOOLENTRY_H
#define C_SDNDEDBDATAPOOLENTRY_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SdNdeDbDataPoolEntry;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDbDataPoolEntry :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbDataPoolEntry(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                                   QWidget * const opc_Parent = NULL);
   ~C_SdNdeDbDataPoolEntry(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDeleteDataPool(C_SdNdeDbDataPoolEntry * const opc_Source, const stw_types::uint32 ou32_Index);

private:
   Ui::C_SdNdeDbDataPoolEntry * mpc_Ui;
   const stw_types::uint32 mu32_DataPoolIndex;

   void m_OnDeleteClick(void);
   void m_Init(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex) const;

   //Avoid call
   C_SdNdeDbDataPoolEntry(const C_SdNdeDbDataPoolEntry &);
   C_SdNdeDbDataPoolEntry & operator =(const C_SdNdeDbDataPoolEntry &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif