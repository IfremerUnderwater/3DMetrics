/*
###############################################################################
#                                                                             #
# The MIT License                                                             #
#                                                                             #
# Copyright (C) 2017 by Juergen Skrotzky (JorgenVikingGod@gmail.com)          #
#               >> https://github.com/Jorgen-VikingGod                        #
#                                                                             #
# Sources: https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle  #
#                                                                             #
###############################################################################
*/

#ifndef TDMMENUBARWINDOW_H
#define TDMMENUBARWINDOW_H

#include <QWidget>
#include "tdmgui.h"
#include <QAction>

namespace Ui {
class TDMMenuBarWindow;
}

class TDMMenuBarWindow : public QWidget {
  Q_OBJECT

 public:
  explicit TDMMenuBarWindow(QWidget *parent = Q_NULLPTR);
  virtual ~TDMMenuBarWindow();
  void setContent(QWidget *w);
  void writeMessage(QString _message);

  QMenuBar* m_menu_bar;
  QMenu* m_file_menu;
  QMenu* m_tools_menu;
  QMenu* m_view_menu;
  QMenu* m_help_menu;

  // File menu actions
  QAction* m_open_3d_model_action;
  //separator
  QAction* m_open_measurement_file_action;
  QAction* m_save_measurement_file_action;
  QAction* m_save_measurement_file_as_action;
  //separator
  QAction* m_open_project_action;
  QAction* m_save_project_action;
  QAction* m_close_project_action;
  //separator
  QAction* m_import_old_measurement_format_action;
  //separator
  QAction* m_quit_action;

  // Tools menu
  QAction* m_export_data_to_csv_action;
  QAction* m_decimate_model_action;
  QAction* m_take_snapshot_action;

  // View menu
  QAction* m_layers_tree_window_action;
  QAction* m_attrib_table_window_action;
  //QAction* m_separator;
  QAction* m_add_axes_action;
  QAction* m_stereo_action;
  QAction* m_light_action;
  //QAction* m_separator;
  QAction* m_z_scale_action;
  //QAction* m_separator;
  QAction* m_depth_colot_chooser_action;
  QAction* m_show_z_scale_action;

  // Help menu
  QAction* m_action_user_manual;
  QAction* m_about_action;

 private:
  bool leftBorderHit(const QPoint &pos);
  bool rightBorderHit(const QPoint &pos);
  bool topBorderHit(const QPoint &pos);
  bool bottomBorderHit(const QPoint &pos);

  TDMGui *m_tdm_gui;

 public slots:
  void setWindowTitle(const QString &text);
  void setWindowIcon(const QIcon &ico);

 private slots:

  // does not follow the code style as they are autoconnected slot and naming is imposed by Qt
  void on_minimize_button_clicked();
  void on_maximize_button_clicked();
  void on_close_button_clicked();
  void on_window_title_bar_doubleClicked();
  // *****************************************************************************************

 protected:
  virtual void changeEvent(QEvent *event);
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void checkBorderDragging(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual bool eventFilter(QObject *obj, QEvent *event);

 private:
  void initMenu();
  void initTitleAndVersion();

  Ui::TDMMenuBarWindow *ui;
  QRect m_StartGeometry;
  const quint8 CONST_DRAG_BORDER_SIZE = 5;
  bool m_mouse_pressed;
  bool m_drag_top;
  bool m_drag_left;
  bool m_drag_right;
  bool m_drag_bottom;

};

#endif  // TDMMENUBARWINDOW_H
