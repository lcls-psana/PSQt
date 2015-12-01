//---------------------------------------------------------------------
// File and Version Information:
//   $Id$
//
// Author: Mikhail S. Dubrovin
//---------------------------------------------------------------------

//--------------------------

#include "PSQt/WdgGeoTree.h"
#include "PSQt/QGUtils.h"
#include "PSQt/Logger.h"

//#include <string>
//#include <fstream>   // ofstream
//#include <iomanip>   // for setw, setfill
//#include <math.h>
//#include <stdio.h>

#include <sstream>   // for stringstream
#include <iostream>    // cout
#include <fstream>    // ifstream(fname), ofstream

//#include <dirent.h> // for DIR, dirent

//using namespace std; // for cout without std::

namespace PSQt {

//--------------------------

WdgGeoTree::WdgGeoTree(QWidget *parent, const std::string& gfname, const unsigned& pbits) //, const std::string& gfname, const unsigned& pbits)
    : Frame(parent)
    , m_pbits(pbits)
{
  m_geotree = new GeoTree(parent,gfname,m_pbits); 
  m_view = (QTreeView*) m_geotree;

  QVBoxLayout *vbox = new QVBoxLayout();
  vbox -> addWidget(m_view);
  setLayout(vbox);
  //move(100,50);  
  this -> setContentsMargins(-9,-9,-9,-9);
  showTips();
}
//--------------------------

PSCalib::GeometryAccess* 
WdgGeoTree::geoacc()
{
  return m_geotree->geoacc();
}

//--------------------------

void
WdgGeoTree::showTips() 
{
  //m_file_geo  -> setToolTip("Select \"geometry\" file");
  //m_file_geo  -> setToolTip("Select ndarray with image file");
  //m_but_exit  -> setToolTip("Exit application");
}

//--------------------------

void 
WdgGeoTree::resizeEvent(QResizeEvent *event)
{
  stringstream ss; ss << "w:" << event->size().width() << " h:" <<  event->size().height();
  setWindowTitle(ss.str().c_str());
}

//--------------------------
void
WdgGeoTree::moveEvent(QMoveEvent *event)
{
  stringstream ss; ss << "x:" << event->pos().x() << " y:" << event->pos().y();
  setWindowTitle(ss.str().c_str());
}

//--------------------------

void 
WdgGeoTree::closeEvent(QCloseEvent *event)
{
  QWidget::closeEvent(event);
  std::stringstream ss; ss << "closeEvent(...): type = " << event -> type();
  MsgInLog(_name_(), INFO, ss.str());
}

//--------------------------

void 
WdgGeoTree::mousePressEvent(QMouseEvent *event)
{
  //int x = event->pos().x();
  //int y = event->pos().y();
  //QString text = "mousePressEvent: " + QString::number(x) + "," + QString::number(y);
  //std::cout << text.toStdString()  << std::endl;
}

//--------------------------
//--------------------------
//--------------------------
//--------------------------
//--------------------------

} // namespace PSQt

//--------------------------
