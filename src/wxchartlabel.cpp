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

#include "wxchartlabel.h"
#include "wxchartbackground.h"
#include "wxchartutilities.h"

wxChartLabel::wxChartLabel(const wxString &text,
                           const wxChartLabelOptions &options)
    : m_options(options), m_text(text), m_angle(0), m_position(0, 0),
    m_size(0, 0)
{
}

wxChartLabel::wxChartLabel(const wxString &text,
                           wxDouble width,
                           wxDouble height,
                           const wxChartLabelOptions &options)
    : m_options(options), m_text(text), m_angle(0), m_position(0, 0),
    m_size(width, height)
{
}

void wxChartLabel::Draw(wxGraphicsContext &gc) const
{
    if (m_options.HasBackground())
    {
        gc.PushState();
        
            wxSize backgroundSize(m_size);
            backgroundSize.x += (m_padding.GetLeft() + m_padding.GetRight());
            backgroundSize.y += (m_padding.GetTop() + m_padding.GetBottom());

            wxPoint2DDouble position(m_position);

            if (m_angle != 0)
            {
                position.m_x = 0;
                position.m_y = 0;

                gc.Translate(m_position.m_x + backgroundSize.x / 2, m_position.m_y);
                gc.Rotate(-m_angle);
            }

            wxChartBackground background(m_options.GetBackgroundOptions());
            background.Draw(position, backgroundSize, gc);
        
        gc.PopState();
    }

    wxFont font = m_options.GetFontOptions().GetFont();
    gc.SetFont(font, m_options.GetFontOptions().GetColor());
    gc.DrawText(m_text, m_position.m_x, m_position.m_y, m_angle);
}

bool wxChartLabel::HitTest(const wxPoint &point) const
{
    return false;
}

wxPoint2DDouble wxChartLabel::GetTooltipPosition() const
{
    return wxPoint2DDouble(0, 0);
}

const wxString& wxChartLabel::GetText() const
{
    return m_text;
}

const wxPoint2DDouble& wxChartLabel::GetPosition() const
{
    return m_position;
}

void wxChartLabel::SetPosition(const wxPoint2DDouble &position)
{
    m_position = position;
}

void wxChartLabel::SetPosition(wxDouble x,
                               wxDouble y)
{
    m_position.m_x = x;
    m_position.m_y = y;
}

const wxSize& wxChartLabel::GetSize() const
{
    return m_size;
}

void wxChartLabel::SetSize(const wxSize &size)
{
    m_size = size;
}

void wxChartLabel::SetSize(wxDouble width, wxDouble height)
{
    m_size.x = width;
    m_size.y = height;
}

void UpdateSizeFromAngle(wxGraphicsContext& gc, const wxDouble angle, wxDouble& width, wxDouble& height)
{
    wxPoint2DDouble a(0, 0);
    wxPoint2DDouble b(0, height);
    wxPoint2DDouble c(width, height);
    wxPoint2DDouble d(width, 0);

    wxGraphicsMatrix Mtx = gc.CreateMatrix();
    Mtx.Rotate(angle);
    
    Mtx.TransformPoint(&a.m_x, &a.m_y);
    Mtx.TransformPoint(&b.m_x, &b.m_y);
    Mtx.TransformPoint(&c.m_x, &c.m_y);
    Mtx.TransformPoint(&d.m_x, &d.m_y);
    
    const wxDouble left(wxMin(wxMin(wxMin(a.m_x, b.m_x), c.m_x), d.m_x));
    const wxDouble right(wxMax(wxMax(wxMax(a.m_x, b.m_x), c.m_x), d.m_x));
    const wxDouble bottom(wxMin(wxMin(wxMin(a.m_y, b.m_y), c.m_y), d.m_y));
    const wxDouble top(wxMax(wxMax(wxMax(a.m_y, b.m_y), c.m_y), d.m_y));
    
    width = abs(right - left);
    height = abs(top - bottom);
}

void wxChartLabel::UpdateSize(wxGraphicsContext &gc)
{
    wxFont font = m_options.GetFontOptions().GetFont();
    wxDouble width = 0;
    wxDouble height = 0;
    wxChartUtilities::GetTextSize(gc, font, m_text, width, height);
    UpdateSizeFromAngle(gc, m_angle, width, height);
    m_size.x = width;
    m_size.y = height;
}

const wxChartPadding& wxChartLabel::GetPadding() const
{
    return m_padding;
}

void wxChartLabel::SetPadding(const wxChartPadding &padding)
{
    m_padding = padding;
}

void wxChartLabel::SetAngle(const wxDouble angle)
{
    m_angle = angle;
}
