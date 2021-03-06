// Inspired by https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle

#include "3DMetricsConfig.h"
#include "tdmmenubarwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>
#include <QScreen>

#include "ui_tdmmenubarwindow.h"

TDMMenuBarWindow::TDMMenuBarWindow(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TDMMenuBarWindow),
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
  //setAttribute(Qt::WA_NoSystemBackground, true);
  //setAttribute(Qt::WA_TranslucentBackground);

  ui->setupUi(this);

  setMouseTracking(true);

  // important to watch mouse move from all child widgets
  QApplication::instance()->installEventFilter(this);

  initMenu();
  initTitleAndVersion();

  m_tdm_gui = new TDMGui(this);
  setContent(m_tdm_gui);

}

TDMMenuBarWindow::~TDMMenuBarWindow()
{
    delete ui;
    delete m_tdm_gui;
}

void TDMMenuBarWindow::on_maximize_button_clicked() {

  if (windowState().testFlag(Qt::WindowNoState)) {
      this->setWindowState(Qt::WindowMaximized);
      this->showMaximized();
  }
  else if (windowState().testFlag(Qt::WindowMaximized)) {
      this->setWindowState(Qt::WindowNoState);
      this->showNormal();
  }

}

void TDMMenuBarWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::WindowStateChange) {
    if (windowState().testFlag(Qt::WindowNoState)) {
      //ui->maximize_button->setVisible(true);
      event->ignore();
    } else if (windowState().testFlag(Qt::WindowMaximized)) {
      //ui->maximize_button->setVisible(false);
      event->ignore();
    }
  }
  event->accept();
}

void TDMMenuBarWindow::setContent(QWidget *w) {
  ui->window_content->layout()->addWidget(w);
}

void TDMMenuBarWindow::writeMessage(QString _message)
{
    ui->message->setText(_message);
}

void TDMMenuBarWindow::setWindowTitle(const QString &text) {
  ui->title_text->setText(text);
}

void TDMMenuBarWindow::setWindowIcon(const QIcon &ico) {
  ui->icon->setPixmap(ico.pixmap(16, 16));
}

void TDMMenuBarWindow::on_minimize_button_clicked() {
  setWindowState(Qt::WindowMinimized);
}

void TDMMenuBarWindow::on_close_button_clicked() { close(); }

void TDMMenuBarWindow::on_window_title_bar_doubleClicked() {
  on_maximize_button_clicked();
}

void TDMMenuBarWindow::mouseDoubleClickEvent(QMouseEvent *event) {
  Q_UNUSED(event);
}

void TDMMenuBarWindow::checkBorderDragging(QMouseEvent *event) {
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
bool TDMMenuBarWindow::leftBorderHit(const QPoint &pos) {
  const QRect &rect = this->geometry();
  if (pos.x() >= rect.x() && pos.x() <= rect.x() + CONST_DRAG_BORDER_SIZE) {
    return true;
  }
  return false;
}

bool TDMMenuBarWindow::rightBorderHit(const QPoint &pos) {
  const QRect &rect = this->geometry();
  int tmp = rect.x() + rect.width();
  if (pos.x() <= tmp && pos.x() >= (tmp - CONST_DRAG_BORDER_SIZE)) {
    return true;
  }
  return false;
}

bool TDMMenuBarWindow::topBorderHit(const QPoint &pos) {
  const QRect &rect = this->geometry();
  if (pos.y() >= rect.y() && pos.y() <= rect.y() + CONST_DRAG_BORDER_SIZE) {
    return true;
  }
  return false;
}

bool TDMMenuBarWindow::bottomBorderHit(const QPoint &pos) {
  const QRect &rect = this->geometry();
  int tmp = rect.y() + rect.height();
  if (pos.y() <= tmp && pos.y() >= (tmp - CONST_DRAG_BORDER_SIZE)) {
    return true;
  }
  return false;
}

void TDMMenuBarWindow::mousePressEvent(QMouseEvent *event) {
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

void TDMMenuBarWindow::mouseReleaseEvent(QMouseEvent *event) {
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

bool TDMMenuBarWindow::eventFilter(QObject *obj, QEvent *event) {
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

void TDMMenuBarWindow::initMenu()
{
    QVBoxLayout* box_layout = new QVBoxLayout(ui->menu_bar); // Main layout of widget
    box_layout->setAlignment(Qt::AlignVCenter);
    //box_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(box_layout);

    // Create menubar
    m_menu_bar = new QMenuBar(ui->menu_bar);
    m_menu_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->menu_bar->layout()->addWidget(m_menu_bar);

    // Add menus to menu bar (& is in front of the keyboard shortcut letter)
    m_file_menu = m_menu_bar->addMenu("&File");
    m_tools_menu = m_menu_bar->addMenu("&Tools");
    m_view_menu = m_menu_bar->addMenu("&View");
    m_help_menu = m_menu_bar->addMenu("&Help");

    // File menu actions
    m_open_3d_model_action = new QAction(tr("&Open a 3D model"),this); // todo add icon
    m_file_menu->addAction(m_open_3d_model_action);
    m_file_menu->addSeparator();

    m_open_measurement_file_action = new QAction(tr("Open a measurement file"), this);
    m_file_menu->addAction(m_open_measurement_file_action);
    m_save_measurement_file_action = new QAction(tr("Save current measurement changes"), this);
    m_file_menu->addAction(m_save_measurement_file_action);
    m_save_measurement_file_as_action = new QAction(tr("Save measurement layer as file"), this);
    m_file_menu->addAction(m_save_measurement_file_as_action);
    m_file_menu->addSeparator();

    m_open_project_action = new QAction(tr("Open a project file"), this);
    m_file_menu->addAction(m_open_project_action);
    m_save_project_action = new QAction(tr("Save current layers as a project file"), this);
    m_file_menu->addAction(m_save_project_action);
    m_close_project_action = new QAction(tr("Close current project"), this);
    m_file_menu->addAction(m_close_project_action);
    m_file_menu->addSeparator();

    m_import_old_measurement_format_action = new QAction(tr("Import old measurement format"), this);
    m_file_menu->addAction(m_import_old_measurement_format_action);
    m_file_menu->addSeparator();

    m_quit_action = new QAction(tr("&Quit"), this);
    m_file_menu->addAction(m_quit_action);

    // Tools menu
    m_export_data_to_csv_action = new QAction(tr("Export data table to text (*.csv, *.txt)"), this);
    m_tools_menu->addAction(m_export_data_to_csv_action);
    m_decimate_model_action = new QAction(tr("Decimate model (smartly reduce model size)"), this);
    m_tools_menu->addAction(m_decimate_model_action);
    m_take_snapshot_action = new QAction(tr("Take a snapshot of current view"), this);
    m_tools_menu->addAction(m_take_snapshot_action);

    // View menu
    m_layers_tree_window_action = new QAction(tr("Layers tree window"), this);
    m_layers_tree_window_action->setCheckable(true);
    m_layers_tree_window_action->setChecked(true);
    m_view_menu->addAction(m_layers_tree_window_action);
    m_attrib_table_window_action = new QAction(tr("Attribute table window"), this);
    m_attrib_table_window_action->setCheckable(true);
    m_attrib_table_window_action->setChecked(true);
    m_view_menu->addAction(m_attrib_table_window_action);
    m_view_menu->addSeparator();

    m_add_axes_action = new QAction(tr("Add reference axis on scene"), this);
    m_add_axes_action->setCheckable(true);
    m_add_axes_action->setChecked(false);
    m_view_menu->addAction(m_add_axes_action);
    m_stereo_action = new QAction(tr("Enable/Disable stereo view"), this);
    m_stereo_action->setCheckable(true);
    m_stereo_action->setChecked(false);
    m_view_menu->addAction(m_stereo_action);
    m_light_action = new QAction(tr("Enable/Disable light"), this);
    m_light_action->setCheckable(true);
    m_light_action->setChecked(false);
    m_view_menu->addAction(m_light_action);
    m_view_menu->addSeparator();

    m_z_scale_action = new QAction(tr("Exaggerating Z scale"), this);
    m_view_menu->addAction(m_z_scale_action);
    m_view_menu->addSeparator();

    m_depth_colot_chooser_action = new QAction(tr("Choose depth colormap"), this);
    m_view_menu->addAction(m_depth_colot_chooser_action);
    m_show_z_scale_action = new QAction(tr("Show/Hide Z scale"), this);
    m_show_z_scale_action->setCheckable(true);
    m_show_z_scale_action->setChecked(false);
    m_view_menu->addAction(m_show_z_scale_action);

    // Help menu
    m_action_user_manual = new QAction(tr("Open user manual"), this);
    m_help_menu->addAction(m_action_user_manual);
    m_about_action = new QAction(tr("About"), this);
    m_help_menu->addAction(m_about_action);
}

void TDMMenuBarWindow::initTitleAndVersion()
{
    ui->title_text->setText(QString("3D Metrics v%1.%2.%3").arg(TDM_VERSION_MAJOR).arg(TDM_VERSION_MINOR).arg(TDM_VERSION_PATCH));
    ui->title_text->setAlignment(Qt::AlignCenter);
}
