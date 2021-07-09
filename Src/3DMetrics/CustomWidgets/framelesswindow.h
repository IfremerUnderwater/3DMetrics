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

#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QWidget>
#include "tdmgui.h"

namespace Ui {
class FramelessWindow;
}

class FramelessWindow : public QWidget {
  Q_OBJECT

 public:
  explicit FramelessWindow(QWidget *parent = Q_NULLPTR);
  virtual ~FramelessWindow();
  void setContent(QWidget *w);

 private:
  bool leftBorderHit(const QPoint &pos);
  bool rightBorderHit(const QPoint &pos);
  bool topBorderHit(const QPoint &pos);
  bool bottomBorderHit(const QPoint &pos);

  QMenuBar* m_menu_bar;
  QMenu *m_file_menu;

  TDMGui m_tdm_gui;

 public slots:
  void setWindowTitle(const QString &text);
  void setWindowIcon(const QIcon &ico);

 private slots:

  // does not follow the code style as they are autoconnected slot and naming is imposed by Qt
  void on_minimize_button_clicked();
  void on_restore_button_clicked();
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
  Ui::FramelessWindow *ui;
  QRect m_StartGeometry;
  const quint8 CONST_DRAG_BORDER_SIZE = 15;
  bool m_mouse_pressed;
  bool m_drag_top;
  bool m_drag_left;
  bool m_drag_right;
  bool m_drag_bottom;
};

#endif  // FRAMELESSWINDOW_H
