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

int convert_hex_to_deci(string s){
	int factor = 1;
	int n = s.length();

	int num = 0;

	for(int i=n-1;i>=0;i--){
		num += (s[i]-'0')*factor;
		factor *= 16;
	}

	return num;
}

map<string,char> binary_to_hex = {{"0000", '0'}, {"0001", '1'}, {"0010", '2'}, {"0011", '3'}, {"0100", '4'}, {"0101", '5'}, {"0110", '6'}, {"0111", '7'}, {"1000", '8'}, {"1001", '9'}, {"1010", 'a'}, {"1011", 'b'}, {"1100", 'c'}, {"1101", 'd'}, {"1110", 'e'}, {"1111", 'f'}};
map<string,string> instructType = {{"add", "R"}, {"sub", "R"}, {"and", "R"}, {"or", "R"}, {"xor", "R"}, {"sll", "R"}, {"srl", "R"}, {"sra", "R"}, {"addi", "I"}, {"xori", "I"}, {"ori", "I"}, {"andi", "I"}, {"slli", "I"}, {"srli", "I"}, {"srai", "I"}, {"lb", "I_l"}, {"lh", "I_l"}, {"lw", "I_l"}, {"ld", "I_l"}, {"lbu", "I_l"}, {"lhu", "I_l"}, {"lwu", "I_l"}, {"sb", "S"}, {"sh", "S"}, {"sw", "S"}, {"sd", "S"}, {"beq", "B"}, {"bne", "B"}, {"blt", "B"}, {"bge", "B"},{"bltu", "B"}, {"bgeu" , "B"}, {"jal", "J"}, {"jalr", "I_J"}, {"lui", "U"}, {"auipc", "U"}};
map<string,string> instruct_opcode = {{"R", "0110011"}, {"I", "0010011"}, {"I_l", "0000011"}, {"S", "0100011"}, {"B", "1100011"}, {"J", "1101111"}, {"I_J", "1101111"}, {"U", "0110111"}};
map<string,string> instructFunct3 = {{"add", "000"}, {"sub", "000"}, {"and", "111"}, {"or", "110"}, {"xor", "100"}, {"sll", "001"}, {"srl", "101"}, {"sra", "101"}, {"addi", "000"}, {"xori", "100"}, {"ori", "110"}, {"andi", "111"}, {"slli", "001"}, {"srli", "101"}, {"srai", "101"}, {"lb", "000"}, {"lh", "001"}, {"lw", "010"}, {"ld", "011"}, {"lbu", "100"}, {"lhu", "101"}, {"lwu", "110"}, {"sb", "000"}, {"sh", "001"}, {"sw", "010"}, {"sd", "011"}, {"beq", "000"}, {"bne", "001"}, {"blt", "010"}, {"bge", "011"},{"bltu", "110"}, {"bgeu" , "111"}, {"jalr", "000"}};
map<string,string> instructFunct7 = {{"add", "0000000"}, {"sub", "0100000"}, {"and", "0000000"}, {"or", "0000000"}, {"xor", "0000000"}, {"sll", "0000000"}, {"srl", "0000000"}, {"sra", "0100000"}};
map<string, string> alias = {{"zero", "x0"}, {"ra", "x1"}, {"sp", "x2"}, {"gp", "x3"}, {"tp", "x4"}, {"t0", "x5"}, {"t1", "x6"}, {"t2", "x7"}, {"s0", "x8"}, {"s1", "x9"}, {"a0", "x10"}, {"a1", "x11"}, {"a2", "x12"}, {"a3", "x13"}, {"a4", "x14"}, {"a5", "x15"}, {"a6", "x16"}, {"a7", "x17"}, {"s2", "x18"}, {"s3", "x19"}, {"s4", "x20"}, {"s5", "x21"}, {"s6", "x22"}, {"s7", "x23"}, {"s8", "x24"}, {"s9", "x25"}, {"s10", "x26"}, {"s11", "x27"}, {"t3", "x28"}, {"t4", "x29"}, {"t5", "x30"}, {"t6", "x31"}};
map<string, int> labels;
vector<string> codes;

string convert_32bits_to_hex(string s){
	string ans = "";
	for(int i=0;i<32; i += 4){
		string r = s.substr(i,4);
		ans += binary_to_hex[r];
	}
	return ans;
}

bool convertR_to_machineCode(string line, int i){
	string op = "", rd = "", rs1 = "", rs2 = "";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;
		
	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(line[a] != ','){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(a < line.length()){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
			return true;
		}
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
	codes.push_back(machine_code);

	return false;
}

bool convertI_to_machineCode(string line, int i){
	string rd = "", rs1 = "", imm = "", op = "";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;

	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(line[a] != ','){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(a < line.length()){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
			return true;
		}
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

	int num = stoi(imm);
	if(num < -2048 || num > 2047){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}

	imm = convert_deci_to_binary(num, 12);
	if(op == "slli" || op == "srli"){
		string temp = imm.substr(7, 5);
		imm = "0000000";
		imm += temp;
	}
	else if(op == "srai"){
		string temp = imm.substr(7, 5);
		imm = "0100000";
		imm += temp;
	}

	string binary_code = "";
	binary_code += imm;
	binary_code += rs1;
	binary_code += instructFunct3[op];
	binary_code += rd;
	binary_code += instruct_opcode["I"];		


	string machine_code = convert_32bits_to_hex(binary_code);
	codes.push_back(machine_code);

	return false;
}

bool convertI_l_to_machineCode(string line, int i){
	string rd = "", rs1 = "", imm = "", op = "";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;

	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(line[a] != '('){
		imm += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;

	while(line[a] != ')'){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	if(a != line.length()-1){
		cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
		return true;
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

	int num = stoi(imm);
	if(num < -2048 || num > 2047){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}
	imm = convert_deci_to_binary(num, 12);

	string binary_code = "";
	binary_code += imm;
	binary_code += rs1;
	binary_code += instructFunct3[op];
	binary_code += rd;
	binary_code += instruct_opcode["I_l"];		


	string machine_code = convert_32bits_to_hex(binary_code);
	codes.push_back(machine_code);

	return false;
}

bool convertS_to_machineCode(string line, int i){
	string op="",rs2="",rs1="",imm="";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;
		
	while(line[a] != ','){
		rs2 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(line[a] != '('){
		imm += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;
	while(line[a] != ')' ){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	if(a != line.length()-1){
		cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
		return true;
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

	int num = stoi(imm);
	if(num < -2048 || num > 2047){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}
	string immB = convert_deci_to_binary(num,12);
	binary_code += immB.substr(0,7);
	binary_code += rs2;
	binary_code += rs1;
	binary_code += instructFunct3[op];
	binary_code += immB.substr(7,5);
	binary_code += instruct_opcode["S"];

	string machine_code = convert_32bits_to_hex(binary_code);
	codes.push_back(machine_code);

	return false;
}

bool convertB_to_machineCode(string line, int i){
	string op="",rs2="",rs1="",imm="";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;
		
	while(line[a] != ','){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(line[a] != ','){
		rs2 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(a < line.length()){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
			return true;
		}
		imm += line[a];
		a++;
	}

	if(labels.find(imm) == labels.end()){
		cout<<"Error found in line "<<i+1<<": Label not defined"<<endl;
		return true;
	}
	int diff = labels[imm] - i;
	diff *= 4;

	string binary_code = "";

	for(int k=0;k<2;k++){
		string temp;
		if(k==0){
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
			rs1 = convert_deci_to_binary(num, 5);
		}
		else{
			rs2 = convert_deci_to_binary(num, 5);
		}
	} 

	if(diff < -4096 || diff > 4095){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}
	string immB = convert_deci_to_binary(diff,13);
	binary_code += immB[0] + immB.substr(2,6);
	binary_code += rs2;
	binary_code += rs1;
	binary_code += instructFunct3[op];
	binary_code += immB.substr(8,4) + immB[1];
	binary_code += instruct_opcode["B"];

	string machine_code = convert_32bits_to_hex(binary_code);
	codes.push_back(machine_code);

	return false;
}

bool convertJ_to_machineCode(string line, int i){
	string op="",rd="",imm="";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;
		
	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(a < line.length() ){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
			return true;
		}
		imm += line[a];
		a++;
	}

	if(labels.find(imm) == labels.end()){
		cout<<"Error found in line "<<i+1<<": Label not defined"<<endl;
		return true;
	}
	int diff = labels[imm] - i;
	diff *= 4;

	string binary_code = "";

	if(rd[0] != 'x'){
		rd = alias[rd];
	}

	int num = 0;
	for(int j=1;j<rd.length();j++){
			num = num*10 + (rd[j]-'0');
	}

	rd = convert_deci_to_binary(num, 5);
	string immB = convert_deci_to_binary(diff ,21);

	binary_code += immB[0];
	binary_code += immB.substr(10,10);
	binary_code += immB[9];
	binary_code += immB.substr(1,8);
	binary_code += rd;
	binary_code += instruct_opcode["J"];

	string machine_code = convert_32bits_to_hex(binary_code);
	codes.push_back(machine_code);

	return false;
}

bool convertU_to_machineCode(string line, int i){
	string op="",rd="",imm="";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;
		
	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(a < line.length() ){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
			return true;
		}
		imm += line[a];
		a++;
	}

	if(rd[0] != 'x'){
		rd = alias[rd];
	}

	int num = 0;
	for(int j=1;j<rd.length();j++){
			num = num*10 + (rd[j]-'0');
	}

	rd = convert_deci_to_binary(num, 5);

	int n = imm.length();
	imm = imm.substr(2, n-2);

	num = convert_hex_to_deci(imm);
	imm = convert_deci_to_binary(num, 32);

	imm = imm.substr(12, 20);

	string binary_code = "";
	binary_code += imm;
	binary_code += rd;
	binary_code += instruct_opcode["U"];

	string machine_code = convert_32bits_to_hex(binary_code);
	codes.push_back(machine_code);

	return false;
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

		for(int j=0;j<line.length();j++){
			if(line[j] == ':'){
				string l = line.substr(0, j);
				if(labels.find(l) != labels.end()){
					cout<<"Error found in line "<<i+1<<": Label is already declared and can't be declared twice"<<endl;
					return 0;
				}

				labels[l] = i;

				int n = line.length();
				text[i] = line.substr(j+2, n-j-2);
				break;
			}
		}
	}

	for(int i=0;i<text.size();i++){
		string line = text[i];
		string op = "";
		int a = 0;
		while(line[a] != ' '){
			op += line[a];
			a++;
		}
		
		if(instructType.find(op) == instructType.end()){
			cout<<"Error found in line "<<i+1<<": Invalid Instruction"<<endl;
			return 0;
		}

		string type = instructType[op];
		if(type=="R"){
			if(convertR_to_machineCode(line, i)){
				return 0;
			}
		}
		else if(type == "S"){
			if(convertS_to_machineCode(line, i)){
				return 0;
			}
		}
		else if(type=="I"){
			if(convertI_to_machineCode(line, i)){
				return 0;
			}
		}
		else if(type=="I_l"){
			if(convertI_l_to_machineCode(line, i)){
				return 0;
			}
		}
		else if(type=="B"){
			if(convertB_to_machineCode(line, i)){
				return 0;
			}
		}
		else if(type=="J"){
			if(convertJ_to_machineCode(line, i)){
				return 0;
			}
		}
		else if(type=="U"){
			if(convertU_to_machineCode(line, i)){
				return 0;
			}
		}
	}

	for(auto i: codes){
		cout<<i<<endl;
	}

	return 0;
}
