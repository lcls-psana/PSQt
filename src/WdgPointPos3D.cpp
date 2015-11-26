//---------------------------------------------------------------------
// File and Version Information:
//   $Id$
//
// Author: Mikhail S. Dubrovin
//---------------------------------------------------------------------

//--------------------------

#include "PSQt/WdgPointPos3D.h"
#include "PSQt/Logger.h"

#include <iostream>    // for std::cout
#include <sstream>   // std::stringstream
//using namespace std; // for cout without std::

namespace PSQt {

//--------------------------

WdgPointPos3D::WdgPointPos3D( QWidget *parent
			, const std::string& label1
			, const std::string& label2 
			, const std::string& label3 
	                , const float& val1
	                , const float& val2
	                , const float& val3
			, const bool& show_boarder
			, const unsigned& fld_width
			, const unsigned& prec1
			, const unsigned& prec2
			, const unsigned& prec3
			  )
//    : QWidget(parent)
  : Frame(parent)
  , m_show_boarder(show_boarder)
  , m_fld_width(fld_width)
  , m_prec1(prec1)
  , m_prec2(prec2)
  , m_prec3(prec3)
{
  Frame::setBoarderVisible(show_boarder);

  m_lab1    = new QLabel(label1.c_str());
  m_lab2    = new QLabel(label2.c_str());
  m_lab3    = new QLabel(label3.c_str());

  m_edi1 = new QLineEdit("");
  m_edi2 = new QLineEdit("");
  m_edi3 = new QLineEdit("");
  m_edi1 -> setFixedWidth(m_fld_width);
  m_edi2 -> setFixedWidth(m_fld_width);
  m_edi3 -> setFixedWidth(m_fld_width);
  m_edi1 -> setValidator(new QDoubleValidator(-100000, 100000, m_prec1, this));
  m_edi2 -> setValidator(new QDoubleValidator(-100000, 100000, m_prec2, this));
  m_edi3 -> setValidator(new QDoubleValidator(-100000, 100000, m_prec3, this));
  //m_edi1 -> setCursor(Qt::PointingHandCursor); 
  //m_edi2 -> setCursor(Qt::PointingHandCursor); 

  //setPointPos(QPointF(val1,val2));
  setPointPos(val1,val2,val3);

  connect(m_edi1, SIGNAL(editingFinished()), this, SLOT(onEdi())); 
  connect(m_edi2, SIGNAL(editingFinished()), this, SLOT(onEdi())); 
  connect(this,   SIGNAL(posIsChanged(const float&, const float&, const float&)), 
          this, SLOT(testPosIsChanged(const float&, const float&, const float&)) ); 
 
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox -> addWidget(m_lab1);
  hbox -> addWidget(m_edi1);
  hbox -> addWidget(m_lab2);
  hbox -> addWidget(m_edi2);
  hbox -> addWidget(m_lab3);
  hbox -> addWidget(m_edi3);
  this -> setLayout(hbox);

  this -> setWindowTitle(tr("WdgPointPos3D"));
  this -> setFixedHeight( (m_show_boarder)? 50 : 36);
  if (! m_show_boarder) this -> setContentsMargins(-9,-9,-9,-9);
  //if (! m_show_boarder) this -> setContentsMargins(-5,-5,-5,-5);
  //this -> setMinimumWidth(200);

  this -> showTips();
}

//--------------------------

void
WdgPointPos3D::showTips() 
{
  m_edi1 -> setToolTip("Type value to change");
  m_edi2 -> setToolTip("Type value to change");
  m_edi3 -> setToolTip("Type value to change");
}

//--------------------------

void 
WdgPointPos3D::resizeEvent(QResizeEvent *event)
{
  //setWindowTitle("Window is resized");
}

//--------------------------

void 
WdgPointPos3D::closeEvent(QCloseEvent *event)
{
  QWidget::closeEvent(event);
  stringstream ss; ss << "closeEvent(...): type = " << event -> type();
  MsgInLog(_name_(), INFO, ss.str());
}

//--------------------------
//--------------------------
//--------------------------
//--------------------------

void 
WdgPointPos3D::setPointPos(const float& x, const float& y, const float& z)
{
  //m_but_file -> setFixedWidth(but_width);
  stringstream ss1, ss2, ss3; 
  ss1 << fixed << std::setprecision(m_prec1) << x;
  ss2 << fixed << std::setprecision(m_prec2) << y;
  ss3 << fixed << std::setprecision(m_prec3) << z;

  m_edi1->setText(ss1.str().c_str());
  m_edi2->setText(ss2.str().c_str());
  m_edi3->setText(ss3.str().c_str());

  //stringstream ss; ss << "Center position is set to x: " << pos.x() << "  y: " << pos.y(); 
  //MsgInLog(_name_(), DEBUG, ss.str());
  //std::cout << ss.str() << '\n'; 
}

//--------------------------

void 
WdgPointPos3D::onEdi()
{
  std::string str1 = (m_edi1 -> displayText()).toStdString();
  std::string str2 = (m_edi2 -> displayText()).toStdString();
  std::string str3 = (m_edi3 -> displayText()).toStdString();

  float val1, val2, val3;
  stringstream ss; ss << str1 << ' ' << str2 << ' ' << str3; 
  ss >> val1 >> val2 >> val3;

  emit posIsChanged(val1, val2, val3);
}

//--------------------------

void 
WdgPointPos3D::testPosIsChanged(const float& x, const float& y, const float& z)
{
  stringstream ss; ss << "3-d point position is changed to x: " << x << " y: " << y << " z: " << z; 
  MsgInLog(_name_(), INFO, ss.str());
  //std::cout << ss.str() << '\n'; 
}

//--------------------------
//--------------------------
//--------------------------

} // namespace PSQt

//--------------------------
