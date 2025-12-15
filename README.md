# Password-Extractor
Password extractor tool that check stored passwords in browsers, SAM / shadow files, and notepad files from temporary locations (user folder location). Prepare username and password database and print on GUI based screen. 

There are three parts of the code that are implemented in password Extractor.
1. Extraction from Shadow Files.
2. Extraction from Files and folders of directories.
3. Extraction from Browser(FireFox) History.

--> Prerequisites
1. OS: Kali Linux
2. Compiler: g++ or gcc (prefered g++) with c++17
3. You can write code seperately or in one file.
4. This read me is for seperate code.
5. You can also follow these steps for making one file.
6. JSON library for filesystem

--> For Shadow Files.
i. Run the command "sudo apt update" for updated packages.

ii. Write the code in a cpp file.

iii. Compile the code but it will need root level access for compilation.

iv. Run "sudo su" to give the root level access.

v. For compiling run "g++ FileName.cpp -o FileName".

vi. If you get any error even after this then Run "chmod +x FileName" to give additional permission as this code is accessing shadow files.

vii. After compiling run the compiled file by "./FileName".

--> For Folder and temporary Files

i. This code doesnot need any root level access.

ii. Write the code in which it will scan the each folder and file in specific directory.

iii- Then it will give the password(encrypted or decrypted).

iv. Compile and run as same as previous one.

v. g++ fileName -o fileName.cpp

vi. ./fileName

--> For Browser (FireFox)

i. Run "sudo apt install nlohmann-json-dev" to install JSON library.

ii. If that doesnot work Run: "sudo apt install nlohmann-json3-dev".

iii. #include <nlohmann/json.hpp> in your code.

iv. Check for your Firefox profile path: "ls ~/.mozilla/firefox".

v. Inside your profile path look for folders like "xxxxxx.default-release", "xxxxxx.default-esr" etc.

vi. Now inside xxxxxx.default-release check for logins.json.

vii. If you couldnot find the logins.json then

viii. Open Firefox -> Go to settings-> Privacy & Security-> Check for any saved passwords.

ix. If you dont find any saved password then go to any website -> login and let Firefox save the password.

x. Again check for login.json.

xi. now write the code if your using mine then change the path 

 string firefoxProfilePath = "/home/kali/.mozilla/firefox/xxxxxxx.default-esr/logins.json

 with your folder name.

xii. Compile by : "g++ -o fileName fileName.cpp -lstdc++fs".
xiii. If you still can't see login.json file then run: "chmod +r ~/.mozilla/firefox/xxxxxxxxx.default-esr/logins.json" to give permission if disabled.
xiv. ./fileName
