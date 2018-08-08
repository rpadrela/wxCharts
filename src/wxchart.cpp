/*
    Copyright (c) 2016-2018 Xavier Leclercq

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#include "wxchart.h"
#include "wxcharttooltip.h"
#include "wxchartmultitooltip.h"

wxChart::wxChart()
    : m_needsFit(true),
    m_activeElements(new wxVector<const wxChartElement*>())
{
}

void wxChart::SetSize(const wxSize &size)
{
    DoSetSize(size);
    m_needsFit = true;
}

void wxChart::Draw(wxGraphicsContext &gc)
{
    DoDraw(gc, !GetCommonOptions().ShowTooltips());
}

bool IsElementInTheList(const wxVector<const wxChartElement*>& Elements, const wxChartElement* Element)
{
    for (int i = 0; i < Elements.size(); ++i)
        if (Elements[i] == Element)
            return true;

    return false;
}

void wxChart::ActivateElementsAt(const wxPoint &point)
{
    wxSharedPtr<wxVector<const wxChartElement*>> NewActiveElements = GetActiveElements(point);
    
    // Deactivate all active elements that are no longer active
    if (m_activeElements.get())
        for (int i = 0; i < m_activeElements->size(); ++i)
            if (!IsElementInTheList(*NewActiveElements.get(), m_activeElements->at(i)))
                DeactivateElement(m_activeElements->at(i));

    // Activate all elements that are active but were not
    if (NewActiveElements.get())
        for (int i = 0; i < NewActiveElements->size(); ++i)
            if (!IsElementInTheList(*m_activeElements.get(), NewActiveElements->at(i)))
                ActivateElement(NewActiveElements->at(i));

    m_activeElements = NewActiveElements;
}

void wxChart::Fit()
{
    if (!m_needsFit)
    {
        return;
    }

    DoFit();

    m_needsFit = false;
}

void wxChart::DrawTooltips(wxGraphicsContext &gc)
{
    if (m_activeElements->size() == 1)
    {
        // If only one element is active draw a normal tooltip
        wxChartTooltip tooltip((*m_activeElements)[0]->GetTooltipPosition(),
            (*m_activeElements)[0]->GetTooltipProvider()->GetTooltipText());
        tooltip.Draw(gc);
    }
    else if (m_activeElements->size() > 1)
    {
        // If more than one element is active draw a multi-tooltip
        wxChartMultiTooltip multiTooltip((*m_activeElements)[0]->GetTooltipProvider()->GetTooltipTitle(),
            GetCommonOptions().GetMultiTooltipOptions());
        for (size_t j = 0; j < m_activeElements->size(); ++j)
        {
            wxChartTooltip tooltip((*m_activeElements)[j]->GetTooltipPosition(),
                (*m_activeElements)[j]->GetTooltipProvider());
            multiTooltip.AddTooltip(tooltip);
        }
        multiTooltip.Draw(gc);
    }
}
