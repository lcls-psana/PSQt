//---------------------------------------------------------------------
// File and Version Information:
//   $Id$
//
// Author: Mikhail S. Dubrovin
//---------------------------------------------------------------------

//--------------------------
#include "PSQt/WdgImage.h"
#include "PSQt/Logger.h"
#include "PSCalib/GeometryAccess.h"
#include "ndarray/ndarray.h" // for img_from_pixel_arrays(...)

#include "PSQt/QGUtils.h"

#include <iostream>    // for std::cout
#include <fstream>     // for std::ifstream(fname)
//using namespace std; // for cout without std::
#include <math.h>  // atan2, round, floor, ceil, trunc
#include <cstring> // for memcpy

namespace PSQt {

//--------------------------

WdgImage::WdgImage(QWidget *parent, const std::string& ifname)
  : QLabel(parent)
  , m_pixmap_raw(0)
  , m_pixmap_scl(0)
  , m_frame(0)
  , m_painter(0)
  , m_geo_img(0)
  , m_pen1(0)  
  , m_pen2(0)  
  , m_point1(0)
  , m_point2(0)
  , m_rect1(0) 
  , m_rect2(0)
  , p_nda_img_raw(0)
{
  MsgInLog(_name_(), DEBUG, "ctor1 - using fname");

  setWdgParams();

  const std::string fname = (ifname!=std::string()) ? ifname
                          : "/reg/neh/home1/dubrovin/LCLS/pubs/galaxy.jpeg";
  //                          : "/reg/neh/home1/dubrovin/LCLS/pubs/reflective-geometry.png";
  loadImageFromFile(fname);
}

//--------------------------

WdgImage::WdgImage(QWidget *parent, const QImage* image)
  : QLabel(parent)
  , m_pixmap_raw(0)
  , m_pixmap_scl(0)
  , m_frame(0)
  , m_painter(0)
  , m_geo_img(0)
  , m_pen1(0)  
  , m_pen2(0)  
  , m_point1(0)
  , m_point2(0)
  , m_rect1(0) 
  , m_rect2(0) 
  , p_nda_img_raw(0)
{
  MsgInLog(_name_(), DEBUG, "ctor2 - using QImage*");

  setWdgParams();
  setPixmapScailedImage(image);
}

//--------------------------

WdgImage::~WdgImage()
{
  if (m_frame)      delete m_frame;  
  if (m_painter)    delete m_painter;  
  if (m_pen1)       delete m_pen1;  
  if (m_pen2)       delete m_pen2;  
  if (m_point1)     delete m_point1;  
  if (m_point2)     delete m_point2;  
  if (m_rect1)      delete m_rect1;  
  if (m_rect2)      delete m_rect2;  
  if (m_pixmap_raw) delete m_pixmap_raw;  
  if (m_pixmap_scl) delete m_pixmap_scl;  
}

//--------------------------

void 
WdgImage::setWdgParams()
{
  this -> setMinimumSize(606,606);
  this -> setAlignment(Qt::AlignTop | Qt::AlignLeft);
  this -> setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  this -> setCursor(Qt::PointingHandCursor); // Qt::SizeAllCursor, Qt::WaitCursor, Qt::PointingHandCursor
  this -> setMargin(0);
  //////////////////////////////////
  //this -> setScaledContents (false);
  this -> setScaledContents (true);
  //////////////////////////////////

  QVector<qreal> dashes;
  qreal space = 4;
  dashes << 4 << space;

  m_pen1   = new QPen(Qt::black, 1, Qt::DashLine);
  m_pen2   = new QPen(Qt::white, 1, Qt::DashLine);
  m_pen1->setDashPattern(dashes);
  m_pen2->setDashPattern(dashes);
  m_pen2->setDashOffset(4);
  m_point1 = new QPoint();
  m_point2 = new QPoint();
  m_rect1  = new QRect();
  m_rect2  = new QRect();
  m_is_pushed = false;

  m_painter = new QPainter(); // (this)

  m_amin    = 0; // - for auto-definition of the intensity range
  m_amax    = 0;
  m_ncolors = 1024; // =0 - color table is not used
  m_hue1    = -120;
  m_hue2    = -360;

  this->resetZoom();

  connect(this, SIGNAL(zoomIsChanged(int&, int&, int&, int&, float&, float&)), 
          this, SLOT(testSignalZoomIsChanged(int&, int&, int&, int&, float&, float&)));
}

//--------------------------

void 
WdgImage::paintEvent(QPaintEvent *event)
{
  static unsigned count=0; count++;
  QLabel::paintEvent(event);

  m_painter->begin(this);
  //-----------
  if(m_is_pushed) drawRect();
  //-----------
  m_painter->end();

  //std::cout << "WdgImage::paintEvent counter = " << count << '\n';
}

//--------------------------

void 
WdgImage::drawRect()
{
  m_rect1->setCoords(m_point1->x(), m_point1->y(), m_point2->x(), m_point2->y());
  m_rect2->setCoords(m_point1->x(), m_point1->y(), m_point2->x(), m_point2->y());

  m_painter->setPen  (*m_pen1);
  m_painter->drawRect(*m_rect1); 
  m_painter->setPen  (*m_pen2);
  m_painter->drawRect(*m_rect2); 
}

//--------------------------

void 
WdgImage::resetZoom()
{
  m_point1->setX(0);
  m_point1->setY(0);
  m_xmin_raw = 0;
  m_ymin_raw = 0;
  //m_xmax_raw = 0;
  //m_ymax_raw = 0;
  m_zoom_is_on = false;
}

//--------------------------

void 
WdgImage::zoomInImage()
{
  MsgInLog(_name_(), DEBUG, "zoomInImage()");

  //std::cout << "  x1:" << m_point1->x() << "  y1:" << m_point1->y() 
  //          << "  x2:" << m_point2->x() << "  y2:" << m_point2->y()<< '\n'; 

  if(m_point1->x() != 0 or m_point1->y() != 0) {

    float sclx = float(m_pixmap_scl->size().width())  / this->size().width();  
    float scly = float(m_pixmap_scl->size().height()) / this->size().height();  
    
    int p1x = int( m_point1->x()*sclx );
    int p1y = int( m_point1->y()*scly );
    int p2x = int( m_point2->x()*sclx );
    int p2y = int( m_point2->y()*scly );
    
    int xmin = min(p1x, p2x);
    int xmax = max(p1x, p2x); 
    int ymin = min(p1y, p2y);
    int ymax = max(p1y, p2y);
    
    m_xmin_raw += xmin;
    m_ymin_raw += ymin;
    m_xmax_raw = m_xmin_raw + xmax-xmin;
    m_ymax_raw = m_ymin_raw + ymax-ymin;
    
    m_point1->setX(0);
    m_point1->setY(0);

    m_zoom_is_on = true;
  }

  if (m_zoom_is_on) {
    int rw = int(m_pixmap_raw->size().width());
    int rh = int(m_pixmap_raw->size().height());

    if(m_xmax_raw > m_xmin_raw+rw) m_xmax_raw = m_xmin_raw+rw;
    if(m_ymax_raw > m_ymin_raw+rh) m_ymax_raw = m_ymin_raw+rh;

     *m_pixmap_scl = m_pixmap_raw->copy(m_xmin_raw, m_ymin_raw, m_xmax_raw-m_xmin_raw, m_ymax_raw-m_ymin_raw);
     setPixmap(m_pixmap_scl->scaled(this->size(), Qt::KeepAspectRatio, Qt::FastTransformation));

     emit zoomIsChanged(m_xmin_raw, m_ymin_raw, m_xmax_raw, m_ymax_raw, m_amin, m_amax);
  }
}

//--------------------------

QPointF
WdgImage::pointInRaw(const QPointF& point_img)
{
  float sclx = float(m_pixmap_scl->size().width())  / this->size().width();
  float scly = float(m_pixmap_scl->size().height()) / this->size().height();

  double x = point_img.x()*sclx + m_xmin_raw;
  double y = point_img.y()*scly + m_ymin_raw;  

  return QPointF(x,y);
}

//--------------------------

QPointF
WdgImage::pointInImage(const QPointF& point_raw)
{
  //float sclx = WdgImage::size().width()  / float((m_zoom_is_on)? (m_xmax_raw-m_xmin_raw) : m_pixmap_scl->size().width());  
  //float scly = WdgImage::size().height() / float((m_zoom_is_on)? (m_ymax_raw-m_ymin_raw) : m_pixmap_scl->size().height());  
  float sclx = this->size().width()  / float((m_zoom_is_on)? (m_xmax_raw-m_xmin_raw) : m_pixmap_scl->size().width());  
  float scly = this->size().height() / float((m_zoom_is_on)? (m_ymax_raw-m_ymin_raw) : m_pixmap_scl->size().height());  

  float x = (point_raw.x()-m_xmin_raw)*sclx;
  float y = (point_raw.y()-m_ymin_raw)*scly;  

  return QPointF(x,y);
}

//--------------------------

void 
WdgImage::setPixmapScailedImage(const QImage* image) // = 0 by default
{  
  //if image is available - reset m_pixmap_raw
  if(image) {

    //QRect r = image->rect();
    //cout << "XXX:setPixmapScailedImage(*image), rect: " << " x=" << r.x() << " y=" << r.y()
    //     << " width=" << r.width() << " height=" << r.height() << '\n';

    if(m_pixmap_raw) delete m_pixmap_raw;
    m_pixmap_raw = new QPixmap(QPixmap::fromImage(*image));
  }

  if (m_zoom_is_on) {
    //zoomInImage();

    QSize size_new = image->size();

    if(size_new.width()  < m_xmax_raw
    || size_new.height() < m_ymax_raw) {
        m_xmax_raw = size_new.width();
        m_ymax_raw = size_new.height();
    }
    
    zoomInImage(); //emit zoomIsChanged inside
    update();
  }
  else {
    if (m_pixmap_scl) delete m_pixmap_scl;
    m_pixmap_scl = new QPixmap(*m_pixmap_raw);
    setPixmap(m_pixmap_scl->scaled(this->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
    m_xmin_raw = 0;
    m_ymin_raw = 0;
    m_xmax_raw = m_pixmap_raw->size().width();
    m_ymax_raw = m_pixmap_raw->size().height();

    //if(m_amin==0 && m_amax==0) setIntensityRange(m_amin,m_amax);
    emit zoomIsChanged(m_xmin_raw, m_ymin_raw, m_xmax_raw, m_ymax_raw, m_amin, m_amax);
  }
}

//--------------------------

void
WdgImage::setFrame() 
{
  m_frame = new QFrame(this);
  m_frame -> setFrameStyle ( QFrame::Box | QFrame::Sunken); // or
  m_frame -> setLineWidth(0);
  m_frame -> setMidLineWidth(1);
  //m_frame -> setCursor(Qt::OpenHandCursor); // Qt::SizeAllCursor, Qt::WaitCursor, Qt::PointingHandCursor
  //m_frame -> setStyleSheet("background-color: rgb(0, 255, 255); color: rgb(255, 255, 100)");
  m_frame -> setGeometry(this->rect());
  m_frame -> setVisible(true);
}

//--------------------------

void 
WdgImage::resizeEvent(QResizeEvent *event)
{
  //m_frame->setGeometry(0, 0, event->size().width(), event->size().height());
  
  //std::cout << "WdgImage::resizeEvent(...): w=" << event->size().width() 
  //	    << "  h=" << event->size().height() << '\n';

  setPixmap(m_pixmap_scl->scaled(this->size(), Qt::KeepAspectRatio, Qt::FastTransformation));

  setWindowTitle("Window is resized");
}

//--------------------------

void 
WdgImage::closeEvent(QCloseEvent *event)
{
  QWidget::closeEvent(event);
  stringstream ss; ss << "closeEvent(...): type = " << event -> type();
  MsgInLog(_name_(), INFO, ss.str());
}

//--------------------------

void 
WdgImage::mousePressEvent(QMouseEvent *e)
{
  QPointF p = pointInRaw(QPointF(e->x(),e->y()));
  const ndarray<GeoImage::raw_image_t,2>& nda = *p_nda_img_raw;

  int  ix = (int)floor(p.x());
  int  iy = (int)floor(p.y());
  float z = (float)nda[iy][ix];

  //std::cout << "mousePressEvent:"
  //          << "  button: " << e->button()
  //          << "  x(),y() = " << e->x() << ", " << e->y()
  //          << "  xr,yr = " <<p.x()  << ", " << p.y()
  //          << "  ix,iy,z = " << ix << ", " << iy << ", " << z
  //          << "  isActiveWindow(): " << this->isActiveWindow()
  //          << '\n';

  emit mouseInPoint((float)ix, (float)iy, z);  

  this -> setCursor(Qt::ClosedHandCursor);

  m_point1->setX(e->x());
  m_point1->setY(e->y());
  m_point2->setX(e->x());
  m_point2->setY(e->y());

  m_is_pushed = true;
  update();
}

//--------------------------

void 
WdgImage::onZoomResetButton()
{
    MsgInLog(_name_(), INFO, "onZoomResetButton()");

    this->resetZoom();
    float amin = 0;
    float amax = 0;
    this->setIntensityRange(amin, amax);
    //setPixmapScailedImage();
    //update();
}

//--------------------------

void 
WdgImage::mouseReleaseEvent(QMouseEvent *e)
{
  /*
  std::cout << "mouseReleaseEvent:"
            << "  button: " << e->button()
            << "  x(),y() = " << e->x() << ", " << e->y()
            << '\n';
  */

  //this -> unsetCursor(); 
  this -> setCursor(Qt::PointingHandCursor); // Qt::SizeAllCursor, Qt::WaitCursor, Qt::PointingHandCursor

  m_is_pushed = false;

  if(e->button() == Qt::MidButton) { // where Qt::MidButton=4
    // Do it for reset
    this->resetZoom();
    setPixmapScailedImage();
    update();
    return;
  }

  if(e->button() == Qt::LeftButton) { // where Qt::LeftButton=1
    // Do it for zoom
    m_point2->setX(e->x());
    m_point2->setY(e->y());
    
    QPoint dist = *m_point2 - *m_point1;
    
    if(abs(dist.x()) > 5 && abs(dist.y()) > 5) {
    
      if(! this->rect().contains(*m_point2)) {
        int x = int(this->rect().x()); 
        int y = int(this->rect().y()); 
        int w = int(this->rect().width()); 
        int h = int(this->rect().height()); 
    
        if(e->x() < x  ) m_point2->setX(x);
        if(e->y() < y  ) m_point2->setY(y);
        if(e->x() > x+w) m_point2->setX(x+w);
        if(e->y() > y+h) m_point2->setY(y+h);
      }
    
      zoomInImage();
      update();
    }
  }
}

//--------------------------

void 
WdgImage::mouseMoveEvent(QMouseEvent *e)
{
  //std::cout << "XXX: mouseMoveEvent: "
  //          << "  button: " << e->button()
  //          << "  x(),y() = "  << e->x() << ", " << e->y()
  //          << '\n';

  m_point2->setX(e->x());
  m_point2->setY(e->y());
  update();
}

//--------------------------

void 
WdgImage::loadImageFromFile(const std::string& fname)
{
  MsgInLog(_name_(), INFO, "Load image from file " + fname);
  //std::cout << "WdgImage::loadImageFromFile: " << fname << '\n';
  //clear();

  QImage image(fname.c_str());

  this->resetZoom();
  setPixmapScailedImage(&image);

  //onZoomResetButton();    
  //float amin = 0;
  //float amax = 0;
  //setIntensityRange(amin, amax);
}

//--------------------------

void 
WdgImage::onFileNameChanged(const std::string& fname)
{
  MsgInLog(_name_(), INFO, "onFileNameChanged: " + fname);
  //std::cout << _name_() << "onFileNameChanged(string) - slot: fname = " << fname << '\n';  
  loadImageFromFile(fname);
}

//--------------------------
void 
WdgImage::onImageIsUpdated(ndarray<GeoImage::raw_image_t,2>& nda)
{
  p_nda_img_raw = &nda;

  stringstream ss; ss << "Update raw image in window,"
                      << " rows:" << nda.shape()[0] << " cols:" << nda.shape()[1]
                      << " amin=" << m_amin
                      << " amax=" << m_amax
                      << " h1="   << m_hue1
                      << " h2="   << m_hue2;

  MsgInLog(_name_(), INFO, ss.str()); 

  const ndarray<GeoImage::image_t,2> nda_norm = 
    getUint32NormalizedImage<const GeoImage::raw_image_t>(nda, m_amin, m_amax, m_ncolors, m_hue1, m_hue2); // from QGUtils
  
  setNormImage(nda_norm);
  update();
}

//--------------------------
void 
WdgImage::setNormImage(const ndarray<GeoImage::image_t,2>& nda)
{
  const unsigned int rows = nda.shape()[0];
  const unsigned int cols = nda.shape()[1];

  stringstream ss; ss << "setNormImage::set normalized image in window, rows:" << nda.shape()[0] << " cols:" << nda.shape()[1] ;
  MsgInLog(_name_(), DEBUG, ss.str()); 
  
  QImage image((const uchar*) nda.data(), cols, rows, QImage::Format_ARGB32);
  setPixmapScailedImage(&image);
  
  static unsigned counter=0; stringstream sst; sst << "Image # " << ++counter;
  setWindowTitle(sst.str().c_str());
}


//--------------------------
void 
WdgImage::getIntensityLimits(float& imin, float& imax)
{
  imin=0; 
  imax=1; 
  if (! p_nda_img_raw) return;

  double a(0);
  double s0(0);    
  double s1(0);    
  double s2(0);    

  const ndarray<GeoImage::raw_image_t,2>& nda = *p_nda_img_raw;
  for(int iy=m_ymin_raw; iy<m_ymax_raw; ++iy) {
    for(int ix=m_xmin_raw; ix<m_xmax_raw; ++ix) {
      a = (float)nda[iy][ix];
  	s0 += 1;
  	s1 += a;
  	s2 += a*a;
    }
  }
  double ave = (s0>0) ? s1/s0 : 0;
  double rms = (s0>0) ? sqrt(s2/s0-ave*ave) : 1;

  imin = floor(ave-3*rms);
  imax =  ceil(ave+6*rms);
  //std::cout << "XXX: getIntensityLimits: "
  //          << "  ave,rms = "  << ave << ", " << rms
  //          << "  imin,imax = "  << imin << ", " << imax
  //          << '\n';
}

//--------------------------
void 
WdgImage::getIntensityLimitsV1(float& imin, float& imax)
{
    float a(0);
    const ndarray<GeoImage::raw_image_t,2>& nda = *p_nda_img_raw;
    imin = (float)nda[m_ymin_raw][m_xmin_raw];
    imax = imin;
    for(int iy=m_ymin_raw; iy<m_ymax_raw; ++iy) {
      for(int ix=m_xmin_raw; ix<m_xmax_raw; ++ix) {
        a = (float)nda[iy][ix];
        if(a<imin) imin = a;
        if(a>imax) imax = a;
      }
    }
}

//--------------------------
void 
WdgImage::setIntensityRange(const float& amin, const float& amax)
{
  if(amin && amax) {
    m_amin = amin;
    m_amax = amax;
  }
  else {
    float Imin, Imax;
    this->getIntensityLimits(Imin, Imax);

    m_amax = (amax==0) ? Imax : amax;
    m_amin = (amin==0) ? Imin : amin;
  }

  if(p_nda_img_raw) this->onImageIsUpdated(*p_nda_img_raw);

  std::stringstream ss; ss << "Set intensity range amin=" << m_amin << " amax=" << m_amax;
  MsgInLog(_name_(), INFO, ss.str());  
}

//--------------------------

void 
WdgImage::onPressOnAxes(QMouseEvent* e, QPointF p)
{
  std::stringstream ss;
  ss << _name_() << " onPressOnAxes  button: " << e->button()
    //<< "  window x(), y() = " << e->x() << ", " << e->y()
     << "  scene x(), y() = " << p.x() << ", " << p.y();

  MsgInLog(_name_(), DEBUG, ss.str());

  float amin = m_amin;
  float amax = m_amax;

  switch (e->button()) {
  case Qt::LeftButton  : amin = p.x(); break;
  case Qt::RightButton : amax = p.x(); break;
  default : 
  case Qt::MidButton   : amin = 0; amax = 0; break;
  }

  setIntensityRange(amin, amax);
}

//--------------------------
void 
WdgImage::onHueAnglesUpdated(const float& h1, const float& h2)
{
  m_hue1 = h1;
  m_hue2 = h2;

  if(p_nda_img_raw) this->onImageIsUpdated(*p_nda_img_raw);

  update();
  std::stringstream ss; ss << ":onHueAnglesUpdated h1:" << m_hue1 << " h2:" << m_hue2;
  MsgInLog(_name_(), INFO, ss.str());  
}

//--------------------------
void 
WdgImage::testSignalZoomIsChanged(int& xmin, int& ymin, int& xmax, int& ymax, float& amin, float& amax)
{
  stringstream ss;
  ss << "testSignalZoomIsChanged(...): zoom is changed to"
     << "  xmin=" << xmin 
     << "  ymin=" << ymin
     << "  xmax=" << xmax 
     << "  ymax=" << ymax
     << "  amin=" << amin 
     << "  amax=" << amax;
  MsgInLog(_name_(), DEBUG, ss.str());
}

//--------------------------
//--------------------------
//----   Test images   -----
//--------------------------
//--------------------------

void 
WdgImage::setColorPixmap()
{
  MsgInLog(_name_(), INFO, "setColorPixmap()");
  //std::cout << "WdgImage::setColorPixmap()\n";
  //clear();

  const int ssize = 1024;
  const int isize = ssize*ssize;

  uint32_t dimg[ssize][ssize]; 
  std::fill_n(&dimg[0][0], int(isize), uint32_t(0xFF000000));

  int vRx = 512 - 128;
  int vRy = 512 - 128;
  int vGx = 512 + 128;
  int vGy = 512 - 128;
  int vBx = 512;
  int vBy = 512 - 128 + 222;

  for(int i=0; i<ssize; ++i) {
  for(int j=0; j<ssize; ++j) {

    int rR = (int) sqrt( pow(i-vRx,2) + pow(j-vRy,2) );
    int rG = (int) sqrt( pow(i-vGx,2) + pow(j-vGy,2) );
    int rB = (int) sqrt( pow(i-vBx,2) + pow(j-vBy,2) );

    //int	r = 255-rR; r = (r>0) ? r : 0;
    //int	g = 255-rG; g = (g>0) ? g : 0;
    //int	b = 255-rB; b = (b>0) ? b : 0;
    int	r = (rR<255) ? rR : 255;
    int	g = (rG<255) ? rG : 255;
    int	b = (rB<255) ? rB : 255;

    dimg[i][j] += (r<<16) + (g<<8) + b;
  }
  }

  QImage image((const uchar*) &dimg[0], ssize, ssize, QImage::Format_ARGB32);
  setPixmapScailedImage(&image);
}

//--------------------------

void 
WdgImage::setColorWhellPixmap()
{
  MsgInLog(_name_(), INFO, "setColorWhellPixmap()");
  //std::cout << "WdgImage::setColorWhellPixmap()\n";
  //clear();

  const int ssize = 1024;
  const int isize = ssize*ssize;

  const int xc = 512;
  const int yc = 512;  

  const float RAD2DEG =  180/3.14159265;

  uint32_t dimg[ssize][ssize]; 
  std::fill_n(&dimg[0][0], int(isize), uint32_t(0xFF000000));


  for(int i=0; i<ssize; ++i) {
  for(int j=0; j<ssize; ++j) {

    float x = i-xc;
    float y = j-yc;
    float r = sqrt(x*x + y*y) / 512;
    r = (r<1) ? r : 0;

    float hue = atan2(y,x) * RAD2DEG;
    hue = (hue<0) ? hue+360 : hue;

    dimg[i][j] = HSV2RGBA(hue, 1, r);
  }
  }

  QImage image((const uchar*) &dimg[0], ssize, ssize, QImage::Format_ARGB32);
  setPixmapScailedImage(&image);
}

//--------------------------

void
WdgImage::setColorBar( const unsigned& rows, 
                       const unsigned& cols,
                       const float&    hue1,
                       const float&    hue2
                      )
{
  MsgInLog(_name_(), INFO, "setColorBar()");
  //std::cout << "WdgImage::setColorBar()\n";
  uint32_t* ctable = ColorTable(cols, hue1, hue2);
  uint32_t dimg[rows][cols]; 

  for(unsigned r=0; r<rows; ++r) {
    std::memcpy(&dimg[r][0], &ctable[0], cols*sizeof(uint32_t));
    //dimg[r][c] = ctable[c];
  }

  QImage image((const uchar*) &dimg[0], cols, rows, QImage::Format_ARGB32);
  setPixmapScailedImage(&image);
}

//--------------------------

void 
WdgImage::setCameraImage(const std::string& ifname_geo, const std::string& ifname_img)
{
  typedef PSCalib::GeometryAccess::image_t image_t;

  const std::string base_dir = "/reg/g/psdm/detector/alignment/cspad/calib-cxi-ds1-2014-05-15/";
  const std::string fname_geo = (ifname_geo != std::string()) ? ifname_geo
                              : base_dir + "calib/CsPad::CalibV1/CxiDs1.0:Cspad.0/geometry/2-end.data"; 
  const std::string fname_img = (ifname_img != std::string()) ? ifname_img
                              : base_dir + "cspad-arr-cxid2714-r0023-lysozyme-rings.txt"; 

  MsgInLog(_name_(), INFO, "setCameraImage()");

  if (m_geo_img) delete m_geo_img;  
  m_geo_img = new GeoImage(fname_geo, fname_img);

  ndarray<uint32_t, 2> inda = m_geo_img->getNormalizedImage();

  QImage image((const uchar*) &inda[0][0], inda.shape()[1], inda.shape()[0], QImage::Format_ARGB32);
  setPixmapScailedImage(&image);
}

//--------------------------

void 
WdgImage::onTest()
{
  MsgInLog(_name_(), INFO, "onTest() - slot");
  //std::cout << "WdgImage::onTest() - slot\n";  

  static unsigned counter = 0; ++counter;

  if(counter%6 == 1) setColorWhellPixmap();
  if(counter%6 == 2) setColorBar();
  if(counter%6 == 3) setColorPixmap();
  if(counter%6 == 4) setCameraImage();
  if(counter%6 == 5) this -> setScaledContents (false);
  if(counter%6 == 0) this -> setScaledContents (true);
}

//--------------------------
//--------------------------
//--------------------------
//--------------------------
//--------------------------
//--------------------------

} // namespace PSQt

//--------------------------
