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
**             Date: 01.08.13                                             **
**          Version: 1.0.0                                                **
****************************************************************************/

#include "layout.h"
#include "core.h"

/*! \page thelayoutsystem The Layout System
 
  The layout system is responsible for positioning and scaling layout elements such as axis rects,
  legends and plot titles in a QCustomPlot.

  \section layoutsystem-classesandmechanisms Classes and mechanisms
  
  The layout system is based on the abstract base class \ref QCPLayoutElement. All objects that
  take part in the layout system derive from this class, either directly or indirectly.
  
  Since QCPLayoutElement itself derives from \ref QCPLayerable, a layout element may draw its own
  content. However, it is perfectly possible for a layout element to only serve as a structuring
  and/or positioning element, not drawing anything on its own.
  
  \subsection layoutsystem-rects Rects of a layout element
  
  A layout element is a rectangular object described by two rects: the inner rect (\ref
  QCPLayoutElement::rect) and the outer rect (\ref QCPLayoutElement::setOuterRect). The inner rect
  is calculated automatically by applying the margin (\ref QCPLayoutElement::setMargins) inward
  from the outer rect. The inner rect is meant for main content while the margin area may either be
  left blank or serve for displaying peripheral graphics. For example, \ref QCPAxisRect positions
  the four main axes at the sides of the inner rect, so graphs end up inside it and the axis labels
  and tick labels are in the margin area.
  
  \subsection layoutsystem-margins Margins
  
  Each layout element may provide a mechanism to automatically determine its margins. Internally,
  this is realized with the \ref QCPLayoutElement::calculateAutoMargin function which takes a \ref
  QCP::MarginSide and returns an integer value which represents the ideal margin for the specified
  side. The automatic margin will be used on the sides specified in \ref
  QCPLayoutElement::setAutoMargins. By default, it is set to \ref QCP::msAll meaning automatic
  margin calculation is enabled for all four sides. In this case, a minimum margin may be set with
  \ref QCPLayoutElement::setMinimumMargins, to prevent the automatic margin mechanism from setting
  margins smaller than desired for a specific situation. If automatic margin calculation is unset
  for a specific side, the margin of that side can be controlled directy via \ref
  QCPLayoutElement::setMargins.
  
  If multiple layout ements are arranged next to or beneath each other, it may be desirable to
  align their inner rects on certain sides. Since they all might have different automatic margins,
  this usually isn't the case. The class \ref QCPMarginGroup and \ref
  QCPLayoutElement::setMarginGroup fix this by allowing to synchronize multiple margins. See the
  documentation there for details.
  
  \subsection layoutsystem-layout Layouts
  
  As mentioned, a QCPLayoutElement may have an arbitrary number of child layout elements and in
  princple can have the only purpose to manage/arrange those child elements. This is what the
  subclass \ref QCPLayout specializes on. It is a QCPLayoutElement itself but has no visual
  representation. It defines an interface to add, remove and manage child layout elements.
  QCPLayout isn't a usable layout though, it's an abstract base class that concrete layouts derive
  from, like \ref QCPLayoutGrid which arranges its child elements in a grid and \ref QCPLayoutInset
  which allows placing child elements freely inside its rect.
  
  Since a QCPLayout is a layout element itself, it may be placed inside other layouts. This way,
  complex hierarchies may be created, offering very flexible arrangements.
  
  <div style="text-align:center">
  <div style="display:inline-block; margin-left:auto; margin-right:auto"><img src="LayoutsystemSketch0.png"></div>
  <div style="display:inline-block; margin-left:auto; margin-right:auto"><img src="LayoutsystemSketch1.png"></div>
  <div style="clear:both"></div>
  <div style="display:inline-block; max-width:1000px; text-align:justify">
  Sketch of the default QCPLayoutGrid accessible via \ref QCustomPlot::plotLayout. The left image
  shows the outer and inner rect of the grid layout itself while the right image shows how two
  child layout elements are placed inside the grid layout next to each other in cells (0, 0) and
  (0, 1).
  </div>
  </div>
  
  \subsection layoutsystem-plotlayout The top level plot layout
  
  Every QCustomPlot has one top level layout of type \ref QCPLayoutGrid. It is accessible via \ref
  QCustomPlot::plotLayout and contains (directly or indirectly via other sub-layouts) all layout
  elements in the QCustomPlot. By default, this top level grid layout contains a single cell which
  holds the main axis rect.
 
  \subsection layoutsystem-examples Examples
  
  <b>Adding a plot title</b> is a typical and simple case to demonstrate basic workings of the layout system.
  \code
  // first we create and prepare a plot title layout element:
  QCPPlotTitle *title = new QCPPlotTitle(customPlot);
  title->setText("Plot Title Example");
  title->setFont(QFont("sans", 12, QFont::Bold));
  // then we add it to the main plot layout:
  customPlot->plotLayout()->insertRow(0); // insert an empty row above the axis rect
  customPlot->plotLayout()->addElement(0, 0, title); // insert the title in the empty cell we just created
  \endcode
  \image html layoutsystem-addingplottitle.png

  <b>Arranging multiple axis rects</b> actually is the central purpose of the layout system.
  \code
  customPlot->plotLayout()->clear(); // let's start from scratch and remove the default axis rect
  // add the first axis rect in second row (row index 1):
  customPlot->plotLayout()->addElement(1, 0, new QCPAxisRect(customPlot));
  // create a sub layout that we'll place in first row:
  QCPLayoutGrid *subLayout = new QCPLayoutGrid;
  customPlot->plotLayout()->addElement(0, 0, subLayout);
  // add two axis rects in the sub layout next to eachother:
  subLayout->addElement(0, 0, new QCPAxisRect(customPlot));
  subLayout->addElement(0, 1, new QCPAxisRect(customPlot));
  subLayout->setColumnStretchFactor(0, 3); // left axis rect shall have 60% of width
  subLayout->setColumnStretchFactor(1, 2); // right one only 40% (3:2 = 60:40)
  \endcode
  \image html layoutsystem-multipleaxisrects.png
  
*/


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPMarginGroup
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPMarginGroup
  \brief A margin group allows synchronization of margin sides if working with multiple layout elements.
  
  QCPMarginGroup allows you to tie a margin side of two or more layout elements together, such that
  they will all have the same size, based on the largest required margin in the group.
  
  \n
  \image html QCPMarginGroup.png "Demonstration of QCPMarginGroup"
  \n
  
  In certain situations it is desirable that margins at specific sides are synchronized across
  layout elements. For example, if one QCPAxisRect is below another one in a grid layout, it will
  provide a cleaner look to the user if the left and right margins of the two axis rects are of the
  same size. The left axis of the top axis rect will then be at the same horizontal position as the
  left axis of the lower axis rect, making them appear aligned. The same applies for the right
  axes. This is what QCPMarginGroup makes possible.
  
  To add/remove a specific side of a layout element to/from a margin group, use the \ref
  QCPLayoutElement::setMarginGroup method. To completely break apart the margin group, either call
  \ref clear, or just delete the margin group.
  
  \section QCPMarginGroup-example Example
  
  First create a margin group:
  \code 
  QCPMarginGroup *group = new QCPMarginGroup(customPlot);
  \endcode
  Then set this group on the layout element sides:
  \code
  customPlot->axisRect(0)->setMarginGroup(QCP::msLeft|QCP::msRight, group);
  customPlot->axisRect(1)->setMarginGroup(QCP::msLeft|QCP::msRight, group);
  \endcode
  Here, we've used the first two axis rects of the plot and synchronized their left margins with
  each other and their right margins with each other.
*/

/* start documentation of inline functions */

/*! \fn QList<QCPLayoutElement*> QCPMarginGroup::elements(QCP::MarginSide side) const
  
  Returns a list of all layout elements that have their margin \a side associated with this margin
  group.
*/

/* end documentation of inline functions */

/*!
  Creates a new QCPMarginGroup instance in \a parentPlot.
*/
QCPMarginGroup::QCPMarginGroup(QCustomPlot *parentPlot) :
  QObject(parentPlot),
  mParentPlot(parentPlot)
{
  mChildren.insert(QCP::msLeft, QList<QCPLayoutElement*>());
  mChildren.insert(QCP::msRight, QList<QCPLayoutElement*>());
  mChildren.insert(QCP::msTop, QList<QCPLayoutElement*>());
  mChildren.insert(QCP::msBottom, QList<QCPLayoutElement*>());
}

QCPMarginGroup::~QCPMarginGroup()
{
  clear();
}

/*!
  Returns whether this margin group is empty. If this function returns true, no layout elements use
  this margin group to synchronize margin sides.
*/
bool QCPMarginGroup::isEmpty() const
{
  QHashIterator<QCP::MarginSide, QList<QCPLayoutElement*> > it(mChildren);
  while (it.hasNext())
  {
    it.next();
    if (!it.value().isEmpty())
      return false;
  }
  return true;
}

/*!
  Clears this margin group. The synchronization of the margin sides that use this margin group is
  lifted and they will use their individual margin sizes again.
*/
void QCPMarginGroup::clear()
{
  // make all children remove themselves from this margin group:
  QHashIterator<QCP::MarginSide, QList<QCPLayoutElement*> > it(mChildren);
  while (it.hasNext())
  {
    it.next();
    const QList<QCPLayoutElement*> elements = it.value();
    for (int i=elements.size()-1; i>=0; --i)
      elements.at(i)->setMarginGroup(it.key(), 0); // removes itself from mChildren via removeChild
  }
}

/*! \internal
  
  Returns the synchronized common margin for \a side. This is the margin value that will be used by
  the layout element on the respective side, if it is part of this margin group.
  
  The common margin is calculated by requesting the automatic margin (\ref
  QCPLayoutElement::calculateAutoMargin) of each element associated with \a side in this margin
  group, and choosing the largest returned value. (QCPLayoutElement::minimumMargins is taken into
  account, too.)
*/
int QCPMarginGroup::commonMargin(QCP::MarginSide side) const
{
  // query all automatic margins of the layout elements in this margin group side and find maximum:
  int result = 0;
  const QList<QCPLayoutElement*> elements = mChildren.value(side);
  for (int i=0; i<elements.size(); ++i)
  {
    if (!elements.at(i)->autoMargins().testFlag(side))
      continue;
    int m = qMax(elements.at(i)->calculateAutoMargin(side), QCP::getMarginValue(elements.at(i)->minimumMargins(), side));
    if (m > result)
      result = m;
  }
  return result;
}

/*! \internal
  
  Adds \a element to the internal list of child elements, for the margin \a side.
  
  This function does not modify the margin group property of \a element.
*/
void QCPMarginGroup::addChild(QCP::MarginSide side, QCPLayoutElement *element)
{
  if (!mChildren[side].contains(element))
    mChildren[side].append(element);
  else
    qDebug() << Q_FUNC_INFO << "element is already child of this margin group side" << reinterpret_cast<quintptr>(element);
}

/*! \internal
  
  Removes \a element from the internal list of child elements, for the margin \a side.
  
  This function does not modify the margin group property of \a element.
*/
void QCPMarginGroup::removeChild(QCP::MarginSide side, QCPLayoutElement *element)
{
  if (!mChildren[side].removeOne(element))
    qDebug() << Q_FUNC_INFO << "element is not child of this margin group side" << reinterpret_cast<quintptr>(element);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPLayoutElement
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPLayoutElement
  \brief The abstract base class for all objects that form \ref thelayoutsystem "the layout system".
  
  This is an abstract base class. As such, it can't be instantiated directly, rather use one of its subclasses.
  
  A Layout element is a rectangular object which can be placed in layouts. It has an outer rect
  (QCPLayoutElement::outerRect) and an inner rect (\ref QCPLayoutElement::rect). The difference
  between outer and inner rect is called its margin. The margin can either be set to automatic or
  manual (\ref setAutoMargins) on a per-side basis. If a side is set to manual, that margin can be
  set explicitly with \ref setMargins and will stay fixed at that value. If it's set to automatic,
  the layout element subclass will control the value itself (via \ref calculateAutoMargin).
  
  Layout elements can be placed in layouts (base class QCPLayout) like QCPLayoutGrid. The top level
  layout is reachable via \ref QCustomPlot::plotLayout, and is a \ref QCPLayoutGrid. Since \ref
  QCPLayout itself derives from \ref QCPLayoutElement, layouts can be nested.
  
  Thus in QCustomPlot one can divide layout elements into two categories: The ones that are
  invisible by themselves, because they don't draw anything. Their only purpose is to manage the
  position and size of other layout elements. This category of layout elements usually use
  QCPLayout as base class. Then there is the category of layout elements which actually draw
  something. For example, QCPAxisRect, QCPLegend and QCPPlotTitle are of this category. This does
  not necessarily mean that the latter category can't have child layout elements. QCPLegend for
  instance, actually derives from QCPLayoutGrid and the individual legend items are child layout
  elements in the grid layout.
*/

/* start documentation of inline functions */

/*! \fn QCPLayout *QCPLayoutElement::layout() const
  
  Returns the parent layout of this layout element.
*/

/*! \fn QRect QCPLayoutElement::rect() const
  
  Returns the inner rect of this layout element. The inner rect is the outer rect (\ref
  setOuterRect) shrinked by the margins (\ref setMargins, \ref setAutoMargins).
  
  In some cases, the area between outer and inner rect is left blank. In other cases the margin
  area is used to display peripheral graphics while the main content is in the inner rect. This is
  where automatic margin calculation becomes interesting because it allows the layout element to
  adapt the margins to the peripheral graphics it wants to draw. For example, \ref QCPAxisRect
  draws the axis labels and tick labels in the margin area, thus needs to adjust the margins (if
  \ref setAutoMargins is enabled) according to the space required by the labels of the axes.
*/

/*! \fn virtual void QCPLayoutElement::mousePressEvent(QMouseEvent *event)
  
  This event is called, if the mouse was pressed while being inside the outer rect of this layout
  element.
*/

/*! \fn virtual void QCPLayoutElement::mouseMoveEvent(QMouseEvent *event)
  
  This event is called, if the mouse is moved inside the outer rect of this layout element.
*/

/*! \fn virtual void QCPLayoutElement::mouseReleaseEvent(QMouseEvent *event)
  
  This event is called, if the mouse was previously pressed inside the outer rect of this layout
  element and is now released.
*/

/*! \fn virtual void QCPLayoutElement::mouseDoubleClickEvent(QMouseEvent *event)
  
  This event is called, if the mouse is double-clicked inside the outer rect of this layout
  element.
*/

/*! \fn virtual void QCPLayoutElement::wheelEvent(QWheelEvent *event)
  
  This event is called, if the mouse wheel is scrolled while the cursor is inside the rect of this
  layout element.
*/

/* end documentation of inline functions */

/*!
  Creates an instance of QCPLayoutElement and sets default values. Note that since QCPLayoutElement
  is an abstract base class, it can't be instantiated directly.
*/
QCPLayoutElement::QCPLayoutElement(QCustomPlot *parentPlot) :
  QCPLayerable(parentPlot), // parenthood is changed as soon as layout element gets inserted into a layout (except for top level layout)
  mParentLayout(0),
  mMinimumSize(),
  mMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX),
  mRect(0, 0, 0, 0),
  mOuterRect(0, 0, 0, 0),
  mMargins(0, 0, 0, 0),
  mMinimumMargins(0, 0, 0, 0),
  mAutoMargins(QCP::msAll)
{
}

QCPLayoutElement::~QCPLayoutElement()
{
  setMarginGroup(QCP::msAll, 0); // unregister at margin groups, if there are any
  // unregister at layout:
  if (qobject_cast<QCPLayout*>(mParentLayout)) // the qobject_cast is just a safeguard in case the layout forgets to call clear() in its dtor and this dtor is called by QObject dtor
    mParentLayout->take(this);
}

/*!
  Sets the outer rect of this layout element. If the layout element is inside a layout, the layout
  sets the position and size of this layout element using this function.
  
  Calling this function externally has no effect, since the layout will overwrite any changes to
  the outer rect upon the next replot.
  
  The layout element will adapt its inner \ref rect by applying the margins inward to the outer rect.
  
  \see rect
*/
void QCPLayoutElement::setOuterRect(const QRect &rect)
{
  if (mOuterRect != rect)
  {
    mOuterRect = rect;
    mRect = mOuterRect.adjusted(mMargins.left(), mMargins.top(), -mMargins.right(), -mMargins.bottom());
  }
}

/*!
  Sets the margins of this layout element. If \ref setAutoMargins is disabled for some or all
  sides, this function is used to manually set the margin on those sides. Sides that are still set
  to be handled automatically are ignored and may have any value in \a margins.
  
  The margin is the distance between the outer rect (controlled by the parent layout via \ref
  setOuterRect) and the inner \ref rect (which usually contains the main content of this layout
  element).
  
  \see setAutoMargins
*/
void QCPLayoutElement::setMargins(const QMargins &margins)
{
  if (mMargins != margins)
  {
    mMargins = margins;
    mRect = mOuterRect.adjusted(mMargins.left(), mMargins.top(), -mMargins.right(), -mMargins.bottom());
  }
}

/*!
  If \ref setAutoMargins is enabled on some or all margins, this function is used to provide
  minimum values for those margins.
  
  The minimum values are not enforced on margin sides that were set to be under manual control via
  \ref setAutoMargins.
  
  \see setAutoMargins
*/
void QCPLayoutElement::setMinimumMargins(const QMargins &margins)
{
  if (mMinimumMargins != margins)
  {
    mMinimumMargins = margins;
  }
}

/*!
  Sets on which sides the margin shall be calculated automatically. If a side is calculated
  automatically, a minimum margin value may be provided with \ref setMinimumMargins. If a side is
  set to be controlled manually, the value may be specified with \ref setMargins.
  
  Margin sides that are under automatic control may participate in a \ref QCPMarginGroup (see \ref
  setMarginGroup), to synchronize (align) it with other layout elements in the plot.
  
  \see setMinimumMargins, setMargins
*/
void QCPLayoutElement::setAutoMargins(QCP::MarginSides sides)
{
  mAutoMargins = sides;
}

/*!
  Sets the minimum size for the inner \ref rect of this layout element. A parent layout tries to
  respect the \a size here by changing row/column sizes in the layout accordingly.
  
  If the parent layout size is not sufficient to satisfy all minimum size constraints of its child
  layout elements, the layout may set a size that is actually smaller than \a size. QCustomPlot
  propagates the layout's size constraints to the outside by setting its own minimum QWidget size
  accordingly, so violations of \a size should be exceptions.
*/
void QCPLayoutElement::setMinimumSize(const QSize &size)
{
  if (mMinimumSize != size)
  {
    mMinimumSize = size;
    if (mParentLayout)
      mParentLayout->sizeConstraintsChanged();
  }
}

/*! \overload
  
  Sets the minimum size for the inner \ref rect of this layout element.
*/
void QCPLayoutElement::setMinimumSize(int width, int height)
{
  setMinimumSize(QSize(width, height));
}

/*!
  Sets the maximum size for the inner \ref rect of this layout element. A parent layout tries to
  respect the \a size here by changing row/column sizes in the layout accordingly.
*/
void QCPLayoutElement::setMaximumSize(const QSize &size)
{
  if (mMaximumSize != size)
  {
    mMaximumSize = size;
    if (mParentLayout)
      mParentLayout->sizeConstraintsChanged();
  }
}

/*! \overload
  
  Sets the maximum size for the inner \ref rect of this layout element.
*/
void QCPLayoutElement::setMaximumSize(int width, int height)
{
  setMaximumSize(QSize(width, height));
}

/*!
  Sets the margin \a group of the specified margin \a sides.
  
  Margin groups allow synchronizing specified margins across layout elements, see the documentation
  of \ref QCPMarginGroup.
  
  To unset the margin group of \a sides, set \a group to 0.
  
  Note that margin groups only work for margin sides that are set to automatic (\ref
  setAutoMargins).
*/
void QCPLayoutElement::setMarginGroup(QCP::MarginSides sides, QCPMarginGroup *group)
{
  QVector<QCP::MarginSide> sideVector;
  if (sides.testFlag(QCP::msLeft)) sideVector.append(QCP::msLeft);
  if (sides.testFlag(QCP::msRight)) sideVector.append(QCP::msRight);
  if (sides.testFlag(QCP::msTop)) sideVector.append(QCP::msTop);
  if (sides.testFlag(QCP::msBottom)) sideVector.append(QCP::msBottom);
  
  for (int i=0; i<sideVector.size(); ++i)
  {
    QCP::MarginSide side = sideVector.at(i);
    if (marginGroup(side) != group)
    {
      QCPMarginGroup *oldGroup = marginGroup(side);
      if (oldGroup) // unregister at old group
        oldGroup->removeChild(side, this);
      
      if (!group) // if setting to 0, remove hash entry. Else set hash entry to new group and register there
      {
        mMarginGroups.remove(side);
      } else // setting to a new group
      {
        mMarginGroups[side] = group;
        group->addChild(side, this);
      }
    }
  }
}

/*!
  Updates the layout element and sub-elements. This function is automatically called upon replot by
  the parent layout element.
  
  Layout elements that have child elements should call the \ref update method of their child
  elements.
  
  The default implementation executes the automatic margin mechanism, so subclasses should make
  sure to call the base class implementation.
*/
void QCPLayoutElement::update()
{
  if (mAutoMargins != QCP::msNone)
  {
    // set the margins of this layout element according to automatic margin calculation, either directly or via a margin group:
    QMargins newMargins = mMargins;
    QVector<QCP::MarginSide> marginSides = QVector<QCP::MarginSide>() << QCP::msLeft << QCP::msRight << QCP::msTop << QCP::msBottom;
    for (int i=0; i<marginSides.size(); ++i)
    {
      QCP::MarginSide side = marginSides.at(i);
      if (mAutoMargins.testFlag(side)) // this side's margin shall be calculated automatically
      {
        if (mMarginGroups.contains(side)) 
          QCP::setMarginValue(newMargins, side, mMarginGroups[side]->commonMargin(side)); // this side is part of a margin group, so get the margin value from that group
        else 
          QCP::setMarginValue(newMargins, side, calculateAutoMargin(side)); // this side is not part of a group, so calculate the value directly
        // apply minimum margin restrictions:
        if (QCP::getMarginValue(newMargins, side) < QCP::getMarginValue(mMinimumMargins, side))
          QCP::setMarginValue(newMargins, side, QCP::getMarginValue(mMinimumMargins, side));
      }
    }
    setMargins(newMargins);
  }
}

/*!
  Returns the minimum size this layout element (the inner \ref rect) may be compressed to.
  
  if a minimum size (\ref setMinimumSize) was not set manually, parent layouts consult this
  function to determine the minimum allowed size of this layout element. (A manual minimum size is
  considered set if it is non-zero.)
*/
QSize QCPLayoutElement::minimumSizeHint() const
{
  return mMinimumSize;
}

/*!
  Returns the maximum size this layout element (the inner \ref rect) may be expanded to.
  
  if a maximum size (\ref setMaximumSize) was not set manually, parent layouts consult this
  function to determine the maximum allowed size of this layout element. (A manual maximum size is
  considered set if it is smaller than Qt's QWIDGETSIZE_MAX.)
*/
QSize QCPLayoutElement::maximumSizeHint() const
{
  return mMaximumSize;
}

/*!
  Returns a list of all child elements in this layout element. If \a recursive is true, all
  sub-child elements are included in the list, too.
  
  Note that there may be entries with value 0 in the returned list. (For example, QCPLayoutGrid may have
  empty cells which yield 0 at the respective index.)
*/
QList<QCPLayoutElement*> QCPLayoutElement::elements(bool recursive) const
{
  Q_UNUSED(recursive)
  return QList<QCPLayoutElement*>();
}

/*!
  Layout elements are sensitive to events inside their outer rect. If \a pos is within the outer
  rect, this method returns a value corresponding to 0.99 times the parent plot's selection
  tolerance. However, layout elements are not selectable by default. So if \ref onlySelectable is
  true, -1.0 is returned.
  
  See \ref QCPLayerable::selectTest for a general explanation of this virtual method.
  
  QCPLayoutElement subclasses may reimplement this method to provide more specific selection test
  behaviour.
*/
double QCPLayoutElement::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
  Q_UNUSED(details)
  
  if (onlySelectable)
    return -1;
  
  if (QRectF(mOuterRect).contains(pos))
  {
    if (mParentPlot)
      return mParentPlot->selectionTolerance()*0.99;
    else
    {
      qDebug() << Q_FUNC_INFO << "parent plot not defined";
      return -1;
    }
  } else
    return -1;
}

/*! \internal 
  
  propagates the parent plot initialization to all child elements, by calling \ref
  QCPLayerable::initializeParentPlot on them.
*/
void QCPLayoutElement::parentPlotInitialized(QCustomPlot *parentPlot)
{
  QList<QCPLayoutElement*> els = elements(false);
  for (int i=0; i<els.size(); ++i)
  {
    if (!els.at(i)->parentPlot())
      els.at(i)->initializeParentPlot(parentPlot);
  }
}

/*! \internal 
  
  Returns the margin size for this \a side. It is used if automatic margins is enabled for this \a
  side (see \ref setAutoMargins). If a minimum margin was set with \ref setMinimumMargins, the
  returned value will not be smaller than the specified minimum margin.
  
  The default implementation just returns the respective manual margin (\ref setMargins) or the
  minimum margin, whichever is larger.
*/
int QCPLayoutElement::calculateAutoMargin(QCP::MarginSide side)
{
  return qMax(QCP::getMarginValue(mMargins, side), QCP::getMarginValue(mMinimumMargins, side));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPLayout
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPLayout
  \brief The abstract base class for layouts
  
  This is an abstract base class for layout elements whose main purpose is to define the position
  and size of other child layout elements. In most cases, layouts don't draw anything themselves
  (but there are exceptions to this, e.g. QCPLegend).
  
  QCPLayout derives from QCPLayoutElement, and thus can itself be nested in other layouts.
  
  QCPLayout introduces a common interface for accessing and manipulating the child elements. Those
  functions are most notably \ref elementCount, \ref elementAt, \ref takeAt, \ref take, \ref
  simplify, \ref removeAt, \ref remove and \ref clear. Individual subclasses may add more functions
  to this interface which are more specialized to the form of the layout. For example, \ref
  QCPLayoutGrid adds functions that take row and column indices to access cells of the layout grid
  more conveniently.
  
  Since this is an abstract base class, you can't instantiate it directly. Rather use one of its
  subclasses like QCPLayoutGrid or QCPLayoutInset.
  
  For a general introduction to the layout system, see the dedicated documentation page \ref
  thelayoutsystem "The Layout System".
*/

/* start documentation of pure virtual functions */

/*! \fn virtual int QCPLayout::elementCount() const = 0
  
  Returns the number of elements/cells in the layout.
  
  \see elements, elementAt
*/

/*! \fn virtual QCPLayoutElement* QCPLayout::elementAt(int index) const = 0
  
  Returns the element in the cell with the given \a index. If \a index is invalid, returns 0.
  
  Note that even if \a index is valid, the respective cell may be empty in some layouts (e.g.
  QCPLayoutGrid), so this function may return 0 in those cases. You may use this function to check
  whether a cell is empty or not.
  
  \see elements, elementCount, takeAt
*/

/*! \fn virtual QCPLayoutElement* QCPLayout::takeAt(int index) = 0
  
  Removes the element with the given \a index from the layout and returns it.
  
  If the \a index is invalid or the cell with that index is empty, returns 0.
  
  Note that some layouts don't remove the respective cell right away but leave an empty cell after
  successful removal of the layout element. To collapse empty cells, use \ref simplify.
  
  \see elementAt, take
*/

/*! \fn virtual bool QCPLayout::take(QCPLayoutElement* element) = 0
  
  Removes the specified \a element from the layout and returns true on success.
  
  If the \a element isn't in this layout, returns false.
  
  Note that some layouts don't remove the respective cell right away but leave an empty cell after
  successful removal of the layout element. To collapse empty cells, use \ref simplify.
  
  \see takeAt
*/

/* end documentation of pure virtual functions */

/*!
  Creates an instance of QCPLayoutElement and sets default values. Note that since QCPLayoutElement
  is an abstract base class, it can't be instantiated directly.
*/
QCPLayout::QCPLayout()
{
}

/*!
  First calls the QCPLayoutElement::update base class implementation to update the margins on this
  layout.
  
  Then calls \ref updateLayout which subclasses reimplement to reposition and resize their cells.
  
  Finally, \ref update is called on all child elements.
*/
void QCPLayout::update()
{
  QCPLayoutElement::update(); // recalculates (auto-)margins
  
  // set child element rects according to layout:
  updateLayout();
  
  // propagate update call to child elements:
  for (int i=0; i<elementCount(); ++i)
  {
    if (QCPLayoutElement *el = elementAt(i))
      el->update();
  }
}

/* inherits documentation from base class */
QList<QCPLayoutElement*> QCPLayout::elements(bool recursive) const
{
  int c = elementCount();
  QList<QCPLayoutElement*> result;
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
  result.reserve(c);
#endif
  for (int i=0; i<c; ++i)
    result.append(elementAt(i));
  if (recursive)
  {
    for (int i=0; i<c; ++i)
    {
      if (result.at(i))
        result << result.at(i)->elements(recursive);
    }
  }
  return result;
}

/*!
  Simplifies the layout by collapsing empty cells. The exact behavior depends on subclasses, the
  default implementation does nothing.
  
  Not all layouts need simplification. For example, QCPLayoutInset doesn't use explicit
  simplification while QCPLayoutGrid does.
*/
void QCPLayout::simplify()
{
}

/*!
  Removes and deletes the element at the provided \a index. Returns true on success. If \a index is
  invalid or points to an empty cell, returns false.
  
  This function internally uses \ref takeAt to remove the element from the layout and then deletes
  the returned element.
  
  \see remove, takeAt
*/
bool QCPLayout::removeAt(int index)
{
  if (QCPLayoutElement *el = takeAt(index))
  {
    delete el;
    return true;
  } else
    return false;
}

/*!
  Removes and deletes the provided \a element. Returns true on success. If \a element is not in the
  layout, returns false.
  
  This function internally uses \ref takeAt to remove the element from the layout and then deletes
  the element.
  
  \see removeAt, take
*/
bool QCPLayout::remove(QCPLayoutElement *element)
{
  if (take(element))
  {
    delete element;
    return true;
  } else
    return false;
}

/*!
  Removes and deletes all layout elements in this layout.
  
  \see remove, removeAt
*/
void QCPLayout::clear()
{
  for (int i=elementCount()-1; i>=0; --i)
  {
    if (elementAt(i))
      removeAt(i);
  }
  simplify();
}

/*!
  Subclasses call this method to report changed (minimum/maximum) size constraints.
  
  If the parent of this layout is again a QCPLayout, forwards the call to the parent's \ref
  sizeConstraintsChanged. If the parent is a QWidget (i.e. is the \ref QCustomPlot::plotLayout of
  QCustomPlot), calls QWidget::updateGeometry, so if the QCustomPlot widget is inside a Qt QLayout,
  it may update itself and resize cells accordingly.
*/
void QCPLayout::sizeConstraintsChanged() const
{
  if (QWidget *w = qobject_cast<QWidget*>(parent()))
    w->updateGeometry();
  else if (QCPLayout *l = qobject_cast<QCPLayout*>(parent()))
    l->sizeConstraintsChanged();
}

/*! \internal
  
  Subclasses reimplement this method to update the position and sizes of the child elements/cells
  via calling their \ref QCPLayoutElement::setOuterRect. The default implementation does nothing.
  
  The geometry used as a reference is the inner \ref rect of this layout. Child elements should stay
  within that rect.
  
  \ref getSectionSizes may help with the reimplementation of this function.
  
  \see update
*/
void QCPLayout::updateLayout()
{
}


/*! \internal
  
  Associates \a el with this layout. This is done by setting the \ref QCPLayoutElement::layout, the
  \ref QCPLayerable::parentLayerable and the QObject parent to this layout.
  
  Further, if \a el didn't previously have a parent plot, calls \ref
  QCPLayerable::initializeParentPlot on \a el to set the paret plot.
  
  This method is used by subclass specific methods that add elements to the layout. Note that this
  method only changes properties in \a el. The removal from the old layout and the insertion into
  the new layout must be done additionally.
*/
void QCPLayout::adoptElement(QCPLayoutElement *el)
{
  if (el)
  {
    el->mParentLayout = this;
    el->setParentLayerable(this);
    el->setParent(this);
    if (!el->parentPlot())
      el->initializeParentPlot(mParentPlot);
  } else
    qDebug() << Q_FUNC_INFO << "Null element passed";
}

/*! \internal
  
  Disassociates \a el from this layout. This is done by setting the \ref QCPLayoutElement::layout
  and the \ref QCPLayerable::parentLayerable to zero. The QObject parent is set to the parent
  QCustomPlot.
  
  This method is used by subclass specific methods that remove elements from the layout (e.g. \ref
  take or \ref takeAt). Note that this method only changes properties in \a el. The removal from
  the old layout must be done additionally.
*/
void QCPLayout::releaseElement(QCPLayoutElement *el)
{
  if (el)
  {
    el->mParentLayout = 0;
    el->setParentLayerable(0);
    el->setParent(mParentPlot);
    // Note: Don't initializeParentPlot(0) here, because layout element will stay in same parent plot
  } else
    qDebug() << Q_FUNC_INFO << "Null element passed";
}

/*! \internal
  
  This is a helper function for the implementation of \ref updateLayout in subclasses.
  
  It calculates the sizes of one-dimensional sections with provided constraints on maximum section
  sizes, minimum section sizes, relative stretch factors and the final total size of all sections.
  
  The QVector entries refer to the sections. Thus all QVectors must have the same size.
  
  \a maxSizes gives the maximum allowed size of each section. If there shall be no maximum size
  imposed, set all vector values to Qt's QWIDGETSIZE_MAX.
  
  \a minSizes gives the minimum allowed size of each section. If there shall be no minimum size
  imposed, set all vector values to zero. If the \a minSizes entries add up to a value greater than
  \a totalSize, sections will be scaled smaller than the proposed minimum sizes. (In other words,
  not exceeding the allowed total size is taken to be more important than not going below minimum
  section sizes.)
  
  \a stretchFactors give the relative proportions of the sections to each other. If all sections
  shall be scaled equally, set all values equal. If the first section shall be double the size of
  each individual other section, set the first number of \a stretchFactors to double the value of
  the other individual values (e.g. {2, 1, 1, 1}).
  
  \a totalSize is the value that the final section sizes will add up to. Due to rounding, the
  actual sum may differ slightly. If you want the section sizes to sum up to exactly that value,
  you could distribute the remaining difference on the sections.
  
  The return value is a QVector containing the section sizes.
*/
QVector<int> QCPLayout::getSectionSizes(QVector<int> maxSizes, QVector<int> minSizes, QVector<double> stretchFactors, int totalSize) const
{
  if (maxSizes.size() != minSizes.size() || minSizes.size() != stretchFactors.size())
  {
    qDebug() << Q_FUNC_INFO << "Passed vector sizes aren't equal:" << maxSizes << minSizes << stretchFactors;
    return QVector<int>();
  }
  if (stretchFactors.isEmpty())
    return QVector<int>();
  int sectionCount = stretchFactors.size();
  QVector<double> sectionSizes(sectionCount);
  // if provided total size is forced smaller than total minimum size, ignore minimum sizes (squeeze sections):
  int minSizeSum = 0;
  for (int i=0; i<sectionCount; ++i)
    minSizeSum += minSizes.at(i);
  if (totalSize < minSizeSum)
  {
    // new stretch factors are minimum sizes and minimum sizes are set to zero:
    for (int i=0; i<sectionCount; ++i)
    {
      stretchFactors[i] = minSizes.at(i);
      minSizes[i] = 0;
    }
  }
  
  QList<int> minimumLockedSections;
  QList<int> unfinishedSections;
  for (int i=0; i<sectionCount; ++i)
    unfinishedSections.append(i);
  double freeSize = totalSize;
  
  int outerIterations = 0;
  while (!unfinishedSections.isEmpty() && outerIterations < sectionCount*2) // the iteration check ist just a failsafe in case something really strange happens
  {
    ++outerIterations;
    int innerIterations = 0;
    while (!unfinishedSections.isEmpty() && innerIterations < sectionCount*2) // the iteration check ist just a failsafe in case something really strange happens
    {
      ++innerIterations;
      // find section that hits its maximum next:
      int nextId = -1;
      double nextMax = 1e12;
      for (int i=0; i<unfinishedSections.size(); ++i)
      {
        int secId = unfinishedSections.at(i);
        double hitsMaxAt = (maxSizes.at(secId)-sectionSizes.at(secId))/stretchFactors.at(secId);
        if (hitsMaxAt < nextMax)
        {
          nextMax = hitsMaxAt;
          nextId = secId;
        }
      }
      // check if that maximum is actually within the bounds of the total size (i.e. can we stretch all remaining sections so far that the found section
      // actually hits its maximum, without exceeding the total size when we add up all sections)
      double stretchFactorSum = 0;
      for (int i=0; i<unfinishedSections.size(); ++i)
        stretchFactorSum += stretchFactors.at(unfinishedSections.at(i));
      double nextMaxLimit = freeSize/stretchFactorSum;
      if (nextMax < nextMaxLimit) // next maximum is actually hit, move forward to that point and fix the size of that section
      {
        for (int i=0; i<unfinishedSections.size(); ++i)
        {
          sectionSizes[unfinishedSections.at(i)] += nextMax*stretchFactors.at(unfinishedSections.at(i)); // increment all sections
          freeSize -= nextMax*stretchFactors.at(unfinishedSections.at(i));
        }
        unfinishedSections.removeOne(nextId); // exclude the section that is now at maximum from further changes
      } else // next maximum isn't hit, just distribute rest of free space on remaining sections
      {
        for (int i=0; i<unfinishedSections.size(); ++i)
          sectionSizes[unfinishedSections.at(i)] += nextMaxLimit*stretchFactors.at(unfinishedSections.at(i)); // increment all sections
        unfinishedSections.clear();
      }
    }
    if (innerIterations == sectionCount*2)
      qDebug() << Q_FUNC_INFO << "Exceeded maximum expected inner iteration count, layouting aborted. Input was:" << maxSizes << minSizes << stretchFactors << totalSize;
    
    // now check whether the resulting section sizes violate minimum restrictions:
    bool foundMinimumViolation = false;
    for (int i=0; i<sectionSizes.size(); ++i)
    {
      if (minimumLockedSections.contains(i))
        continue;
      if (sectionSizes.at(i) < minSizes.at(i)) // section violates minimum
      {
        sectionSizes[i] = minSizes.at(i); // set it to minimum
        foundMinimumViolation = true; // make sure we repeat the whole optimization process
        minimumLockedSections.append(i);
      }
    }
    if (foundMinimumViolation)
    {
      freeSize = totalSize;
      for (int i=0; i<sectionCount; ++i)
      {
        if (!minimumLockedSections.contains(i)) // only put sections that haven't hit their minimum back into the pool
          unfinishedSections.append(i);
        else
          freeSize -= sectionSizes.at(i); // remove size of minimum locked sections from available space in next round
      }
      // reset all section sizes to zero that are in unfinished sections (all others have been set to their minimum):
      for (int i=0; i<unfinishedSections.size(); ++i)
        sectionSizes[unfinishedSections.at(i)] = 0;
    }
  }
  if (outerIterations == sectionCount*2)
    qDebug() << Q_FUNC_INFO << "Exceeded maximum expected outer iteration count, layouting aborted. Input was:" << maxSizes << minSizes << stretchFactors << totalSize;
  
  QVector<int> result(sectionCount);
  for (int i=0; i<sectionCount; ++i)
    result[i] = qRound(sectionSizes.at(i));
  return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPLayoutGrid
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class QCPLayoutGrid
  \brief A layout that arranges child elements in a grid
  
  Elements are laid out in a grid with configurable stretch factors (\ref setColumnStretchFactor,
  \ref setRowStretchFactor) and spacing (\ref setColumnSpacing, \ref setRowSpacing).
  
  Elements can be added to cells via \ref addElement. The grid is expanded if the specified row or
  column doesn't exist yet. Whether a cell contains a valid layout element can be checked with \ref
  hasElement, that element can be retrieved with \ref element. If rows and columns that only have
  empty cells shall be removed, call \ref simplify. Removal of elements is either done by just
  adding the element to a different layout or by using the QCPLayout interface \ref take or \ref
  remove.
  
  Row and column insertion can be performed with \ref insertRow and \ref insertColumn.
*/

/*!
  Creates an instance of QCPLayoutGrid and sets default values.
*/
QCPLayoutGrid::QCPLayoutGrid() :
  mColumnSpacing(5),
  mRowSpacing(5)
{
}

QCPLayoutGrid::~QCPLayoutGrid()
{
  // clear all child layout elements. This is important because only the specific layouts know how
  // to handle removing elements (clear calls virtual removeAt method to do that).
  clear();
}

/*!
  Returns the element in the cell in \a row and \a column.
  
  Returns 0 if either the row/column is invalid or if the cell is empty. In those cases, a qDebug
  message is printed. To check whether a cell exists and isn't empty, use \ref hasElement.
  
  \see addElement, hasElement
*/
QCPLayoutElement *QCPLayoutGrid::element(int row, int column) const
{
  if (row >= 0 && row < mElements.size())
  {
    if (column >= 0 && column < mElements.first().size())
    {
      if (QCPLayoutElement *result = mElements.at(row).at(column))
        return result;
      else
        qDebug() << Q_FUNC_INFO << "Requested cell is empty. Row:" << row << "Column:" << column;
    } else
      qDebug() << Q_FUNC_INFO << "Invalid column. Row:" << row << "Column:" << column;
  } else
    qDebug() << Q_FUNC_INFO << "Invalid row. Row:" << row << "Column:" << column;
  return 0;
}

/*!
  Returns the number of rows in the layout.
  
  \see columnCount
*/
int QCPLayoutGrid::rowCount() const
{
  return mElements.size();
}

/*!
  Returns the number of columns in the layout.
  
  \see rowCount
*/
int QCPLayoutGrid::columnCount() const
{
  if (mElements.size() > 0)
    return mElements.first().size();
  else
    return 0;
}

/*!
  Adds the \a element to cell with \a row and \a column. If \a element is already in a layout, it
  is first removed from there. If \a row or \a column don't exist yet, the layout is expanded
  accordingly.
  
  Returns true if the element was added successfully, i.e. if the cell at \a row and \a column
  didn't already have an element.
  
  \see element, hasElement, take, remove
*/
bool QCPLayoutGrid::addElement(int row, int column, QCPLayoutElement *element)
{
  if (element)
  {
    if (!hasElement(row, column))
    {
      if (element->layout()) // remove from old layout first
        element->layout()->take(element);
      expandTo(row+1, column+1);
      mElements[row][column] = element;
      adoptElement(element);
      return true;
    } else
      qDebug() << Q_FUNC_INFO << "There is already an element in the specified row/column:" << row << column; 
  } else
    qDebug() << Q_FUNC_INFO << "Can't add null element to row/column:" << row << column;
  return false;
}

/*!
  Returns whether the cell at \a row and \a column exists and contains a valid element, i.e. isn't
  empty.
  
  \see element
*/
bool QCPLayoutGrid::hasElement(int row, int column)
{
  if (row >= 0 && row < rowCount() && column >= 0 && column < columnCount())
    return mElements.at(row).at(column);
  else
    return false;
}

/*!
  Sets the stretch \a factor of \a column.
  
  Stretch factors control the relative sizes of rows and columns. Cells will not be resized beyond
  their minimum and maximum widths/heights (\ref QCPLayoutElement::setMinimumSize, \ref
  QCPLayoutElement::setMaximumSize), regardless of the stretch factor.
  
  The default stretch factor of newly created rows/columns is 1.
  
  \see setColumnStretchFactors, setRowStretchFactor
*/
void QCPLayoutGrid::setColumnStretchFactor(int column, double factor)
{
  if (column >= 0 && column < columnCount())
  {
    if (factor > 0)
      mColumnStretchFactors[column] = factor;
    else
      qDebug() << Q_FUNC_INFO << "Invalid stretch factor, must be positive:" << factor;
  } else
    qDebug() << Q_FUNC_INFO << "Invalid column:" << column;
}

/*!
  Sets the stretch \a factors of all columns. \a factors must have the size \ref columnCount.
  
  Stretch factors control the relative sizes of rows and columns. Cells will not be resized beyond
  their minimum and maximum widths/heights (\ref QCPLayoutElement::setMinimumSize, \ref
  QCPLayoutElement::setMaximumSize), regardless of the stretch factor.
  
  The default stretch factor of newly created rows/columns is 1.
  
  \see setColumnStretchFactor, setRowStretchFactors
*/
void QCPLayoutGrid::setColumnStretchFactors(const QList<double> &factors)
{
  if (factors.size() == mColumnStretchFactors.size())
  {
    mColumnStretchFactors = factors;
    for (int i=0; i<mColumnStretchFactors.size(); ++i)
    {
      if (mColumnStretchFactors.at(i) <= 0)
      {
        qDebug() << Q_FUNC_INFO << "Invalid stretch factor, must be positive:" << mColumnStretchFactors.at(i);
        mColumnStretchFactors[i] = 1;
      }
    }
  } else
    qDebug() << Q_FUNC_INFO << "Column count not equal to passed stretch factor count:" << factors;
}

/*!
  Sets the stretch \a factor of \a row.
  
  Stretch factors control the relative sizes of rows and columns. Cells will not be resized beyond
  their minimum and maximum widths/heights (\ref QCPLayoutElement::setMinimumSize, \ref
  QCPLayoutElement::setMaximumSize), regardless of the stretch factor.
  
  The default stretch factor of newly created rows/columns is 1.
  
  \see setColumnStretchFactors, setRowStretchFactor
*/
void QCPLayoutGrid::setRowStretchFactor(int row, double factor)
{
  if (row >= 0 && row < rowCount())
  {
    if (factor > 0)
      mRowStretchFactors[row] = factor;
    else
      qDebug() << Q_FUNC_INFO << "Invalid stretch factor, must be positive:" << factor;
  } else
    qDebug() << Q_FUNC_INFO << "Invalid row:" << row;
}

/*!
  Sets the stretch \a factors of all rows. \a factors must have the size \ref rowCount.
  
  Stretch factors control the relative sizes of rows and columns. Cells will not be resized beyond
  their minimum and maximum widths/heights (\ref QCPLayoutElement::setMinimumSize, \ref
  QCPLayoutElement::setMaximumSize), regardless of the stretch factor.
  
  The default stretch factor of newly created rows/columns is 1.
  
  \see setRowStretchFactor, setColumnStretchFactors
*/
void QCPLayoutGrid::setRowStretchFactors(const QList<double> &factors)
{
  if (factors.size() == mRowStretchFactors.size())
  {
    mRowStretchFactors = factors;
    for (int i=0; i<mRowStretchFactors.size(); ++i)
    {
      if (mRowStretchFactors.at(i) <= 0)
      {
        qDebug() << Q_FUNC_INFO << "Invalid stretch factor, must be positive:" << mRowStretchFactors.at(i);
        mRowStretchFactors[i] = 1;
      }
    }
  } else
    qDebug() << Q_FUNC_INFO << "Row count not equal to passed stretch factor count:" << factors;
}

/*!
  Sets the gap that is left blank between columns to \a pixels.
  
  \see setRowSpacing
*/
void QCPLayoutGrid::setColumnSpacing(int pixels)
{
  mColumnSpacing = pixels;
}

/*!
  Sets the gap that is left blank between rows to \a pixels.
  
  \see setColumnSpacing
*/
void QCPLayoutGrid::setRowSpacing(int pixels)
{
  mRowSpacing = pixels;
}

/*!
  Expands the layout to have \a newRowCount rows and \a newColumnCount columns. So the last valid
  row index will be \a newRowCount-1, the last valid column index will be \a newColumnCount-1.
  
  If the current column/row count is already larger or equal to \a newColumnCount/\a newRowCount,
  this function does nothing in that dimension.
  
  Newly created cells are empty, new rows and columns have the stretch factor 1.
  
  Note that upon a call to \ref addElement, the layout is expanded automatically to contain the
  specified row and column, using this function.
  
  \see simplify
*/
void QCPLayoutGrid::expandTo(int newRowCount, int newColumnCount)
{
  // add rows as necessary:
  while (rowCount() < newRowCount)
  {
    mElements.append(QList<QCPLayoutElement*>());
    mRowStretchFactors.append(1);
  }
  // go through rows and expand columns as necessary:
  int newColCount = qMax(columnCount(), newColumnCount);
  for (int i=0; i<rowCount(); ++i)
  {
    while (mElements.at(i).size() < newColCount)
      mElements[i].append(0);
  }
  while (mColumnStretchFactors.size() < newColCount)
    mColumnStretchFactors.append(1);
}

/*!
  Inserts a new row with empty cells at the row index \a newIndex. Valid values for \a newIndex
  range from 0 (inserts a row at the top) to \a rowCount (appends a row at the bottom).
  
  \see insertColumn
*/
void QCPLayoutGrid::insertRow(int newIndex)
{
  if (mElements.isEmpty() || mElements.first().isEmpty()) // if grid is completely empty, add first cell
  {
    expandTo(1, 1);
    return;
  }
  
  if (newIndex < 0)
    newIndex = 0;
  if (newIndex > rowCount())
    newIndex = rowCount();
  
  mRowStretchFactors.insert(newIndex, 1);
  QList<QCPLayoutElement*> newRow;
  for (int col=0; col<columnCount(); ++col)
    newRow.append((QCPLayoutElement*)0);
  mElements.insert(newIndex, newRow);
}

/*!
  Inserts a new column with empty cells at the column index \a newIndex. Valid values for \a
  newIndex range from 0 (inserts a row at the left) to \a rowCount (appends a row at the right).
  
  \see insertRow
*/
void QCPLayoutGrid::insertColumn(int newIndex)
{
  if (mElements.isEmpty() || mElements.first().isEmpty()) // if grid is completely empty, add first cell
  {
    expandTo(1, 1);
    return;
  }
  
  if (newIndex < 0)
    newIndex = 0;
  if (newIndex > columnCount())
    newIndex = columnCount();
  
  mColumnStretchFactors.insert(newIndex, 1);
  for (int row=0; row<rowCount(); ++row)
    mElements[row].insert(newIndex, (QCPLayoutElement*)0);
}

/* inherits documentation from base class */
void QCPLayoutGrid::updateLayout()
{
  QVector<int> minColWidths, minRowHeights, maxColWidths, maxRowHeights;
  getMinimumRowColSizes(&minColWidths, &minRowHeights);
  getMaximumRowColSizes(&maxColWidths, &maxRowHeights);
  
  int totalRowSpacing = (rowCount()-1) * mRowSpacing;
  int totalColSpacing = (columnCount()-1) * mColumnSpacing;
  QVector<int> colWidths = getSectionSizes(maxColWidths, minColWidths, mColumnStretchFactors.toVector(), mRect.width()-totalColSpacing);
  QVector<int> rowHeights = getSectionSizes(maxRowHeights, minRowHeights, mRowStretchFactors.toVector(), mRect.height()-totalRowSpacing);
  
  // go through cells and set rects accordingly:
  int yOffset = mRect.top();
  for (int row=0; row<rowCount(); ++row)
  {
    if (row > 0)
      yOffset += rowHeights.at(row-1)+mRowSpacing;
    int xOffset = mRect.left();
    for (int col=0; col<columnCount(); ++col)
    {
      if (col > 0)
        xOffset += colWidths.at(col-1)+mColumnSpacing;
      if (mElements.at(row).at(col))
        mElements.at(row).at(col)->setOuterRect(QRect(xOffset, yOffset, colWidths.at(col), rowHeights.at(row)));
    }
  }
}

/* inherits documentation from base class */
int QCPLayoutGrid::elementCount() const
{
  return rowCount()*columnCount();
}

/* inherits documentation from base class */
QCPLayoutElement *QCPLayoutGrid::elementAt(int index) const
{
  if (index >= 0 && index < elementCount())
    return mElements.at(index / columnCount()).at(index % columnCount());
  else
    return 0;
}

/* inherits documentation from base class */
QCPLayoutElement *QCPLayoutGrid::takeAt(int index)
{
  if (QCPLayoutElement *el = elementAt(index))
  {
    releaseElement(el);
    mElements[index / columnCount()][index % columnCount()] = 0;
    return el;
  } else
  {
    qDebug() << Q_FUNC_INFO << "Attempt to take invalid index:" << index;
    return 0;
  }
}

/* inherits documentation from base class */
bool QCPLayoutGrid::take(QCPLayoutElement *element)
{
  if (element)
  {
    for (int i=0; i<elementCount(); ++i)
    {
      if (elementAt(i) == element)
      {
        takeAt(i);
        return true;
      }
    }
    qDebug() << Q_FUNC_INFO << "Element not in this layout, couldn't take";
  } else
    qDebug() << Q_FUNC_INFO << "Can't take null element";
  return false;
}

/* inherits documentation from base class */
QList<QCPLayoutElement*> QCPLayoutGrid::elements(bool recursive) const
{
  QList<QCPLayoutElement*> result;
  int colC = columnCount();
  int rowC = rowCount();
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
  result.reserve(colC*rowC);
#endif
  for (int row=0; row<rowC; ++row)
  {
    for (int col=0; col<colC; ++col)
    {
      result.append(mElements.at(row).at(col));
    }
  }
  if (recursive)
  {
    int c = result.size();
    for (int i=0; i<c; ++i)
    {
      if (result.at(i))
        result << result.at(i)->elements(recursive);
    }
  }
  return result;
}

/*!
  Simplifies the layout by collapsing rows and columns which only contain empty cells.
*/
void QCPLayoutGrid::simplify()
{
  // remove rows with only empty cells:
  for (int row=rowCount()-1; row>=0; --row)
  {
    bool hasElements = false;
    for (int col=0; col<columnCount(); ++col)
    {
      if (mElements.at(row).at(col))
      {
        hasElements = true;
        break;
      }
    }
    if (!hasElements)
    {
      mRowStretchFactors.removeAt(row);
      mElements.removeAt(row);
      if (mElements.isEmpty()) // removed last element, also remove stretch factor (wouldn't happen below because also columnCount changed to 0 now)
        mColumnStretchFactors.clear();
    }
  }
  
  // remove columns with only empty cells:
  for (int col=columnCount()-1; col>=0; --col)
  {
    bool hasElements = false;
    for (int row=0; row<rowCount(); ++row)
    {
      if (mElements.at(row).at(col))
      {
        hasElements = true;
        break;
      }
    }
    if (!hasElements)
    {
      mColumnStretchFactors.removeAt(col);
      for (int row=0; row<rowCount(); ++row)
        mElements[row].removeAt(col);
    }
  }
}

/* inherits documentation from base class */
QSize QCPLayoutGrid::minimumSizeHint() const
{
  QVector<int> minColWidths, minRowHeights;
  getMinimumRowColSizes(&minColWidths, &minRowHeights);
  QSize result(0, 0);
  for (int i=0; i<minColWidths.size(); ++i)
    result.rwidth() += minColWidths.at(i);
  for (int i=0; i<minRowHeights.size(); ++i)
    result.rheight() += minRowHeights.at(i);
  result.rwidth() += qMax(0, columnCount()-1) * mColumnSpacing + mMargins.left() + mMargins.right();
  result.rheight() += qMax(0, rowCount()-1) * mRowSpacing + mMargins.top() + mMargins.bottom();
  return result;
}

/* inherits documentation from base class */
QSize QCPLayoutGrid::maximumSizeHint() const
{
  QVector<int> maxColWidths, maxRowHeights;
  getMaximumRowColSizes(&maxColWidths, &maxRowHeights);
  
  QSize result(0, 0);
  for (int i=0; i<maxColWidths.size(); ++i)
    result.setWidth(qMin(result.width()+maxColWidths.at(i), QWIDGETSIZE_MAX));
  for (int i=0; i<maxRowHeights.size(); ++i)
    result.setHeight(qMin(result.height()+maxRowHeights.at(i), QWIDGETSIZE_MAX));
  result.rwidth() += qMax(0, columnCount()-1) * mColumnSpacing + mMargins.left() + mMargins.right();
  result.rheight() += qMax(0, rowCount()-1) * mRowSpacing + mMargins.top() + mMargins.bottom();
  return result;
}

/*! \internal
  
  Places the minimum column widths and row heights into \a minColWidths and \a minRowHeights
  respectively.
  
  The minimum height of a row is the largest minimum height of any element in that row. The minimum
  width of a column is the largest minimum width of any element in that column.
  
  This is a helper function for \ref updateLayout.
  
  \see getMaximumRowColSizes
*/
void QCPLayoutGrid::getMinimumRowColSizes(QVector<int> *minColWidths, QVector<int> *minRowHeights) const
{
  *minColWidths = QVector<int>(columnCount(), 0);
  *minRowHeights = QVector<int>(rowCount(), 0);
  for (int row=0; row<rowCount(); ++row)
  {
    for (int col=0; col<columnCount(); ++col)
    {
      if (mElements.at(row).at(col))
      {
        QSize minHint = mElements.at(row).at(col)->minimumSizeHint();
        QSize min = mElements.at(row).at(col)->minimumSize();
        QSize final(min.width() > 0 ? min.width() : minHint.width(), min.height() > 0 ? min.height() : minHint.height());
        if (minColWidths->at(col) < final.width())
          (*minColWidths)[col] = final.width();
        if (minRowHeights->at(row) < final.height())
          (*minRowHeights)[row] = final.height();
      }
    }
  }
}

/*! \internal
  
  Places the maximum column widths and row heights into \a maxColWidths and \a maxRowHeights
  respectively.
  
  The maximum height of a row is the smallest maximum height of any element in that row. The
  maximum width of a column is the smallest maximum width of any element in that column.
  
  This is a helper function for \ref updateLayout.
  
  \see getMinimumRowColSizes
*/
void QCPLayoutGrid::getMaximumRowColSizes(QVector<int> *maxColWidths, QVector<int> *maxRowHeights) const
{
  *maxColWidths = QVector<int>(columnCount(), QWIDGETSIZE_MAX);
  *maxRowHeights = QVector<int>(rowCount(), QWIDGETSIZE_MAX);
  for (int row=0; row<rowCount(); ++row)
  {
    for (int col=0; col<columnCount(); ++col)
    {
      if (mElements.at(row).at(col))
      {
        QSize maxHint = mElements.at(row).at(col)->maximumSizeHint();
        QSize max = mElements.at(row).at(col)->maximumSize();
        QSize final(max.width() < QWIDGETSIZE_MAX ? max.width() : maxHint.width(), max.height() < QWIDGETSIZE_MAX ? max.height() : maxHint.height());
        if (maxColWidths->at(col) > final.width())
          (*maxColWidths)[col] = final.width();
        if (maxRowHeights->at(row) > final.height())
          (*maxRowHeights)[row] = final.height();
      }
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// QCPLayoutInset
////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \class QCPLayoutInset
  \brief A layout that places child elements aligned to the border or arbitrarily positioned
  
  Elements are placed either aligned to the border or at arbitrary position in the area of the
  layout. Which placement applies is controlled with the \ref InsetPlacement (\ref
  setInsetPlacement).

  Elements are added via \ref addElement(QCPLayoutElement *element, Qt::Alignment alignment) or
  addElement(QCPLayoutElement *element, const QRectF &rect). If the first method is used, the inset
  placement will default to \ref ipBorderAligned and the element will be aligned according to the
  \a alignment parameter. The second method defaults to \ref ipFree and allows placing elements at
  arbitrary position and size, defined by \a rect.
  
  The alignment or rect can be set via \ref setInsetAlignment or \ref setInsetRect, respectively.
  
  This is the layout that every QCPAxisRect has as \ref QCPAxisRect::insetLayout.
*/

/* start documentation of inline functions */

/*! \fn virtual void QCPLayoutInset::simplify()
  
  The QCPInsetLayout does not need simplification since it can never have empty cells due to its
  linear index structure. This method does nothing.
*/

/* end documentation of inline functions */

/*!
  Creates an instance of QCPLayoutInset and sets default values.
*/
QCPLayoutInset::QCPLayoutInset()
{
}

QCPLayoutInset::~QCPLayoutInset()
{
  // clear all child layout elements. This is important because only the specific layouts know how
  // to handle removing elements (clear calls virtual removeAt method to do that).
  clear();
}

/*!
  Returns the placement type of the element with the specified \a index.
*/
QCPLayoutInset::InsetPlacement QCPLayoutInset::insetPlacement(int index) const
{
  if (elementAt(index))
    return mInsetPlacement.at(index);
  else
  {
    qDebug() << Q_FUNC_INFO << "Invalid element index:" << index;
    return ipFree;
  }
}

/*!
  Returns the alignment of the element with the specified \a index. The alignment only has a
  meaning, if the inset placement (\ref setInsetPlacement) is \ref ipBorderAligned.
*/
Qt::Alignment QCPLayoutInset::insetAlignment(int index) const
{
  if (elementAt(index))
    return mInsetAlignment.at(index);
  else
  {
    qDebug() << Q_FUNC_INFO << "Invalid element index:" << index;
    return 0;
  }
}

/*!
  Returns the rect of the element with the specified \a index. The rect only has a
  meaning, if the inset placement (\ref setInsetPlacement) is \ref ipFree.
*/
QRectF QCPLayoutInset::insetRect(int index) const
{
  if (elementAt(index))
    return mInsetRect.at(index);
  else
  {
    qDebug() << Q_FUNC_INFO << "Invalid element index:" << index;
    return QRectF();
  }
}

/*!
  Sets the inset placement type of the element with the specified \a index to \a placement.
  
  \see InsetPlacement
*/
void QCPLayoutInset::setInsetPlacement(int index, QCPLayoutInset::InsetPlacement placement)
{
  if (elementAt(index))
    mInsetPlacement[index] = placement;
  else
    qDebug() << Q_FUNC_INFO << "Invalid element index:" << index;
}

/*!
  If the inset placement (\ref setInsetPlacement) is \ref ipBorderAligned, this function
  is used to set the alignment of the element with the specified \a index to \a alignment.
  
  \a alignment is an or combination of the following alignment flags: Qt::AlignLeft,
  Qt::AlignHCenter, Qt::AlighRight, Qt::AlignTop, Qt::AlignVCenter, Qt::AlignBottom. Any other
  alignment flags will be ignored.
*/
void QCPLayoutInset::setInsetAlignment(int index, Qt::Alignment alignment)
{
  if (elementAt(index))
    mInsetAlignment[index] = alignment;
  else
    qDebug() << Q_FUNC_INFO << "Invalid element index:" << index;
}

/*!
  If the inset placement (\ref setInsetPlacement) is \ref ipFree, this function is used to set the
  position and size of the element with the specified \a index to \a rect.
  
  \a rect is given in fractions of the whole inset layout rect. So an inset with rect (0, 0, 1, 1)
  will span the entire layout. An inset with rect (0.6, 0.1, 0.35, 0.35) will be in the top right
  corner of the layout, with 35% width and height of the parent layout.
  
  Note that the minimum and maximum sizes of the embedded element (\ref
  QCPLayoutElement::setMinimumSize, \ref QCPLayoutElement::setMaximumSize) are enforced.
*/
void QCPLayoutInset::setInsetRect(int index, const QRectF &rect)
{
  if (elementAt(index))
    mInsetRect[index] = rect;
  else
    qDebug() << Q_FUNC_INFO << "Invalid element index:" << index;
}

/* inherits documentation from base class */
void QCPLayoutInset::updateLayout()
{
  for (int i=0; i<mElements.size(); ++i)
  {
    QRect insetRect;
    QSize finalMinSize, finalMaxSize;
    QSize minSizeHint = mElements.at(i)->minimumSizeHint();
    QSize maxSizeHint = mElements.at(i)->maximumSizeHint();
    finalMinSize.setWidth(mElements.at(i)->minimumSize().width() > 0 ? mElements.at(i)->minimumSize().width() : minSizeHint.width());
    finalMinSize.setHeight(mElements.at(i)->minimumSize().height() > 0 ? mElements.at(i)->minimumSize().height() : minSizeHint.height());
    finalMaxSize.setWidth(mElements.at(i)->maximumSize().width() < QWIDGETSIZE_MAX ? mElements.at(i)->maximumSize().width() : maxSizeHint.width());
    finalMaxSize.setHeight(mElements.at(i)->maximumSize().height() < QWIDGETSIZE_MAX ? mElements.at(i)->maximumSize().height() : maxSizeHint.height());
    if (mInsetPlacement.at(i) == ipFree)
    {
      insetRect = QRect(rect().x()+rect().width()*mInsetRect.at(i).x(),
                        rect().y()+rect().height()*mInsetRect.at(i).y(),
                        rect().width()*mInsetRect.at(i).width(),
                        rect().height()*mInsetRect.at(i).height());
      if (insetRect.size().width() < finalMinSize.width())
        insetRect.setWidth(finalMinSize.width());
      if (insetRect.size().height() < finalMinSize.height())
        insetRect.setHeight(finalMinSize.height());
      if (insetRect.size().width() > finalMaxSize.width())
        insetRect.setWidth(finalMaxSize.width());
      if (insetRect.size().height() > finalMaxSize.height())
        insetRect.setHeight(finalMaxSize.height());
    } else if (mInsetPlacement.at(i) == ipBorderAligned)
    {
      insetRect.setSize(finalMinSize);
      Qt::Alignment al = mInsetAlignment.at(i);
      if (al.testFlag(Qt::AlignLeft)) insetRect.moveLeft(rect().x());
      else if (al.testFlag(Qt::AlignRight)) insetRect.moveRight(rect().x()+rect().width());
      else insetRect.moveLeft(rect().x()+rect().width()*0.5-finalMinSize.width()*0.5); // default to Qt::AlignHCenter
      if (al.testFlag(Qt::AlignTop)) insetRect.moveTop(rect().y());
      else if (al.testFlag(Qt::AlignBottom)) insetRect.moveBottom(rect().y()+rect().height());
      else insetRect.moveTop(rect().y()+rect().height()*0.5-finalMinSize.height()*0.5); // default to Qt::AlignVCenter
    }
    mElements.at(i)->setOuterRect(insetRect);
  }
}

/* inherits documentation from base class */
int QCPLayoutInset::elementCount() const
{
  return mElements.size();
}

/* inherits documentation from base class */
QCPLayoutElement *QCPLayoutInset::elementAt(int index) const
{
  if (index >= 0 && index < mElements.size())
    return mElements.at(index);
  else
    return 0;
}

/* inherits documentation from base class */
QCPLayoutElement *QCPLayoutInset::takeAt(int index)
{
  if (QCPLayoutElement *el = elementAt(index))
  {
    releaseElement(el);
    mElements.removeAt(index);
    mInsetPlacement.removeAt(index);
    mInsetAlignment.removeAt(index);
    mInsetRect.removeAt(index);
    return el;
  } else
  {
    qDebug() << Q_FUNC_INFO << "Attempt to take invalid index:" << index;
    return 0;
  }
}

/* inherits documentation from base class */
bool QCPLayoutInset::take(QCPLayoutElement *element)
{
  if (element)
  {
    for (int i=0; i<elementCount(); ++i)
    {
      if (elementAt(i) == element)
      {
        takeAt(i);
        return true;
      }
    }
    qDebug() << Q_FUNC_INFO << "Element not in this layout, couldn't take";
  } else
    qDebug() << Q_FUNC_INFO << "Can't take null element";
  return false;
}

/*!
  The inset layout is sensitive to events only at areas where its child elements are sensitive. If
  the selectTest method of any of the child elements returns a positive number for \a pos, this
  method returns a value corresponding to 0.99 times the parent plot's selection tolerance. The
  inset layout is not selectable itself by default. So if \ref onlySelectable is true, -1.0 is
  returned.
  
  See \ref QCPLayerable::selectTest for a general explanation of this virtual method.
*/
double QCPLayoutInset::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
  Q_UNUSED(details)
  if (onlySelectable)
    return -1;
  
  for (int i=0; i<mElements.size(); ++i)
  {
    // inset layout shall only return positive selectTest, if actually an inset object is at pos
    // else it would block the entire underlying QCPAxisRect with its surface.
    if (mElements.at(i)->selectTest(pos, onlySelectable) >= 0)
      return mParentPlot->selectionTolerance()*0.99;
  }
  return -1;
}

/*!
  Adds the specified \a element to the layout as an inset aligned at the border (\ref
  setInsetAlignment is initialized with \ref ipBorderAligned). The alignment is set to \a
  alignment.
  
  \a alignment is an or combination of the following alignment flags: Qt::AlignLeft,
  Qt::AlignHCenter, Qt::AlighRight, Qt::AlignTop, Qt::AlignVCenter, Qt::AlignBottom. Any other
  alignment flags will be ignored.
  
  \see addElement(QCPLayoutElement *element, const QRectF &rect)
*/
void QCPLayoutInset::addElement(QCPLayoutElement *element, Qt::Alignment alignment)
{
  if (element)
  {
    if (element->layout()) // remove from old layout first
      element->layout()->take(element);
    mElements.append(element);
    mInsetPlacement.append(ipBorderAligned);
    mInsetAlignment.append(alignment);
    mInsetRect.append(QRectF(0.6, 0.6, 0.4, 0.4));
    adoptElement(element);
  } else
    qDebug() << Q_FUNC_INFO << "Can't add null element";
}

/*!
  Adds the specified \a element to the layout as an inset with free positioning/sizing (\ref
  setInsetAlignment is initialized with \ref ipFree). The position and size is set to \a
  rect.
  
  \a rect is given in fractions of the whole inset layout rect. So an inset with rect (0, 0, 1, 1)
  will span the entire layout. An inset with rect (0.6, 0.1, 0.35, 0.35) will be in the top right
  corner of the layout, with 35% width and height of the parent layout.
  
  \see addElement(QCPLayoutElement *element, Qt::Alignment alignment)
*/
void QCPLayoutInset::addElement(QCPLayoutElement *element, const QRectF &rect)
{
  if (element)
  {
    if (element->layout()) // remove from old layout first
      element->layout()->take(element);
    mElements.append(element);
    mInsetPlacement.append(ipFree);
    mInsetAlignment.append(Qt::AlignRight|Qt::AlignTop);
    mInsetRect.append(rect);
    adoptElement(element);
  } else
    qDebug() << Q_FUNC_INFO << "Can't add null element";
}