
/// @file

#ifndef _WX_CHART_EVENT_H_
#define _WX_CHART_EVENT_H_

#include <wx/event.h>
#include "wxlinechart.h"

namespace wxChartEvents
{
    enum Type
    {
        Highlighted = 0,
        NotHighlighted
    };
}

class wxChartElementHighlightedEvent : public wxEvent
{
public:
    wxChartElementHighlightedEvent(const int index, const wxLineChartDataset::ptr dataset, const int Id = 0);
    wxChartElementHighlightedEvent(const wxChartElementHighlightedEvent& Other);
    
    virtual wxEvent* Clone() const override;
    
    // this is important to avoid that calling wxEventLoopBase::YieldFor thread events
    // gets processed when this is unwanted:
    virtual wxEventCategory GetEventCategory() const override;
    
    int GetIndex() const;
    const wxLineChartDataset::ptr GetDataset() const;
    
protected:
    int m_index;
    wxLineChartDataset::ptr m_dataset;
};

inline int wxChartElementHighlightedEvent::GetIndex() const
{
    return m_index;
}

inline const wxLineChartDataset::ptr wxChartElementHighlightedEvent::GetDataset() const
{
    return m_dataset;
}

wxDECLARE_EVENT(WX_CHART_ELEMENT_HIGHLIGHTED_EVT, wxChartElementHighlightedEvent);

#define EVT_CHART_ELEMENT_HIGHLIGHTED(id, func) \
    wx__DECLARE_EVT1(WX_CHART_ELEMENT_HIGHLIGHTED_EVT, id, wxEVENT_HANDLER_CAST(wxEventFunction, func))

#endif
