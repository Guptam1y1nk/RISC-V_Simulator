#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>
using namespace std;
#define ll long long

ll registers[32];
ll PC = 0;
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

string getPC(ll PC){
    string PC_num = "";
  
    while (PC != 0) {
        int rem = 0;
        char ch;
        rem = PC % 16;

        if (rem < 10) {
            ch = rem + 48;
        }
        else {
            ch = rem + 55;
        }
        PC_num += ch;
        PC = PC / 16;
    }
    
    int i = 0, j = PC_num.size() - 1;
    while(i <= j)
    {
      swap(PC_num[i], PC_num[j]);
      i++;
      j--;
    }

	string ans = "0x";
	for(int i=0;i<8-PC_num.size();i++){
		ans += '0';
	}
	ans += PC_num;

	return ans;
}

map<string,string> instructType = {{"add", "R"}, {"sub", "R"}, {"and", "R"}, {"or", "R"}, {"xor", "R"}, {"sll", "R"}, {"srl", "R"}, {"sra", "R"}, {"addi", "I"}, {"xori", "I"}, {"ori", "I"}, {"andi", "I"}, {"slli", "I"}, {"srli", "I"}, {"srai", "I"}, {"lb", "I_l"}, {"lh", "I_l"}, {"lw", "I_l"}, {"ld", "I_l"}, {"lbu", "I_l"}, {"lhu", "I_l"}, {"lwu", "I_l"}, {"sb", "S"}, {"sh", "S"}, {"sw", "S"}, {"sd", "S"}, {"beq", "B"}, {"bne", "B"}, {"blt", "B"}, {"bge", "B"},{"bltu", "B"}, {"bgeu" , "B"}, {"jal", "J"}, {"jalr", "I_l"}, {"lui", "U"}, {"auipc", "U"}};
map<string, string> alias = {{"zero", "x0"}, {"ra", "x1"}, {"sp", "x2"}, {"gp", "x3"}, {"tp", "x4"}, {"t0", "x5"}, {"t1", "x6"}, {"t2", "x7"}, {"s0", "x8"}, {"s1", "x9"}, {"a0", "x10"}, {"a1", "x11"}, {"a2", "x12"}, {"a3", "x13"}, {"a4", "x14"}, {"a5", "x15"}, {"a6", "x16"}, {"a7", "x17"}, {"s2", "x18"}, {"s3", "x19"}, {"s4", "x20"}, {"s5", "x21"}, {"s6", "x22"}, {"s7", "x23"}, {"s8", "x24"}, {"s9", "x25"}, {"s10", "x26"}, {"s11", "x27"}, {"t3", "x28"}, {"t4", "x29"}, {"t5", "x30"}, {"t6", "x31"}};
map<string, int> labels;
vector<string> text;
vector<string> codes;

int getRegister(string temp){
	if(temp[0] != 'x'){
		temp = alias[temp];
	}
	int num = 0;
	for(int j=1;j<temp.length();j++){
		num = num*10 + (temp[j]-'0');
	}

	return num;
}

bool execute_Rtype(string line, int i){
	PC += 4;
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

	int rs1_num = getRegister(rs1);
	int rs2_num = getRegister(rs2);
	int rd_num = getRegister(rd); 

	if(rs1_num < 0 || rs1_num>31 || rs2_num < 0 || rs2_num>31 || rd_num < 0 || rd_num>31){
		cout<<"Error found in line "<<i+1<<": This code doesn't support more than 32 Registers"<<endl;
		return true;
	}
		
	if (op == "add") {
		registers[rd_num] = registers[rs1_num] + registers[rs2_num];
	} else if (op == "sub") {
		registers[rd_num] = registers[rs1_num] - registers[rs2_num];
	} else if (op == "and") {
		registers[rd_num] = registers[rs1_num] & registers[rs2_num];
	} else if (op == "or") {
		registers[rd_num] = registers[rs1_num] | registers[rs2_num];
	} else if (op == "xor") {
		registers[rd_num] = registers[rs1_num] ^ registers[rs2_num];
	} else if (op == "sll") {
		registers[rd_num] = registers[rs1_num] << registers[rs2_num];
	} else if (op == "srl") {
		registers[rd_num] = registers[rs1_num] >> registers[rs2_num];
	} else if (op == "sra") {
		registers[rd_num] = registers[rs1_num] >> registers[rs2_num];
	} else {
		cout << "Invalid Instruction" << endl;
	}

	return false;
}

bool execute_Itype(string line, int i){
	PC += 4;
	string rd = "", rs1 = "", imm = "", op = "";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expected value"<<endl;
			return true;
		}
	}
	a++;

	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expected value"<<endl;
			return true;
		}
	}
	a += 2;

	while(line[a] != ','){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expected value"<<endl;
			return true;
		}
	}
	a += 2;

	while(a < line.length()){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expected value"<<endl;
			return true;
		}
		imm += line[a];
		a++;
	}
	
	int rs1_num = getRegister(rs1);
	int rd_num = getRegister(rd);

	if(rs1_num < 0 || rs1_num>31 || rd_num < 0 || rd_num>31){
		cout<<"Error found in line "<<i+1<<": This code doesn't support more than 32 Registers"<<endl;
		return true;
	}

	int num = stoi(imm);
	if(num < -2048 || num > 2047){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}

	imm = convert_deci_to_binary(num, 12);
	
	if(op == "addi"){
		registers[rd_num] = registers[rs1_num] + num;
	} else if(op == "andi"){
		registers[rd_num] = registers[rs1_num] & num;
	} else if(op == "ori"){
		registers[rd_num] = registers[rs1_num] | num;
	} else if(op == "slli"){
		registers[rd_num] = registers[rs1_num] << num;
	} else if(op == "srli"){
		registers[rd_num] = registers[rs1_num] >> num;
	} else if(op == "srai"){
		registers[rd_num] = registers[rs1_num] >> num;
	}

	return false;
}

int unsignedComp(int n, int m){
	string n_unsigned = convert_deci_to_binary(n,16);
	string m_unsigned = convert_deci_to_binary(m,16);

	int size = n_unsigned.size();
	for(int i=0;i<size;i++){
		if(n_unsigned[i] == '0' && m_unsigned[i] == '1'){
			return 1; // n<m
		}
		if(n_unsigned[i] == '1' && m_unsigned[i] == '0'){
			return 0; // n>m
		}
	}
	return 0; // n==m
}

bool execute_Btype(string line, int i){
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

	int rs1_num = getRegister(rs1);
	int rs2_num = getRegister(rs2);
	int diff = labels[imm] - i;
	diff *= 4;

	if(rs1_num < 0 || rs1_num>31 || rs2_num < 0 || rs2_num>31 ){
		cout<<"Error found in line "<<i+1<<": This code doesn't support more than 32 Registers"<<endl;
		return true;
	}

	if(diff < -4096 || diff > 4095){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}
	
	if(op == "beq" && registers[rs1_num] == registers[rs2_num]){
		PC = diff;
	} else if(op == "bne" && registers[rs1_num] != registers[rs2_num]){
		PC = diff;
	} else if(op == "blt" && registers[rs1_num] < registers[rs2_num]){
		PC = diff;
	} else if(op == "bge" && registers[rs1_num] >= registers[rs2_num]){
		PC = diff;
	} else if(op == "bltu" && (unsignedComp(registers[rs1_num] , registers[rs2_num]) == 1) ){
		cout<<"PC+=4";
		PC = diff;
	} else if(op == "bgeu" && (unsignedComp(registers[rs1_num] , registers[rs2_num]) == 0) ){
		PC = diff;
	} else{
		cout<<"PC+=4";
		PC += 4;
	}

	return false;
}
// bool execute_I_ltype(string line, int i){
// 	string rd = "", rs1 = "", imm = "", op = "";
// 	int a = 0;
// 	while(line[a] != ' '){
// 		op += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	a++;

// 	while(line[a] != ','){
// 		rd += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	a += 2;

// 	while(line[a] != '('){
// 		imm += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	a++;

// 	while(line[a] != ')'){
// 		rs1 += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	if(a != line.length()-1){
// 		cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
// 		return true;
// 	}

// 	for(int k=0;k<2;k++){
// 		string temp;
// 		if(k==0){
// 			temp = rd;
// 		}
// 		else{
// 			temp = rs1;
// 		}

// 		if(temp[0] != 'x'){
// 			temp = alias[temp];
// 		}

// 		int num = 0;
// 		for(int j=1;j<temp.length();j++){
// 			num = num*10 + (temp[j]-'0');
// 		}

// 		if(k==0){
// 			rd = convert_deci_to_binary(num, 5);
// 		}
// 		else{
// 			rs1 = convert_deci_to_binary(num, 5);
// 		}
// 	} 	

// 	int num = stoi(imm);
// 	if(num < -2048 || num > 2047){
// 		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
// 		return true;
// 	}
// 	imm = convert_deci_to_binary(num, 12);

// 	string binary_code = "";
// 	binary_code += imm;
// 	binary_code += rs1;
// 	binary_code += instructFunct3[op];
// 	binary_code += rd;
// 	binary_code += instruct_opcode["I_l"];	

// 	string machine_code = convert_32bits_to_hex(binary_code);
// 	codes.push_back(machine_code);

// 	return false;
// }

// bool execute_Stype(string line, int i){
// 	string op="",rs2="",rs1="",imm="";
// 	int a = 0;
// 	while(line[a] != ' '){
// 		op += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	a++;
		
// 	while(line[a] != ','){
// 		rs2 += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	a += 2;

// 	while(line[a] != '('){
// 		imm += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	a++;
// 	while(line[a] != ')' ){
// 		rs1 += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	if(a != line.length()-1){
// 		cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
// 		return true;
// 	}

// 	string binary_code = "";

// 	for(int k=0;k<2;k++){
// 		string temp;
// 		if(k==0){
// 			temp = rs2;
// 		}
// 		else{
// 			temp = rs1;
// 		}

// 		if(temp[0] != 'x'){
// 			temp = alias[temp];
// 		}
// 		int num = 0;
// 		for(int j=1;j<temp.length();j++){
// 			num = num*10 + (temp[j]-'0');
// 		}
// 		if(k==0){
// 			rs2 = convert_deci_to_binary(num, 5);
// 		}
// 		else{
// 			rs1 = convert_deci_to_binary(num, 5);
// 		}
// 	} 

// 	int num = stoi(imm);
// 	if(num < -2048 || num > 2047){
// 		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
// 		return true;
// 	}
// 	string immB = convert_deci_to_binary(num,12);
// 	binary_code += immB.substr(0,7);
// 	binary_code += rs2;
// 	binary_code += rs1;
// 	binary_code += instructFunct3[op];
// 	binary_code += immB.substr(7,5);
// 	binary_code += instruct_opcode["S"];

// 	string machine_code = convert_32bits_to_hex(binary_code);
// 	codes.push_back(machine_code);

// 	return false;
// }


// bool execute_Jtype(string line, int i){
// 	string op="",rd="",imm="";
// 	int a = 0;
// 	while(line[a] != ' '){
// 		op += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	a++;
		
// 	while(line[a] != ','){
// 		rd += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	a += 2;

// 	while(a < line.length() ){
// 		if(line[a] == ','){
// 			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
// 			return true;
// 		}
// 		imm += line[a];
// 		a++;
// 	}

// 	if(labels.find(imm) == labels.end()){
// 		cout<<"Error found in line "<<i+1<<": Label not defined"<<endl;
// 		return true;
// 	}
// 	int diff = labels[imm] - i;
// 	diff *= 4;

// 	string binary_code = "";

// 	if(rd[0] != 'x'){
// 		rd = alias[rd];
// 	}

// 	int num = 0;
// 	for(int j=1;j<rd.length();j++){
// 			num = num*10 + (rd[j]-'0');
// 	}

// 	rd = convert_deci_to_binary(num, 5);
// 	string immB = convert_deci_to_binary(diff ,21);

// 	binary_code += immB[0];
// 	binary_code += immB.substr(10,10);
// 	binary_code += immB[9];
// 	binary_code += immB.substr(1,8);
// 	binary_code += rd;
// 	binary_code += instruct_opcode["J"];

// 	string machine_code = convert_32bits_to_hex(binary_code);
// 	codes.push_back(machine_code);

// 	return false;
// }

// bool execute_Utype(string line, int i){
// 	string op="",rd="",imm="";
// 	int a = 0;
// 	while(line[a] != ' '){
// 		op += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	a++;
		
// 	while(line[a] != ','){
// 		rd += line[a];
// 		a++;
// 		if(a == line.length()){
// 			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
// 			return true;
// 		}
// 	}
// 	a += 2;

// 	while(a < line.length() ){
// 		if(line[a] == ','){
// 			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
// 			return true;
// 		}
// 		imm += line[a];
// 		a++;
// 	}

// 	if(rd[0] != 'x'){
// 		rd = alias[rd];
// 	}

// 	int num = 0;
// 	for(int j=1;j<rd.length();j++){
// 			num = num*10 + (rd[j]-'0');
// 	}

// 	rd = convert_deci_to_binary(num, 5);

// 	int n = imm.length();
// 	imm = imm.substr(2, n-2);

// 	num = convert_hex_to_deci(imm);
// 	imm = convert_deci_to_binary(num, 32);

// 	imm = imm.substr(12, 20);

// 	string binary_code = "";
// 	binary_code += imm;
// 	binary_code += rd;
// 	binary_code += instruct_opcode["U"];

// 	string machine_code = convert_32bits_to_hex(binary_code);
// 	codes.push_back(machine_code);

// 	return false;
// }

void loadCommand(string filename){
	ifstream inputFile(filename);
    string line;
	text.clear();
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
					return;
				}

				labels[l] = i;

				int n = line.length();
				text[i] = line.substr(j+2, n-j-2);
				break;
			}
		}
	}

	memset(registers, 0, 32*sizeof(registers[0]));
}

int main(){
	while(1){
		string command;
		getline(cin, command);

		int i = 0, n = command.length();
		string operation;
		while(i < n && command[i] != ' '){
			operation += command[i];
			i++;
		}

		if(operation == "load"){
			string filename = command.substr(i+1, n-i-1);
			loadCommand(filename);
			cout<<endl;
			PC = 0;
		}
		else if(operation == "run"){
			for(int i=0;i<text.size();i++){
				string line = text[i];
				cout<<"Executed "<<line<<"; PC="<<getPC(PC)<<endl;
				string op = "";
				int a = 0;
				while(line[a] != ' '){
					op += line[a];
					a++;
					if(a == line.length()){
						cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
						return true;
					}
				}

				string type = instructType[op];
				if(type=="R"){
					if(execute_Rtype(line, i)){
						cout<<"Called"<<endl;
						return 0;
					}
				}
				else if(type=="I"){
				 	if(execute_Itype(line, i)){
				 		return 0;
				 	}
				}
				else if(type=="B"){
				 	if(execute_Btype(line, i)){
				 		return 0;
				 	}
				}

				// else if(type == "S"){
				// 	if(execute_Stype(line, i)){
				// 		return 0;
				// 	}
				// }
				
				// else if(type=="I_l"){
				// 	if(execute_I_ltype(line, i)){
				// 		return 0;
				// 	}
				// }
				// else if(type=="J"){
				// 	if(execute_Jtype(line, i)){
				// 		return 0;
				// 	}
				// }
				// else if(type=="U"){
				// 	if(execute_Utype(line, i)){
				// 		return 0;
				// 	}
				// }
			}
			cout<<endl;
		}
		else if(operation == "regs"){
			cout<<"Registers:"<<endl;
			for(int i=0;i<32;i++){
				long long val = registers[i];
				stringstream ss;
				ss << hex << val;
				string res(ss.str());
				cout<<"x"<<i<<" = 0x"<<res<<endl;
			}
			cout<<endl;
		}
		else if(operation == "exit"){
			cout<<"Exited the simulator"<<endl<<endl;
			break;
		}
	}

	return 0;
}
