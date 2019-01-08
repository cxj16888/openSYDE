//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Add undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYADDCOMMAND_H
#define C_SDMANUNOTOPOLOGYADDCOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "C_SdManUnoTopologyAddBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdManUnoTopologyAddCommand :
   public C_SdManUnoTopologyAddBaseCommand
{
public:
   enum E_ElementType
   {
      eNODE,          ///< Data element for node
      eCAN_BUS,       ///< Data element for can bus
      eETHERNET_BUS,  ///< Data element for bus
      eLINE_ARROW,    ///< Data element for line arrow
      eBOUNDARY,      ///< Data element for boundary
      eTEXT_ELEMENT,  ///< Data element for text element
      eIMAGE,         ///< Data element for image element
      eBUS_CONNECTOR, ///< Data element for bus connector
      eUNKNOWN
   };

   C_SdManUnoTopologyAddCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                const E_ElementType & ore_Type, const QPointF & orc_NewPos,
                                const QString & orc_AdditionalInformation = "", QUndoCommand * const opc_Parent = NULL,
                                const bool & orq_ForceUseAdditionalInformation = false);
   C_SdManUnoTopologyAddCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                const E_ElementType & ore_Type, const QPointF & orc_NewPos,
                                const stw_types::uint64 & oru64_BusConnectorNodeID = 0,
                                const stw_types::uint64 & oru64_BusConnectorBusID = 0,
                                const stw_types::uint8 & oru8_InterfaceNumber = 0,
                                const stw_types::uint8 & oru8_NodeId = 0, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologyAddCommand(void);

protected:
   virtual void m_AddNew(void);

private:
   const E_ElementType me_Type;
   const QPointF mc_NewPos;
   const QString mc_AdditionalInformation;
   const stw_types::uint64 mu64_BusConnectorNodeID;
   const stw_types::uint64 mu64_BusConnectorBusID;
   const stw_types::uint8 mu8_InterfaceNumber;
   const stw_types::uint8 mu8_NodeId;
   const bool mq_ForceUseAdditionalInformation;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif