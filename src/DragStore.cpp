//---------------------------------------------------------------------
// File and Version Information:
//   $Id$
//
// Author: Mikhail S. Dubrovin
//---------------------------------------------------------------------

//--------------------------

#include "PSQt/DragStore.h"

#include "PSQt/DragCircle.h"
#include "PSQt/DragCenter.h"
//#include "PSQt/DragLine.h"

using namespace std;   // cout without std::

//--------------------------

namespace PSQt {

//--------------------------

DragStore::DragStore(WdgImage* wimg)
  : m_wimg(wimg)
  , p_dragfig_sel(0)
{
  //std::cout  << strTimeStamp() << " c-tor DragStore()\n";

  QPointF center_raw(2000,2000);
  v_dragfigs.clear();
  v_dragfigs.push_back((DragFig){DragStore::Create(m_wimg, DRAGCENTER, &center_raw), DRAGCENTER, 0});

  QPen pen_boarder(Qt::white, 1, Qt::DashLine); // Qt::SolidLine
  this -> addCircle(100, &pen_boarder);
  this -> addCircle(200, &pen_boarder);
  this -> addCircle(300, &pen_boarder);
  this -> addCircle(400, &pen_boarder);
  this -> addCircle(500, &pen_boarder);
}

//--------------------------

void
DragStore::addCircle(const float& rad_raw, const QPen* pen)
{
  QPointF pc = this -> getCenter();
  QPointF points_raw[2] = { pc,
                            pc + QPointF(rad_raw,rad_raw) };
  v_dragfigs.push_back((DragFig){DragStore::Create(m_wimg, DRAGCIRCLE, &points_raw[0]), DRAGCIRCLE, 0});

  if(pen) v_dragfigs.back().ptr_obj->setPenDraw(*pen);
  //cout << "DragStore::addCircle center x:" << pc.x() << " x:" << pc.y() << '\n';  
}

//--------------------------

void
DragStore::drawFigs(const DRAGMODE& mode)
{
  for(std::vector<DragFig>::iterator it=v_dragfigs.begin(); it!= v_dragfigs.end(); ++ it) {
    
    if ((p_dragfig_sel && p_dragfig_sel->type == DRAGCENTER)
    || &(*it) == p_dragfig_sel) it -> ptr_obj -> draw(mode);
    else                        it -> ptr_obj -> draw();
  }
}

//--------------------------

bool
DragStore::containFigs(const QPointF& p)
{
  for(std::vector<DragFig>::iterator it=v_dragfigs.begin(); it!= v_dragfigs.end(); ++ it) 
    if(it->ptr_obj -> contains(p)) {
      p_dragfig_sel = &(*it); 
      return true;
    }
  return false;
}

//--------------------------

void
DragStore::moveFigs(const QPointF& p)
{
  //std::cout << "DragStore::moveFigs(p)  x:" << p.x() << "  y:" << p.y() << '\n';
  p_dragfig_sel -> ptr_obj -> move(p);
}

//--------------------------

void
DragStore::moveFigsIsCompleted(const QPointF& p)
{
  //std::cout << "DragStore::moveFigsIsCompleted(p)  x:" << p.x() << "  y:" << p.y() << '\n';
  p_dragfig_sel -> ptr_obj -> moveIsCompleted(p);
}

//--------------------------

void
DragStore::deleteFig()
{
  //std::cout << "DragStore::deleteFig()\n";
  if(v_dragfigs.size()<4) { MsgInLog(_name_(), WARNING, "Last two circles can't be removed" ); return; }

  if(p_dragfig_sel->type == DRAGCENTER) { MsgInLog(_name_(), WARNING, "Center can't be removed" ); return; }

  for(std::vector<DragFig>::iterator it=v_dragfigs.begin(); it!= v_dragfigs.end(); ++ it) 

    if( &(*it) == p_dragfig_sel) {
        v_dragfigs.erase(it); // delete element from vector 
	p_dragfig_sel = 0;
        return;
    }
}

//--------------------------

void
DragStore::print()
{
  stringstream ss; ss << strTimeStamp() << " DragStore::print():";
  std::cout << ss.str() << '\n';
}

//--------------------------
//--------------------------
//-----static methods-------
//--------------------------
//--------------------------

const char* 
DragStore::cstrDragType(const DRAGTYPE& dfigtype)
{
  if (dfigtype == DRAGCIRCLE) { return "CIRCLE"; }
  if (dfigtype == DRAGCENTER) { return "CENTER"; }
  if (dfigtype == DRAGLINE  ) { return "LINE";   }
  return "N/A TYPE"; 
}

//--------------------------

PSQt::DragBase*
DragStore::Create(WdgImage* wimg, const DRAGTYPE& dfigtype, const QPointF* points, const int& npoints)
{
  stringstream ss; ss << "Create " << std::string(cstrDragType(dfigtype)) 
                      << " with pars: npoints=" << npoints;
  for(int i=0; i<npoints; ++i) ss << ", (" << points[i].x() << ", " << points[i].y() << ")";
  MsgInLog(_name_(), INFO, ss.str());

  if (dfigtype == DRAGCIRCLE) { return new PSQt::DragCircle(wimg, points); }
  if (dfigtype == DRAGCENTER) { return new PSQt::DragCenter(wimg, points); }
  //if (dfigtype == DRAGLINE) { return new PSQt::DragLine(wimg); }

  stringstream ss2; ss2 << "Unknown draggable figure name " << std::string(cstrDragType(dfigtype)) << " - return 0-pointer...";
  MsgInLog(_name_(), WARNING, ss2.str() );  
  //abort();
  return 0; // NULL;
}

//--------------------------

} // namespace PSQt

//--------------------------


