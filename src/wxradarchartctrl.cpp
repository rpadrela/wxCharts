/*
	Copyright (c) 2016 Xavier Leclercq

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

/*
	Part of this file were copied from the Chart.js project (http://chartjs.org/)
	and translated into C++.

	The files of the Chart.js project have the following copyright and license.

	Copyright (c) 2013-2016 Nick Downie
	Released under the MIT license
	https://github.com/nnnick/Chart.js/blob/master/LICENSE.md
*/

#include "wxradarchartctrl.h"
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

wxRadarChartData::wxRadarChartData(const wxVector<wxString> &labels)
	: m_labels(labels)
{
}

const wxVector<wxString>& wxRadarChartData::GetLabels() const
{
	return m_labels;
};

wxRadarChartCtrl::wxRadarChartCtrl(wxWindow *parent,
								   wxWindowID id,
								   const wxPoint &pos,
								   const wxSize &size,
								   long style)
	: wxChartCtrl(parent, id, pos, size, style),
	m_grid(size, GetMinValue(), GetMaxValue(),
	m_options.GetGridOptions())
{
}

const wxRadarChartOptions& wxRadarChartCtrl::GetOptions() const
{
	return m_options;
}

wxDouble wxRadarChartCtrl::GetMinValue()
{
	wxDouble result = 0;
	return result;
}

wxDouble wxRadarChartCtrl::GetMaxValue()
{
	wxDouble result = 0;
	return result;
}

void wxRadarChartCtrl::Resize(const wxSize &size)
{
	m_grid.Resize(size);
}

wxSharedPtr<wxVector<const wxChartElement*> > wxRadarChartCtrl::GetActiveElements(const wxPoint &point)
{
	wxSharedPtr<wxVector<const wxChartElement*> > activeElements(new wxVector<const wxChartElement*>());
	return activeElements;
}

void wxRadarChartCtrl::OnPaint(wxPaintEvent &evt)
{
	wxAutoBufferedPaintDC dc(this);

	dc.Clear();

	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
	if (gc)
	{
		m_grid.Draw(*gc);

		delete gc;
	}
}

BEGIN_EVENT_TABLE(wxRadarChartCtrl, wxChartCtrl)
	EVT_PAINT(wxRadarChartCtrl::OnPaint)
END_EVENT_TABLE()