#include <list>
#include <ctime>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unordered_map>

using namespace std;

string curr_time();

class TreeNode
{
public:
    string name;
    list<string> contents;
    char type;
    string cdate;
    string mdate;
    int permission;
    TreeNode *parent;
    TreeNode *link;
    TreeNode *child;

    TreeNode(TreeNode *pwd, const std::string &name)
        : name(name), parent(pwd), permission(6),
          cdate(curr_time()), mdate(curr_time()),
          link(nullptr), child(nullptr) {}

    std::string get_permission() const
    {
        static const std::unordered_map<int, std::string> permissions = {
            {0, "---"}, {1, "--x"}, {2, "-w-"}, {3, "-wx"}, {4, "r--"}, {5, "r-x"}, {6, "rw-"}, {7, "rwx"}};

        auto it = permissions.find(permission);
        return (it != permissions.end()) ? it->second : "---";
    }

    ~TreeNode() = default;
};

void linux_tree(TreeNode *root);
void print_help();
void print_tree(TreeNode *root, string prev);
void print_ls(TreeNode *pwd);
void print_stat(TreeNode *root, TreeNode *pwd, string path);
string pwd_str(TreeNode *root, TreeNode *pwd);
list<string> find_names(TreeNode *root, TreeNode *pwd, string name);
TreeNode *find_node(TreeNode *root, TreeNode *pwd, string path);
TreeNode *find_on_pwd(TreeNode *pwd, string name);
list<string> split(string str, char delim);
string join(list<string> str, char delim);
string *split_name(string str);
TreeNode *cd(TreeNode *root, TreeNode *pwd, string path);
TreeNode *create(TreeNode *root, TreeNode *pwd, string path, char type);
void remove(TreeNode *root, TreeNode *pwd, string path);
void dupl(TreeNode *root, TreeNode *pwd, string src, string dst, int keep);
void edit(TreeNode *root, TreeNode *pwd, string path);
void cat(TreeNode *root, TreeNode *pwd, string path);
void chmod(TreeNode *root, TreeNode *pwd, string path, string new_modes);
void clear_screen();

int main()
{
    TreeNode *root = new TreeNode(nullptr, "");
    root->type = 'd';
    TreeNode *pwd = root;
    linux_tree(root);

    cout << endl;
    print_help();

    std::string cmd;
    std::cout << std::endl
              << pwd_str(root, pwd) << ">> ";
    while (std::getline(std::cin >> std::ws, cmd))
    {
        std::list<std::string> args = split(cmd, ' ');
        TreeNode *temp_pwd = nullptr;

        if (cmd == "help")
        {
            print_help();
        }
        else if (args.front() == "ls")
        {
            args.pop_front();
            if (args.empty())
            {
                print_ls(pwd->child);
            }
            else
            {
                for (const std::string &arg : args)
                {
                    std::cout << arg << ":" << std::endl;
                    temp_pwd = cd(root, pwd, arg);
                    if (temp_pwd != nullptr)
                    {
                        print_ls(temp_pwd->child);
                    }
                }
            }
        }
        else if (args.front() == "tree")
        {
            args.pop_front();
            if (args.empty())
            {
                print_tree(pwd->child, "");
            }
            else
            {
                for (const std::string &arg : args)
                {
                    std::cout << arg << ":\n.\n";
                    temp_pwd = cd(root, pwd, arg);
                    if (temp_pwd != nullptr)
                    {
                        print_tree(temp_pwd->child, "");
                    }
                }
            }
        }
        else if (cmd == "pwd")
        {
            std::cout << pwd_str(root, pwd) << std::endl;
        }
        else if (args.front() == "cd")
        {
            args.pop_front();
            if (args.empty())
            {
                pwd = root;
            }
            else
            {
                temp_pwd = cd(root, pwd, args.front());
                if (temp_pwd != nullptr)
                {
                    pwd = temp_pwd;
                }
            }
        }
        else if (args.front() == "find")
        {
            args.pop_front();
            if (args.empty())
            {
                std::cout << "find: missing operand" << std::endl;
            }
            else
            {
                for (const string &arg : args)
                {
                    list<string> res;
                    if (arg[0] == '/')
                    {
                        res = find_names(root, root, arg);
                    }
                    else
                    {
                        res = find_names(pwd, pwd, arg);
                    }
                    if (res.empty())
                    {
                        cout << "find: '" << arg << "': no such file or directory" << std::endl;
                    }
                    else
                    {
                        for (const std::string &path : res)
                        {
                            std::cout << path << std::endl;
                        }
                    }
                }
            }
        }
        // else if (args.front() == "stat")
        // {
        //     args.pop_front();
        //     if (args.empty())
        //     {
        //         std::cout << "stat: missing operand" << std::endl;
        //     }
        //     else
        //     {
        //         for (const std::string &arg : args)
        //         {
        //             print_stat(root, pwd, arg);
        //         }
        //     }
        // }
        else if (args.front() == "mkdir")
        {
            args.pop_front();
            if (args.empty())
            {
                std::cout << "mkdir: missing operand" << std::endl;
            }
            else
            {
                for (const std::string &arg : args)
                {
                    create(root, pwd, arg, 'd');
                }
            }
        }
        else if (args.front() == "touch")
        {
            args.pop_front();
            if (args.empty())
            {
                std::cout << "touch: missing operand" << std::endl;
            }
            else
            {
                for (const std::string &arg : args)
                {
                    create(root, pwd, arg, '-');
                }
            }
        }
        else if (args.front() == "rm" || args.front() == "rmdir")
        {
            std::string command = args.front();
            args.pop_front();
            if (args.empty())
            {
                std::cout << command << ": missing operand" << std::endl;
            }
            else
            {
                for (const std::string &arg : args)
                {
                    remove(root, pwd, arg);
                }
            }
        }
        // else if (args.front() == "cp" || args.front() == "mv")
        // {
        //     std::string command = args.front();
        //     args.pop_front();
        //     if (args.size() != 2)
        //     {
        //         std::cout << command << ": missing operand" << std::endl;
        //     }
        //     else
        //     {
        //         int keep = (command == "cp") ? 1 : 0;
        //         std::string src = args.front();
        //         args.pop_front();
        //         std::string dst = args.front();
        //         args.pop_front();
        //         dupl(root, pwd, src, dst, keep);
        //     }
        // }
        // else if (args.front() == "edit")
        // {
        //     args.pop_front();
        //     if (args.empty())
        //     {
        //         std::cout << "edit: missing operand" << std::endl;
        //     }
        //     else
        //     {
        //         edit(root, pwd, args.front());
        //     }
        // }
        // else if (args.front() == "cat")
        // {
        //     args.pop_front();
        //     if (args.empty())
        //     {
        //         std::cout << "cat: missing operand" << std::endl;
        //     }
        //     else
        //     {
        //         cat(root, pwd, args.front());
        //     }
        // }
        // else if (args.front() == "chmod")
        // {
        //     args.pop_front();
        //     if (args.size() < 2)
        //     {
        //         std::cout << "chmod: missing operand" << std::endl;
        //     }
        //     else
        //     {
        //         std::string perm = args.front();
        //         args.pop_front();
        //         chmod(root, pwd, args.front(), perm);
        //     }
        // }
        else if (cmd == "clear")
        {
            clear_screen();
        }
        else if (cmd == "exit")
        {
            break;
        }
        else
        {
            std::cout << "Unknown command" << std::endl;
        }

        std::cout << std::endl
                  << pwd_str(root, pwd) << ">> ";
    }

    delete root;

    std::cout << std::endl;
    return 0;
}

void print_help()
{
    std::cout << "*** Follows the syntax of Linux shell commands ***" << std::endl
              << std::endl;
    std::cout << "\thelp      -   print this message" << std::endl;
    std::cout << "\tls        -   list contents of the current directory" << std::endl;
    std::cout << "\ttree      -   list contents of the current directory in a tree-like format" << std::endl;
    std::cout << "\tpwd       -   print the current working directory" << std::endl;
    std::cout << "\tcd DIR    -   change directory to DIR" << std::endl;
    std::cout << "\tfind N    -   find file or directory named N" << std::endl;
    std::cout << "\tstat P    -   print metadata of file or directory at path P" << std::endl;
    std::cout << "\tmkdir D   -   create a directory named D" << std::endl;
    std::cout << "\ttouch F   -   create a file named F" << std::endl;
    std::cout << "\trm P      -   remove the file or directory at path P" << std::endl;
    std::cout << "\trmdir P   -   remove the directory at path P" << std::endl;
    std::cout << "\tcp S D    -   copy file or directory from S to D" << std::endl;
    std::cout << "\tmv S D    -   move file or directory from S to D" << std::endl;
    std::cout << "\tedit P    -   edit the file at path P" << std::endl;
    std::cout << "\tcat P     -   print the contents of the file at path P" << std::endl;
    std::cout << "\tchmod M P -   change permissions of the file at path P to mode M" << std::endl;
    std::cout << "\tclear     -   clear the console screen" << std::endl;
    std::cout << "\texit      -   exit the shell" << std::endl;
}

void linux_tree(TreeNode *root)
{
    // Create some directories and files for demonstration purposes
    TreeNode *home = create(root, root, "home", 'd');
    TreeNode *user = create(root, home, "user", 'd');
    TreeNode *docs = create(root, user, "documents", 'd');
    create(root, docs, "file1.txt", '-');
    create(root, docs, "file2.txt", '-');
    TreeNode *pics = create(root, user, "pictures", 'd');
    create(root, pics, "image1.jpg", '-');
    create(root, pics, "image2.png", '-');
}

void print_tree(TreeNode *pwd, string prev)
{
    if (pwd == nullptr)
    {
        return;
    }
    else if (pwd->name.empty())
    {
        print_tree(pwd->child, prev);
        return;
    }
    string name = prev + "/" + pwd->name;
    cout << name << endl;
    print_tree(pwd->child, name);
    print_tree(pwd->link, prev);
}

void print_ls(TreeNode *pwd)
{
    if (pwd == nullptr)
    {
        return;
    }
    else if (pwd->name.empty())
    {
        print_ls(pwd->child);
        return;
    }
    cout << pwd->name << "\t" << pwd->type << pwd->get_permission() << "\t" << pwd->mdate << endl;
    print_ls(pwd->link);
}

// void print_stat(TreeNode *root, TreeNode *pwd, string path)
// {
//     TreeNode *temp = find_node(root, pwd, path);
//     if (temp != nullptr)
//     {
//         cout << "File: " << temp->name << endl;
//         cout << "Type: " << temp->type << endl;
//         cout << "Permission: " << temp->get_permission() << endl;
//         cout << "Created: " << temp->cdate << endl;
//         cout << "Modified: " << temp->mdate << endl;
//     }
//     else
//     {
//         cout << "stat: cannot stat '" << path << "': No such file or directory" << endl;
//     }
// }

string pwd_str(TreeNode *root, TreeNode *pwd)
{
    string path = pwd->name;
    while (pwd->parent != nullptr)
    {
        pwd = pwd->parent;
        path = pwd->name + "/" + path;
    }
    if (path.empty())
    {
        return "/";
    }
    return path;
}

list<string> find_names(TreeNode *root, TreeNode *pwd, string name)
{
    list<string> res;
    if (pwd == nullptr)
    {
        return res;
    }
    if (pwd->name == name)
    {
        res.push_back(pwd_str(root, pwd));
    }
    list<string> res1 = find_names(root, pwd->child, name);
    list<string> res2 = find_names(root, pwd->link, name);
    res.insert(res.end(), res1.begin(), res1.end());
    res.insert(res.end(), res2.begin(), res2.end());
    return res;
}

TreeNode *find_node(TreeNode *root, TreeNode *pwd, string path)
{
    list<string> paths = split(path, '/');
    string last = paths.back();
    paths.pop_back();
    string prev = join(paths, '/');
    TreeNode *temp = cd(root, pwd, prev);
    return find_on_pwd(temp, last);
}

TreeNode *find_on_pwd(TreeNode *pwd, string name)
{
    if (pwd == nullptr)
    {
        return nullptr;
    }
    if (pwd->name == name)
    {
        return pwd;
    }
    return find_on_pwd(pwd->link, name);
}

list<string> split(string str, char delim)
{
    list<string> res;
    string temp;
    for (char ch : str)
    {
        if (ch == delim)
        {
            if (!temp.empty())
            {
                res.push_back(temp);
                temp.clear();
            }
        }
        else
        {
            temp.push_back(ch);
        }
    }
    if (!temp.empty())
    {
        res.push_back(temp);
    }
    return res;
}

string join(list<string> str, char delim)
{
    string res;
    for (const string &s : str)
    {
        if (!res.empty())
        {
            res.push_back(delim);
        }
        res += s;
    }
    return res;
}

string *split_name(string str)
{
    size_t pos = str.find_last_of('/');
    if (pos == string::npos)
    {
        return new string[2]{"", str};
    }
    return new string[2]{str.substr(0, pos), str.substr(pos + 1)};
}

TreeNode *cd(TreeNode *root, TreeNode *pwd, string path)
{
    if (path.empty())
    {
        return pwd;
    }
    if (path[0] == '/')
    {
        pwd = root;
        path = path.substr(1);
    }
    list<string> paths = split(path, '/');
    for (const string &dir : paths)
    {
        if (dir == ".")
        {
            continue;
        }
        if (dir == "..")
        {
            if (pwd->parent != nullptr)
            {
                pwd = pwd->parent;
            }
            continue;
        }
        pwd = find_on_pwd(pwd->child, dir);
        if (pwd == nullptr)
        {
            std::cout << "cd: " << path << ": No such file or directory" << std::endl;
            return nullptr;
        }
    }
    return pwd;
}

TreeNode *create(TreeNode *root, TreeNode *pwd, string path, char type)
{
    string *paths = split_name(path);
    TreeNode *dir = cd(root, pwd, paths[0]);
    if (dir == nullptr)
    {
        return nullptr;
    }
    TreeNode *newNode = new TreeNode(dir, paths[1]);
    newNode->type = type;
    newNode->link = dir->child;
    dir->child = newNode;
    if (type == 'd')
    {
        cout << "mkdir: created directory '" << path << "'" << endl;
    }
    else
    {
        cout << "touch: created file '" << path << "'" << endl;
    }
    return newNode;
}

void remove(TreeNode *root, TreeNode *pwd, string path)
{
    string *paths = split_name(path);
    TreeNode *dir = cd(root, pwd, paths[0]);
    if (dir == nullptr)
    {
        return;
    }
    TreeNode *prev = nullptr;
    TreeNode *curr = dir->child;
    while (curr != nullptr && curr->name != paths[1])
    {
        prev = curr;
        curr = curr->link;
    }
    if (curr == nullptr)
    {
        std::cout << "rm: " << path << ": No such file or directory" << std::endl;
        return;
    }
    if (curr->type == 'd' && curr->child != nullptr)
    {
        std::cout << "rmdir: " << path << ": Directory not empty" << std::endl;
        return;
    }
    if (prev == nullptr)
    {
        dir->child = curr->link;
    }
    else
    {
        prev->link = curr->link;
    }
    delete curr;
    cout << "rm: removed '" << path << "'" << endl;
}

// void dupl(TreeNode *root, TreeNode *pwd, string src, string dst, int keep)
// {
//     string *src_paths = split_name(src);
//     string *dst_paths = split_name(dst);
//     TreeNode *src_dir = cd(root, pwd, src_paths[0]);
//     TreeNode *dst_dir = cd(root, pwd, dst_paths[0]);
//     if (src_dir == nullptr || dst_dir == nullptr)
//     {
//         return;
//     }
//     TreeNode *src_node = find_on_pwd(src_dir->child, src_paths[1]);
//     if (src_node == nullptr)
//     {
//         std::cout << "cp: " << src << ": No such file or directory" << std::endl;
//         return;
//     }
//     if (src_node->type == 'd')
//     {
//         std::cout << "cp: omitting directory '" << src << "'" << std::endl;
//         return;
//     }
//     TreeNode *dst_node = find_on_pwd(dst_dir->child, dst_paths[1]);
//     if (dst_node != nullptr)
//     {
//         std::cout << "cp: cannot overwrite non-directory '" << dst << "' with directory '" << src << "'" << std::endl;
//         return;
//     }
//     TreeNode *newNode = new TreeNode(dst_dir, dst_paths[1]);
//     newNode->type = src_node->type;
//     newNode->contents = src_node->contents;
//     newNode->permission = src_node->permission;
//     newNode->cdate = src_node->cdate;
//     newNode->mdate = src_node->mdate;
//     newNode->link = dst_dir->child;
//     dst_dir->child = newNode;
//     if (keep == 0)
//     {
//         remove(root, pwd, src);
//     }
//     cout << "cp: copied '" << src << "' to '" << dst << "'" << endl;
// }

// void edit(TreeNode *root, TreeNode *pwd, string path)
// {
//     TreeNode *file = find_node(root, pwd, path);
//     if (file == nullptr)
//     {
//         std::cout << "edit: " << path << ": No such file or directory" << std::endl;
//         return;
//     }
//     std::cout << "Enter new contents for " << path << " (end with an empty line):" << std::endl;
//     file->contents.clear();
//     std::string line;
//     while (std::getline(std::cin, line))
//     {
//         if (line.empty())
//         {
//             break;
//         }
//         file->contents.push_back(line);
//     }
//     file->mdate = curr_time();
//     cout << "edit: updated contents of '" << path << "'" << endl;
// }

// void cat(TreeNode *root, TreeNode *pwd, string path)
// {
//     TreeNode *file = find_node(root, pwd, path);
//     if (file == nullptr)
//     {
//         std::cout << "cat: " << path << ": No such file or directory" << std::endl;
//         return;
//     }
//     for (const std::string &line : file->contents)
//     {
//         std::cout << line << std::endl;
//     }
// }

// void chmod(TreeNode *root, TreeNode *pwd, string path, string new_modes)
// {
//     TreeNode *file = find_node(root, pwd, path);
//     if (file == nullptr)
//     {
//         std::cout << "chmod: " << path << ": No such file or directory" << std::endl;
//         return;
//     }
//     try
//     {
//         int new_perm = std::stoi(new_modes, nullptr, 8);
//         file->permission = new_perm;
//         file->mdate = curr_time();
//         cout << "chmod: updated permissions of '" << path << "'" << endl;
//     }
//     catch (const std::exception &e)
//     {
//         std::cout << "chmod: invalid mode: " << new_modes << std::endl;
//     }
// }

void clear_screen()
{
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    system("clear");
#endif
}

string curr_time()
{
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}