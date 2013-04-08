/**
 * @file
 * @brief Level markers (annotations).
**/

#ifndef __MAPMARK_H__
#define __MAPMARK_H__

#include "dgnevent.h"
#include "clua.h"
#include <map>
#include <string>
#include <vector>
#include <memory>

//////////////////////////////////////////////////////////////////////////
// Map markers

class reader;
class writer;

void remove_markers_and_listeners_at(coord_def p);

bool marker_vetoes_operation(const char *op);
coord_def find_marker_position_by_prop(const string &prop,
                                       const string &expected = "");
vector<coord_def> find_marker_positions_by_prop(const string &prop,
                                                const string &expected = "",
                                                unsigned maxresults = 0);
vector<map_marker*> find_markers_by_prop(const string &prop,
                                         const string &expected = "",
                                         unsigned maxresults = 0);

class map_marker
{
public:
    map_marker(map_marker_type type, const coord_def &pos);
    virtual ~map_marker();

    map_marker_type get_type() const { return type; }

    virtual map_marker *clone() const = 0;
    virtual void activate(bool verbose = true);
    virtual void write(writer &) const;
    virtual void read(reader &);
    virtual string debug_describe() const = 0;
    virtual string property(const string &pname) const;

    static map_marker *read_marker(reader &);
    static map_marker *parse_marker(const string &text,
                                    const string &ctx = "") throw (string);

public:
    coord_def pos;

protected:
    map_marker_type type;

    typedef map_marker *(*marker_reader)(reader &, map_marker_type);
    typedef map_marker *(*marker_parser)(const string &, const string &);
    static marker_reader readers[NUM_MAP_MARKER_TYPES];
    static marker_parser parsers[NUM_MAP_MARKER_TYPES];
};

class map_feature_marker : public map_marker
{
public:
    map_feature_marker(const coord_def &pos = coord_def(0, 0),
                       dungeon_feature_type feat = DNGN_UNSEEN);
    map_feature_marker(const map_feature_marker &other);
    void write(writer &) const;
    void read(reader &);
    string debug_describe() const;
    map_marker *clone() const;
    static map_marker *read(reader &, map_marker_type);
    static map_marker *parse(const string &s, const string &) throw (string);

public:
    dungeon_feature_type feat;
};

class map_corruption_marker : public map_marker
{
public:
    map_corruption_marker(const coord_def &pos = coord_def(0, 0),
                          int dur = 0);

    void write(writer &) const;
    void read(reader &);
    map_marker *clone() const;
    string debug_describe() const;

    static map_marker *read(reader &, map_marker_type);

public:
    int duration;
};

class map_tomb_marker : public map_marker
{
public:
    map_tomb_marker(const coord_def& pos = coord_def(0, 0),
                    int dur = 0, int src = 0, int targ = 0);

    void write(writer &) const;
    void read(reader &);
    map_marker *clone() const;
    string debug_describe() const;

    static map_marker *read(reader &, map_marker_type);

public:
    int duration, source, target;
};

class map_malign_gateway_marker : public map_marker
{
public:
    map_malign_gateway_marker (const coord_def& pos = coord_def(0, 0),
                    int dur = 0, bool ip = false, string caster = "",
                    beh_type bh = BEH_HOSTILE, god_type gd = GOD_NO_GOD,
                    int pow = 0);

    void write (writer &) const;
    void read (reader &);
    map_marker *clone() const;
    string debug_describe() const;

    static map_marker *read(reader &, map_marker_type);

public:
    int duration;
    bool is_player;
    bool monster_summoned;
    string summoner_string;
    beh_type behaviour;
    god_type god;
    int power;
};

// A marker powered by phoenixes!
class map_phoenix_marker : public map_marker
{
public:
    map_phoenix_marker (const coord_def& pos = coord_def(0, 0),
                    int dur = 0, int mnum = 0, beh_type bh = BEH_HOSTILE,
                    mon_attitude_type at = ATT_HOSTILE, god_type gd = GOD_NO_GOD,
                    coord_def cp = coord_def(-1, -1)
                    );

    void write (writer &) const;
    void read (reader &);
    map_marker *clone() const;
    string debug_describe() const;

    static map_marker *read(reader &, map_marker_type);

public:
    int duration;
    int mon_num;
    beh_type behaviour;
    mon_attitude_type attitude;
    god_type god;
    coord_def corpse_pos;
};

// A marker for sealed doors
class map_door_seal_marker : public map_marker
{
public:
    map_door_seal_marker (const coord_def& pos = coord_def(0, 0),
                    int dur = 0, int mnum = 0,
                    dungeon_feature_type oldfeat = DNGN_CLOSED_DOOR);

    void write (writer &) const;
    void read (reader &);
    map_marker *clone() const;
    string debug_describe() const;

    static map_marker *read(reader &, map_marker_type);

public:
    int duration;
    int mon_num;
    dungeon_feature_type old_feature;
};

// A marker for temporary terrain changes
class map_terrain_change_marker : public map_marker
{
public:
    map_terrain_change_marker (const coord_def& pos = coord_def(0, 0),
                    dungeon_feature_type oldfeat = DNGN_FLOOR,
                    dungeon_feature_type newfeat = DNGN_FLOOR,
                    int dur = 0, terrain_change_type type = TERRAIN_CHANGE_GENERIC,
                    int mnum = 0);

    void write (writer &) const;
    void read (reader &);
    map_marker *clone() const;
    string debug_describe() const;

    static map_marker *read(reader &, map_marker_type);

public:
    int duration;
    int mon_num;
    dungeon_feature_type old_feature;
    dungeon_feature_type new_feature;
    terrain_change_type  change_type;
};

// A marker powered by Lua.
class map_lua_marker : public map_marker, public dgn_event_listener
{
public:
    map_lua_marker();
    map_lua_marker(const lua_datum &function);
    map_lua_marker(const string &s, const string &ctx,
                   bool mapdef_marker = true);
    ~map_lua_marker();

    void activate(bool verbose);

    void write(writer &) const;
    void read(reader &);
    map_marker *clone() const;
    string debug_describe() const;
    string property(const string &pname) const;

    bool notify_dgn_event(const dgn_event &e);

    static map_marker *read(reader &, map_marker_type);
    static map_marker *parse(const string &s, const string &) throw (string);

    string debug_to_string() const;
private:
    bool initialised;
    unique_ptr<lua_datum> marker_table;

private:
    void check_register_table();
    bool get_table() const;
    void push_fn_args(const char *fn) const;
    bool callfn(const char *fn, bool warn_err = false, int args = -1) const;
    string call_str_fn(const char *fn) const;
};

class map_wiz_props_marker : public map_marker
{
public:
    map_wiz_props_marker(const coord_def &pos = coord_def(0, 0));
    map_wiz_props_marker(const map_wiz_props_marker &other);
    void write(writer &) const;
    void read(reader &);
    string debug_describe() const;
    string property(const string &pname) const;
    string set_property(const string &key, const string &val);
    map_marker *clone() const;
    static map_marker *read(reader &, map_marker_type);
    static map_marker *parse(const string &s, const string &) throw (string);

public:
    map<string, string> properties;
};

class map_position_marker : public map_marker
{
public:
    map_position_marker(const coord_def &pos = coord_def(0, 0),
                        const coord_def _dest = INVALID_COORD);
    map_position_marker(const map_position_marker &other);
    void write(writer &) const;
    void read(reader &);
    string debug_describe() const;
    map_marker *clone() const;
    static map_marker *read(reader &, map_marker_type);

public:
    coord_def dest;
};

#endif
