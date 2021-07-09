// Inspired by https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle

#include "framelesswindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>
#include <QScreen>

#include "ui_framelesswindow.h"

FramelessWindow::FramelessWindow(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::FramelessWindow),
      m_mouse_pressed(false),
      m_drag_top(false),
      m_drag_left(false),
      m_drag_right(false),
      m_drag_bottom(false)
{

  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
  // append minimize button flag in case of windows,
  // for correct windows native handling of minimize function
#if defined(Q_OS_WIN)
  setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
#endif
  setAttribute(Qt::WA_NoSystemBackground, true);
  setAttribute(Qt::WA_TranslucentBackground);

  ui->setupUi(this);

  setMouseTracking(true);

  // important to watch mouse move from all child widgets
  QApplication::instance()->installEventFilter(this);

  setContent(&m_tdm_gui);

  QVBoxLayout* box_layout = new QVBoxLayout(ui->menu_bar); // Main layout of widget
  box_layout->setAlignment(Qt::AlignVCenter);
  //box_layout->setContentsMargins(0, 0, 0, 0);
  setLayout(box_layout);

  m_menu_bar = new QMenuBar(ui->menu_bar);
  m_menu_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  ui->menu_bar->layout()->addWidget(m_menu_bar);

  QAction *newAct = new QAction(tr("&New"), this);

  m_file_menu = m_menu_bar->addMenu("File");
  m_file_menu->addAction(newAct);



}

FramelessWindow::~FramelessWindow() { delete ui; }

void FramelessWindow::on_restore_button_clicked() {
  //ui->restore_button->setVisible(false);

  ui->maximize_button->setVisible(true);
  setWindowState(Qt::WindowNoState);
  // on MacOS this hack makes sure the
  // background window is repaint correctly
  hide();
  show();
}

void FramelessWindow::on_maximize_button_clicked() {
  ui->maximize_button->setVisible(false);
  this->setWindowState(Qt::WindowMaximized);
  this->showMaximized();
}

void FramelessWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::WindowStateChange) {
    if (windowState().testFlag(Qt::WindowNoState)) {
      ui->maximize_button->setVisible(true);
      event->ignore();
    } else if (windowState().testFlag(Qt::WindowMaximized)) {
      ui->maximize_button->setVisible(false);
      event->ignore();
    }
  }
  event->accept();
}

void FramelessWindow::setContent(QWidget *w) {
  ui->window_content->layout()->addWidget(w);
}

void FramelessWindow::setWindowTitle(const QString &text) {
  ui->title_text->setText(text);
}

void FramelessWindow::setWindowIcon(const QIcon &ico) {
  ui->icon->setPixmap(ico.pixmap(16, 16));
}

void FramelessWindow::on_minimize_button_clicked() {
  setWindowState(Qt::WindowMinimized);
}

void FramelessWindow::on_close_button_clicked() { close(); }

void FramelessWindow::on_window_title_bar_doubleClicked() {
  if (windowState().testFlag(Qt::WindowNoState)) {
    on_maximize_button_clicked();
  } else if (windowState().testFlag(Qt::WindowFullScreen)) {
    on_restore_button_clicked();
  }
}

void FramelessWindow::mouseDoubleClickEvent(QMouseEvent *event) {
  Q_UNUSED(event);
}

void FramelessWindow::checkBorderDragging(QMouseEvent *event) {
  if (isMaximized()) {
    return;
  }

  QPoint globalMousePos = event->globalPos();
  if (m_mouse_pressed) {
    QScreen *screen = QGuiApplication::primaryScreen();
	// available geometry excludes taskbar
    QRect availGeometry = screen->availableGeometry();
    int h = availGeometry.height();
    int w = availGeometry.width();
    QList<QScreen *> screenlist = screen->virtualSiblings();
    if (screenlist.contains(screen)) {
      QSize sz = QApplication::desktop()->size();
      h = sz.height();
      w = sz.width();
    }

    // top right corner
    if (m_drag_top && m_drag_right) {
      int diff =
          globalMousePos.x() - (m_StartGeometry.x() + m_StartGeometry.width());
      int neww = m_StartGeometry.width() + diff;
      diff = globalMousePos.y() - m_StartGeometry.y();
      int newy = m_StartGeometry.y() + diff;
      if (neww > 0 && newy > 0 && newy < h - 50) {
        QRect newg = m_StartGeometry;
        newg.setWidth(neww);
        newg.setX(m_StartGeometry.x());
        newg.setY(newy);
        setGeometry(newg);
      }
    }
    // top left corner
    else if (m_drag_top && m_drag_left) {
      int diff = globalMousePos.y() - m_StartGeometry.y();
      int newy = m_StartGeometry.y() + diff;
      diff = globalMousePos.x() - m_StartGeometry.x();
      int newx = m_StartGeometry.x() + diff;
      if (newy > 0 && newx > 0) {
        QRect newg = m_StartGeometry;
        newg.setY(newy);
        newg.setX(newx);
        setGeometry(newg);
      }
    }
    // bottom right corner
    else if (m_drag_bottom && m_drag_left) {
      int diff =
          globalMousePos.y() - (m_StartGeometry.y() + m_StartGeometry.height());
      int newh = m_StartGeometry.height() + diff;
      diff = globalMousePos.x() - m_StartGeometry.x();
      int newx = m_StartGeometry.x() + diff;
      if (newh > 0 && newx > 0) {
        QRect newg = m_StartGeometry;
        newg.setX(newx);
        newg.setHeight(newh);
        setGeometry(newg);
      }
    } else if (m_drag_top) {
      int diff = globalMousePos.y() - m_StartGeometry.y();
      int newy = m_StartGeometry.y() + diff;
      if (newy > 0 && newy < h - 50) {
        QRect newg = m_StartGeometry;
        newg.setY(newy);
        setGeometry(newg);
      }
    } else if (m_drag_left) {
      int diff = globalMousePos.x() - m_StartGeometry.x();
      int newx = m_StartGeometry.x() + diff;
      if (newx > 0 && newx < w - 50) {
        QRect newg = m_StartGeometry;
        newg.setX(newx);
        setGeometry(newg);
      }
    } else if (m_drag_right) {
      int diff =
          globalMousePos.x() - (m_StartGeometry.x() + m_StartGeometry.width());
      int neww = m_StartGeometry.width() + diff;
      if (neww > 0) {
        QRect newg = m_StartGeometry;
        newg.setWidth(neww);
        newg.setX(m_StartGeometry.x());
        setGeometry(newg);
      }
    } else if (m_drag_bottom) {
      int diff =
          globalMousePos.y() - (m_StartGeometry.y() + m_StartGeometry.height());
      int newh = m_StartGeometry.height() + diff;
      if (newh > 0) {
        QRect newg = m_StartGeometry;
        newg.setHeight(newh);
        newg.setY(m_StartGeometry.y());
        setGeometry(newg);
      }
    }
  } else {
    // no mouse pressed
    if (leftBorderHit(globalMousePos) && topBorderHit(globalMousePos)) {
      setCursor(Qt::SizeFDiagCursor);
    } else if (rightBorderHit(globalMousePos) && topBorderHit(globalMousePos)) {
      setCursor(Qt::SizeBDiagCursor);
    } else if (leftBorderHit(globalMousePos) &&
               bottomBorderHit(globalMousePos)) {
      setCursor(Qt::SizeBDiagCursor);
    } else {
      if (topBorderHit(globalMousePos)) {
        setCursor(Qt::SizeVerCursor);
      } else if (leftBorderHit(globalMousePos)) {
        setCursor(Qt::SizeHorCursor);
      } else if (rightBorderHit(globalMousePos)) {
        setCursor(Qt::SizeHorCursor);
      } else if (bottomBorderHit(globalMousePos)) {
        setCursor(Qt::SizeVerCursor);
      } else {
        m_drag_top = false;
        m_drag_left = false;
        m_drag_right = false;
        m_drag_bottom = false;
        setCursor(Qt::ArrowCursor);
      }
    }
  }
}

// pos in global virtual desktop coordinates
bool FramelessWindow::leftBorderHit(const QPoint &pos) {
  const QRect &rect = this->geometry();
  if (pos.x() >= rect.x() && pos.x() <= rect.x() + CONST_DRAG_BORDER_SIZE) {
    return true;
  }
  return false;
}

bool FramelessWindow::rightBorderHit(const QPoint &pos) {
  const QRect &rect = this->geometry();
  int tmp = rect.x() + rect.width();
  if (pos.x() <= tmp && pos.x() >= (tmp - CONST_DRAG_BORDER_SIZE)) {
    return true;
  }
  return false;
}

bool FramelessWindow::topBorderHit(const QPoint &pos) {
  const QRect &rect = this->geometry();
  if (pos.y() >= rect.y() && pos.y() <= rect.y() + CONST_DRAG_BORDER_SIZE) {
    return true;
  }
  return false;
}

bool FramelessWindow::bottomBorderHit(const QPoint &pos) {
  const QRect &rect = this->geometry();
  int tmp = rect.y() + rect.height();
  if (pos.y() <= tmp && pos.y() >= (tmp - CONST_DRAG_BORDER_SIZE)) {
    return true;
  }
  return false;
}

void FramelessWindow::mousePressEvent(QMouseEvent *event) {
  if (isMaximized()) {
    return;
  }

  m_mouse_pressed = true;
  m_StartGeometry = this->geometry();

  QPoint globalMousePos = mapToGlobal(QPoint(event->x(), event->y()));

  if (leftBorderHit(globalMousePos) && topBorderHit(globalMousePos)) {
    m_drag_top = true;
    m_drag_left = true;
    setCursor(Qt::SizeFDiagCursor);
  } else if (rightBorderHit(globalMousePos) && topBorderHit(globalMousePos)) {
    m_drag_right = true;
    m_drag_top = true;
    setCursor(Qt::SizeBDiagCursor);
  } else if (leftBorderHit(globalMousePos) && bottomBorderHit(globalMousePos)) {
    m_drag_left = true;
    m_drag_bottom = true;
    setCursor(Qt::SizeBDiagCursor);
  } else {
    if (topBorderHit(globalMousePos)) {
      m_drag_top = true;
      setCursor(Qt::SizeVerCursor);
    } else if (leftBorderHit(globalMousePos)) {
      m_drag_left = true;
      setCursor(Qt::SizeHorCursor);
    } else if (rightBorderHit(globalMousePos)) {
      m_drag_right = true;
      setCursor(Qt::SizeHorCursor);
    } else if (bottomBorderHit(globalMousePos)) {
      m_drag_bottom = true;
      setCursor(Qt::SizeVerCursor);
    }
  }
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event) {
  Q_UNUSED(event);
  if (isMaximized()) {
    return;
  }

  m_mouse_pressed = false;
  bool bSwitchBackCursorNeeded =
      m_drag_top || m_drag_left || m_drag_right || m_drag_bottom;
  m_drag_top = false;
  m_drag_left = false;
  m_drag_right = false;
  m_drag_bottom = false;
  if (bSwitchBackCursorNeeded) {
    setCursor(Qt::ArrowCursor);
  }
}

bool FramelessWindow::eventFilter(QObject *obj, QEvent *event) {
  if (isMaximized()) {
    return QWidget::eventFilter(obj, event);
  }

  // check mouse move event when mouse is moved on any object
  if (event->type() == QEvent::MouseMove) {
    QMouseEvent *pMouse = dynamic_cast<QMouseEvent *>(event);
    if (pMouse) {
      checkBorderDragging(pMouse);
    }
  }
  // press is triggered only on frame window
  else if (event->type() == QEvent::MouseButtonPress && obj == this) {
    QMouseEvent *pMouse = dynamic_cast<QMouseEvent *>(event);
    if (pMouse) {
      mousePressEvent(pMouse);
    }
  } else if (event->type() == QEvent::MouseButtonRelease) {
    if (m_mouse_pressed) {
      QMouseEvent *pMouse = dynamic_cast<QMouseEvent *>(event);
      if (pMouse) {
        mouseReleaseEvent(pMouse);
      }
    }
  }

  return QWidget::eventFilter(obj, event);
}
