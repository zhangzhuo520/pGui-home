#include <iostream>
#include "oasis_types.h"
#include "oasis_parser.h"
#include "oasis_layout.h"
#include "oasis_out_stream.h"
#include "oasis_exception.h"
#include "gds_parser.h"

#include "config_utilities.h"
#include "system_utilities.h"
#include <boost/filesystem.hpp>

#include "unistd.h"
#include "stdio.h"
#include <sstream>
#include <time.h>
#include <sys/time.h>

using namespace std;
using namespace oasis;

static std::string oas_timestamp(const struct timeval &tv)
{
    struct tm t;
    localtime_r(&tv.tv_sec, &t);
    char buf0[128], buf1[128];
    strftime(buf0, sizeof buf0, "%F %T", &t);
    snprintf(buf1, sizeof buf1, "%s %3d ms", buf0, (int)(tv.tv_usec/1000));
    return buf1;
}

class oas_trace
{
public:
    oas_trace(const std::string& nm = "") : name(nm)
    {
        gettimeofday(&start, 0);
        cout << endl << name << " start " << oas_timestamp(start) << endl;
    }
    ~oas_trace()
    {
        gettimeofday(&end, 0);
        cout << name << " end " << oas_timestamp(end) << endl;
        cout << name << " duration " << ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000 << "ms" << endl;
    }
private:
    std::string name;
    struct timeval start;
    struct timeval end;
};

typedef void (*functype)(sys::ConfigNode);
static std::map<std::string, functype> commands;
static std::map<std::string, OasisLayout*> layouts;

void parsefile(sys::ConfigNode command)
{
    try
    {
        string filename = command["inputfile"].as_string();
        string prepname = command["outputfile"].as_string();
        string format = command["format"].as_string();
        OasisParseOption option;
        if(command["layers"])
        {
            sys::ConfigNode layers = command["layers"];
            for(size_t i = 0; i < layers.size(); ++i)
            {
                int32 layer = layers[i]["layer"].as_int();
                int32 datatype = layers[i]["datatype"].as_int();
                option.add_layer(layer, datatype);
            }
        }

        if (command["parseover"])
        {
            bool parseover = command["parseover"].as_int();
            option.parse_over(parseover);
        }

        if (command["threadcount"])
        {
            int threadcount = command["threadcount"].as_int();
            option.thread_count(threadcount);
        }

        if (command["compress_geometry"])
        {
            bool compress_geometry = command["compress_geometry"].as_int();
            option.compress_geometry(compress_geometry);
        }

        oas_trace tr("parse");
        if(format == "oas")
        {
            OasisParser parser;
            OasisLayout layout(prepname);
            parser.set_parse_option(option);
            parser.open_file(filename);
            parser.parse_file(&layout);
        }
        else if(format == "gds")
        {
            GDSParser parser;
            parser.set_parse_option(option);
            OasisLayout layout(prepname);
            parser.open_file(filename);
            parser.parse_file(&layout);
        }
        else
        {
            cout << "unknown format" << endl;
        }
    }
    catch(OasisException& e)
    {
        cout << e.what() << endl;
    }
}

void loadfile(sys::ConfigNode command)
{
    try
    {
        string filename = command["inputfile"].as_string();
        string handle = command["handlename"].as_string();
        OasisParser parser;
        OasisLayout *layout = new OasisLayout(filename);
        oas_trace tr("load");
        parser.import_file(layout);
        layouts[handle] = layout;
    }
    catch(OasisException& e)
    {
        cout << e.what() << endl;
    }
}

void printhierarchy(sys::ConfigNode command)
{
    try
    {
        string handle = command["handlename"].as_string();
        if(layouts.find(handle) == layouts.end())
        {
            cout << "handle not found" << endl;
            return;
        }
        OasisLayout *layout = layouts[handle];
        layout->build_hierarchy();
        layout->print_hierarchy();
    }
    catch (OasisException& e)
    {
        cout << e.what() << endl;
    }
}

void fetchpolygon(sys::ConfigNode command)
{
    string handle = command["handlename"].as_string();
    string outputfile = command["outputfile"].as_string();
    if(layouts.find(handle) == layouts.end())
    {
        cout << "handle not found" << endl;
        return;
    }
    OasisLayout *layout = layouts[handle];

    double left = command["box"]["left"].as_double();
    double bottom = command["box"]["bottom"].as_double();
    double right = command["box"]["right"].as_double();
    double top = command["box"]["top"].as_double();

    std::vector<LDType> lds;
    if(command["layers"])
    {
        for(size_t i = 0; i < command["layers"].size(); ++i)
        {
            int32 layer = command["layers"][i]["layer"].as_int();
            int32 datatype = command["layers"][i]["datatype"].as_int();
            lds.push_back(LDType(layer, datatype));
        }
    }
    else
    {
        int32 layer = command["layer"].as_int();
        int32 datatype = command["datatype"].as_int();
        lds.push_back(LDType(layer, datatype));
    }
    float64 dbu = layout->get_dbu();
    Box box;
    box.set_left((int64)rint(left / dbu));
    box.set_bottom((int64)rint(bottom / dbu));
    box.set_right((int64)rint(right / dbu));
    box.set_top((int64)rint(top / dbu));


    string topcell = "TOPCELL";
    OasisOutStream oos;
    oos.open_stream(outputfile);
    oos.init_stream(dbu, topcell);

    uint64 refnum = oos.open_cell("cellx");
    for(size_t l = 0; l < lds.size(); ++l)
    {
        std::vector<Polygon> results;
        {
            oas_trace tr("fetch");
            layout->get_polygons(-1, box, OasisTrans(), lds[l], results);
        }

        uint64 numpts = 0;
        for(uint32 i = 0; i < results.size(); ++i)
        {
            Polygon& p = results[i];
            numpts += p.size();
        }
        cout << results.size() << " polygons, " << numpts << " points fetched" << endl;

        for(uint32 i = 0; i < results.size(); ++i)
        {
            oos.add_polygon(refnum, results[i], lds[l].layer, lds[l].datatype);
        }
    }
    oos.add_rectangle(refnum, box, lds[0].layer + 1000, lds[0].datatype);
    oos.close_cell(refnum);

    oos.add_top_placement(refnum, OasisTrans());
    oos.close_stream();
}

void printcontext(sys::ConfigNode command)
{
    string handle = command["handlename"].as_string();
    if(layouts.find(handle) == layouts.end())
    {
        cout << "handle not found" << endl;
        return;
    }
    OasisLayout *layout = layouts[handle];

    Box box;
    if (command["box"])
    {
        double left = command["box"]["left"].as_double();
        double bottom = command["box"]["bottom"].as_double();
        double right = command["box"]["right"].as_double();
        double top = command["box"]["top"].as_double();

        float64 dbu = layout->get_dbu();
        box.set_left((int64)rint(left / dbu));
        box.set_bottom((int64)rint(bottom / dbu));
        box.set_right((int64)rint(right / dbu));
        box.set_top((int64)rint(top / dbu));
    }
    else
    {
        box = Box::world();
    }

    layout->print_context(box, OasisTrans());
}

void printlayers(sys::ConfigNode command)
{
    std::string handle = command.get_string("handlename");
    if(layouts.find(handle) == layouts.end())
    {
        cout << "handle not found" << endl;
        return;
    }
    OasisLayout *layout = layouts[handle];

    layout->print_layers();
}

void loadbyblocks(sys::ConfigNode command)
{
    std::string handle = command.get_string("handlename");
    if(layouts.find(handle) == layouts.end())
    {
        cout << "handle not found" << endl;
        return;
    }
    OasisLayout *layout = layouts[handle];

    int sleep = command.get_int("usleep", 100000);
    int col = command.get_int("col", 10);
    int row = command.get_int("row", 10);
    Box bbox = layout->get_bbox();

    int spanx = bbox.width() / col;
    int spany = bbox.height() / row;
    for (int c = 0; c < col; ++c)
    {
        for (int r = 0; r < row; ++r)
        {
            Box qbox(
                bbox.left()+ spanx * c,
                bbox.bottom()+ spany * r,
                bbox.left()+ spanx * (c + 1),
                bbox.bottom() + spany * (r + 1));
            LDType ld(0, 0);
            std::vector<Polygon> results;
            layout->get_polygons(-1, qbox, OasisTrans(), ld, results);
            std::cout << "load block c " << c << ", r " << r << std::endl;
            cout << "memory " << get_memory_usage(false) << ", " << get_memory_usage(true) << endl;
            usleep(sleep);
        }
    }
}

void loadallcell(sys::ConfigNode command)
{
    std::string handle = command.get_string("handlename");
    if(layouts.find(handle) == layouts.end())
    {
        cout << "handle not found" << endl;
        return;
    }
    OasisLayout *layout = layouts[handle];

    size_t cell_count = layout->get_cell_count();
    for (size_t i = 0; i < cell_count; ++i)
    {
        OasisCellInfo &info = layout->get_cell_info(i);
        OasisCell &cell = layout->get_cell(info.get_cell_index());
        cell.load(*layout);
        cout << "load cell " << i << " memory " << get_memory_usage(false) << ", " << get_memory_usage(true) << endl;
    }
}

void smartclean(sys::ConfigNode command)
{
    std::string handle = command.get_string("handlename");
    if(layouts.find(handle) == layouts.end())
    {
        cout << "handle not found" << endl;
        return;
    }
    OasisLayout *layout = layouts[handle];

    int sleep = command.get_double("usleep", 1000000);
    int cycle = command.get_int("cycle", 3);
    int expire = command.get_int("expire", 8);
    int cache = command.get_int("cache", 4 * 1024 * 1024);

    for (int i = 0; i < 10; ++i)
    {
        cout << "before smart clean memory " << get_memory_usage(false) << ", " << get_memory_usage(true) << endl;
        layout->smart_clean_cache(cache, expire, cycle);
        cout << "after smart clean memory " << get_memory_usage(false) << ", " << get_memory_usage(true) << endl;
        usleep(sleep);
    }
}

void deletefile(sys::ConfigNode command)
{
    std::string handle = command.get_string("handlename");
    if(layouts.find(handle) == layouts.end())
    {
        cout << "handle not found" << endl;
        return;
    }
    OasisLayout *layout = layouts[handle];

    cout << "before memory " << get_memory_usage(false) << ", " << get_memory_usage(true) << endl;
    delete layout;
    layouts.erase(handle);
    cout << "after memory " << get_memory_usage(false) << ", " << get_memory_usage(true) << endl;
}

void cleancache(sys::ConfigNode command)
{
    string handle = command["handlename"].as_string();
    if(layouts.find(handle) == layouts.end())
    {
        cout << "not found" << endl;
        return;
    }
    OasisLayout *layout = layouts[handle];
    oas_trace tr("clean");
    layout->unload();
}

void arrayfile(sys::ConfigNode command)
{
    try
    {
        oas_trace tr("arrayfile");
        std::string inputfile = command.get_string("inputfile");
        std::string outputfile = command.get_string("outputfile");
        std::string topcell = command.get_string("topcell");
        int rows = command.get_int("rows");
        int cols = command.get_int("cols");
        double pitch_x = command.get_double("pitch_x");
        double pitch_y = command.get_double("pitch_y");
        double center_x = command.get_double("center_x", 0.0);
        double center_y = command.get_double("center_y", 0.0);

        OasisParser parser;
        OasisLayout *layout = new OasisLayout(inputfile);
        parser.import_file(layout);

        double dbu = layout->get_dbu();
        Box bbox = layout->get_bbox();

        int64 px = (int64)rint(pitch_x / dbu);
        int64 py = (int64)rint(pitch_y / dbu);
        int64 cx = (int64)rint(center_x / dbu);
        int64 cy = (int64)rint(center_y / dbu);

        int64 ox = cx - bbox.left() - ((cols - 1) * px + bbox.width()) / 2;
        int64 oy = cy - bbox.bottom() - ((rows - 1) * py + bbox.height()) / 2;

        Point col_vector(px, 0);
        Point row_vector(0, py);
        Point origin(ox, oy);

        layout->array_layout(topcell, origin, rows, cols, row_vector, col_vector);

        OasisWriter writer;
        FILE* fp = fopen(outputfile.c_str(), "w");
        writer.set_file_offset(fp, 0);
        layout->export_file(writer);
        fclose(fp);

        delete layout;
    }
    catch(OasisException& e)
    {
        cout << e.what() << endl;
    }
}

int run_command(sys::ConfigNode command)
{
    std::string cname = command["operation"].as_string();
    if(commands.find(cname) == commands.end())
    {
        cout << "command " << command.to_string() << " not found" << endl;
        return -1;
    }
    commands[cname](command);
    return 0;
}

int flow(int argc, char* argv[])
{
    commands["parsefile"] = &parsefile;
    commands["loadfile"] = &loadfile;
    commands["printhierarchy"] = &printhierarchy;
    commands["fetchpolygon"] = &fetchpolygon;
    commands["cleancache"] = &cleancache;
    commands["printcontext"] = &printcontext;
    commands["arrayfile"] = &arrayfile;
    commands["printlayers"] = &printlayers;
    commands["loadbyblocks"] = &loadbyblocks;
    commands["smartclean"] = &smartclean;
    commands["deletefile"] = &deletefile;
    commands["loadallcell"] = &loadallcell;

    std::string cf(argv[1]);
    sys::ConfigNode config;
    config.load_file(cf);
    if(config["option"])
    {
        if(config["option"]["debuglevel"])
        {
            OasisDebug::loglevel(config["option"]["debuglevel"].as_int());
        }
    }
    sys::ConfigNode cmds = config["flow"];
    for(size_t i = 0; i < cmds.size(); ++i)
    {
        sys::ConfigNode cmd = cmds[i];
        if(run_command(cmd) < 0)
        {
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    return flow(argc, argv);
}
