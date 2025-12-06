#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <unordered_map>
#include <functional>
namespace fs = std::filesystem;
bool Ifiostream=0;
// ====== 新增：萬用語法指令註冊器 ======
std::unordered_map<std::string, std::function<std::string(const std::string&)>> cpsCommands;
std::unordered_map<std::string, std::string> g_vars; // 全域變數
void registerCommand(const std::string& name,
                     std::function<std::string(const std::string&)> handler) {
    cpsCommands[name] = handler;
}
//去空格
inline std::string noblank(const std::string& token) {
       // 去掉前後空格
    std::string v = token;
    auto trim = [](std::string& s){
        while (!s.empty() && isspace(s.front())) s.erase(s.begin());
        while (!s.empty() && isspace(s.back())) s.pop_back();
    };  trim(v);
    // 空字串直接跳過
    if (v.empty()) return "";
    // 布林判斷
    if (v == "true")  return "printf(\"true\");\n";  if (v == "false") return "printf(\"false\");\n";
  // 判斷數字
    char* end;
    double num = std::strtod(v.c_str(), &end);
    if (*end == '\0') {// 判斷整數或浮點
        if (num == (int)num) return "{ auto _t = (int)" + v + "; printf(\"%d\", _t); }\n";
        else return "{ auto _t = " + v + "; printf(\"%g\", _t); }\n";  }
 // 判斷字串（含 "abc"）或 fallback
    if (!v.empty() && v.front()=='"' && v.back()=='"')  return "printf(" + v + ");\n";
    // 其他情況也當字串
    return "printf(\"" + v + "\");\n";}

// 萃取 print("abc",1,2) 裡面的參數部分
std::string extractArgs(const std::string& line) {
    size_t l = line.find("(");
    size_t r = line.rfind(")");
    if (l == std::string::npos || r == std::string::npos || r < l) return "";
    return line.substr(l + 1, r - l - 1);
}
// 通用呼叫，用於主迴圈：直接給一行程式碼，它會自動選擇指令
std::string runCommand(const std::string& line) {
    for (auto& p : cpsCommands) {
        if (line.rfind(p.first, 0) == 0) { // 以指令開頭
            if(p.first == "@function") { continue;}
            std::string args = extractArgs(line);
            return p.second(args);
        }
    }
    return ""; // 找不到指令就忽略
}


// 讀 ini 檔，用逗號分割，轉成 -I 或 -L 參數
std::string parseIni(const std::string& path, const std::string& flag) {
    std::ifstream infile(path);
    if (!infile) return "";

    std::string line;
    std::getline(infile, line);
    std::stringstream ss(line);
    std::string token;
    std::string result;
    while (std::getline(ss, token, ',')) {
        if (!token.empty()) {
            result += flag + "\"" + token + "\" ";
        }
    }
    return result;
}
// 判斷字串是否為布林值
auto is_bool = [](const std::string& s){
    return s == "true" || s == "false";
};
auto is_number = [](const std::string& s){
    char* end;
    std::strtod(s.c_str(), &end);
    return *end == '\0';
};
 // 去掉前後空白
    auto trim = [](std::string s) {
        while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.erase(s.begin());
        while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) s.pop_back();
        return s;
    };
int main(int argc, char* argv[]) {
    bool enableclang =false;
    //註冊
    registerCommand("print", [](const std::string& args) {
    std::stringstream ss(args);
    std::string tok;
    std::vector<std::string> v;
    while (std::getline(ss, tok, ',')) v.push_back(tok);
    std::string out;double iorf;
      //處理非" "的參數
      if(v.empty()) {for (size_t i =0; i < v.size(); ++i)v[i]= noblank(v[i]);}
    if (!v.empty()) { 
    for (size_t i =0; i < v.size(); ++i) {
    std::string cur = v[i];
    if ( cur == "true") { out += "printf(\"true\");\n";}
    else if (cur == "false") { out += "printf(\"false\");\n"; }
    else if (is_number(cur)) {
        double iorf = std::stod(cur);
        if (iorf == (int)iorf)   out += "{ auto _t = " + cur + "; printf(\"%d\", _t); }\n";
       else out += "{ auto _t = " + cur + "; printf(\"%g\", _t); }\n"; }
       else if(Ifiostream==true) out +="std::cout<<"+cur+";\n";
       else {  out += "printf(" + cur + ");\n";}
}
    }

    return out;
});

// println() 指令
registerCommand("println", [](const std::string& args) {
    return "printf(" + args + "); printf(\"\\n\");\n";
});
registerCommand("input", [](const std::string& args) {
        std::stringstream ss(args);
    std::string tok,out;
    std::vector<std::string> v;
    while (std::getline(ss, tok, ',')) v.push_back(tok);
    if(Ifiostream==false) out= "printf(\"need import iostream\")";
    if (!v.empty()) { 
    for(int i=0;i<v.size();i++){
        out +="std::cin>>"+v[i]+";\n";
    }}
    return out;
});

registerCommand("@inject", [](const std::string& args) {
    return args;
});
////////
    if (argc < 2) {
        std::cerr << "Usage: cppsp_compiler(if not in environment path:.\\cppsp_compiler.exe or c:\\...\\cppsp_compiler.exe) script.cppsp\ninclude.ini:C:\\...\\include1,c:\\...\\include2\nlib.ini:C:\\...\\lib1,c:\\...\\lib2\n";
        return 1;
    }

    fs::path cpsPath(argv[1]);
    if (!fs::exists(cpsPath)) {
        std::cerr << "File not found.\n";
        return 1;
    }

    std::ifstream infile(cpsPath);std::ifstream funcfile(cpsPath);
    if (!infile) {
        std::cerr << "Cannot open file.\n";
        return 1;
    }

    fs::path cppPath = cpsPath.parent_path() / (cpsPath.stem().string() + ".cpp");
    std::ofstream outfile(cppPath);
    if (!outfile) {
        std::cerr << "Cannot create cpp file.\n";
        return 1;
    }

    outfile << "#include <stdio.h>\n";
    std::string importline; std::ifstream fileinclude(cpsPath);
while (std::getline(fileinclude, importline)) {
    bool comment=0;
    if (importline.find("//") != std::string::npos) { comment =1;continue; }
if (!comment && importline.find("import ") != std::string::npos) {
    size_t pos = importline.find("import ");
    std::string imports = importline.substr(pos + 7); // "import " 長度 7

    imports = trim(imports);

    // 拆成多個標頭
    std::stringstream ss(imports);
    std::string header;
    while (std::getline(ss, header, ',')) {
        header = trim(header); // 每個標頭也要去掉空白
        if (header.empty()) continue;

        fs::path importFile = cpsPath.parent_path() / header;
        outfile << "#include \"" << importFile.lexically_relative(cpsPath.parent_path()).string() << "\"\n";

        if (importFile.filename().string() == "iostream") {
            Ifiostream = true;
        }
    }
}
}
 std::string funcline;
   while (std::getline(funcfile, funcline)) {
        if(funcline.find("@function<<") != std::string::npos){
            size_t pos = funcline.find("@function<<");
            std::string funcname = funcline.substr(pos + 11); // "@function(" 長度 10
            funcname = funcname.substr(0, funcname.find(">>"));
            if (funcname.empty()) continue;
             else {
                outfile <<funcname+"\n";
            }
        }
    }
   
        outfile << "int main() {\n";
    std::string line;
    std::string extraFlags; // 存 @command() 的內容
    while (std::getline(infile, line)) {
        std::string code = runCommand(line);
        if (!code.empty()) outfile << code;
        if (line.find("@command(") != std::string::npos) {
            size_t start = line.find("\"");
            size_t end = line.rfind("\"");
            if (start != std::string::npos && end != std::string::npos && end > start)
                extraFlags = line.substr(start + 1, end - start - 1);
        }
        if(line.find("#useclang")!= std::string::npos){enableclang=true;}
        if(line.find("#usegcc")!= std::string::npos){enableclang=false;}
    }

    outfile << "\nreturn 0;\n}\n";
    outfile.close();

    fs::path exePath = cpsPath.parent_path() / (cpsPath.stem().string() + ".exe");

    // 讀 include.ini 和 lib.ini
    std::string includeFlags = parseIni("include.ini", "-I");
    std::string libFlags = parseIni("lib.ini", "-L");

    std::string gppCommand = "g++.exe \"" + cppPath.string() + "\" -o \"" + exePath.string() + "\" "
                             + extraFlags + " "
                             + includeFlags + " "
                             + libFlags;
    if(enableclang) gppCommand = "clang++ \"" + cppPath.string() + "\" -o \"" + exePath.string() + "\" "
                             + extraFlags + " "
                             + includeFlags + " "
                             + libFlags;

    std::cout << "Compiling: " << gppCommand << "\n";
    int ret = system(gppCommand.c_str());

    if (ret != 0) {
        std::cerr << "Compilation failed!\n";
        return 1;
    }

    std::cout << "Compilation succeeded! Executable: " << exePath.string() << "\n";
    return 0;
}
