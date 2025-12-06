# cppsp
cppsp -a script language base on c++
## Install
Download the cppsp_compiler.exe or compiler the sourcecode by yourself
* Optional: put the folder path of exe to environment
# Warning ⚠️
* Cannot accept any space/blank before keyworld!
* No multi-line!Do not use something like:
```
print(1)☑️
 print(1)🚫
@inject(int a;
float b;)🚫
```
## Usage
* Use cmd or other console to compiler .cppsp file:
cppsp_compiler(if not in environment path:.\cppsp_compiler.exe or c:\...\cppsp_compiler.exe) script.cppsp
* Setting c++ include/lib folder by .ini file
include.ini:C:\...\include1,c:\...\include2
lib.ini:C:\...\lib1,c:\...\lib2
## Feature
* can compile when there is only print("hello world") in .cppsp
* can use almost c++ header by import
* can use c++ code  by @inject and @function
## Keyword
* #useclang or #usegcc : use clang++ or g++ compile command
* @command("..."): add command when compile like:-Os、-m64
* import :import header in c++ and accept import x,y,.....
* @funcuion<<...>>: inject everything(void()、int()、bool()、even #define and using namespace) in <<...>> to the space under #include above int main()
* @inject(...) :inject everything in (...) to int main{...}
* print(): print content to console like print("12\n"," ",1," ",2.1,true,false," ")
* input(): input data to variables,but need @inject() to declare varibles
* //:comment
## Example
```cpp
 print("hello world")
```
