#ifndef LOADING_MODE_H
#define LOADING_MODE_H
enum LoadingMode {
    // default (not for GRD files)
    // no processing (project file)
    LoadingModeDefault = 0,

    // grid files only
    LoadingModePoint = 11,
    LoadingModeTriangle = 12,
    // default for GRD file if no tiles present
    LoadingModeTriangleNormals = 13,
    LoadingModeTrianglePoint = 14,

    // LOD (no tiles) for obj files only
    LoadingModeUseSmartLOD = 21,
    LoadingModeBuildAndUseSmartLOD = 22,
    LoadingModeBuildLOD = 23,
    // USE : mode default (with osgb file)
    // could be LOD
    LoadingModeUseOSGB = 24,
    LoadingModeBuildOSGB = 25,

    // Tiles
    LoadingModeLODTiles = 31,
    LoadingModeLODTilesDir = 32,

    LoadingModeSmartLODTiles = 33,
    LoadingModeSmartLODTilesDir = 34,

    // Build tiles (could be slow)
    // (in current directory)
    LoadingModeBuildLODTiles = 40,

    // Only build tiles (for Grid Files)
    LoadingModeBuildTiles = 50

};

#endif // LOADING_MODE_H

