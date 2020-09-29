#ifndef SMARTLOD_H
#define SMARTLOD_H

#include <osg/LOD>

class  SmartLOD : public osg::LOD
{
public:
    // extensions for SmartLOD
    static constexpr const char *const EXTLOD0 = "-0.osgb";
    static constexpr const char *const EXTLOD1 = "-1.osgb";
    static constexpr const char *const EXTLOD2 = "-2.osgb";

    SmartLOD();

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    SmartLOD(const SmartLOD&,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    META_Node(osg, SmartLOD)

    virtual void traverse(osg::NodeVisitor& nv);

    virtual bool addChild(Node *child);

    virtual bool addChild(Node *child, float rmin, float rmax);

    virtual bool addChild(const std::string& filename, float rmin, float rmax);

    template<class T> bool addChild( const osg::ref_ptr<T>& child, float rmin, float rmax) { return addChild(child.get(), rmin, rmax); }

    virtual bool removeChildren(unsigned int pos,unsigned int numChildrenToRemove=1);

    void doNotDiscardChild(int _pos, bool _state = true);

    struct  PerRangeData
    {
        PerRangeData();
        PerRangeData(const PerRangeData& prd);
        PerRangeData& operator = (const PerRangeData& prd);

        std::string                     _filename;
        bool                            _nodeLoaded;
        bool                            _doNotDiscard;
    };

    typedef std::vector<PerRangeData> PerRangeDataList;

    void setFileName(unsigned int childNo, const std::string& filename) { expandPerRangeDataTo(childNo); m_perRangeDataList[childNo]._filename=filename; }
    const std::string& getFileName(unsigned int childNo) const { return m_perRangeDataList[childNo]._filename; }
    unsigned int getNumFileNames() const { return static_cast<unsigned int>(m_perRangeDataList.size()); }


    /** Set the optional database osgDB::Options object to use when reading children.*/
    void setDatabaseOptions(osg::Referenced* options) { m_databaseOptions = options; }

    /** Get the optional database osgDB::Options object used when reading children.*/
    osg::Referenced* getDatabaseOptions() { return m_databaseOptions.get(); }

    /** Get the optional database osgDB::Options object used when reading children.*/
    const osg::Referenced* getDatabaseOptions() const { return m_databaseOptions.get(); }

    static bool hasLODFiles( const std::string& pathToFile);

protected :

    virtual ~SmartLOD();

    void expandPerRangeDataTo(unsigned int pos);

    osg::ref_ptr<Referenced> m_databaseOptions;

    PerRangeDataList    m_perRangeDataList;
    int                 m_lastIndex;

};

#endif // SMARTLOD_H
