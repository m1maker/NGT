//Cartertemm's BGTPackFile. Edeted  and rewritten in C++ by M_maker
#include "pack.h"
#include<stdio.h>
#include <iostream>
#include<sstream>
#include <fstream>
#include <vector>
#include "scriptarray/scriptarray.h"
#include "scriptfile/scriptfile.h"
//Constants:
const int PF_CLOSED = -1;
const int PF_READ = 0;
const int PF_CREATE = 1;
//Sum functions:
std::vector<char> read_buffered(std::ifstream& f, int buffsize, int until = 0) {
    /*
    Generator that reads a file in chunks.
    This is used in an attempt at memory efficiency, in addition to combatting the classic MemoryError when attempting to read an entire file at once.
    */
    std::vector<char> buffer(buffsize);
    int begin_offset = f.tellg();
    int bytes_read = 0;
    if (until > 0 && buffsize > until) {
        buffsize = until;
    }
    std::vector<char> data;
    while (true) {
        f.read(buffer.data(), buffsize);
        int bytes_read = static_cast<int>(f.tellg()) - begin_offset;
        if (!f) { //we've reached the end of the file
            break;
        }
        if (bytes_read == until) {
            if (!data.empty()) {
                data.insert(data.end(), buffer.begin(), buffer.begin() + f.gcount());
            }
            break;
        }
        else if (until > 0 && bytes_read + buffsize > until) {
            buffsize = until - bytes_read;
        }
        data.insert(data.end(), buffer.begin(), buffer.begin() + f.gcount());
    }
    return data;
}

class bad_pack: public std::exception {
private:
    std::string reason;
public:
    bad_pack(const std::string& reason) : reason(reason) {}
    const char* what() const noexcept override {
        return reason.c_str();
    }
};

//A container for attributes given to every file in the pack
//Only used when opened for reading
class file{
public:
    std::string name;
    int start_offset;
    int content_length;
    file(const std::string& name, int content_length, int start_offset = 0)
        : name(name), start_offset(start_offset), content_length(content_length) {}
};

class pack{
private:
    const std::string header = "SFPv";
    const int version = 1;
    std::string name;
    std::fstream fileobj;
    std::vector<file> files;
    int mode = PF_CLOSED;

    int get_files_added() {
        int orig_pos = fileobj.tellg();
        fileobj.seekg(8);
        int data = 0;
        fileobj >> data;
        fileobj.seekg(orig_pos);
        return data;
    }

    void set_files_added(int num) {
        int orig_pos = fileobj.tellp();
        fileobj.seekp(8);
        fileobj << num;
        fileobj.seekp(orig_pos);
    }
    file* _get_fileobj(const std::string& filename) {
        file* found = nullptr;
        for (auto& file : files) {
            if (file.name == filename) {
                found = &file;
                break;
            }
        }
        return found;
    }
public:
    void create(const std::string& filename) {
        /*
        Create a pack file and open it in write mode.
        If the file already exists, it will be overwritten.
        */
        if (active()) {
            close();
        }
        fileobj.open(filename, std::ios::binary | std::ios::out);
        mode = PF_CREATE;
        fileobj.write(header.c_str(), header.size());
        //Write dummy info for now, we'll touch it later
        int dummy_info[3] = { version, 0, 0 };
        fileobj.write(reinterpret_cast<const char*>(dummy_info), sizeof(dummy_info));
        name = filename;
    }

    bool add_file(const std::string& file_on_disk, const std::string& internal_name) {
        /*
        Adds a file to the currently open pack file.
        The pack file must be opened in write mode with the create method for this operation to be successful.
        Internal_name is simply used to refer to the file inside the pack. This can contain any characters and can be laid out in any format.
        */
        if (mode != PF_CREATE) {
            return false;
        }
        std::ifstream f(file_on_disk, std::ios::binary);
        f.seekg(0, std::ios::end);
        int size = f.tellg();
        f.seekg(0);
        //Put pointer to the end if we're not already there
        fileobj.seekp(8);
        fileobj.seekp(0, std::ios::end);
        fileobj.write(header.c_str(), header.size());
                int file_info[3] = { static_cast<int>(internal_name.size()), 0, size };
        fileobj.write(reinterpret_cast<const char*>(file_info), sizeof(file_info));
        fileobj.write(internal_name.c_str(), internal_name.size());
        //If size is bigger than 50MB, read in a buffer stream.
        if (size > (1024 * 1024) * 50) {
            //Read in 20MB increments
            std::vector<char> buffer((1024 * 1024) * 20);
            while (size > 0) {
                int chunk_size = std::min(size, static_cast<int>(buffer.size()));
                f.read(buffer.data(), chunk_size);
                fileobj.write(buffer.data(), chunk_size);
                size -= chunk_size;
            }
        }
        else {
            fileobj << f.rdbuf();
        }
        return true;
    }

    void open(const std::string& filename) {
        /*
        Open an existing pack file for reading.
        The pack file will be opened in read mode. Please note that a pack file cannot be added to once closed unless it is recreated from scratch.
        Returns True on success, False and raises an exception on failure.
        */
        if (active()) {
            close();
        }
        fileobj.open(filename, std::ios::binary | std::ios::in);
        mode = PF_READ;
        char header_buffer[4];
        fileobj.read(header_buffer, sizeof(header_buffer));
        if (std::string(header_buffer, sizeof(header_buffer)) != header) {
            throw bad_pack("Invalid header");
        }
        int version, num_files, something;
        fileobj.read(reinterpret_cast<char*>(&version), sizeof(version));
        fileobj.read(reinterpret_cast<char*>(&num_files), sizeof(num_files));
        fileobj.read(reinterpret_cast<char*>(&something), sizeof(something));
        for (int i = 0; i < num_files; i++) {
            char file_header[4];
            fileobj.read(file_header, sizeof(file_header));
            if (std::string(file_header, sizeof(file_header)) != header) {
                throw bad_pack("Abrupt ending to data");
            }
            int name_length, content_length;
            fileobj.read(reinterpret_cast<char*>(&name_length), sizeof(name_length));
            fileobj.read(reinterpret_cast<char*>(&something), sizeof(something));
            fileobj.read(reinterpret_cast<char*>(&content_length), sizeof(content_length));
            std::vector<char> name_buffer(name_length);
            fileobj.read(name_buffer.data(), name_length);
            files.emplace_back(std::string(name_buffer.data(), name_length), content_length, fileobj.tellg());
            fileobj.seekg(static_cast<int>(fileobj.tellg()) + content_length);
        }
        name = filename;
    }

    CScriptArray* list_files() {
        /*
        List all files in the pack.
        Although internal names can take the form of directory formatting, the pack format cannot differentiate between directories and files. Therefore if you have an internal name of music/level1.ogg, that is what will appear in the relevant entry in the list.
        Files will be listed in the order they were added to the pack file.
        This method can only be used on a pack opened for reading.
        */
        if (mode != PF_READ) {
            return {};
        }
        asIScriptContext* ctx = asGetActiveContext();
        asIScriptEngine* engine = ctx->GetEngine();
        asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<string>"));
        CScriptArray* file_list= CScriptArray::Create(arrayType, (asUINT)0);

        for (int i = 0; i < files.size();i++) {
            file_list->Resize(file_list->GetSize() + 1);
            ((std::string*)file_list->At(i))->assign(files[i].name);

        }
        return file_list;
    }

    std::string get_file(const std::string& internal_name) {
        /*
        Passes data from a file inside a pack to a standard string. This allows a file to be read in chunks from inside the pack without the need to extract it to disk. The file object does not contain a copy of the data, it reads from the original pack but only the relevant section of the file, so that you need not worry about accessing other data in the pack by accident.
        This method can only be used on a pack opened for reading.
        */
        if (mode != PF_READ) {
            return "";
        }
        //Can we find the given internal_name in our pack?
        file* found = _get_fileobj(internal_name);
        if (!found) {
            return "";
        }
        fileobj.seekg(found->start_offset);
        std::stringstream ss;
        ss << fileobj.rdbuf();
        std::string buffer;
buffer=ss.str();
        return buffer;
    }

    bool file_exists(const std::string& filename) {
        /*
        Checks whether a given file exists in the pack.
        Only works in instances opened for reading.
        */
        if (mode != PF_READ) {
            return false;
        }
        return _get_fileobj(filename) != nullptr;
    }

    bool extract_file(const std::string& internal_name, const std::string& file_on_disk) {
        /*
        Extract a file from an open pack file back onto disk.
        Only works on a pack file opened for reading.
        */
        if (mode != PF_READ) {
            return false;
        }
        file* found = _get_fileobj(internal_name);
        if (!found) {
            return false;
        }
        std::ofstream f(file_on_disk, std::ios::binary);
        fileobj.seekg(found->start_offset);
        //If size is bigger than 50MB, read in a buffer stream.
        if (found->content_length > (1024 * 1024) * 50) {
            //Read in 20MB increments
            std::vector<char> buffer((1024 * 1024) * 20);
            while (found->content_length > 0) {
                int chunk_size = std::min(found->content_length, static_cast<int>(buffer.size()));
                fileobj.read(buffer.data(), chunk_size);
                f.write(buffer.data(), chunk_size);
                found->content_length -= chunk_size;
            }
        }
        else {
            f << fileobj.rdbuf();
        }
        return true;
    }

    bool active() const {
        /*Set to True when a pack file is associated with this instance, False otherwise.*/
        return fileobj.is_open();
    }

    int file_count() const {
        /*Returns the number of files stored in the currently open pack, or -1 on error.*/
        if (mode != PF_READ) {
            return -1;
        }
        return files.size();
    }

    void close() {
        /*
        Close any pack file associated with an instance of this class.
        Content is written if opened for writing.
        */
        if (active()) {
            fileobj.close();
            fileobj.clear();
          mode = PF_CLOSED;
        }
        name = "";
    }

    ~pack() {
        close();
    }
};
pack* fpack() { return new pack; }
void register_pack(asIScriptEngine* engine) {
    engine->RegisterObjectType("pack", sizeof(pack), asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectBehaviour("pack", asBEHAVE_FACTORY, "pack@ p()", asFUNCTION(fpack), asCALL_CDECL);
    engine->RegisterObjectMethod("pack", "void open(const string &in)const", asMETHOD(pack, open), asCALL_THISCALL);
    engine->RegisterObjectMethod("pack", "void create(const string &in)const", asMETHOD(pack, create), asCALL_THISCALL);
    engine->RegisterObjectMethod("pack", "void close()const", asMETHOD(pack, close), asCALL_THISCALL);
    engine->RegisterObjectMethod("pack", "bool file_exists(const string &in)const", asMETHOD(pack, file_exists), asCALL_THISCALL);
    engine->RegisterObjectMethod("pack", "void extract_file(const string &in, const string &in)const", asMETHOD(pack, extract_file), asCALL_THISCALL);
    engine->RegisterObjectMethod("pack", "array<string>@ list_files()const", asMETHOD(pack, list_files), asCALL_THISCALL);
    engine->RegisterObjectMethod("pack", "void add_file(const string &in, const string &in)const", asMETHOD(pack, add_file), asCALL_THISCALL);
    engine->RegisterObjectMethod("pack", "string get_file(const string &in)const", asMETHOD(pack, get_file), asCALL_THISCALL);
    engine->RegisterObjectMethod("pack", "int get_file_count()const property", asMETHOD(pack, file_count), asCALL_THISCALL);

}
