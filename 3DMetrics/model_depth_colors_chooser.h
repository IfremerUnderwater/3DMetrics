#ifndef MODEL_DEPTH_COLORS_CHOOSER_H
#define MODEL_DEPTH_COLORS_CHOOSER_H

#include <QDialog>

namespace Ui {
class ModelDepthColorsChooser;
}

class ModelDepthColorsChooser : public QDialog
{
    Q_OBJECT

public:
    explicit ModelDepthColorsChooser(QWidget *parent = 0);
    ~ModelDepthColorsChooser();

    // accessors
    double zmin() const
    {
        return m_zmin;
    }

    void setZmin(double zmin)
    {
        m_zmin = zmin;
    }

    double zmax() const
    {
        return m_zmax;
    }

    void setZmax(double zmax)
    {
        m_zmax = zmax;
    }

    double edit_zmin() const
    {
        return m_edit_zmin;
    }

    void setEdit_zmin(double edit_zmin)
    {
        m_edit_zmin = edit_zmin;
    }
    double edit_zmax() const
    {
        return m_edit_zmax;
    }

    void setEdit_zmax(double edit_zmax)
    {
        m_edit_zmax = edit_zmax;
    }


signals:
    void signal_minmaxchanged(double _zmin, double _zmax, bool _useModelsDefault);

public slots:
    void slot_zminvaluchanged();
    void slot_zmaxvaluchanged();
    void slot_zmindefault();
    void slot_zmaxdefault();

    void slot_reset();
    void slot_apply();

private:
    Ui::ModelDepthColorsChooser *ui;

    double m_zmin;
    double m_zmax;

    double m_edit_zmin;
    double m_edit_zmax;
};

#endif // MODEL_DEPTH_COLORS_CHOOSER_H
