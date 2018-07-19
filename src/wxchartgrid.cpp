/*
    Copyright (c) 2016-2018 Xavier Leclercq and the wxCharts contributors.

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

#include "wxchartgrid.h"
#include "wxchartnumericalaxis.h"
#include "wxchartcategoricalaxis.h"
#include <wx/pen.h>

static const wxDouble MinDistance = 1.0e-3;
static const wxDouble MaxDistance = 1.0e3;

wxChartGrid::wxChartGrid(const wxPoint2DDouble &position,
                         const wxSize &size,
                         wxChartAxis::ptr xAxis,
                         wxChartAxis::ptr yAxis,
                         const wxChartGridOptions& options)
    : m_options(options), m_position(position),
    m_XAxis(xAxis), m_YAxis(yAxis),
    m_mapping(size, m_XAxis, m_YAxis),
    m_needsFit(true)
{
}

wxChartGrid::wxChartGrid(const wxPoint2DDouble &position,
                         const wxSize &size,
                         wxDouble minXValue,
                         wxDouble maxXValue,
                         wxDouble minYValue,
                         wxDouble maxYValue,
                         const wxChartGridOptions& options)
    : m_options(options), m_position(position),
      m_XAxis(new wxChartNumericalAxis("x", minXValue, maxXValue, options.GetXAxisOptions())),
      m_YAxis(new wxChartNumericalAxis("y", minYValue, maxYValue, options.GetYAxisOptions())),
      m_mapping(size, m_XAxis, m_YAxis),
      m_needsFit(true),
      m_origAxisLimits(minXValue,maxXValue,minYValue,maxYValue),
      m_curAxisLimits(minXValue,maxXValue,minYValue,maxYValue)
{

}

void wxChartGrid::Draw(wxGraphicsContext &gc) const
{
    const wxChartAxis* verticalAxis = 0;
    if (m_XAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_LEFT)
    {
        verticalAxis = m_XAxis.get();
    }
    else if (m_YAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_LEFT)
    {
        verticalAxis = m_YAxis.get();
    }

    const wxChartAxis* horizontalAxis = 0;
    if (m_XAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_BOTTOM)
    {
        horizontalAxis = m_XAxis.get();
    }
    else if (m_YAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_BOTTOM)
    {
        horizontalAxis = m_YAxis.get();
    }

    if (m_options.GetHorizontalGridLineOptions().ShowGridLines())
    {
        DrawHorizontalGridLines(*horizontalAxis, *verticalAxis, m_options.GetHorizontalGridLineOptions(), gc);
    }

    if (m_options.GetVerticalGridLineOptions().ShowGridLines())
    {
        DrawVerticalGridLines(*horizontalAxis, *verticalAxis, m_options.GetVerticalGridLineOptions(), gc);
    }

    m_XAxis->Draw(gc);
    m_YAxis->Draw(gc);
}

bool wxChartGrid::HitTest(const wxPoint &point) const
{
    return false;
}

wxPoint2DDouble wxChartGrid::GetTooltipPosition() const
{
    return wxPoint2DDouble(0, 0);
}

void wxChartGrid::Fit(wxGraphicsContext &gc)
{
    if (!m_needsFit)
    {
        return;
    }

    // Apply padding settings to the start and end point.
    //this.startPoint += this.padding;
    //this.endPoint -= this.padding;

    // need to find out the max width without an angle
    m_XAxis->GetLabels().SetAngle(0);
    m_YAxis->UpdateLabelSizes(gc);
    m_XAxis->UpdateLabelSizes(gc);

    // now check if we need an angle

    // enlarge max_width by height to not allow the labels to get too close together
    // before starting to angle them
    const wxDouble maxWidth = m_XAxis->GetLabels().GetMaxWidth() + m_XAxis->GetLabels().GetMaxHeight();
    if (maxWidth > m_XAxis->GetDistanceBetweenTickMarks())
    {
        const wxDouble overlap = maxWidth - m_XAxis->GetDistanceBetweenTickMarks();
        const wxDouble interpolator = wxMax(0, wxMin(1, overlap / m_XAxis->GetDistanceBetweenTickMarks()));

        // calculate angle in the range [min, max]
        wxDouble angle = interpolator * (m_XAxis->GetOptions().GetMaxAngle() - m_XAxis->GetOptions().GetMinAngle()) + m_XAxis->GetOptions().GetMinAngle();

        // ensure angle is in the correct range
        if (m_XAxis->GetOptions().GetMinAngle() > m_XAxis->GetOptions().GetMaxAngle())
            angle = wxMax(wxMin(m_XAxis->GetOptions().GetMinAngle(), angle), m_XAxis->GetOptions().GetMaxAngle());
        else
            angle = wxMin(wxMax(m_XAxis->GetOptions().GetMinAngle(), angle), m_XAxis->GetOptions().GetMaxAngle());

        m_XAxis->GetLabels().SetAngle(angle);

        // need to update sizes again with angle so the padding will be calculated correctly
        m_YAxis->UpdateLabelSizes(gc);
        m_XAxis->UpdateLabelSizes(gc);
    }

    const wxDouble xAxisLabelsBottomPadding = m_XAxis->GetLabels().GetMaxHeight() + 15 + 5;
    
    const wxDouble startPoint = m_mapping.GetSize().GetHeight() - xAxisLabelsBottomPadding;
    const wxDouble endPoint = m_YAxis->GetOptions().GetFontOptions().GetSize();

    wxDouble leftPadding = 0;
    wxDouble rightPadding = 0;
    wxDouble bottomPadding = 0;
    wxDouble topPadding = 0;
    CalculatePadding(*m_XAxis, *m_YAxis, leftPadding, rightPadding, bottomPadding, topPadding);

    if (m_XAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_BOTTOM)
    {
        m_XAxis->Fit(wxPoint2DDouble(leftPadding, startPoint - bottomPadding), wxPoint2DDouble(m_mapping.GetSize().GetWidth() - rightPadding, startPoint - bottomPadding));
        m_YAxis->Fit(wxPoint2DDouble(leftPadding, startPoint - bottomPadding), wxPoint2DDouble(leftPadding, endPoint + topPadding));
    }
    else if (m_XAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_LEFT)
    {
        m_XAxis->Fit(wxPoint2DDouble(leftPadding, startPoint), wxPoint2DDouble(leftPadding, endPoint));
        m_YAxis->Fit(wxPoint2DDouble(leftPadding, startPoint), wxPoint2DDouble(m_mapping.GetSize().GetWidth() - rightPadding, startPoint));
    }

    m_XAxis->UpdateLabelPositions();
    m_YAxis->UpdateLabelPositions();

    m_needsFit = false;
}

void wxChartGrid::Resize(const wxSize &size)
{
    m_mapping.SetSize(size);
    m_needsFit = true;
}

const wxChartGridMapping& wxChartGrid::GetMapping() const
{
    return m_mapping;
}

void wxChartGrid::SetOptions(const wxChartGridOptions& opt)
{
    m_options = opt;
}

const wxChartGridOptions& wxChartGrid::GetOptions() const
{
    return m_options;
}

bool wxChartGrid::Scale(int c)
{
    if(c)
    {
        c = c>0 ? 4 : -2;
        double deltaX = (m_curAxisLimits.MaxX-m_curAxisLimits.MinX)/c;
        double deltaY = (m_curAxisLimits.MaxY-m_curAxisLimits.MinY)/c;

        auto absDX = std::abs(deltaX);
        auto absDY = std::abs(deltaY);

        if(absDX > MaxDistance || absDX < MinDistance
                || absDY > MaxDistance || absDY < MinDistance)
            return false;

        m_curAxisLimits.MinX+=deltaX;
        m_curAxisLimits.MaxX-=deltaX;
        m_curAxisLimits.MinY+=deltaY;
        m_curAxisLimits.MaxY-=deltaY;
    }
    else
        m_curAxisLimits = m_origAxisLimits;
    Update();
    return true;
}

void wxChartGrid::Shift(double dx,double dy)
{
    double deltaX = (m_curAxisLimits.MaxX-m_curAxisLimits.MinX)*dx;
    m_curAxisLimits.MinX+=deltaX;
    m_curAxisLimits.MaxX+=deltaX;

    double deltaY = (m_curAxisLimits.MaxY-m_curAxisLimits.MinY)*dy;
    m_curAxisLimits.MinY+=deltaY;
    m_curAxisLimits.MaxY+=deltaY;

    Update();
}

void wxChartGrid::UpdateAxisLimit(const std::string& axisId, wxDouble min, wxDouble max)
{
    if(axisId == "x")
    {
        m_origAxisLimits.MinX = min;
        m_origAxisLimits.MaxX = max;

        m_curAxisLimits.MinX = min;
        m_curAxisLimits.MaxX = max;

        m_XAxis = new wxChartNumericalAxis(axisId,m_curAxisLimits.MinX,
                                           m_curAxisLimits.MaxX,m_options.GetXAxisOptions());
    }
    else if(axisId == "y")
    {
        m_origAxisLimits.MinY = min;
        m_origAxisLimits.MaxY = max;

        m_curAxisLimits.MinY = min;
        m_curAxisLimits.MaxY = max;
        m_YAxis = new wxChartNumericalAxis(axisId,m_curAxisLimits.MinY,
                                           m_curAxisLimits.MaxY, m_options.GetYAxisOptions());
    }
    m_mapping = wxChartGridMapping(m_mapping.GetSize(), m_XAxis, m_YAxis);
    m_needsFit = true;
}

void wxChartGrid::ChangeLabels(const std::string& axisId, const wxVector<wxString> &labels, wxChartAxisOptions options)
{
    if(axisId == "x")
        m_XAxis = wxChartCategoricalAxis::make_shared(axisId,labels,options);
    else if(axisId == "y")
        m_YAxis = wxChartCategoricalAxis::make_shared(axisId,labels,options);

    m_mapping = wxChartGridMapping(m_mapping.GetSize(), m_XAxis, m_YAxis);
    m_needsFit = true;
}

void wxChartGrid::Update()
{
    m_XAxis = new wxChartNumericalAxis("x", m_curAxisLimits.MinX,
        m_curAxisLimits.MaxX,m_options.GetXAxisOptions());
    m_YAxis = new wxChartNumericalAxis("y", m_curAxisLimits.MinY,
        m_curAxisLimits.MaxY, m_options.GetYAxisOptions());
    m_mapping = wxChartGridMapping(m_mapping.GetSize(), m_XAxis, m_YAxis);
    m_needsFit = true;
}

void wxChartGrid::CalculatePadding(const wxChartAxis &xAxis,
                                   const wxChartAxis &yAxis,
                                   wxDouble &left,
                                   wxDouble &right,
                                   wxDouble &bottom,
                                   wxDouble &top)
{
    if (xAxis.GetOptions().GetPosition() == wxCHARTAXISPOSITION_BOTTOM)
    {
        // Either the first x label or any of the y labels can be the widest
        // so they are all taken into account to compute the left padding
        left = yAxis.GetLabels().GetMaxWidth() + 10;
        if ((xAxis.GetLabels().size() > 0) && ((xAxis.GetLabels().front().GetSize().GetWidth() / 2) > left))
        {
            left = (xAxis.GetLabels().front().GetSize().GetWidth() / 2);
        }

        right = 0;
        if (xAxis.GetLabels().size() > 0)
        {
            right = xAxis.GetLabels().back().GetSize().GetWidth();
            if (xAxis.GetLabels().GetAngle() == 0)
                right /= 2;
        }
    }
    else if (xAxis.GetOptions().GetPosition() == wxCHARTAXISPOSITION_LEFT)
    {
        // Either the first y label or any of the x labels can be the widest
        // so they are all taken into account to compute the left padding
        left = xAxis.GetLabels().GetMaxWidth() + 10;
        if ((yAxis.GetLabels().size() > 0) && ((yAxis.GetLabels().front().GetSize().GetWidth() / 2) > left))
        {
            left = (yAxis.GetLabels().front().GetSize().GetWidth() / 2);
        }

        right = 0;
        if (yAxis.GetLabels().size() > 0)
        {
            right = (yAxis.GetLabels().back().GetSize().GetWidth() / 2);
        }
    }
}

void wxChartGrid::DrawHorizontalGridLines(const wxChartAxis &horizontalAxis,
        const wxChartAxis &verticalAxis,
        const wxChartGridLineOptions &options,
        wxGraphicsContext &gc)
{
    for (size_t i = 1; i < verticalAxis.GetNumberOfTickMarks(); ++i)
    {
        wxPoint2DDouble lineStartPosition = verticalAxis.GetTickMarkPosition(i);
        wxPoint2DDouble lineEndPosition = horizontalAxis.GetTickMarkPosition(horizontalAxis.GetNumberOfTickMarks() - 1);
        if (horizontalAxis.GetOptions().GetEndMarginType() == wxCHARTAXISMARGINTYPE_TICKMARKOFFSET)
        {
            lineEndPosition.m_x += horizontalAxis.GetDistanceBetweenTickMarks();
        }

        wxGraphicsPath path = gc.CreatePath();
        path.MoveToPoint(lineStartPosition);
        path.AddLineToPoint(lineEndPosition.m_x + horizontalAxis.GetOptions().GetOverhang(), lineStartPosition.m_y);

        wxPen pen1(options.GetMajorGridLineColor(), options.GetMajorGridLineWidth());
        gc.SetPen(pen1);
        gc.StrokePath(path);

        unsigned int n = options.GetNumberOfMinorGridLinesBetweenTickMarks();
        if (n != 0)
        {
            wxDouble spacing = verticalAxis.GetDistanceBetweenTickMarks() / (n + 1);
            wxDouble currentSpacing = spacing;
            for (size_t j = 0; j < n; ++j)
            {
                wxGraphicsPath path = gc.CreatePath();
                path.MoveToPoint(lineStartPosition.m_x, lineStartPosition.m_y + currentSpacing);
                path.AddLineToPoint(lineEndPosition.m_x + horizontalAxis.GetOptions().GetOverhang(), lineStartPosition.m_y + currentSpacing);

                wxPen pen1(wxColor(0, 0, 0, 0x0C), 1);
                gc.SetPen(pen1);
                gc.StrokePath(path);

                currentSpacing += spacing;
            }
        }
    }
}

void wxChartGrid::DrawVerticalGridLines(const wxChartAxis &horizontalAxis,
                                        const wxChartAxis &verticalAxis,
                                        const wxChartGridLineOptions &options,
                                        wxGraphicsContext &gc)
{
    size_t i = 1;
    if (horizontalAxis.GetOptions().GetStartMarginType() == wxCHARTAXISMARGINTYPE_TICKMARKOFFSET)
    {
        i = 0;
    }
    size_t end = horizontalAxis.GetNumberOfTickMarks();
    if (horizontalAxis.GetOptions().GetEndMarginType() == wxCHARTAXISMARGINTYPE_TICKMARKOFFSET)
    {
        ++end;
    }
    for (; i < end; ++i)
    {
        wxPoint2DDouble lineStartPosition = horizontalAxis.GetTickMarkPosition(i);
        wxPoint2DDouble lineEndPosition = verticalAxis.GetTickMarkPosition(verticalAxis.GetNumberOfTickMarks() - 1);

        wxGraphicsPath path = gc.CreatePath();
        path.MoveToPoint(lineStartPosition);
        path.AddLineToPoint(lineStartPosition.m_x, lineEndPosition.m_y - verticalAxis.GetOptions().GetOverhang());

        wxPen pen1(options.GetMajorGridLineColor(), options.GetMajorGridLineWidth());
        gc.SetPen(pen1);
        gc.StrokePath(path);

        unsigned int n = options.GetNumberOfMinorGridLinesBetweenTickMarks();
        if (n != 0)
        {
            wxDouble spacing = horizontalAxis.GetDistanceBetweenTickMarks() / (n + 1);
            wxDouble currentSpacing = spacing;
            for (size_t j = 0; j < n; ++j)
            {
                wxGraphicsPath path = gc.CreatePath();
                path.MoveToPoint(lineStartPosition.m_x - currentSpacing, lineStartPosition.m_y);
                path.AddLineToPoint(lineStartPosition.m_x - currentSpacing, lineEndPosition.m_y - verticalAxis.GetOptions().GetOverhang());

                wxPen pen1(wxColor(0, 0, 0, 0x0C), 1);
                gc.SetPen(pen1);
                gc.StrokePath(path);

                currentSpacing += spacing;
            }
        }
    }
}
