
#include "wxchartevent.h"

wxDEFINE_EVENT(WX_CHART_ELEMENT_HIGHLIGHTED_EVT, wxChartElementHighlightedEvent);

wxChartElementHighlightedEvent::wxChartElementHighlightedEvent(const int index, const wxLineChartDataset::ptr dataset, const int Id)
: wxEvent(Id, WX_CHART_ELEMENT_HIGHLIGHTED_EVT), m_index(index), m_dataset(dataset)
{
}

wxChartElementHighlightedEvent::wxChartElementHighlightedEvent(const wxChartElementHighlightedEvent& Other)
: wxEvent(Other), m_index(Other.m_index), m_dataset(Other.m_dataset)
{
}

wxEvent* wxChartElementHighlightedEvent::Clone() const
{
    return new wxChartElementHighlightedEvent(*this);
}

wxEventCategory wxChartElementHighlightedEvent::GetEventCategory() const
{
    return wxEVT_CATEGORY_USER_INPUT;
}
