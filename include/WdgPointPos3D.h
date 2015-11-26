#ifndef WDGPOINTPOSITION3D_H
#define WDGPOINTPOSITION3D_H

#include <Qt>
#include <QtGui>
#include <QtCore>

#include "PSQt/Frame.h"

namespace PSQt {

/**
 *  @ingroup PSQt
 * 
 *  @brief Widget to display 3 values for 3-d point (x,y,z/intensity). 
 * 
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @see GUIImageViewer
 *
 *  @version $Id$
 *
 *  @author Mikhail Dubrovin
 */

class WdgPointPos3D : public Frame // QWidget
{
 Q_OBJECT // macro is needed for connection of signals and slots

public:
    WdgPointPos3D( QWidget *parent = 0
	    , const std::string& label1=std::string("X:")
	    , const std::string& label2=std::string("Y:") 
	    , const std::string& label3=std::string("Z:") 
	    , const float& val1=0
	    , const float& val2=0
	    , const float& val3=0
	    , const bool& show_boarder=true
	    , const unsigned& fld_width=80
	    , const unsigned& prec1=0
	    , const unsigned& prec2=0
	    , const unsigned& prec3=0);

    ~WdgPointPos3D(){}

    void resizeEvent(QResizeEvent *event = 0);
    void closeEvent (QCloseEvent  *event = 0);

 signals :
    void posIsChanged(const float&, const float&, const float&);

 public slots:
    void onEdi() ;
    void testPosIsChanged(const float& val1, const float& val2, const float& val3);
    void setPointPos(const float& val1, const float& val2, const float& val3);

 private :
    bool         m_show_boarder;
    unsigned     m_fld_width;
    unsigned     m_prec1;
    unsigned     m_prec2;
    unsigned     m_prec3;

    QLabel*      m_lab1;
    QLabel*      m_lab2;
    QLabel*      m_lab3;
    QLineEdit*   m_edi1;
    QLineEdit*   m_edi2;
    QLineEdit*   m_edi3;

    inline const char* _name_(){return "WdgPointPos3D";}
    void showTips() ;
};

} // namespace PSQt

#endif // WDGPOINTPOSITION3D_H
