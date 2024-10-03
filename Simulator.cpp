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
bitset<8> mem[0x40000];
ll instruction_pos = 0;

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
	PC *= 4;
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

string toHex(long num1){
	if (num1 == 0)
		return "0";
	long num = num1;
	string s = "";
	while (num) {
		int temp = num % 16;
		if (temp <= 9)
		s += (48 + temp);
		else
		s += (55 + temp);
		num = num / 16;
	}
	reverse(s.begin(), s.end());
	return s;
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
	instruction_pos++;

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
	instruction_pos++;
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
	
	int num = stoi(imm);
	if(num < -2048 || num > 2047){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}
	
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

bool execute_I_ltype(string line, int i){
	instruction_pos++;
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

	int rs1_num = getRegister(rs1);
	int rd_num = getRegister(rd);

	int num = stoi(imm);
	if(num < -2048 || num > 2047){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}

	ll pos = registers[rs1_num] + num - 0x10000;
	ll data = 0;

	if(op=="lb"){
		long byte;
		for(int i=0;i>=0;i--){
			byte = mem[pos+i].to_ulong();
			data += byte;
			if(i>0){
				data = data << 8;
			}
		}
	}
	else if(op=="lh"){
		long byte = 0;
		for(int i=1;i>=0;i--){
			byte = mem[pos+i].to_ulong();
			data += byte;
			if(i>0){
				data = data << 8;
			}
		}
	}
	else if(op=="lw"){
		long byte = 0;
		for(int i=3;i>=0;i--){
			byte = mem[pos+i].to_ulong();
			data += byte;
			if(i>0){
				data = data << 8;
			}
		}
	}
	else if(op=="ld"){
		long byte = 0;
		for(int i=7;i>=0;i--){
			byte = mem[pos+i].to_ulong();
			data += byte;
			if(i>0){
				data = data << 8;
			}
		}
	}

	registers[rd_num] = data;

	return false;
}

bool execute_Stype(string line, int i){
	instruction_pos++;
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

	int rs1_num = getRegister(rs1);
	int rs2_num = getRegister(rs2);

	int num = stoi(imm);
	if(num < -2048 || num > 2047){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}

	ll pos = registers[rs1_num] + num - 0x10000;
	ll data = registers[rs2_num];

	if(op=="sb"){
		int byte = 0;
		for(int i=1;i<=1;i++){
			byte = (data & (0b11111111));
			mem[pos++] = byte; 
			data = data >> 8;
		}
	}
	else if(op=="sh"){
		int byte = 0;
		for(int i=1;i<=2;i++){
			byte = (data & (0b11111111));
			mem[pos++] = byte; 
			data = data >> 8;
		}
	}
	else if(op=="sw"){
		int byte = 0;
		for(int i=1;i<=4;i++){
			byte = (data & (0b11111111));
			mem[pos++] = byte; 
			data = data >> 8;
		}
	}
	else if(op=="sd"){
		int byte = 0;
		for(int i=1;i<=8;i++){
			byte = (data & (0b11111111));
			mem[pos++] = byte; 
			data = data >> 8;
		}
	}
	else{
		cout<<"Invalid Instruction"<<endl;
	}

	return false;
}

int unsignedComp(int n, int m){
	string n_unsigned = convert_deci_to_binary(n,16);
	string m_unsigned = convert_deci_to_binary(m,16);

	int size = n_unsigned.size();
	for(int i=0;i<size;i++){
		if(n_unsigned[i] == '0' && m_unsigned[i] == '1'){
			return 1;
		}
		if(n_unsigned[i] == '1' && m_unsigned[i] == '0'){
			return 0;
		}
	}
	return 0;
}

pair<int, bool> execute_Btype(string line, int i){
	string op="",rs2="",rs1="",imm="";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1, true};
		}
	}
	a++;
		
	while(line[a] != ','){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1, true};
		}
	}
	a += 2;

	while(line[a] != ','){
		rs2 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1, true};
		}
	}
	a += 2;

	while(a < line.length()){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
			return {-1, true};
		}
		imm += line[a];
		a++;
	}

	if(labels.find(imm) == labels.end()){
		cout<<"Error found in line "<<i+1<<": Label not defined"<<endl;
		return {-1, true};
	}

	int rs1_num = getRegister(rs1);
	int rs2_num = getRegister(rs2);
	int jump_index = labels[imm];
	int diff = (labels[imm] - i)*4;

	if(rs1_num < 0 || rs1_num>31 || rs2_num < 0 || rs2_num>31 ){
		cout<<"Error found in line "<<i+1<<": This code doesn't support more than 32 Registers"<<endl;
		return {-1, true};
	}

	if(diff < -4096 || diff > 4095){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return {-1, true};
	}
	
	if(op == "beq" && registers[rs1_num] == registers[rs2_num]){
		instruction_pos = jump_index;
	} else if(op == "bne" && registers[rs1_num] != registers[rs2_num]){
		instruction_pos = jump_index;;
	} else if(op == "blt" && registers[rs1_num] < registers[rs2_num]){
		instruction_pos = jump_index;;
	} else if(op == "bge" && registers[rs1_num] >= registers[rs2_num]){
		instruction_pos = jump_index;;
	} else if(op == "bltu" && (unsignedComp(registers[rs1_num] , registers[rs2_num]) == 1) ){
		instruction_pos = jump_index;;
	} else if(op == "bgeu" && (unsignedComp(registers[rs1_num] , registers[rs2_num]) == 0) ){
		instruction_pos = jump_index;;
	} else{
		instruction_pos++;
	}

	return {instruction_pos, false};
}

pair<int,bool> execute_Jtype(string line, int i){
	string op="",rd="",imm="";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1,true};
		}
	}
	a++;
		
	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1,true};
		}
	}
	a += 2;

	while(a < line.length() ){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
			return {-1,true};
		}
		imm += line[a];
		a++;
	}

	if(labels.find(imm) == labels.end()){
		cout<<"Error found in line "<<i+1<<": Label not defined"<<endl;
		return {-1,true};
	}
	int diff = labels[imm] - i;
	diff *= 4;
	int jump_index = labels[imm];

	int rd_num = getRegister(rd);
	if(rd_num < 0 || rd_num>31){
		cout<<"Error found in line "<<i+1<<": This code doesn't support more than 32 Registers"<<endl;
		return {-1,true};
	}

	if(op == "jal"){
		registers[rd_num] = instruction_pos + 1;
		instruction_pos = jump_index;
	}
	return {instruction_pos,false};
}

ll getAddress(string imm){
	ll address = 0;
	int n = imm.length();

	for(int i=2;i<n;i++){
		int val = 0;
		if(imm[i]>='0' && imm[i]<='9'){
			val = imm[i] - '0';
		}
		else{
			val = (imm[i] - 'a') + 10;
		}
		address = address*16 + val;
	}

	return address;
}

bool execute_Utype(string line, int i){
	instruction_pos++;
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

	ll num = getAddress(imm);
	int rd_num = getRegister(rd);
	num *= pow(16, 3);

	if(op=="lui"){
		registers[rd_num] = num;
	}
	else if(op=="auipc"){
		num += (instruction_pos*4);
		registers[rd_num] = num;
	}

	return false;
}

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
	memset(mem, 0, 0x40000*sizeof(mem[0]));
}

bool runInstruction(int i){
	string line = text[i];
	cout<<"Executed "<<line<<"; PC="<<getPC(i)<<endl;
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
			return true;
		}
	}
	else if(type == "S"){
		if(execute_Stype(line, i)){
			return true;
		}
	}
	else if(type=="I"){
		if(execute_Itype(line, i)){
			return true;
		}
	}
	else if(type=="I_l"){
		if(execute_I_ltype(line, i)){
			return true;
		}
	}
	else if(type=="B"){
		pair<int, bool> ans = execute_Btype(line, i);
		if(ans.second){
			return true;
		}
		else{
			i = ans.first - 1;
		}
	}
	else if(type=="J"){
		pair<int, bool> ans = execute_Jtype(line, i);
		if(ans.second){
			return true;
		}
		else{
			i = ans.first - 1;
		}
	}
	else if(type=="U"){
		if(execute_Utype(line, i)){
			return true;
		}
	}

	return false;
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
			instruction_pos = 0;
		}
		else if(operation == "run"){
			if(instruction_pos == text.size()){
				cout<<"Nothing to run"<<endl<<endl;
				continue;
			}
			for(int i=instruction_pos;i<text.size();i++){
				if(runInstruction(i)){
					return 0;
				}
			}
			cout<<endl;
		}
		else if(operation == "step"){
			if(instruction_pos == text.size()){
				cout<<"Nothing to step"<<endl<<endl;
			}
			else{
				if(runInstruction(instruction_pos)){
					return 0;
				}
				cout<<endl;
			}
		}
		else if(operation == "mem"){
			string temp = "";
			i++;
			while(i<n && command[i] != ' '){
				temp += command[i++];
			}
			int address = getAddress(temp);

			temp = "";
			i++;
			while(i<n){
				temp += command[i++];
			}
			int num = stoi(temp);

			int start = address - 0x10000;
			for(int i=0;i<num;i++){
				cout<<"Memory[0x";
				long long val = address + i;
				stringstream ss;
				ss << hex << val;
				string res(ss.str());
				cout<<res<<"] = 0x";

				long byte = mem[start+i].to_ulong();
				string hexval = toHex(byte);
				cout<<hexval<<endl;
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
		else{
			cout<<"Invalid Operation"<<endl<<endl;
			break;
		}
	}

	return 0;
}
