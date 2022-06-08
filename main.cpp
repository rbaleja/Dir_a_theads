#include <QCoreApplication>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <thread>

using namespace std;
namespace fs = std::filesystem;

char type;
char* ptr_type = &type;

string permision;
string* ptr_permision = &permision;


static tuple<fs::path, fs::file_status, size_t> file_info(const fs::directory_entry &entry)
{
    const auto fs (status(entry));
    return {entry.path(), fs, is_regular_file(fs) ? file_size(entry.path()) : 0u};

}


void type_char(fs::file_status fs)
    {
        if      (is_directory(fs))
            { *ptr_type = 'D'; }
        else if (is_symlink(fs))
            { *ptr_type = 'L'; }
        else if (is_character_file(fs))
            { *ptr_type = 'C'; }
        else if (is_block_file(fs))
            { *ptr_type = 'B'; }
        else if (is_fifo(fs))
            { *ptr_type = 'P'; }
        else if (is_socket(fs))
            { *ptr_type = 'S'; }
        else if (is_other(fs))
            { *ptr_type = 'O'; }
        else if (is_regular_file(fs))
            { *ptr_type = 'F'; }
    }


void rwx(fs::perms p)
{
    char perm[10];
    auto check ([p](fs::perms bit, char c) { return (p & bit) == fs::perms::none ? '-' : c; });


       perm[0] = check(fs::perms::owner_read,   'r');
       perm[1] = check(fs::perms::owner_write,  'w');
       perm[2] = check(fs::perms::owner_exec,   'x');
       perm[3] = check(fs::perms::group_read,   'r');
       perm[4] = check(fs::perms::group_write,  'w');
       perm[5] = check(fs::perms::group_exec,   'x');
       perm[6] = check(fs::perms::others_read,  'r');
       perm[7] = check(fs::perms::others_write, 'w');
       perm[8] = check(fs::perms::others_exec,  'x');


      string permi(perm);
      *ptr_permision = permi;


}


string size_string(size_t size)
{

    stringstream ss;

    if      (size >= 1000000000)
        { ss << (size / 1000000000) << 'G'; }
    else if (size >= 1000000)
        { ss << (size / 1000000)    << 'M'; }
    else if (size >= 1000)
        { ss << (size / 1000)       << 'K'; }
    else if (size > 1 && size < 1000)
        { ss << size                << 'B'; }
    else if (size == 0)
        { ss << '-' << '-'; }
    else { ss << size << '?'; }

    return ss.str();

}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    fs::path dir = {argc > 1 ? argv[1] : "."};

        if (!exists(dir))
        {
            cout << "Ścieżka dostępu " << dir << " nie istnieje.\n";
            return 1;
        }

        else
        {
            vector<tuple<fs::path, fs::file_status, size_t>> items;

            transform(fs::directory_iterator{dir}, {}, back_inserter(items), file_info);

            for (const auto &[path, status, size]: items)
            {
                std::thread thd1 (type_char, status);
                std::thread thd2 (rwx, status.permissions());

                thd1.join();
                thd2.join();

                cout << "Typ " << type << " Uprawnienia " << permision << " Rozmiar "
                     << setw(4) << right << size_string(size) << " Nazwa "
                     << path.filename().string() << '\n';
            }


        }

    return a.exec();
}
