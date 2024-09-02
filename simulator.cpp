#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

void twoComplement(string &s){
	int n = s.size()-1;
	while(s[n] != '1'){
		n--;
	}
	for(int i = n-1;i>=0;i--){
		if(s[i] == '1'){
			s[i] = '0';
		}
		else if(s[i] == '0'){
			s[i] = '1';
		}
	}
}

string convert_deci_to_binary(int n,int width){
	string s;
	bool neg = false;

	if(n<0){
		neg = true;
		n = -n;
	}

	while(n>0){
		s.push_back( (n%2) + '0');
		n = n/2;
	}
	reverse(s.begin(),s.end());
	
	int len = s.size();
	string s1(width-len,'0');
	s = s1+s;
	if(neg){
		twoComplement(s);
	}

	return s;
}

map<string,char> binary_to_hex = {{"0000", '0'}, {"0001", '1'}, {"0010", '2'}, {"0011", '3'}, {"0100", '4'}, {"0101", '5'}, {"0110", '6'}, {"0111", '7'}, {"1000", '8'}, {"1001", '9'}, {"1010", 'a'}, {"1011", 'b'}, {"1100", 'c'}, {"1101", 'd'}, {"1110", 'e'}, {"1111", 'f'}};
map<string,string> instructType = {{"add", "R"}, {"sub", "R"}, {"and", "R"}, {"or", "R"}, {"xor", "R"}, {"sll", "R"}, {"srl", "R"}, {"sra", "R"}, {"addi", "I"}, {"xori", "I"}, {"ori", "I"}, {"andi", "I"}, {"slli", "I"}, {"srli", "I"}, {"srai", "I"}, {"lb", "I_l"}, {"lh", "I_l"}, {"lw", "I_l"}, {"ld", "I_l"}, {"lbu", "I_l"}, {"lhu", "I_l"}, {"lwu", "I_l"}, {"sb", "S"}, {"sh", "S"}, {"sw", "S"}, {"sd", "S"}};
map<string,string> instruct_opcode = {{"R", "0110011"}, {"I", "0010011"}, {"I_l", "0000011"}, {"S", "0100011"}, {"B", "1100011"}, {"J", "1101111"}, {"U", "0110111"}};
map<string,string> instructFunct3 = {{"add", "000"}, {"sub", "000"}, {"and", "111"}, {"or", "110"}, {"xor", "100"}, {"sll", "001"}, {"srl", "101"}, {"sra", "101"}, {"addi", "000"}, {"xori", "100"}, {"ori", "110"}, {"andi", "111"}, {"slli", "001"}, {"srli", "101"}, {"srai", "101"}, {"lb", "000"}, {"lh", "001"}, {"lw", "010"}, {"ld", "011"}, {"lbu", "100"}, {"lhu", "101"}, {"lwu", "110"}, {"sb", "000"}, {"sh", "001"}, {"sw", "010"}, {"sd", "011"}};
map<string,string> instructFunct7 = {{"add", "0000000"}, {"sub", "0100000"}, {"and", "0000000"}, {"or", "0000000"}, {"xor", "0000000"}, {"sll", "0000000"}, {"srl", "0000000"}, {"sra", "0100000"}};
map<string, string> alias = {{"zero", "x0"}, {"ra", "x1"}, {"sp", "x2"}, {"gp", "x3"}, {"tp", "x4"}, {"t0", "x5"}, {"t1", "x6"}, {"t2", "x7"}, {"s0", "x8"}, {"s1", "x9"}, {"a0", "x10"}, {"a1", "x11"}, {"a2", "x12"}, {"a3", "x13"}, {"a4", "x14"}, {"a5", "x15"}, {"a6", "x16"}, {"a7", "x17"}, {"s2", "x18"}, {"s3", "x19"}, {"s4", "x20"}, {"s5", "x21"}, {"s6", "x22"}, {"s7", "x23"}, {"s8", "x24"}, {"s9", "x25"}, {"s10", "x26"}, {"s11", "x27"}, {"t3", "x28"}, {"t4", "x29"}, {"t5", "x30"}, {"t6", "x31"}};

string convert_32bits_to_hex(string s){
	string ans = "";
	for(int i=0;i<32; i += 4){
		string r = s.substr(i,4);
		ans += binary_to_hex[r];
	}
	return ans;
}

void convertR_to_machineCode(string line){
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
	binary_code += instruct_opcode["R"];

	string machine_code = convert_32bits_to_hex(binary_code);
	cout<<machine_code<<endl;
}

void convertI_to_machineCode(string line){
	string rd = "", rs1 = "", imm = "", op = "";
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
		imm += line[a];
		a++;
	}

	for(int k=0;k<2;k++){
		string temp;
		if(k==0){
			temp = rd;
		}
		else{
			temp = rs1;
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
		else{
			rs1 = convert_deci_to_binary(num, 5);
		}
	} 	

	imm = convert_deci_to_binary(stoi(imm), 12);

	string binary_code = "";
	binary_code += imm;
	binary_code += rs1;
	binary_code += instructFunct3[op];
	binary_code += rd;
	binary_code += instruct_opcode["I"];		


	string machine_code = convert_32bits_to_hex(binary_code);
	cout<<machine_code<<endl;
}

void convertI_l_to_machineCode(string line){
	string rd = "", rs1 = "", imm = "", op = "";
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

	while(line[a] != '('){
		imm += line[a];
		a++;
	}
	a++;

	while(line[a] != ')'){
		rs1 += line[a];
		a++;
	}

	for(int k=0;k<2;k++){
		string temp;
		if(k==0){
			temp = rd;
		}
		else{
			temp = rs1;
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
		else{
			rs1 = convert_deci_to_binary(num, 5);
		}
	} 	

	imm = convert_deci_to_binary(stoi(imm), 12);

	string binary_code = "";
	binary_code += imm;
	binary_code += rs1;
	binary_code += instructFunct3[op];
	binary_code += rd;
	binary_code += instruct_opcode["I_l"];		


	string machine_code = convert_32bits_to_hex(binary_code);
	cout<<machine_code<<endl;
}

void convertS_to_machineCode(string line){
	string op="",rs2="",rs1="",imm="";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
	}
	a++;
		
	while(line[a] != ','){
		rs2 += line[a];
		a++;
	}
	a += 2;

	while(line[a] != '('){
		imm += line[a];
		a++;
	}
	a++;
	while(line[a] != ')' ){
		rs1 += line[a];
		a++;
	}
	string binary_code = "";

	for(int k=0;k<2;k++){
		string temp;
		if(k==0){
			temp = rs2;
		}
		else{
			temp = rs1;
		}

		if(temp[0] != 'x'){
			temp = alias[temp];
		}
		int num = 0;
		for(int j=1;j<temp.length();j++){
			num = num*10 + (temp[j]-'0');
		}
		if(k==0){
			rs2 = convert_deci_to_binary(num, 5);
		}
		else{
			rs1 = convert_deci_to_binary(num, 5);
		}
	} 
	string immB = convert_deci_to_binary(stoi(imm),12);
	binary_code += immB.substr(0,7);
	binary_code += rs2;
	binary_code += rs1;
	binary_code += instructFunct3[op];
	binary_code += immB.substr(7,5);
	binary_code += instruct_opcode["S"];

	string machine_code = convert_32bits_to_hex(binary_code);
	cout<<machine_code<<endl;

}

int main(){
	ifstream inputFile("input.s");
    vector<string> text;
    string line;
    while(getline(inputFile,line)){
        text.push_back(line);
    }
    inputFile.close();

	for(int i=0;i<text.size();i++){
		string line = text[i];
		string op = "";
		int a = 0;
		while(line[a] != ' '){
			op += line[a];
			a++;
		}
		string type = instructType[op];
		if(type=="R"){
			convertR_to_machineCode(line);
		}
		else if(type == "S"){
			convertS_to_machineCode(line);
		}
		else if(type=="I"){
			convertI_to_machineCode(line);
		}
		else if(type=="I_l"){
			convertI_l_to_machineCode(line);
		}
	}

	return 0;
}
