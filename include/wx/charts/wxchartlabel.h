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

#ifndef _WX_CHARTS_WXCHARTLABEL_H_
#define _WX_CHARTS_WXCHARTLABEL_H_

#include "wxchartelement.h"
#include <wx/graphics.h>
#include <wx/string.h>

class wxChartLabel : public wxChartElement
{
public:
	wxChartLabel(const wxString &text);
	wxChartLabel(const wxString &text, wxDouble width,
		wxDouble height);

	void Draw(wxDouble x, wxDouble y, 
		wxGraphicsContext &gc);

	const wxString& GetText() const;
	const wxSize& GetSize() const;
	void SetSize(const wxSize &size);
	void SetSize(wxDouble width, wxDouble height);

private:
	wxString m_text;
	wxSize m_size;
};

#endif