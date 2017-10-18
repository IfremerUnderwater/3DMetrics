TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = Libraries \
          3DMetrics

3DMetrics.depends = Libraries

OTHER_FILES += .qmake.conf
OTHER_FILES += ./Scripts/Deploy_Release.pro \
               ./Scripts/Deploy_Debug.pro
