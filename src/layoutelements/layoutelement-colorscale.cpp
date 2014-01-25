/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011, 2012, 2013 Emanuel Eichhammer                     **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 09.12.13                                             **
**          Version: 1.1.1                                                **
****************************************************************************/

#include "layoutelement-colorscale.h"

#include "../painter.h"
#include "../core.h"
#include "../plottable.h"
#include "../axis.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPColorScale
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPColorScale
  \brief A color scale for use with color coding data such as QCPColorMap
  
  This layout element can be placed on the plot to correlate a color gradient with data values. It
  is usually used in combination with one or multiple \ref QCPColorMap "QCPColorMaps".

  \image html QCPColorScale.png
  
  The color scale can be either horizontal or vertical, as shown in the image above. The
  orientation and the side where the numbers appear is controlled with \ref setType.
  
  Use \ref QCPColorMap::setColorScale to connect a color map with a color scale. Once they are
  connected, they share their gradient, data range and data scale type (\ref setGradient, \ref
  setDataRange, \ref setDataScaleType). Multiple color maps may be associated with a single color
  scale, to make them all synchronize these properties.
  
  To have finer control over the number display and axis behaviour, you can directly access the
  \ref axis. See the documentation of QCPAxis for details about configuring axes. For example, if you want to
  set the label of the axis, call
  \code
  colorScale->axis()->setLabel("some label text");
  \endcode
  
  Placing a color scale next to the main axis rect works like with any other layout element:
  \code
  QCPColorScale *colorScale = new QCPColorScale(customPlot);
  customPlot->plotLayout()->addElement(0, 1, colorScale);
  \endcode
  In this case we have placed it to the right of the default axis rect, so it wasn't necessary to
  call \ref setType, since \ref QCP::atRight is already the default.
  
  For optimum appearance (like in the image above), it may be desirable to line up the axis rect and
  the borders of the color scale. Use a \ref QCPMarginGroup to achieve this:
  \code
  QCPMarginGroup *group = new QCPMarginGroup(customPlot);
  colorScale->setMarginGroup(QCP::msTop|QCP::msBottom, group);
  customPlot->axisRect()->setMarginGroup(QCP::msTop|QCP::msBottom, group);
  \endcode
  
  Color scales are initialized with a non-zero minimum top and bottom margin (\ref
  setMinimumMargins), because vertical color scales are most common and the minimum top/bottom
  margin makes sure it keeps some distance to the top/bottom widget border. So if you change to a
  horizontal color scale by setting \ref setType to \ref QCP::atBottom or \ref QCP::atTop, you
  might want to also change the minimum margins accordingly, e.g. \ref
  setMinimumMargins(QMargins(6, 0, 6, 0)).
*/

/*!
  Constructs a new QCPColorScale. 
*/
QCPColorScale::QCPColorScale(QCustomPlot *parentPlot) :
  QCPLayoutElement(parentPlot),
  mAxisType(QCPAxis::atTop), // set to atTop such that setType(QCPAxis::atRight) below doesn't skip work because it thinks it's already atRight
  mDataScaleType(QCPAxis::stLinear),
  mBarWidth(20),
  mAxisRect(new QCPColorScaleAxisRectPrivate(this))
{
  setMinimumMargins(QMargins(0, 6, 0, 6)); // for default right color scale types, keep some room at bottom and top (important if no margin group is used)
  setType(QCPAxis::atRight);
  setDataRange(QCPRange(0, 6));
}

QCPColorScale::~QCPColorScale()
{
  delete mAxisRect;
}

/* undocumented getter */
bool QCPColorScale::rangeDrag() const
{
  if (!mAxisRect)
  {
    qDebug() << Q_FUNC_INFO << "internal axis rect was deleted";
    return false;
  }
  if (!mColorAxis)
  {
    qDebug() << Q_FUNC_INFO << "internal color axis undefined";
    return false;
  }
  
  return mAxisRect.data()->rangeDrag().testFlag(QCPAxis::orientation(mAxisType)) &&
      mAxisRect.data()->rangeDragAxis(QCPAxis::orientation(mAxisType)) &&
      mAxisRect.data()->rangeDragAxis(QCPAxis::orientation(mAxisType))->orientation() == QCPAxis::orientation(mAxisType);
}

/* undocumented getter */
bool QCPColorScale::rangeZoom() const
{
  if (!mAxisRect)
  {
    qDebug() << Q_FUNC_INFO << "internal axis rect was deleted";
    return false;
  }
  if (!mColorAxis)
  {
    qDebug() << Q_FUNC_INFO << "internal color axis undefined";
    return false;
  }
  
  return mAxisRect.data()->rangeZoom().testFlag(QCPAxis::orientation(mAxisType)) &&
      mAxisRect.data()->rangeZoomAxis(QCPAxis::orientation(mAxisType)) &&
      mAxisRect.data()->rangeZoomAxis(QCPAxis::orientation(mAxisType))->orientation() == QCPAxis::orientation(mAxisType);
}

/*!
  Sets at which side of the color scale the axis is placed, and thus also its orientation.
  
  Note that after setting \a axisType to a different value, the axis returned by \ref axis() will
  be a different one. The new axis will adopt the following properties from the previous axis: The
  range, scale type, log base and label.
*/
void QCPColorScale::setType(QCPAxis::AxisType axisType)
{
  if (!mAxisRect)
  {
    qDebug() << Q_FUNC_INFO << "internal axis rect was deleted";
    return;
  }
  if (mAxisType != axisType)
  {
    mAxisType = axisType;
    QCPRange rangeTransfer(0, 6);
    double logBaseTransfer = 10;
    QString labelTransfer;
    // revert some settings on old axis:
    if (mColorAxis)
    {
      rangeTransfer = mColorAxis.data()->range();
      labelTransfer = mColorAxis.data()->label();
      logBaseTransfer = mColorAxis.data()->scaleLogBase();
      mColorAxis.data()->setLabel("");
      disconnect(mColorAxis.data(), SIGNAL(rangeChanged(QCPRange)), this, SLOT(setDataRange(QCPRange)));
      disconnect(mColorAxis.data(), SIGNAL(scaleTypeChanged(QCPAxis::ScaleType)), this, SLOT(setDataScaleType(QCPAxis::ScaleType)));
    }
    foreach (QCPAxis::AxisType type, QList<QCPAxis::AxisType>() << QCPAxis::atLeft << QCPAxis::atRight << QCPAxis::atBottom << QCPAxis::atTop)
    {
      mAxisRect.data()->axis(type)->setTicks(type == mAxisType);
      mAxisRect.data()->axis(type)->setTickLabels(type== mAxisType);
    }
    // set new mColorAxis pointer:
    mColorAxis = mAxisRect.data()->axis(mAxisType);
    // transfer settings to new axis:
    mColorAxis.data()->setRange(rangeTransfer); // transfer range of old axis to new one (necessary if axis changes from vertical to horizontal or vice versa)
    mColorAxis.data()->setLabel(labelTransfer);
    mColorAxis.data()->setScaleLogBase(logBaseTransfer); // scaleType is synchronized among axes in realtime via signals (connected in QCPColorScale ctor), so we only need to take care of log base here
    connect(mColorAxis.data(), SIGNAL(rangeChanged(QCPRange)), this, SLOT(setDataRange(QCPRange)));
    connect(mColorAxis.data(), SIGNAL(scaleTypeChanged(QCPAxis::ScaleType)), this, SLOT(setDataScaleType(QCPAxis::ScaleType)));
    mAxisRect.data()->setRangeDragAxes(QCPAxis::orientation(mAxisType) == Qt::Horizontal ? mColorAxis.data() : 0,
                                       QCPAxis::orientation(mAxisType) == Qt::Vertical ? mColorAxis.data() : 0);
  }
}

/*!
  Sets the range spanned by the color gradient and that is shown by the axis in the color scale.
  
  It is equivalent to calling QCPColorMap::setDataRange on any of the connected color maps. It is
  also equivalent to directly accessing the \ref axis and setting its range with \ref
  QCPAxis::setRange.
  
  \see setDataScaleType, setGradient
*/
void QCPColorScale::setDataRange(const QCPRange &dataRange)
{
  if (mDataRange.lower != dataRange.lower || mDataRange.upper != dataRange.upper)
  {
    mDataRange = dataRange;
    if (mColorAxis)
      mColorAxis.data()->setRange(mDataRange);
    emit dataRangeChanged(mDataRange);
  }
}

/*!
  Sets the scale type of the color scale, i.e. whether values are linearly associated with colors
  or logarithmically.
  
  It is equivalent to calling QCPColorMap::setDataScaleType on any of the connected color maps. It is
  also equivalent to directly accessing the \ref axis and setting its scale type with \ref
  QCPAxis::setScaleType.
  
  \see setDataRange, setGradient
*/
void QCPColorScale::setDataScaleType(QCPAxis::ScaleType scaleType)
{
  if (mDataScaleType != scaleType)
  {
    mDataScaleType = scaleType;
    if (mColorAxis)
      mColorAxis.data()->setScaleType(mDataScaleType);
    if (mDataScaleType == QCPAxis::stLogarithmic)
      setDataRange(mDataRange.sanitizedForLogScale());
    emit dataScaleTypeChanged(mDataScaleType);
  }
}

/*!
  Sets the color gradient that will be used to represent data values.
  
  It is equivalent to calling QCPColorMap::setGradient on any of the connected color maps.
  
  \see setDataRange, setDataScaleType
*/
void QCPColorScale::setGradient(const QCPColorGradient &gradient)
{
  if (mGradient != gradient)
  {
    mGradient = gradient;
    if (mAxisRect)
      mAxisRect.data()->mGradientImageInvalidated = true;
    emit gradientChanged(mGradient);
  }
}

/*!
  Sets the width (or height, for horizontal color scales) the bar where the gradient is displayed
  will have.
*/
void QCPColorScale::setBarWidth(int width)
{
  mBarWidth = width;
}

/*!
  Sets whether the user can drag the data range (\ref setDataRange).
  
  Note that \ref QCP::iRangeDrag must be in the QCustomPlot's interactions (\ref
  QCustomPlot::setInteractions) to allow range dragging.
*/
void QCPColorScale::setRangeDrag(bool enabled)
{
  if (!mAxisRect)
  {
    qDebug() << Q_FUNC_INFO << "internal axis rect was deleted";
    return;
  }
  
  if (enabled)
    mAxisRect.data()->setRangeDrag(QCPAxis::orientation(mAxisType));
  else
    mAxisRect.data()->setRangeDrag(0);
}

/*!
  Sets whether the user can zoom the data range (\ref setDataRange) by scrolling the mouse wheel.
  
  Note that \ref QCP::iRangeZoom must be in the QCustomPlot's interactions (\ref
  QCustomPlot::setInteractions) to allow range dragging.
*/
void QCPColorScale::setRangeZoom(bool enabled)
{
  if (!mAxisRect)
  {
    qDebug() << Q_FUNC_INFO << "internal axis rect was deleted";
    return;
  }
  
  if (enabled)
    mAxisRect.data()->setRangeZoom(QCPAxis::orientation(mAxisType));
  else
    mAxisRect.data()->setRangeZoom(0);
}

/* inherits documentation from base class */
void QCPColorScale::update(UpdatePhase phase)
{
  QCPLayoutElement::update(phase);
  if (!mAxisRect)
  {
    qDebug() << Q_FUNC_INFO << "internal axis rect was deleted";
    return;
  }
  
  mAxisRect.data()->update(phase);
  
  switch (phase)
  {
    case upMargins:
    {
      if (mAxisType == QCPAxis::atBottom || mAxisType == QCPAxis::atTop)
      {
        setMaximumSize(QWIDGETSIZE_MAX, mBarWidth+mAxisRect.data()->margins().top()+mAxisRect.data()->margins().bottom()+margins().top()+margins().bottom());
        setMinimumSize(0,               mBarWidth+mAxisRect.data()->margins().top()+mAxisRect.data()->margins().bottom()+margins().top()+margins().bottom());
      } else
      {
        setMaximumSize(mBarWidth+mAxisRect.data()->margins().left()+mAxisRect.data()->margins().right()+margins().left()+margins().right(), QWIDGETSIZE_MAX);
        setMinimumSize(mBarWidth+mAxisRect.data()->margins().left()+mAxisRect.data()->margins().right()+margins().left()+margins().right(), 0);
      }
      break;
    }
    case upLayout:
    {
      mAxisRect.data()->setOuterRect(rect());
      break;
    }
    default: break;
  }
}

/* inherits documentation from base class */
void QCPColorScale::applyDefaultAntialiasingHint(QCPPainter *painter) const
{
  painter->setAntialiasing(false);
}

/* inherits documentation from base class */
void QCPColorScale::mousePressEvent(QMouseEvent *event)
{
  if (!mAxisRect)
  {
    qDebug() << Q_FUNC_INFO << "internal axis rect was deleted";
    return;
  }
  mAxisRect.data()->mousePressEvent(event);
}

/* inherits documentation from base class */
void QCPColorScale::mouseMoveEvent(QMouseEvent *event)
{
  if (!mAxisRect)
  {
    qDebug() << Q_FUNC_INFO << "internal axis rect was deleted";
    return;
  }
  mAxisRect.data()->mouseMoveEvent(event);
}

/* inherits documentation from base class */
void QCPColorScale::mouseReleaseEvent(QMouseEvent *event)
{
  if (!mAxisRect)
  {
    qDebug() << Q_FUNC_INFO << "internal axis rect was deleted";
    return;
  }
  mAxisRect.data()->mouseReleaseEvent(event);
}

/* inherits documentation from base class */
void QCPColorScale::wheelEvent(QWheelEvent *event)
{
  if (!mAxisRect)
  {
    qDebug() << Q_FUNC_INFO << "internal axis rect was deleted";
    return;
  }
  mAxisRect.data()->wheelEvent(event);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPColorScaleAxisRectPrivate
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPColorScaleAxisRectPrivate \internal
  \brief An axis rect subclass for use in a QCPColorScale
  
  This is a private class and not part of the public QCustomPlot interface.
  
  It provides the axis rect functionality for the QCPColorScale class.
*/


/*!
  Creates a new instance, as a child of \a parentColorScale.
*/
QCPColorScaleAxisRectPrivate::QCPColorScaleAxisRectPrivate(QCPColorScale *parentColorScale) :
  QCPAxisRect(parentColorScale->parentPlot(), true),
  mParentColorScale(parentColorScale),
  mGradientImageInvalidated(true)
{
  setParentLayerable(parentColorScale);
  setMinimumMargins(QMargins(0, 0, 0, 0));
  foreach (QCPAxis::AxisType type, QList<QCPAxis::AxisType>() << QCPAxis::atBottom << QCPAxis::atTop << QCPAxis::atLeft << QCPAxis::atRight)
  {
    axis(type)->setVisible(true);
    axis(type)->grid()->setVisible(false);
    axis(type)->setPadding(0);
    connect(axis(type), SIGNAL(selectionChanged(QCPAxis::SelectableParts)), this, SLOT(axisSelectionChanged(QCPAxis::SelectableParts)));
    connect(axis(type), SIGNAL(selectableChanged(QCPAxis::SelectableParts)), this, SLOT(axisSelectableChanged(QCPAxis::SelectableParts)));
  }

  connect(axis(QCPAxis::atLeft), SIGNAL(rangeChanged(QCPRange)), axis(QCPAxis::atRight), SLOT(setRange(QCPRange)));
  connect(axis(QCPAxis::atRight), SIGNAL(rangeChanged(QCPRange)), axis(QCPAxis::atLeft), SLOT(setRange(QCPRange)));
  connect(axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), axis(QCPAxis::atTop), SLOT(setRange(QCPRange)));
  connect(axis(QCPAxis::atTop), SIGNAL(rangeChanged(QCPRange)), axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
  connect(axis(QCPAxis::atLeft), SIGNAL(scaleTypeChanged(QCPAxis::ScaleType)), axis(QCPAxis::atRight), SLOT(setScaleType(QCPAxis::ScaleType)));
  connect(axis(QCPAxis::atRight), SIGNAL(scaleTypeChanged(QCPAxis::ScaleType)), axis(QCPAxis::atLeft), SLOT(setScaleType(QCPAxis::ScaleType)));
  connect(axis(QCPAxis::atBottom), SIGNAL(scaleTypeChanged(QCPAxis::ScaleType)), axis(QCPAxis::atTop), SLOT(setScaleType(QCPAxis::ScaleType)));
  connect(axis(QCPAxis::atTop), SIGNAL(scaleTypeChanged(QCPAxis::ScaleType)), axis(QCPAxis::atBottom), SLOT(setScaleType(QCPAxis::ScaleType)));
  
  // make layer transfers of color scale transfer to axis rect and axes
  // the axes must be set after axis rect, such that they appear above color gradient drawn by axis rect:
  connect(parentColorScale, SIGNAL(layerChanged(QCPLayer*)), this, SLOT(setLayer(QCPLayer*)));
  foreach (QCPAxis::AxisType type, QList<QCPAxis::AxisType>() << QCPAxis::atBottom << QCPAxis::atTop << QCPAxis::atLeft << QCPAxis::atRight)
    connect(parentColorScale, SIGNAL(layerChanged(QCPLayer*)), axis(type), SLOT(setLayer(QCPLayer*)));
}

/*! \internal
  Updates the color gradient image if necessary, by calling \ref updateGradientImage, then draws
  it. Then the axes are drawn by calling the \ref QCPAxisRect::draw base class implementation.
*/
void QCPColorScaleAxisRectPrivate::draw(QCPPainter *painter)
{
  if (mGradientImageInvalidated)
    updateGradientImage();
  
  bool mirrorHorz = false;
  bool mirrorVert = false;
  if (mParentColorScale->mColorAxis)
  {
    mirrorHorz = mParentColorScale->mColorAxis.data()->rangeReversed() && (mParentColorScale->axisType() == QCPAxis::atBottom || mParentColorScale->axisType() == QCPAxis::atTop); 
    mirrorVert = mParentColorScale->mColorAxis.data()->rangeReversed() && (mParentColorScale->axisType() == QCPAxis::atLeft || mParentColorScale->axisType() == QCPAxis::atRight); 
  }
  
  painter->drawImage(rect(), mGradientImage.mirrored(mirrorHorz, mirrorVert));
  QCPAxisRect::draw(painter);
}

/*! \internal

  Uses the current gradient of the parent \ref QCPColorScale (specified in the constructor) to
  generate a gradient image. This gradient image will be used in the \ref draw method.
*/
void QCPColorScaleAxisRectPrivate::updateGradientImage()
{
  if (rect().isEmpty())
    return;
  
  int n = mParentColorScale->mGradient.levelCount();
  int w, h;
  QVector<double> data(n);
  for (int i=0; i<n; ++i)
    data[i] = i;
  if (mParentColorScale->mAxisType == QCPAxis::atBottom || mParentColorScale->mAxisType == QCPAxis::atTop)
  {
    w = n;
    h = rect().height();
    mGradientImage = QImage(w, h, QImage::Format_RGB32);
    QVector<QRgb*> pixels;
    for (int y=0; y<h; ++y)
      pixels.append(reinterpret_cast<QRgb*>(mGradientImage.scanLine(y)));
    mParentColorScale->mGradient.colorize(data.constData(), QCPRange(0, n-1), pixels.first(), n);
    for (int y=1; y<h; ++y)
      memcpy(pixels.at(y), pixels.first(), n*sizeof(QRgb));
  } else
  {
    w = rect().width();
    h = n;
    mGradientImage = QImage(w, h, QImage::Format_RGB32);
    for (int y=0; y<h; ++y)
    {
      QRgb *pixels = reinterpret_cast<QRgb*>(mGradientImage.scanLine(y));
      const QRgb lineColor = mParentColorScale->mGradient.color(data[h-1-y], QCPRange(0, n-1));
      for (int x=0; x<w; ++x)
        pixels[x] = lineColor;
    }
  }
  mGradientImageInvalidated = false;
}

/*! \internal

  This slot is connected to the selectionChanged signals of the four axes in the constructor. It
  synchronizes the selection state of the axes.
*/
void QCPColorScaleAxisRectPrivate::axisSelectionChanged(QCPAxis::SelectableParts selectedParts)
{
  // axis bases of four axes shall always (de-)selected synchronously:
  foreach (QCPAxis::AxisType type, QList<QCPAxis::AxisType>() << QCPAxis::atBottom << QCPAxis::atTop << QCPAxis::atLeft << QCPAxis::atRight)
  {
    if (QCPAxis *senderAxis = qobject_cast<QCPAxis*>(sender()))
      if (senderAxis->axisType() == type)
        continue;
    
    if (axis(type)->selectableParts().testFlag(QCPAxis::spAxis))
    {
      if (selectedParts.testFlag(QCPAxis::spAxis))
        axis(type)->setSelectedParts(axis(type)->selectedParts() | QCPAxis::spAxis);
      else
        axis(type)->setSelectedParts(axis(type)->selectedParts() & ~QCPAxis::spAxis);
    }
  }
}

/*! \internal

  This slot is connected to the selectableChanged signals of the four axes in the constructor. It
  synchronizes the selectability of the axes.
*/
void QCPColorScaleAxisRectPrivate::axisSelectableChanged(QCPAxis::SelectableParts selectableParts)
{
  // synchronize axis base selectability:
  foreach (QCPAxis::AxisType type, QList<QCPAxis::AxisType>() << QCPAxis::atBottom << QCPAxis::atTop << QCPAxis::atLeft << QCPAxis::atRight)
  {
    if (QCPAxis *senderAxis = qobject_cast<QCPAxis*>(sender()))
      if (senderAxis->axisType() == type)
        continue;
    
    if (axis(type)->selectableParts().testFlag(QCPAxis::spAxis))
    {
      if (selectableParts.testFlag(QCPAxis::spAxis))
        axis(type)->setSelectableParts(axis(type)->selectableParts() | QCPAxis::spAxis);
      else
        axis(type)->setSelectableParts(axis(type)->selectableParts() & ~QCPAxis::spAxis);
    }
  }
}
