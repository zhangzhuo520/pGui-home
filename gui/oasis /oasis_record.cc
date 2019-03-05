#include "oasis_record.h"

using namespace std;
using namespace oasis;

const char* oasis_record_names[] =
{
    "Pad",
    "Start",
    "End",
    "Cellname",
    "CellnameRef",
    "Textstring",
    "TextstringRef",
    "Propname",
    "PropnameRef",
    "Propstring",
    "PropstringRef",
    "Layername",
    "LayernameText",
    "CellRef",
    "Cell",
    "XYAbsolute",
    "XYRelative",
    "Placement",
    "PlacementTrans",
    "Text",
    "Rectangle",
    "Polygon",
    "Path",
    "Trapezoid",
    "TrapezoidA",
    "TrapezoidB",
    "CTrapezoid",
    "Circle",
    "Property",
    "PropertyLast",
    "XName",
    "XNameRef",
    "XElement",
    "XGeometry",
    "Cblock"
};

string oasis::oasis_record_id_name(const OasisRecordId& rid)
{
    oas_assert(rid < ORID_Max);
    return oasis_record_names[rid];
}

const char* gds_record_names[] =
{
    "Header",
    "BgnLib",
    "LibName",
    "Units",
    "EndLib",
    "BgnStr",
    "StrName",
    "EndStr",
    "Boundary",
    "Path",
    "Sref",
    "Aref",
    "Text",
    "Layer",
    "Datatype",
    "Width",
    "XY",
    "EndEl",
    "Sname",
    "ColRow",
    "TextNode",
    "Node",
    "Texttype",
    "Presentation",
    "Spacing",
    "String",
    "Strans",
    "Mag",
    "Aangle",
    "UInteger",
    "UString",
    "RefLibs",
    "Fonts",
    "Pathtype",
    "Generations",
    "AttrTable",
    "StypTable",
    "StrType",
    "Eflags",
    "ElKey",
    "LinkType",
    "LinkKeys",
    "Nodetype",
    "PropAttr",
    "PropValue",
    "Box",
    "Boxtype",
    "Plex",
    "BgnExtn",
    "EndExtn",
    "TapeNum",
    "TapeCode",
    "StrClass",
    "Reserved1",
    "Reserved2",
    "Mask",
    "EndMasks",
    "LibDirSize",
    "SrfName",
    "LibSecur"
};

string oasis::gds_record_id_name(const GDSRecordId& rid)
{
    uint16 id = rid;
    id >>= 8;
    oas_assert(id < GRID_Max);
    return gds_record_names[id];
}

