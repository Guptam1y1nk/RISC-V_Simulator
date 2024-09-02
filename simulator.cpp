#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

string convert_deci_to_binary(int n, int len){
	string s;
	while(n>0){
		s.push_back( (n%2) + '0');
		n = n/2;
	}
	reverse(s.begin(),s.end());
	int x = s.size();
	string s1(len-x,'0');
	s = s1+s;
	return s;
}

void convert_hex_to_binary(map<char,string> &hex_to_binary){
	hex_to_binary['0'] = "0000";
	hex_to_binary['1'] = "0001";
	hex_to_binary['2'] = "0010";
	hex_to_binary['3'] = "0011";
	hex_to_binary['4'] = "0100";
	hex_to_binary['5'] = "0101";
	hex_to_binary['6'] = "0110";
	hex_to_binary['7'] = "0111";
	hex_to_binary['8'] = "1000";
	hex_to_binary['9'] = "1001";
	hex_to_binary['a'] = "1010";
	hex_to_binary['b'] = "1011";
	hex_to_binary['c'] = "1100";
	hex_to_binary['d'] = "1101";
	hex_to_binary['e'] = "1110";
	hex_to_binary['f'] = "1111";
}

void convert_binary_to_hex(map<string,char> &binary_to_hex){
	binary_to_hex["0000"] = '0';
	binary_to_hex["0001"] = '1';
	binary_to_hex["0010"] = '2';
	binary_to_hex["0011"] = '3';
	binary_to_hex["0100"] = '4';
	binary_to_hex["0101"] = '5';
	binary_to_hex["0110"] = '6';
	binary_to_hex["0111"] = '7';
	binary_to_hex["1000"] = '8';
	binary_to_hex["1001"] = '9';
	binary_to_hex["1010"] = 'a';
	binary_to_hex["1011"] = 'b';
	binary_to_hex["1100"] = 'c';
	binary_to_hex["1101"] = 'd';
	binary_to_hex["1110"] = 'e';
	binary_to_hex["1111"] = 'f';
}

void find_instructType(map<string,string> &instructType){
	instructType["add"] = "R"; 
	instructType["sub"] = "R"; 
	instructType["and"] = "R"; 
	instructType["or"] = "R"; 
	instructType["xor"] = "R"; 
	instructType["sll"] = "R"; 
	instructType["srl"] = "R"; 
	instructType["sra"] = "R"; 

	instructType["addi"] = "I";
	instructType["xori"] = "I";
	instructType["ori"] = "I";
	instructType["andi"] = "I";
	instructType["slli"] = "I";
	instructType["srli"] = "I";
	instructType["srai"] = "I";

	instructType["lb"] = "I_l";
	instructType["lh"] = "I_l";
	instructType["lw"] = "I_l";
	instructType["ld"] = "I_l";
	instructType["lbu"] = "I_l";
	instructType["lhu"] = "I_l";
	instructType["lwu"] = "I_l";

	instructType["sb"] = "S";
	instructType["sh"] = "S";
	instructType["sw"] = "S";
	instructType["sd"] = "S";
}

void find_opcode(map<string,string> &instruct_opcode){
	instruct_opcode["R"] = "0110011";
	instruct_opcode["I"] = "0010011";
	instruct_opcode["I_l"] = "0000011";
	instruct_opcode["S"] = "0100011";
	instruct_opcode["B"] = "1100011";
	instruct_opcode["J"] = "1101111";
	instruct_opcode["U"] = "0110111";
}

void find_funct3(map<string,string> &instructFunct3){
	instructFunct3["add"] = "000"; 
	instructFunct3["sub"] = "000"; 
	instructFunct3["and"] = "111"; 
	instructFunct3["or"] = "110"; 
	instructFunct3["xor"] = "100"; 
	instructFunct3["sll"] = "001"; 
	instructFunct3["srl"] = "101"; 
	instructFunct3["sra"] = "101"; 

	instructFunct3["addi"] = "000";
	instructFunct3["xori"] = "100";
	instructFunct3["ori"] = "110";
	instructFunct3["andi"] = "111";
	instructFunct3["slli"] = "001";
	instructFunct3["srli"] = "101";
	instructFunct3["srai"] = "101";

	instructFunct3["lb"] = "000";
	instructFunct3["lh"] = "001";
	instructFunct3["lw"] = "010";
	instructFunct3["ld"] = "011";
	instructFunct3["lbu"] = "100";
	instructFunct3["lhu"] = "101";
	instructFunct3["lwu"] = "110";

	instructFunct3["sb"] = "000";
	instructFunct3["sh"] = "001";
	instructFunct3["sw"] = "010";
	instructFunct3["sd"] = "011";

}

void find_funct7(map<string,string> &instructFunct7){
	instructFunct7["add"] = "0000000"; 
	instructFunct7["sub"] = "0100000"; 
	instructFunct7["and"] = "0000000"; 
	instructFunct7["or"] = "0000000"; 
	instructFunct7["xor"] = "0000000"; 
	instructFunct7["sll"] = "0000000"; 
	instructFunct7["srl"] = "0000000"; 
	instructFunct7["sra"] = "0100000"; 

	instructFunct7["addi"] = "000";
	instructFunct7["xori"] = "100";
	instructFunct7["ori"] = "110";
	instructFunct7["andi"] = "111";
	instructFunct7["slli"] = "001";
	instructFunct7["srli"] = "101";
	instructFunct7["srai"] = "101";

	instructFunct7["lb"] = "000";
	instructFunct7["lh"] = "001";
	instructFunct7["lw"] = "010";
	instructFunct7["ld"] = "011";
	instructFunct7["lbu"] = "100";
	instructFunct7["lhu"] = "101";
	instructFunct7["lwu"] = "110";

	instructFunct7["sb"] = "000";
	instructFunct7["sh"] = "001";
	instructFunct7["sw"] = "010";
	instructFunct7["sd"] = "011";

}

map<int,string> deci_to_binary;
map<char,string> hex_to_binary;
map<string,char> binary_to_hex;
map<string,string> instructType;
map<string,string> instruct_opcode;
map<string,string> instructFunct3;
map<string,string> instructFunct7;
map<string, string> alias = {{"zero", "x0"}, {"ra", "x1"}, {"sp", "x2"}, {"gp", "x3"}, {"tp", "x4"}, {"t0", "x5"}, {"t1", "x6"}, {"t2", "x7"}, {"s0", "x8"}, {"s1", "x9"}, {"a0", "x10"}, {"a1", "x11"}, {"a2", "x12"}, {"a3", "x13"}, {"a4", "x14"}, {"a5", "x15"}, {"a6", "x16"}, {"a7", "x17"}, {"s2", "x18"}, {"s3", "x19"}, {"s4", "x20"}, {"s5", "x21"}, {"s6", "x22"}, {"s7", "x23"}, {"s8", "x24"}, {"s9", "x25"}, {"s10", "x26"}, {"s11", "x27"}, {"t3", "x28"}, {"t4", "x29"}, {"t5", "x30"}, {"t6", "x31"}};

string convert_32bits_to_hex(string s){
	string ans = "";
	for(int i=0;i<32; i += 4){
		string r = s.substr(i,4);
		ans += binary_to_hex[r];
	}
	return ans;
}

int main(){
    find_instructType(instructType);
    find_opcode(instruct_opcode);
    find_funct3(instructFunct3);
    find_funct7(instructFunct7);
    convert_binary_to_hex(binary_to_hex);

	ifstream inputFile("input.s");
    vector<string> text;
    string line;
    while(getline(inputFile,line)){
        text.push_back(line);
    }
    inputFile.close();

	for(int i=0;i<text.size();i++){
		string line = text[i];
		string op = "", rd = "", rs1 = "", rs2 = "";

		int a = 0;
		while(line[a] != ' '){
			op += line[a];
			a++;
		}
		a++;

		while(line[a] != ','){
			rd += line[a];
			a++;
		}
		a += 2;

		while(line[a] != ','){
			rs1 += line[a];
			a++;
		}
		a += 2;

		while(a < line.length()){
			rs2 += line[a];
			a++;
		}

		string binary_code = "";

		string type = instructType[op];
		if(type=="R"){
			for(int k=0;k<3;k++){
				string temp;
				if(k==0){
					temp = rd;
				}
				else if(k==1){
					temp = rs1;
				}
				else{
					temp = rs2;
				}

				if(temp[0] != 'x'){
					temp = alias[temp];
				}

				int num = 0;
				for(int j=1;j<temp.length();j++){
					num = num*10 + (temp[j]-'0');
				}

				if(k==0){
					rd = convert_deci_to_binary(num, 5);
				}
				else if(k==1){
					rs1 = convert_deci_to_binary(num, 5);
				}
				else{
					rs2 = convert_deci_to_binary(num, 5);
				}
			} 

			binary_code += instructFunct7[op];
			binary_code += rs2;
			binary_code += rs1;
			binary_code += instructFunct3[op];
			binary_code += rd;
			binary_code += instruct_opcode[type];
		}

		string machine_code = convert_32bits_to_hex(binary_code);
		cout<<machine_code<<endl;
	}

	return 0;
}
