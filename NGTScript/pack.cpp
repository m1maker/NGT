#include "pack.h"
#include <string>
#include "scriptarray/scriptarray.h"
using namespace std;
typedef unsigned int uint;
struct ngt_pack_header{
    const char* header = "NGTDATA";
    long e_magic[4264];
    long e_cblp[1];
    long e_cp[2];
    long e_crlc[4];
    long e_cparhdr[8];
    long e_minalloc;
    long e_maxalloc;
    long e_ss;
    long e_sp;
    long e_csum;
    long e_ip;
    long e_cs;
    long e_lfarlc;
    long e_ovno;
    long e_res[4];
    long e_oemid;
    long e_oeminfo;
    long e_res2[10];
    long   e_lfanew;
};

class Pack
{
public:
    bool open(const string& file_name, uint mode, bool create = false) const;
    bool close() const;
    bool add_file(const string& file_name, const string& destination_path, bool overwrite = false) const;
    bool add_memory(const string& data, const string& destination_path, bool overwrite = false) const;
    bool delete_file(const string& file_name) const;
    bool file_exists(const string& file_name) const;
    string get_file_name(int index) const;
    CScriptArray* list_files() const;
    uint get_file_size(const string& file_name) const;
    uint get_file_offset(const string& file_name) const;
    string read_file(const string& file_name, uint offset, uint size) const;
    bool raw_seek(int position) const;
    bool stream_close(uint stream_id) const;
    uint stream_open(const string& file_name, uint mode) const;
    string stream_read(uint stream_id, uint size) const;
    uint stream_pos(uint stream_id) const;
    uint stream_seek(uint stream_id, uint offset, int origin) const;
    uint stream_size(uint stream_id) const;

    // Properties
    bool get_active() ;
    uint get_size() ;
};