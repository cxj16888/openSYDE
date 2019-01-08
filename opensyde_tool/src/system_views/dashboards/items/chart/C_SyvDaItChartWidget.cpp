//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a chart with its data configuration and data selection.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.08.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPen>

#include "stwerrors.h"

#include "constants.h"

#include "C_SyvDaItChartWidget.h"
#include "ui_C_SyvDaItChartWidget.h"

#include "C_GtGetText.h"
#include "C_OSCUtils.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvDbWidgetBase.h"
#include "TGLTime.h"
#include "TGLUtils.h"
#include "C_OSCLoggingHandler.h"
#include "C_SdNdeDataPoolContentUtil.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;
using namespace QtCharts;

/* -- Module Global Constants ---------------------------------------------- */
const QColor C_SyvDaItChartWidget::mhac_DataColors[10] =
{
   QColor(192, 0, 0),
   QColor(0, 112, 192),
   QColor(0, 176, 80),
   QColor(255, 0, 0),
   QColor(255, 192, 0),
   QColor(146, 208, 80),
   QColor(0, 176, 240),
   QColor(0, 32, 96),
   QColor(255, 242, 0),
   QColor(112, 48, 160)
};

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex             Index of system view
   \param[in]     ou32_MaximumDataElements   Maximum number of shown data elements of the widget
   \param[in,out] opc_Parent                 Optional pointer to parent

   \created     25.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDaItChartWidget::C_SyvDaItChartWidget(const uint32 ou32_ViewIndex, const uint32 ou32_MaximumDataElements,
                                           QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaItChartWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_MaximumDataElements(ou32_MaximumDataElements),
   msn_WidthLineSelected(3),
   msn_WidthLineDefault(2),
   mf64_MaxTime(60000.0),
   // 60 seconds as initial value
   mf64_MaxValue(5.0),
   mf64_MinValue(0.0),
   mu32_TimeStampOfStart(0U),
   mf64_DefaultTimeSlot(60000.0),
   // 60 seconds as default value
   mf64_CurrentTimeSlot(this->mf64_DefaultTimeSlot)
{
   QPen c_Pen;
   QFont c_Font;

   this->mc_DataColorsUsed.resize(this->mu32_MaximumDataElements);

   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_ChartSelectorWidget->SetView(ou32_ViewIndex);

   // Create chart and its view
   mpc_Chart = new C_OgeChaChartBase();
   this->mpc_Chart->legend()->setVisible(false);

   mpc_ChartView = new C_OgeChaViewBase(this->mpc_Chart, this->mpc_Ui->pc_ChartViewWidget);
   this->mpc_Ui->pc_VerticalLayoutChartView->addWidget(this->mpc_ChartView);
   this->mpc_ChartView->setRenderHint(QPainter::Antialiasing);

   // Create the axis
   mpc_AxisTime = new QValueAxis();
   mpc_AxisValue = new QValueAxis();
   mpc_AxisValueInvisible = new QValueAxis();

   // Customize the axis
   c_Pen.setWidth(1);

   //Convert point to pixel
   c_Font = mc_STYLE_GUIDE_FONT_REGULAR_12;
   c_Font.setPixelSize(c_Font.pointSize());

   this->mpc_AxisTime->setLabelsFont(c_Font);
   this->mpc_AxisTime->setTitleFont(c_Font);
   this->mpc_AxisTime->setLinePen(c_Pen);
   this->mpc_AxisValue->setLabelsFont(c_Font);
   this->mpc_AxisValue->setTitleFont(c_Font);
   this->mpc_AxisValue->setLinePen(c_Pen);

   this->mpc_AxisTime->setGridLineVisible(true);
   this->mpc_AxisValue->setGridLineVisible(true);

   this->mpc_AxisValueInvisible->setVisible(false);

   this->mpc_Chart->addAxis(this->mpc_AxisTime, Qt::AlignBottom);
   this->mpc_Chart->addAxis(this->mpc_AxisValue, Qt::AlignLeft);
   this->mpc_Chart->addAxis(this->mpc_AxisValueInvisible, Qt::AlignLeft);

   // Init the range
   this->mpc_AxisValue->setMax(this->mf64_MaxValue);
   this->mpc_AxisValue->setMin(this->mf64_MinValue);
   this->mpc_AxisValueInvisible->setMax(this->mf64_MaxValue);
   this->mpc_AxisValueInvisible->setMin(this->mf64_MinValue);
   this->mpc_AxisTime->setMax(this->mf64_MaxTime);
   this->mpc_AxisTime->setMin(0.0);
   this->mpc_AxisTime->setTitleText("[ms]");

   // Configure the splitter
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 1);

   connect(this->mpc_Ui->pc_ChartSelectorWidget, &C_SyvDaItChartDataSelectorWidget::SigDataItemToggled,
           this, &C_SyvDaItChartWidget::m_DataItemToggled);
   connect(this->mpc_Ui->pc_ChartSelectorWidget, &C_SyvDaItChartDataSelectorWidget::SigDataItemSelected,
           this, &C_SyvDaItChartWidget::m_DataItemSelected);

   this->SetDisplayStyle(C_PuiSvDbWidgetBase::eOPENSYDE, false);
   this->SetWidthOfDataSeriesSelector(330);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     25.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDaItChartWidget::~C_SyvDaItChartWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of the elements and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the chart data as initialize configuration

   \param[in]     orc_Data       Chart data with configured data elements

   \created     05.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::SetData(const C_PuiSvDbChart & orc_Data)
{
   this->mc_Data = orc_Data;

   tgl_assert(this->mpc_Chart != NULL);
   if (this->mpc_Chart != NULL)
   {
      tgl_assert(this->mc_Data.c_DataPoolElementsActive.size() == this->mc_Data.c_DataPoolElementsConfig.size());

      for (uint32 u32_It = 0; u32_It < this->mc_Data.c_DataPoolElementsConfig.size(); ++u32_It)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = this->mc_Data.c_DataPoolElementsConfig[u32_It];
         const C_OSCNodeDataPoolListElement * const pc_Element =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                       rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                       rc_Config.c_ElementId.u32_ListIndex,
                                                                       rc_Config.c_ElementId.u32_ElementIndex);
         bool q_Warning = false;
         bool q_Invalid = false;
         QString c_InvalidNamePlaceholder = "";
         C_OSCNodeDataPool::E_Type e_InvalidDataPoolTypePlaceholder = C_OSCNodeDataPool::eDIAG;
         QString c_ToolTipErrorTextHeading = "";
         QString c_ToolTipErrorText = "";

         if (pc_Element != NULL)
         {
            // Save the minimum to have a content instance of the element to have the type
            this->mc_DataPoolElementContentMin.push_back(pc_Element->c_MinValue);
         }
         else
         {
            // No datapool element exists, add dummy instance
            C_OSCNodeDataPoolContent c_Tmp;
            this->mc_DataPoolElementContentMin.push_back(c_Tmp);
         }

         if (rc_Config.c_ElementId.GetIsValid() == false)
         {
            // Element is invalid, it was deleted in the system definition
            q_Warning = true;
            q_Invalid = true;
            c_InvalidNamePlaceholder = rc_Config.c_ElementId.GetInvalidNamePlaceholder();
            e_InvalidDataPoolTypePlaceholder = rc_Config.c_ElementId.GetInvalidTypePlaceholder();
            c_ToolTipErrorTextHeading = C_GtGetText::h_GetText("Configuration warning");
            c_ToolTipErrorText = C_GtGetText::h_GetText("Data element was deleted in system definition");
         }
         else
         {
            const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
            tgl_assert(pc_View != NULL);
            if (pc_View != NULL)
            {
               const std::vector<uint8> & rc_ActiveFlags = pc_View->GetNodeActiveFlags();
               if (rc_Config.c_ElementId.u32_NodeIndex < rc_ActiveFlags.size())
               {
                  if (rc_ActiveFlags[rc_Config.c_ElementId.u32_NodeIndex] == 0U)
                  {
                     // Node with data element is not active in current view
                     q_Warning = true;
                     c_ToolTipErrorTextHeading = C_GtGetText::h_GetText("Configuration warning");
                     c_ToolTipErrorText = C_GtGetText::h_GetText("There is a data element of an inactive node");
                  }
               }
            }
         }

         this->m_AddDataSerie(u32_It, q_Warning, q_Invalid, c_InvalidNamePlaceholder, e_InvalidDataPoolTypePlaceholder,
                              c_ToolTipErrorTextHeading,
                              c_ToolTipErrorText);
      }

      tgl_assert(this->mc_Data.c_DataPoolElementsActive.size() == this->mc_DataPoolElementContentMin.size());
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the chart data with the element configuration

   \return
   Chart data

   \created     05.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_PuiSvDbChart & C_SyvDaItChartWidget::GetData(void)
{
   return this->mc_Data;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply style

   \param[in] oe_Style     New style type
   \param[in] oq_DarkMode  Flag if dark mode is active

   \created     31.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   if (this->mpc_Chart != NULL)
   {
      QBrush c_Brush;

      switch (oe_Style)
      {
      //TBD by Karsten: done
      case C_PuiSvDbWidgetBase::eOPENSYDE:
         if (oq_DarkMode == true)
         {
            // Color for background
            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_36);
            this->mpc_Chart->setBackgroundBrush(c_Brush);

            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_8);

            // Customize the axis
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_2);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_2);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_2);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }
         else
         {
            // Color for background
            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_12);
            this->mpc_Chart->setBackgroundBrush(c_Brush);

            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_10);

            // Customize the axis and gridline
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_34);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }

         break;

      case C_PuiSvDbWidgetBase::eFLAT:
         if (oq_DarkMode == true)
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_8);

            // Customize the axis
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_0);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }
         else
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_26); //34);

            // Customize the axis and gridline
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_34);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }

         break;

      case C_PuiSvDbWidgetBase::eSKEUOMORPH:
         if (oq_DarkMode == true)
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_34);

            // Customize the axis
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_0);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_34);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }
         else
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_10);

            // Customize the axis and gridline
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_33);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_34);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_34);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }

         break;

      case C_PuiSvDbWidgetBase::eOPENSYDE_2:
         if (oq_DarkMode == true)
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_8);

            // Customize the axis
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_0);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_8);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_0);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }
         else
         {
            // Color for the splitter
            this->mpc_Ui->pc_Splitter->SetColor(mc_STYLE_GUIDE_COLOR_10);

            // Customize the axis and gridline
            this->mpc_AxisTime->setLinePenColor(mc_STYLE_GUIDE_COLOR_33);
            this->mpc_AxisTime->setLabelsColor(mc_STYLE_GUIDE_COLOR_6);

            this->mpc_AxisTime->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);
            this->mpc_AxisValue->setGridLineColor(mc_STYLE_GUIDE_COLOR_10);

            c_Brush.setColor(mc_STYLE_GUIDE_COLOR_6);
            this->mpc_AxisTime->setTitleBrush(c_Brush);
         }

         break;

      default:
         // Nothing to do
         break;
      }
      //this->ReInitializeSize();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the width of the data series selector widget left of the splitter

   \param[in]     osn_Width       Width of the selector widget

   \created     05.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::SetWidthOfDataSeriesSelector(const sintn osn_Width) const
{
   this->mpc_Ui->pc_Splitter->SetFirstSegment(osn_Width);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the width of the data series selector widget left of the splitter

   \return
   Width of the selector widget

   \created     05.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sintn C_SyvDaItChartWidget::GetWidthOfDataSeriesSelector(void) const
{
   sintn sn_Width = 250;
   const QList<sintn> & rc_List = this->mpc_Ui->pc_Splitter->sizes();

   if (rc_List.size() > 0)
   {
      sn_Width = rc_List.at(0);
   }

   return sn_Width;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the current id of the selected data element

   \param[out]     oru32_DataPoolElementConfigIndex         Datapool element configuration index

   \return
   true     data element exists
   false    data element does not exist

   \created     07.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvDaItChartWidget::GetCurrentDataSerie(stw_types::uint32 & oru32_DataPoolElementConfigIndex) const
{
   return this->mpc_Ui->pc_ChartSelectorWidget->GetCurrentDataSerie(oru32_DataPoolElementConfigIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Information about the start or stop of a connection

   \param[in]  oq_Active      Flag if connection is active or not active now

   \created     01.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::ConnectionActiveChanged(const bool oq_Active)
{
   if (oq_Active == true)
   {
      this->m_ResetChart();
      this->mu32_TimeStampOfStart = stw_tgl::TGL_GetTickCount();
   }
   else
   {
      // In case of disconnect the transmission errors must be reseted, but not the warnings
      this->mpc_Ui->pc_ChartSelectorWidget->ResetError();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a specific data serie

   \param[in]     orc_DataPoolElementId   Datapool element identification
   \param[in]     orc_ElementScaling      Datapool element scaling configuration

   \return
   C_NO_ERR OK
   C_RANGE  Something out of range
   C_CONFIG Chart not initialized

   \created     05.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sint32 C_SyvDaItChartWidget::AddNewDataSerie(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                             const C_PuiSvDbDataElementScaling & orc_ElementScaling)
{
   sint32 s32_Return = C_CONFIG;

   tgl_assert(this->mpc_Chart != NULL);
   if (this->mpc_Chart != NULL)
   {
      if (this->mc_Data.c_DataPoolElementsConfig.size() < this->mu32_MaximumDataElements)
      {
         C_PuiSvDbNodeDataElementConfig c_Config;
         const uint32 u32_NewIndex = this->mc_Data.c_DataPoolElementsConfig.size();

         const C_OSCNodeDataPoolListElement * const pc_Element =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_DataPoolElementId.u32_NodeIndex,
                                                                       orc_DataPoolElementId.u32_DataPoolIndex,
                                                                       orc_DataPoolElementId.u32_ListIndex,
                                                                       orc_DataPoolElementId.u32_ElementIndex);
         if (pc_Element != NULL)
         {
            c_Config.c_ElementId = orc_DataPoolElementId;
            c_Config.c_ElementScaling = orc_ElementScaling;

            this->mc_Data.c_DataPoolElementsConfig.push_back(c_Config);
            this->mc_Data.c_DataPoolElementsActive.push_back(true);

            // Save the minimum to have a content instance of the element to have the type
            this->mc_DataPoolElementContentMin.push_back(pc_Element->c_MinValue);

            this->m_AddDataSerie(u32_NewIndex);

            s32_Return = C_NO_ERR;
         }
         else
         {
            s32_Return = C_RANGE;
         }
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Removes the current data serie

   \param[out]    orc_ElementId     Data element id of removed data serie

   \return
   true     data element removed
   false    nothing removed or a multiple registration of an element id was removed

   \created     07.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SyvDaItChartWidget::RemoveDataSerie(C_PuiSvDbNodeDataPoolListElementId & orc_ElementId)
{
   bool q_Return;
   uint32 u32_DataPoolElementConfigIndex;

   q_Return = this->mpc_Ui->pc_ChartSelectorWidget->GetCurrentDataSerie(u32_DataPoolElementConfigIndex);

   if (q_Return == true)
   {
      q_Return = this->mpc_Ui->pc_ChartSelectorWidget->RemoveDataSerie(u32_DataPoolElementConfigIndex);
   }

   if ((q_Return == true) &&
       (this->mpc_Chart != NULL))
   {
      uint32 u32_Counter;

      // Remove data serie
      if (u32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataSeries.size())
      {
         this->mpc_Chart->removeSeries(this->mc_DataPoolElementsDataSeries[u32_DataPoolElementConfigIndex]);
         delete this->mc_DataPoolElementsDataSeries[u32_DataPoolElementConfigIndex];
         this->mc_DataPoolElementsDataSeries.erase(
            this->mc_DataPoolElementsDataSeries.begin() + u32_DataPoolElementConfigIndex);
      }

      // Remove used color
      if (u32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataColorIndexes.size())
      {
         tgl_assert(this->mc_DataColorsUsed.size() >
                    this->mc_DataPoolElementsDataColorIndexes[u32_DataPoolElementConfigIndex]);
         std::vector<bool>::reference c_Value =
            this->mc_DataColorsUsed[this->mc_DataPoolElementsDataColorIndexes[u32_DataPoolElementConfigIndex]];

         c_Value = false;

         this->mc_DataPoolElementsDataColorIndexes.erase(
            this->mc_DataPoolElementsDataColorIndexes.begin() + u32_DataPoolElementConfigIndex);
      }

      if (this->mc_DataPoolElementsDataSeries.size() > 0)
      {
         // Select an other item
         this->SelectDataSeriesAxis(0);
         this->mpc_Ui->pc_ChartSelectorWidget->SelectDataSerie(0);
      }
      else
      {
         this->mpc_AxisValue->setTitleText("");
      }

      if (u32_DataPoolElementConfigIndex < this->mc_Data.c_DataPoolElementsConfig.size())
      {
         orc_ElementId = this->mc_Data.c_DataPoolElementsConfig[u32_DataPoolElementConfigIndex].c_ElementId;

         // Check if there is an further registration of this data element
         for (u32_Counter = 0U; u32_Counter < this->mc_Data.c_DataPoolElementsConfig.size(); ++u32_Counter)
         {
            if ((u32_Counter != u32_DataPoolElementConfigIndex) &&
                (orc_ElementId == this->mc_Data.c_DataPoolElementsConfig[u32_Counter].c_ElementId))
            {
               // An other registration found. The upper layer shall think nothing happened
               q_Return = false;
            }
         }

         // Remove configuration itself
         this->mc_Data.c_DataPoolElementsConfig.erase(
            this->mc_Data.c_DataPoolElementsConfig.begin() + u32_DataPoolElementConfigIndex);
         this->mc_Data.c_DataPoolElementsActive.erase(
            this->mc_Data.c_DataPoolElementsActive.begin() + u32_DataPoolElementConfigIndex);
         this->mc_DataPoolElementContentMin.erase(
            this->mc_DataPoolElementContentMin.begin() + u32_DataPoolElementConfigIndex);
      }
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Selects and shows the axix for the specific datapool element

   \param[in]     orc_DataPoolElementId   Datapool element identification

   \return
   possible return value(s) and description

   \created     28.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::SelectDataSeriesAxis(const uint32 ou32_DataPoolElementConfigIndex)
{
   if ((ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataSeries.size()) &&
       (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataColorIndexes.size()))
   {
      QLineSeries * const pc_LineSerie = this->mc_DataPoolElementsDataSeries[ou32_DataPoolElementConfigIndex];
      const QColor c_Color = C_SyvDaItChartWidget::mhac_DataColors[
         this->mc_DataPoolElementsDataColorIndexes[ou32_DataPoolElementConfigIndex]];
      uint32 u32_CounterLine;
      const QString c_Name = this->mpc_Ui->pc_ChartSelectorWidget->GetDataElementName(ou32_DataPoolElementConfigIndex);
      const QString c_Unit = this->mpc_Ui->pc_ChartSelectorWidget->GetDataElementUnit(ou32_DataPoolElementConfigIndex);
      QBrush c_Brush;
      QPen c_Pen;

      pc_LineSerie->attachAxis(this->mpc_AxisValue);

      // Adapt the width of the selected data series
      c_Pen = pc_LineSerie->pen();
      c_Pen.setWidth(msn_WidthLineSelected);
      pc_LineSerie->setPen(c_Pen);

      // Adapt all axis colors for this data element
      this->mpc_AxisValue->setLinePenColor(c_Color);
      this->mpc_AxisValue->setLabelsColor(c_Color);
      c_Brush.setColor(c_Color);
      this->mpc_AxisValue->setTitleBrush(c_Brush);
      this->mpc_AxisValue->setTitleText(c_Name + " [" + c_Unit + "]");

      // Detach all other data series from the visible axis
      for (u32_CounterLine = 0U;
           u32_CounterLine < this->mc_DataPoolElementsDataSeries.size();
           ++u32_CounterLine)
      {
         if (u32_CounterLine != ou32_DataPoolElementConfigIndex)
         {
            const QList<QAbstractAxis *> & rc_ListAxis =
               this->mc_DataPoolElementsDataSeries[u32_CounterLine]->attachedAxes();
            QList<QAbstractAxis *>::const_iterator c_ItAxis;

            // Detach the axis only if attached. Search it.
            for (c_ItAxis = rc_ListAxis.begin(); c_ItAxis != rc_ListAxis.end(); ++c_ItAxis)
            {
               if ((*c_ItAxis) == this->mpc_AxisValue)
               {
                  this->mc_DataPoolElementsDataSeries[u32_CounterLine]->detachAxis(this->mpc_AxisValue);
               }
            }

            // Adapt the drawn line of the data series
            c_Pen = this->mc_DataPoolElementsDataSeries[u32_CounterLine]->pen();
            c_Pen.setWidth(msn_WidthLineDefault);
            this->mc_DataPoolElementsDataSeries[u32_CounterLine]->setPen(c_Pen);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds the newest values of datapool elements to its associated data series of the chart

   \param[in]     orc_DataPoolElementId   Datapool element identification
   \param[in]     orc_Values              List with all read elements
   \param[in]     orc_Timestamps          List with all timestamps for each element

   \created     28.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::AddDataSerieContent(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                               const QVector<float64> & orc_Values,
                                               const QVector<uint32> & orc_Timestamps)
{
   // Find the correct data series
   if (orc_Values.size() > 0)
   {
      uint32 u32_ConfigCounter;

      for (u32_ConfigCounter = 0U; u32_ConfigCounter < this->mc_Data.c_DataPoolElementsConfig.size();
           ++u32_ConfigCounter)
      {
         if ((this->mc_Data.c_DataPoolElementsConfig[u32_ConfigCounter].c_ElementId == orc_DataPoolElementId) &&
             (u32_ConfigCounter < this->mc_DataPoolElementsDataSeries.size()))
         {
            QLineSeries * const pc_LineSerie = this->mc_DataPoolElementsDataSeries[u32_ConfigCounter];

            if (pc_LineSerie != NULL)
            {
               sintn sn_ValueCounter;
               float64 f64_Value = 0.0;
               C_OSCNodeDataPoolContent c_Tmp = this->mc_DataPoolElementContentMin[u32_ConfigCounter];
               QString c_Value;

               tgl_assert(orc_Values.size() == orc_Timestamps.size());

               for (sn_ValueCounter = 0U; sn_ValueCounter < orc_Values.size(); ++sn_ValueCounter)
               {
                  // Start time must be zero, adapt the read timestamp
                  const uint32 u32_TimeStamp = orc_Timestamps[sn_ValueCounter];
                  float64 f64_Timestamp;

                  if (u32_TimeStamp > this->mu32_TimeStampOfStart)
                  {
                     f64_Timestamp = static_cast<float64>(u32_TimeStamp);
                     f64_Timestamp -= static_cast<float64>(this->mu32_TimeStampOfStart);
                  }
                  else
                  {
                     f64_Timestamp = 0.0;
                  }

                  f64_Value = orc_Values[sn_ValueCounter];

                  f64_Value = C_OSCUtils::h_GetValueScaled(
                     f64_Value,
                     this->mc_Data.c_DataPoolElementsConfig[u32_ConfigCounter].c_ElementScaling.f64_Factor,
                     this->mc_Data.c_DataPoolElementsConfig[u32_ConfigCounter].c_ElementScaling.f64_Offset);

                  // Adapt range for time
                  if (this->mf64_MaxTime < f64_Timestamp)
                  {
                     const float64 f64_Diff = f64_Timestamp - this->mf64_MaxTime;

                     // Save the new maximum
                     this->mf64_MaxTime = f64_Timestamp;

                     // Do not use the new maximum for the axis. In case of zooming, we need an dynamic adaption
                     this->mpc_AxisTime->setMax(this->mpc_AxisTime->max() + f64_Diff);

                     // Adapt minimum
                     this->mpc_AxisTime->setMin(this->mpc_AxisTime->min() + f64_Diff);
                  }

                  // Adapt range for value
                  if (this->mf64_MaxValue < f64_Value)
                  {
                     // Add a little free space to the maximum
                     this->mf64_MaxValue = f64_Value * 1.01;
                     this->mpc_AxisValue->setMax(this->mf64_MaxValue);
                     this->mpc_AxisValueInvisible->setMax(this->mf64_MaxValue);
                  }
                  if (this->mf64_MinValue > f64_Value)
                  {
                     this->mf64_MinValue = f64_Value;
                     this->mpc_AxisValue->setMin(this->mf64_MinValue);
                     this->mpc_AxisValueInvisible->setMin(this->mf64_MinValue);
                  }

                  pc_LineSerie->append(f64_Timestamp, f64_Value);
               }

               // Show the last value in the selector widget
               C_SdNdeDataPoolContentUtil::h_SetValueInContent(f64_Value, c_Tmp, 0UL);
               // No need of scaling here, it is already scaled
               C_SdNdeDataPoolContentUtil::h_GetValueAsScaledString(c_Tmp, 1.0, 0.0, c_Value, 0UL);
               this->mpc_Ui->pc_ChartSelectorWidget->UpdateDataSerieValue(u32_ConfigCounter, c_Value);
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets a scaling configuration for a specific datapool element configuration

   \param[in]     ou32_DataPoolElementConfigIndex         Datapool element configuration index
   \param[in]     orc_DisplayName                         Datapool element display name
   \param[in]     orc_ElementScaling                      Datapool element scaling configuration

   \created     06.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::SetScaling(const uint32 ou32_DataPoolElementConfigIndex, const QString & orc_DisplayName,
                                      const C_PuiSvDbDataElementScaling & orc_ElementScaling)
{
   if (ou32_DataPoolElementConfigIndex < this->mc_Data.c_DataPoolElementsConfig.size())
   {
      this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_DisplayName = orc_DisplayName;
      this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementScaling = orc_ElementScaling;
      this->mpc_Ui->pc_ChartSelectorWidget->SetDataElementUnit(ou32_DataPoolElementConfigIndex, orc_DisplayName,
                                                               orc_ElementScaling.c_Unit);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the count of registered data series

   \return
   Count of registered data series

   \created     07.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
uint32 C_SyvDaItChartWidget::GetCountDataSeries(void) const
{
   return this->mc_DataPoolElementsDataSeries.size();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapts and updates the time axis if necessary

   \created     15.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::UpdateTimeAxis(void)
{
   const uint32 u32_CurrentTimeStamp = stw_tgl::TGL_GetTickCount() - this->mu32_TimeStampOfStart;
   const uint32 u32_NextTimeStamp = u32_CurrentTimeStamp + static_cast<uint32>(msn_TIMER_GUI_REFRESH);

   if (this->mf64_MaxTime < (static_cast<float64>(u32_NextTimeStamp)))
   {
      const float64 f64_Diff = static_cast<float64>(u32_NextTimeStamp) - this->mf64_MaxTime;

      // Save the new maximum
      this->mf64_MaxTime = static_cast<float64>(u32_NextTimeStamp);

      // Do not use the new maximum for the axis. In case of zooming, we need an dynamic adaption
      this->mpc_AxisTime->setMax(this->mpc_AxisTime->max() + f64_Diff);

      // Adapt minimum
      this->mpc_AxisTime->setMin(this->mpc_AxisTime->min() + f64_Diff);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Error update for data element

   \param[in] ou32_WidgetDataPoolElementIndex Index of shown datapool element in widget
   \param[in] orc_ErrorText                   Error description
   \param[in] orq_IsTransmissionError         Flag if transmission error occurred

   \created     21.08.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::UpdateError(const uint32 ou32_DataElementIndex, const QString & orc_ErrorText,
                                       const bool oq_IsTransmissionError) const
{
   std::map<stw_types::uint32, stw_types::uint32>::const_iterator c_It =
      this->mc_ElementHandlerRegIndexToDataElementIndex.find(ou32_DataElementIndex);

   if (c_It != this->mc_ElementHandlerRegIndexToDataElementIndex.end())
   {
      this->mpc_Ui->pc_ChartSelectorWidget->UpdateError(c_It->second, orc_ErrorText, oq_IsTransmissionError);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update of the color transparence value configured by the actual timeout state

   \param[in] ou32_WidgetDataPoolElementIndex Index of shown datapool element in widget
   \param[in] osn_Value                       Value for transparence (0..255)

   \created     31.07.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::UpdateTransparence(const uint32 ou32_DataElementIndex, const sintn osn_Value) const
{
   std::map<stw_types::uint32, stw_types::uint32>::const_iterator c_It =
      this->mc_ElementHandlerRegIndexToDataElementIndex.find(ou32_DataElementIndex);

   if (c_It != this->mc_ElementHandlerRegIndexToDataElementIndex.end())
   {
      this->mpc_Ui->pc_ChartSelectorWidget->UpdateTransparence(c_It->second, osn_Value);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information

   \created     28.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);
   QWidget::paintEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a specific data serie

   \param[in]     orc_DataPoolElementId               Datapool element identification
   \param[in]     oq_Warning                          Flag if a warning for this data element was detected
   \param[in]     oq_Invalid                          Flag if data elment is invalid and invalid placeholder info
                                                      shall be used
   \param[in]     orc_InvalidPlaceholderName          Placeholder name of data element in case of invalid data element
   \param[in]     oe_InvalidPlaceholderDataPoolType   Placeholder datapool type of data element in case of
                                                      invalid data element
   \param[in]     orc_ToolTipErrorTextHeading         Heading of tool tip in case of a warning
   \param[in]     orc_ToolTipErrorText                Text of tool tip in case of a warning

   \created     28.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_AddDataSerie(const stw_types::uint32 ou32_DataPoolElementConfigIndex,
                                          const bool oq_Warning, const bool oq_Invalid,
                                          const QString & orc_InvalidPlaceholderName,
                                          const C_OSCNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType,
                                          const QString & orc_ToolTipErrorTextHeading,
                                          const QString & orc_ToolTipErrorText)
{
   // Color selection
   QColor c_Color = this->m_GetColor();
   QLineSeries * const pc_Serie = new QLineSeries();
   QPen c_Pen;

   if (oq_Invalid == false)
   {
      this->mc_ElementHandlerRegIndexToDataElementIndex.insert(
         std::pair<uint32, uint32>(
            static_cast<uint32>(this->mc_ElementHandlerRegIndexToDataElementIndex.size()),
            ou32_DataPoolElementConfigIndex));
   }

   if (oq_Warning == false)
   {
      this->mpc_Ui->pc_ChartSelectorWidget->AddDataSerie(
         ou32_DataPoolElementConfigIndex,
         this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementId,
         this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_DisplayName,
         this->mc_Data.c_DataPoolElementsActive[ou32_DataPoolElementConfigIndex], c_Color,
         this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementScaling.c_Unit);
   }
   else
   {
      this->mpc_Ui->pc_ChartSelectorWidget->AddDataSerie(
         ou32_DataPoolElementConfigIndex,
         this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementId,
         orc_InvalidPlaceholderName,
         this->mc_Data.c_DataPoolElementsActive[ou32_DataPoolElementConfigIndex], c_Color,
         this->mc_Data.c_DataPoolElementsConfig[ou32_DataPoolElementConfigIndex].c_ElementScaling.c_Unit,
         oq_Warning, oq_Invalid, oe_InvalidPlaceholderDataPoolType, orc_ToolTipErrorTextHeading, orc_ToolTipErrorText);
   }

   c_Pen.setColor(c_Color);
   c_Pen.setWidth(msn_WidthLineDefault);
   pc_Serie->setPen(c_Pen);
   this->mpc_Chart->addSeries(pc_Serie);
   pc_Serie->attachAxis(this->mpc_AxisTime);
   // Each data serie needs minimum one y axis. Use the invisible one as default
   pc_Serie->attachAxis(this->mpc_AxisValueInvisible);
   pc_Serie->setVisible(this->mc_Data.c_DataPoolElementsActive[ou32_DataPoolElementConfigIndex]);
   this->mc_DataPoolElementsDataSeries.push_back(pc_Serie);

   // Set the axis for the new data serie
   this->SelectDataSeriesAxis(ou32_DataPoolElementConfigIndex);
   //lint -e{429}  no memory leak because of the parent of pc_Series and the Qt memory management
}

//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_ResetChart(void)
{
   uint32 u32_CounterItem;

   for (u32_CounterItem = 0;
        u32_CounterItem < this->mc_DataPoolElementsDataSeries.size();
        ++u32_CounterItem)
   {
      this->mc_DataPoolElementsDataSeries[u32_CounterItem]->clear();
   }

   // Reset the range
   this->mf64_MaxValue = 1.0;
   this->mf64_MinValue = 0.0;
   this->mf64_MaxTime = this->mf64_DefaultTimeSlot;

   this->mpc_AxisValue->setMax(this->mf64_MaxValue);
   this->mpc_AxisValue->setMin(this->mf64_MinValue);
   this->mpc_AxisValueInvisible->setMax(this->mf64_MaxValue);
   this->mpc_AxisValueInvisible->setMin(this->mf64_MinValue);
   this->mpc_AxisTime->setMax(this->mf64_MaxTime);
   this->mpc_AxisTime->setMin(0.0);
}

//-----------------------------------------------------------------------------
QColor C_SyvDaItChartWidget::m_GetColor(void)
{
   uint32 u32_Counter;
   QColor c_Color;

   // Search the next free Color
   for (u32_Counter = 0U; u32_Counter < this->mu32_MaximumDataElements; ++u32_Counter)
   {
      if (this->mc_DataColorsUsed[u32_Counter] == false)
      {
         std::vector<bool>::reference c_Value = this->mc_DataColorsUsed[u32_Counter];
         c_Value = true;
         c_Color = C_SyvDaItChartWidget::mhac_DataColors[u32_Counter];

         // Save the association between datapool element and the color
         this->mc_DataPoolElementsDataColorIndexes.push_back(u32_Counter);
         break;
      }
   }

   return c_Color;
}

//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_DataItemToggled(const stw_types::uint32 ou32_DataPoolElementConfigIndex,
                                             const bool oq_Checked)
{
   if (ou32_DataPoolElementConfigIndex < this->mc_DataPoolElementsDataSeries.size())
   {
      this->mc_DataPoolElementsDataSeries[ou32_DataPoolElementConfigIndex]->setVisible(oq_Checked);
   }

   if (ou32_DataPoolElementConfigIndex < this->mc_Data.c_DataPoolElementsActive.size())
   {
      std::vector<bool>::reference c_Value = this->mc_Data.c_DataPoolElementsActive[ou32_DataPoolElementConfigIndex];
      c_Value = oq_Checked;
   }
}

//-----------------------------------------------------------------------------
void C_SyvDaItChartWidget::m_DataItemSelected(const uint32 ou32_DataPoolElementConfigIndex)
{
   this->SelectDataSeriesAxis(ou32_DataPoolElementConfigIndex);
}