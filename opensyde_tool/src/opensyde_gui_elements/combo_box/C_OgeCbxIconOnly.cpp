//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Custom combo box with icons only (implementation)

   Custom combo box.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.11.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeCbxIconDelegate.h"
#include "C_OgeCbxIconOnly.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     07.11.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeCbxIconOnly::C_OgeCbxIconOnly(QWidget * const opc_Parent) :
   C_OgeCbxToolTipBase(opc_Parent)
{
   //this code allows to handle the QAbstractItemView::item in stylesheets
   C_OgeCbxIconDelegate * const pc_ItemDelegate = new C_OgeCbxIconDelegate();

   this->setItemDelegate(pc_ItemDelegate);
   //lint -e{429} Qt takes ownership of delegate (look at docu)
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     13.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeCbxIconOnly::~C_OgeCbxIconOnly(void)
{
}