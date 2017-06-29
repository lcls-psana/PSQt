#ifndef WDGGEOTREE_H
#define WDGGEOTREE_H

//#include "PSQt/WdgImage.h"
//#include "PSQt/WdgFile.h"

#include "PSQt/GeoTree.h"

#include "PSQt/Frame.h"
#include "PSCalib/GeometryAccess.h"
#include "PSCalib/GeometryObject.h"
#include <boost/shared_ptr.hpp>

#include <Qt>
#include <QtWidgets>
#include <QtCore>

namespace PSQt {

typedef boost::shared_ptr<PSCalib::GeometryObject> shpGO;

class GeoTree;

/**
 *  @ingroup PSQt
 *
 *  @brief Widget to display and edit geometry tree.
 * 
 *  WdgGeoTree is a sub-class of Frame (QWidget)
 *  GeoTree is a sub-class of QTreeView
 *
 *  Geometry tree for input file is displayed in the window.
 *  GeoTree emits signal "selectedGO(shpGO&)" when the geometry object is selected.
 *
 *
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @see GUIMain
 *
 *  @version $Id$
 *
 *  @author Mikhail Dubrovin
 */

class WdgGeoTree : public Frame
{
 Q_OBJECT // macro is needed for connection of signals and slots

 public:

  WdgGeoTree( QWidget *parent = 0
            , const std::string& gfname = "/reg/g/psdm/detector/alignment/cspad/geo-cspad-test-2-end.data"
            , const unsigned& pbits=255);

    void resizeEvent     (QResizeEvent *event = 0) ;
    void moveEvent       (QMoveEvent   *event = 0) ;
    void mousePressEvent (QMouseEvent  *event = 0) ;
    void closeEvent      (QCloseEvent  *event = 0) ;

    QTreeView* get_view() { return m_view; };
    GeoTree* get_geotree() { return m_geotree; };
    PSCalib::GeometryAccess* geoacc();
    void showTips() ;

 private:
    unsigned    m_pbits;
    GeoTree*    m_geotree;
    QTreeView*  m_view;
    inline const char* _name_(){return "WdgGeoTree";}
};

} // namespace PSQt

#endif // WDGGEOTREE_H
