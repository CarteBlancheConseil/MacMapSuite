# MacMapSuite
MacMapSuite is an Xcode project containing C utilities and C++ classes to :
- read and write MacMap's v3 file format
- read and write various file formats (DBF, Shapefile, Mif/Tab, text, DXF, NMEA, ...)
- manage MacMap's geometries
- perform web requests (WMS, tiling)
- manage projections

It embeds some of the most famous GIS libraries :
- PROJ-4
- MITAB (including OGR and CPL)
- ShapeLib
- GEOS
- Libpq (from Postgres)

All binaries, libriries, includes, etc... are available in the "unix" folder of the compiled framework.

**Installation location :**
Default location for MacMap is "/Library/Framework", but like any Mac OS X framework, you can copy MacMapSuite.framework to any standard location. (See https://developer.apple.com/library/mac/documentation/MacOSX/Conceptual/BPFrameworks/Tasks/InstallingFrameworks.html for more information).

